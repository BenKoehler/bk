/*
 * MIT License
 *
 * Copyright (c) 2018 Benjamin Köhler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <bkCMR/PhaseUnwrapping2DT.h>

#include <array>
#include <cassert>
#include <complex>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include <bk/FFT>
#include <bk/MathFunctions>
#include <bk/Matrix>
#include <bk/ThreadPool>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

#include <bkCMR/FlowImage2DT.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct PhaseUnwrapping2DT::Impl
    {
        std::vector<std::pair<unsigned int/*lid*/, char/*nr*/>> lids; //!< list ids of wrapped voxels
        bool                                                    is_initialized;

        Impl()
            : is_initialized(false)
        { /* do nothing */ }

        Impl(const Impl&) = default;
        Impl(Impl&&) = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    PhaseUnwrapping2DT::PhaseUnwrapping2DT() = default;
    PhaseUnwrapping2DT::PhaseUnwrapping2DT(const PhaseUnwrapping2DT&) = default;
    PhaseUnwrapping2DT::PhaseUnwrapping2DT(PhaseUnwrapping2DT&&) noexcept = default;
    PhaseUnwrapping2DT::~PhaseUnwrapping2DT() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    bool PhaseUnwrapping2DT::is_wrapped() const
    { return num_wrapped_voxels() != 0; }

    unsigned int PhaseUnwrapping2DT::num_wrapped_voxels() const
    { return _pdata->lids.size(); }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    PhaseUnwrapping2DT& PhaseUnwrapping2DT::operator=(const PhaseUnwrapping2DT&) = default;
    PhaseUnwrapping2DT& PhaseUnwrapping2DT::operator=(PhaseUnwrapping2DT&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void PhaseUnwrapping2DT::clear()
    {
        _pdata->lids.clear();
        _pdata->is_initialized = false;
    }

    Clock PhaseUnwrapping2DT::init(const FlowImage2DT& _ff, double venc)
    {
        Clock clock;
        clock.start();

        clear();

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(22, ___("analyzing phase wraps (2D+T)"));
        #endif

        FlowImage2DT ff(_ff);

        /*
         * scale flow velocities to [-pi,+pi]
         */
        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(ff.num_values()); ++i)
        {
            ff[i] /= venc;
            ff[i] *= pi<double>;
        }

        /*
         * ceil the size of each dimension of the original flow image
         * to the next power of 2
         */
        const Vec3ui& size                             = ff.geometry().size(); // original flow image size
        Vec3ui            size2(size); // each dimension extended to the next power of 2
        Vec3ui            off; // offset (original to scaled) per dimension
        unsigned int      N2                           = 1; // number of elements in the upscaled image
        for (unsigned int i                            = 0; i < 3 /*dims*/; ++i)
        {
            unsigned int p = 1;
            while (p < size2[i])
            { p <<= 1; }

            size2[i] = p;
            N2 *= p;

            off[i] = (size2[i] - size[i]) / 2;
        }

        /*
         * laplace kernel in frequency space
         */
        std::vector<std::complex<double>> laplacefft(N2);
        auto                              task_laplace = bk_threadpool.enqueue([&]()
                                                                               {
                                                                                   for (unsigned int x = (size2[0] / 2) - 1; x <= (size2[0] / 2) + 1; ++x)
                                                                                   {
                                                                                       for (unsigned int y = (size2[1] / 2) - 1; y <= (size2[1] / 2) + 1; ++y)
                                                                                       {
                                                                                           for (unsigned int t = (size2[2] / 2) - 1; t <= (size2[2] / 2) + 1; ++t)
                                                                                           { laplacefft[grid_to_list_id(size2, x, y, t)] = -1; }
                                                                                       }
                                                                                   }

                                                                                   laplacefft[grid_to_list_id(size2, size2[0] / 2, size2[1] / 2, size2[2] / 2)] = 26; //3*3*3 - 1;

                                                                                   FFT3D(laplacefft.data(), size2[0], size2[1], size2[2]);

                                                                                   #ifdef BK_EMIT_PROGRESS
                                                                                   prog.increment(1);
                                                                                   #endif
                                                                               });

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        std::vector<std::complex<double>> temp0(N2);
        std::vector<std::complex<double>> temp1(N2);

        /*
         * - calculate sin/cos image fft laplace
         */
        #pragma omp parallel for
        for (int x_ = static_cast<int>(off[0]); x_ < static_cast<int>(off[0] + size[0]); ++x_)
        {
            const unsigned int x = static_cast<unsigned int>(x_);
            for (unsigned int  y = off[1]; y < off[1] + size[1]; ++y)
            {
                for (unsigned int t = off[2]; t < off[2] + size[2]; ++t)
                {
                    const unsigned int lid = grid_to_list_id(size2, x, y, t);
                    const double       fvec  = ff(x - off[0], y - off[1], t - off[2]);
                    temp0[lid] = std::sin(fvec);
                    temp1[lid] = std::cos(fvec);
                } // for t
            } // for y
        } // for x

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        auto task_sin_fft = bk_threadpool.enqueue([&]()
                                                  {
                                                      FFT3D(temp0.data(), size2[0], size2[1], size2[2]);

                                                      #ifdef BK_EMIT_PROGRESS
                                                      prog.increment(1);
                                                      #endif
                                                  });
        auto task_cos_fft = bk_threadpool.enqueue([&]()
                                                  {
                                                      FFT3D(temp1.data(), size2[0], size2[1], size2[2]);

                                                      #ifdef BK_EMIT_PROGRESS
                                                      prog.increment(1);
                                                      #endif
                                                  });

        task_laplace.get(); // calculation of laplace kernel fft
        task_sin_fft.get();
        task_cos_fft.get();

        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(N2); ++i)
        {
            temp0[i] *= laplacefft[i];
            temp1[i] *= laplacefft[i];
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        auto task_sin_ifft = bk_threadpool.enqueue([&]()
                                                   {
                                                       IFFT3D(temp0.data(), size2[0], size2[1], size2[2]);

                                                       #ifdef BK_EMIT_PROGRESS
                                                       prog.increment(1);
                                                       #endif

                                                       FFTShift3D(temp0, size2[0], size2[1], size2[2]);

                                                       #ifdef BK_EMIT_PROGRESS
                                                       prog.increment(1);
                                                       #endif
                                                   });

        auto task_cos_ifft = bk_threadpool.enqueue([&]()
                                                   {
                                                       IFFT3D(temp1.data(), size2[0], size2[1], size2[2]);

                                                       #ifdef BK_EMIT_PROGRESS
                                                       prog.increment(1);
                                                       #endif

                                                       FFTShift3D(temp1, size2[0], size2[1], size2[2]);

                                                       #ifdef BK_EMIT_PROGRESS
                                                       prog.increment(1);
                                                       #endif
                                                   });

        task_sin_ifft.get();
        task_cos_ifft.get();

        auto task_reset_zero = bk_threadpool.enqueue([&]()
                                                     {
                                                         for (unsigned int x = 0; x < size2[0]; ++x)
                                                         {
                                                             for (unsigned int y = 0; y < size2[1]; ++y)
                                                             {
                                                                 for (unsigned int t = 0; t < size2[2]; ++t)
                                                                 {
                                                                     if (x < off[0] || x >= off[0] + size[0] || y < off[1] || y >= off[1] + size[1] || t < off[2] || t >= off[2] + size[2])
                                                                     { temp1[grid_to_list_id(size2, x, y, t)] = 0; }
                                                                 } // for t
                                                             } // for y
                                                         } // for x

                                                         #ifdef BK_EMIT_PROGRESS
                                                         prog.increment(1);
                                                         #endif
                                                     });

        // true phase estimation: cos(phi_w)*lap(sin(phi_w)) - sin(phi_w)*lap(cos(phi_w))
        #pragma omp parallel for
        for (int x_ = static_cast<int>(off[0]); x_ < static_cast<int>(off[0] + size[0]); ++x_)
        {
            const unsigned int x = static_cast<unsigned int>(x_);
            for (unsigned int  y = off[1]; y < off[1] + size[1]; ++y)
            {
                for (unsigned int t = off[2]; t < off[2] + size[2]; ++t)
                {
                    const unsigned int lid = grid_to_list_id(size2, x, y, t);
                    const double       fvec  = ff(x - off[0], y - off[1], t - off[2]);

                    temp0[lid] *= std::cos(fvec);
                    temp0[lid] -= std::sin(fvec) * temp1[lid].real();

                    temp1[lid] = fvec;
                } // for t
            } // for y
        } // for x

        task_reset_zero.get();

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        /*
         * ff laplace
         */
        FFT3D(temp1.data(), size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(N2); ++i)
        { temp1[i] *= laplacefft[i]; }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        IFFT3D(temp1.data(), size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        FFTShift3D(temp1, size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        /*
         * true phase estimation laplace - measured phase laplace
         */
        #pragma omp parallel for
        for (int x_ = static_cast<int>(off[0]); x_ < static_cast<int>(off[0] + size[0]); ++x_)
        {
            const unsigned int x = static_cast<unsigned int>(x_);
            for (unsigned int  y = off[1]; y < off[1] + size[1]; ++y)
            {
                for (unsigned int t = off[2]; t < off[2] + size[2]; ++t)
                {
                    const unsigned int lid = grid_to_list_id(size2, x, y, t);
                    temp0[lid].real(temp0[lid].real() - temp1[lid].real());
                } // for t
            } // for y
        } // for x

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        /*
         * true phase estimation fft inverse laplace
         */
        FFT3D(temp0.data(), size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        #pragma omp parallel for
        for (int i = 0; i < static_cast<int>(N2); ++i)
        {
            if (std::real(laplacefft[i]) * std::real(laplacefft[i]) + std::imag(laplacefft[i]) * std::imag(laplacefft[i]) >= 1e-13)
            { temp0[i] /= laplacefft[i]; }
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        IFFT3D(temp0.data(), size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        FFTShift3D(temp0, size2[0], size2[1], size2[2]);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        constexpr const double pifac = 1.0 / (2.0 * pi<double>);

        #pragma omp parallel for
        for (int x_ = static_cast<int>(off[0]); x_ < static_cast<int>(off[0] + size[0]); ++x_)
        {
            const unsigned int x = static_cast<unsigned int>(x_);
            for (unsigned int  y = off[1]; y < off[1] + size[1]; ++y)
            {
                for (unsigned int t = off[2]; t < off[2] + size[2]; ++t)
                {
                    const unsigned int lid = grid_to_list_id(size2, x, y, t);
                    temp0[lid].real(std::round(pifac * temp0[lid].real()));

                    if (temp0[lid].real() != 0)
                    {
                        unsigned int lid2            = grid_to_list_id(size, x - off[0], y - off[1], t - off[2]);
                        char         num_phase_wraps = static_cast<char>(temp0[lid].real());

                        #pragma omp critical
                        { _pdata->lids.emplace_back(lid2, num_phase_wraps); }
                    }
                } // for t
            } // for y
        } // for x

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        _pdata->is_initialized = true;

        return clock;
    }

    void PhaseUnwrapping2DT::apply(FlowImage2DT& ff, double venc) const
    {
        assert(_pdata->is_initialized && "PhaseUnwrapping2DT::apply - init() or load() first");

        for (const auto[lid, nr] : _pdata->lids)
        {
            // flow vectors are scaled to [-venc,+venc]
            ff[lid] += nr * 2 * venc;
        }
    }

    bool PhaseUnwrapping2DT::save(std::string_view filepath) const
    {
        if (!_pdata->is_initialized)
        { return false; }

        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        return save(file);
    }

    bool PhaseUnwrapping2DT::save(std::ofstream& file) const
    {
        if (!file.good())
        { return false; }

        std::uint32_t ui32temp = 0;
        std::int8_t   i8temp   = 0;

        // number of wrapped voxels
        const std::uint32_t n = static_cast<std::uint32_t>(num_wrapped_voxels());
        file.write(reinterpret_cast<const char*>(&n), sizeof(std::uint32_t));

        for (const auto[lid, nr] : _pdata->lids)
        {
            // lid
            ui32temp = static_cast<std::uint32_t>(lid);
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            // nr
            i8temp = static_cast<std::int8_t>(nr);
            file.write(reinterpret_cast<char*>(&i8temp), sizeof(std::int8_t));
        }

        file.close();

        return true;
    }

    bool PhaseUnwrapping2DT::load(std::string_view filepath)
    {
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);
        return load(file);
    }

    bool PhaseUnwrapping2DT::load(std::ifstream& file)
    {
        clear();

        if (!file.good())
        { return false; }

        std::uint32_t ui32temp = 0;
        std::int8_t   i8temp   = 0;

        // number of wrapped voxels
        std::uint32_t n = 0;
        file.read(reinterpret_cast<char*>(&n), sizeof(std::uint32_t));

        _pdata->lids.resize(n);

        // list of pairs of listID + number of wraps (nr)
        for (unsigned int i = 0; i < n; ++i)
        {
            // lid
            file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
            _pdata->lids[i].first = ui32temp;

            // nr
            file.read(reinterpret_cast<char*>(&i8temp), sizeof(std::int8_t));
            _pdata->lids[i].second = static_cast<char>(i8temp);
        }

        file.close();

        _pdata->is_initialized = true;

        return true;
    }
  } // inline namespace cmr
} // namespace bk

