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

#include <bkCMR/PhaseUnwrapping3DT.h>

#include <cassert>
#include <complex>
#include <cstdint>
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

#include <bkCMR/FlowImage3DT.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct PhaseUnwrapping3DT::Impl
    {
        std::array<std::vector<std::pair<unsigned int /*lid*/, char /*nr*/>>, 3> lids; //!< list ids of wrapped voxels
        bool                                                                     is_initialized;

        Impl()
            : is_initialized(false)
        { /* do nothing */ }

        Impl(const Impl&) = default;
        Impl(Impl&&) noexcept = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    PhaseUnwrapping3DT::PhaseUnwrapping3DT() = default;
    PhaseUnwrapping3DT::PhaseUnwrapping3DT(const PhaseUnwrapping3DT&) = default;
    PhaseUnwrapping3DT::PhaseUnwrapping3DT(PhaseUnwrapping3DT&&) noexcept = default;
    PhaseUnwrapping3DT::~PhaseUnwrapping3DT() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    bool PhaseUnwrapping3DT::is_wrapped() const
    { return num_wrapped_voxels() != 0; }

    bool PhaseUnwrapping3DT::is_initialized() const
    { return _pdata->is_initialized; }

    unsigned int PhaseUnwrapping3DT::num_wrapped_voxels() const
    { return num_wrapped_voxels(0) + num_wrapped_voxels(1) + num_wrapped_voxels(2); }

    unsigned int PhaseUnwrapping3DT::num_wrapped_voxels(unsigned int x0y1z2) const
    {
        assert(x0y1z2 <= 2 && "invalid id, must be 0, 1, or 2!");
        return _pdata->lids[x0y1z2].size();
    }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    PhaseUnwrapping3DT& PhaseUnwrapping3DT::operator=(const PhaseUnwrapping3DT&) = default;
    PhaseUnwrapping3DT& PhaseUnwrapping3DT::operator=(PhaseUnwrapping3DT&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void PhaseUnwrapping3DT::clear()
    {
        for (unsigned int i = 0; i < 3; ++i)
        { _pdata->lids[i].clear(); }

        _pdata->is_initialized = false;
    }

    Clock PhaseUnwrapping3DT::init(const FlowImage3DT& _ff, double venc)
    { return init(_ff, {venc, venc, venc}); }

    Clock PhaseUnwrapping3DT::init(const FlowImage3DT& _ff, const std::array<double, 3>& venc)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(62, ___("Analyzing phase wraps (3D+T)"));
        #endif

        Clock clock;
        clock.start();

        /*
         * copy flowfield
         */
        FlowImage3DT ff(_ff);

        /*
         * scale flow vectors to [-pi,+pi]
         */
        #pragma omp parallel for
        for (unsigned int i = 0; i < ff.num_values(); ++i)
        {
            for (unsigned int v = 0; v < 3; ++v)
            { ff[i][v] /= venc[v]; }

            ff[i] *= pi<double>;
        }

        /*
         * ceil the size of each dimension of the original flow image
         * to the next power of 2
         */
        const Vec4ui& size = ff.geometry().size(); // original flow image size
        Vec4ui       size2(size); // each dimension extended to the next power of 2
        Vec4ui       off; // offset (original to scaled) per dimension
        unsigned int N2    = 1; // number of elements in the upscaled image

        for (unsigned int i = 0; i < 4 /*dims*/; ++i)
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
                                                                                           for (unsigned int z = (size2[2] / 2) - 1; z <= (size2[2] / 2) + 1; ++z)
                                                                                           {
                                                                                               for (unsigned int t = (size2[3] / 2) - 1; t <= (size2[3] / 2) + 1; ++t)
                                                                                               { laplacefft[grid_to_list_id(size2, x, y, z, t)] = -1; }
                                                                                           }
                                                                                       }
                                                                                   }

                                                                                   laplacefft[grid_to_list_id(size2, size2[0] / 2, size2[1] / 2, size2[2] / 2, size2[3] / 2)] = 80; //3*3*3*3 - 1;

                                                                                   FFT4D(laplacefft.data(), size2[0], size2[1], size2[2], size2[3]);

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
         * unwrap for each vector component separately
         */
        for (unsigned int v = 0; v < 3; ++v)
        {
            auto task_reset_zero0 = bk_threadpool.enqueue([&]()
                                                          {
                                                              if (v != 0)
                                                              {
                                                                  #pragma omp parallel for
                                                                  for (unsigned int i = 0; i < N2; ++i)
                                                                  {
                                                                      temp0[i].real(0);
                                                                      temp0[i].imag(0);

                                                                      temp1[i].real(0);
                                                                      temp1[i].imag(0);
                                                                  }
                                                              }
                                                          });

            /*
             * - calculate sin/cos image fft laplace
             */
            #pragma omp parallel for
            for (unsigned int x = off[0]; x < off[0] + size[0]; ++x)
            {
                for (unsigned int y = off[1]; y < off[1] + size[1]; ++y)
                {
                    for (unsigned int z = off[2]; z < off[2] + size[2]; ++z)
                    {
                        for (unsigned int t = off[3]; t < off[3] + size[3]; ++t)
                        {
                            const unsigned int lid  = grid_to_list_id(size2, x, y, z, t);
                            const double       fval = ff(x - off[0], y - off[1], z - off[2], t - off[3])[v];
                            temp0[lid] = std::sin(fval);
                            temp1[lid] = std::cos(fval);
                        } // for t
                    } // for z
                } // for y
            } // for x

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            task_reset_zero0.get();

            auto task_sin_fft = bk_threadpool.enqueue([&]()
                                                      {
                                                          FFT4D(temp0.data(), size2[0], size2[1], size2[2], size2[3]);

                                                          #ifdef BK_EMIT_PROGRESS
                                                          prog.increment(1);
                                                          #endif
                                                      });

            auto task_cos_fft = bk_threadpool.enqueue([&]()
                                                      {
                                                          FFT4D(temp1.data(), size2[0], size2[1], size2[2], size2[3]);

                                                          #ifdef BK_EMIT_PROGRESS
                                                          prog.increment(1);
                                                          #endif
                                                      });

            if (v == 0) // calculation of laplace kernel fft
            { task_laplace.get(); }

            task_sin_fft.get();
            task_cos_fft.get();

            #pragma omp parallel for
            for (unsigned int i = 0; i < N2; ++i)
            {
                temp0[i] *= laplacefft[i].real();
                temp1[i] *= laplacefft[i].real();
            }

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            auto task_sin_ifft = bk_threadpool.enqueue([&]()
                                                       {
                                                           IFFT4D(temp0.data(), size2[0], size2[1], size2[2], size2[3]);

                                                           #ifdef BK_EMIT_PROGRESS
                                                           prog.increment(1);
                                                           #endif

                                                           FFTShift4D(temp0, size2[0], size2[1], size2[2], size2[3]);

                                                           #ifdef BK_EMIT_PROGRESS
                                                           prog.increment(1);
                                                           #endif
                                                       });
            auto task_cos_ifft = bk_threadpool.enqueue([&]()
                                                       {
                                                           IFFT4D(temp1.data(), size2[0], size2[1], size2[2], size2[3]);

                                                           #ifdef BK_EMIT_PROGRESS
                                                           prog.increment(1);
                                                           #endif

                                                           FFTShift4D(temp1, size2[0], size2[1], size2[2], size2[3]);

                                                           #ifdef BK_EMIT_PROGRESS
                                                           prog.increment(1);
                                                           #endif
                                                       });

            task_sin_ifft.get();
            task_cos_ifft.get();

            auto task_reset_zero = bk_threadpool.enqueue([&]()
                                                         {
                                                             #pragma omp parallel for
                                                             for (unsigned int x = 0; x < size2[0]; ++x)
                                                             {
                                                                 for (unsigned int y = 0; y < size2[1]; ++y)
                                                                 {
                                                                     for (unsigned int z = 0; z < size2[2]; ++z)
                                                                     {
                                                                         for (unsigned int t = 0; t < size2[3]; ++t)
                                                                         {
                                                                             if (x < off[0] || x >= off[0] + size[0] || y < off[1] || y >= off[1] + size[1] || z < off[2] || z >= off[2] + size[2] || t < off[3] || t >= off[3] + size[3])
                                                                             { temp1[grid_to_list_id(size2, x, y, z, t)] = 0; }
                                                                         } // for t
                                                                     } // for z
                                                                 } // for y
                                                             } // for x

                                                             #ifdef BK_EMIT_PROGRESS
                                                             prog.increment(1);
                                                             #endif
                                                         });

            // true phase estimation: cos(phi_w)*lap(sin(phi_w)) - sin(phi_w)*lap(cos(phi_w))
            #pragma omp parallel for
            for (unsigned int x = off[0]; x < off[0] + size[0]; ++x)
            {
                for (unsigned int y = off[1]; y < off[1] + size[1]; ++y)
                {
                    for (unsigned int z = off[2]; z < off[2] + size[2]; ++z)
                    {
                        for (unsigned int t = off[3]; t < off[3] + size[3]; ++t)
                        {
                            const unsigned int lid  = grid_to_list_id(size2, x, y, z, t);
                            const double       fval = ff(x - off[0], y - off[1], z - off[2], t - off[3])[v];

                            temp0[lid] *= std::cos(fval);
                            temp0[lid] -= std::sin(fval) * temp1[lid].real();

                            temp1[lid] = fval;
                        } // for t
                    } // for z
                } // for y
            } // for x

            task_reset_zero.get();

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            /*
             * ff laplace
             */
            FFT4D(temp1.data(), size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            #pragma omp parallel for
            for (unsigned int i = 0; i < N2; ++i)
            { temp1[i] *= laplacefft[i].real(); }

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            IFFT4D(temp1.data(), size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            FFTShift4D(temp1, size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            /*
             * true phase estimation laplace - measured phase laplace
             */
            #pragma omp parallel for
            for (unsigned int x = off[0]; x < off[0] + size[0]; ++x)
            {
                for (unsigned int y = off[1]; y < off[1] + size[1]; ++y)
                {
                    for (unsigned int z = off[2]; z < off[2] + size[2]; ++z)
                    {
                        for (unsigned int t = off[3]; t < off[3] + size[3]; ++t)
                        {
                            const unsigned int lid = grid_to_list_id(size2, x, y, z, t);
                            temp0[lid].real(temp0[lid].real() - temp1[lid].real());
                        } // for t
                    } // for z
                } // for y
            } // for x

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            /*
             * true phase estimation fft inverse laplace
             */
            FFT4D(temp0.data(), size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            #pragma omp parallel for
            for (unsigned int i = 0; i < N2; ++i)
            {
                if (std::real(laplacefft[i]) * std::real(laplacefft[i]) >= 1e-13)
                { temp0[i] /= laplacefft[i].real(); }
            }

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            IFFT4D(temp0.data(), size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            FFTShift4D(temp0, size2[0], size2[1], size2[2], size2[3]);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            constexpr const double pifac = 1.0 / (2.0 * pi<double>);

            #pragma omp parallel for
            for (unsigned int x = off[0]; x < off[0] + size[0]; ++x)
            {
                for (unsigned int y = off[1]; y < off[1] + size[1]; ++y)
                {
                    for (unsigned int z = off[2]; z < off[2] + size[2]; ++z)
                    {
                        for (unsigned int t = off[3]; t < off[3] + size[3]; ++t)
                        {
                            const unsigned int lid = grid_to_list_id(size2, x, y, z, t);
                            temp0[lid].real(std::round(pifac * temp0[lid].real()));

                            if (temp0[lid].real() != 0)
                            {
                                unsigned int lid2            = grid_to_list_id(size, x - off[0], y - off[1], z - off[2], t - off[3]);
                                char         num_phase_wraps = static_cast<char>(temp0[lid].real());

                                #pragma omp critical
                                { _pdata->lids[v].push_back(std::make_pair(lid2, num_phase_wraps)); }
                            }
                        } // for t
                    } // for z
                } // for y
            } // for x

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        } // for v = 0,1,2 (vector component)

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        _pdata->is_initialized = true;

        clock.stop();
        return clock;
    }

    void PhaseUnwrapping3DT::apply(FlowImage3DT& ff, double venc) const
    { apply(ff, {venc, venc, venc}); }

    void PhaseUnwrapping3DT::apply(FlowImage3DT& ff, const std::array<double, 3>& venc) const
    {
        assert(_pdata->is_initialized && "PhaseUnwrapping3DT::apply - init() or load() first");

        for (unsigned int v = 0; v < 3; ++v)
        {
            for (const auto[lid, nr] : _pdata->lids[v])
            {
                // flow vectors are scaled to [-venc,+venc]
                ff[lid][v] += nr * 2 * venc[v];
            }
        }
    }

    bool PhaseUnwrapping3DT::save(std::string_view filepath) const
    {
        if (!_pdata->is_initialized)
        { return false; }

        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint32_t ui32temp = 0;
        std::int8_t   i8temp   = 0;

        for (unsigned int v = 0; v < 3; ++v)
        {
            // number of wrapped voxels
            const std::uint32_t n = static_cast<std::uint32_t>(num_wrapped_voxels(v));
            file.write(reinterpret_cast<const char*>(&n), sizeof(std::uint32_t));

            for (const auto[lid, nr] : _pdata->lids[v])
            {
                // lid
                ui32temp = static_cast<std::uint32_t>(lid);
                file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

                // nr
                i8temp = static_cast<std::int8_t>(nr);
                file.write(reinterpret_cast<char*>(&i8temp), sizeof(std::int8_t));
            }
        }

        file.close();

        return true;
    }

    bool PhaseUnwrapping3DT::load(std::string_view filepath)
    {
        clear();

        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint32_t ui32temp = 0;
        std::int8_t   i8temp   = 0;

        for (unsigned int v = 0; v < 3; ++v)
        {
            // number of wrapped voxels
            std::uint32_t n = 0;
            file.read(reinterpret_cast<char*>(&n), sizeof(std::uint32_t));

            _pdata->lids[v].resize(n);

            // list of pairs of listID + number of wraps (nr)
            for (unsigned int i = 0; i < n; ++i)
            {
                // lid
                file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
                _pdata->lids[v][i].first = ui32temp;

                // nr
                file.read(reinterpret_cast<char*>(&i8temp), sizeof(std::int8_t));
                _pdata->lids[v][i].second = static_cast<char>(i8temp);
            }
        }

        file.close();

        _pdata->is_initialized = true;

        return true;
    }
  } // inline namespace cmr
} // namespace bk

