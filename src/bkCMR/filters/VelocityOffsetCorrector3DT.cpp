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

#include <bkCMR/filters/VelocityOffsetCorrector3DT.h>

#include <iostream>
#include <vector>

#include <bkCMR/dataset/FlowImage3DT.h>
#include <bk/Matrix>
#include <bk/StringUtils>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct VelocityOffsetCorrector3DT::Impl
    {
        double ivsd_static_tissue_threshold;
        unsigned int end_diastolic_time_point;
        std::vector<std::vector<Vec3d>> plane_coeffs_per_slice;
        bool is_initialized;

        Impl()
            : ivsd_static_tissue_threshold(0),
              end_diastolic_time_point(0),
              is_initialized(false)
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
    VelocityOffsetCorrector3DT::VelocityOffsetCorrector3DT() = default;
    VelocityOffsetCorrector3DT::VelocityOffsetCorrector3DT(const VelocityOffsetCorrector3DT&) = default;
    VelocityOffsetCorrector3DT::VelocityOffsetCorrector3DT(VelocityOffsetCorrector3DT&&) noexcept = default;
    VelocityOffsetCorrector3DT::~VelocityOffsetCorrector3DT() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    double VelocityOffsetCorrector3DT::ivsd_static_tissue_threshold() const
    { return _pdata->ivsd_static_tissue_threshold; }

    unsigned int VelocityOffsetCorrector3DT::end_diastolic_time_point() const
    { return _pdata->end_diastolic_time_point; }

    bool VelocityOffsetCorrector3DT::is_initialized() const
    { return _pdata->is_initialized; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    [[maybe_unused]] VelocityOffsetCorrector3DT& VelocityOffsetCorrector3DT::operator=(const VelocityOffsetCorrector3DT&) = default;
    [[maybe_unused]] VelocityOffsetCorrector3DT& VelocityOffsetCorrector3DT::operator=(VelocityOffsetCorrector3DT&&) noexcept = default;

    void VelocityOffsetCorrector3DT::set_ivsd_static_tissue_threshold(double t)
    { _pdata->ivsd_static_tissue_threshold = t; }

    void VelocityOffsetCorrector3DT::set_end_diastolic_time_point(unsigned int id)
    { _pdata->end_diastolic_time_point = id; }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    Clock VelocityOffsetCorrector3DT::init(const FlowImage3DT& ff, const DicomImage<double, 3>& ivsd)
    {
        const unsigned int numSlices = ff.geometry().size(2);

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(numSlices, ___("Analyzing velocity offsets (3D+T)"));
        #endif

        bk::Clock clock;
        clock.start();

        _pdata->plane_coeffs_per_slice.clear();
        _pdata->plane_coeffs_per_slice.resize(3); // one list of lids per flow image

        for (unsigned int v = 0; v < 3; ++v)
        { _pdata->plane_coeffs_per_slice[v].resize(numSlices); }

        #pragma omp parallel for
        for (unsigned int z = 0; z < numSlices; ++z)
        {
            for (unsigned int v = 0; v < 3; ++v)
            {
                double sx = 0.0;
                double sy = 0.0;
                double sxx = 0.0;
                double syy = 0.0;
                double sxy = 0.0;
                double sn = 0.0;

                double sxz = 0.0;
                double syz = 0.0;
                double sz = 0.0;

                for (unsigned int y = 0; y < ff.geometry().size(1); ++y)
                {
                    for (unsigned int x = 0; x < ff.geometry().size(0); ++x)
                    {
                        if (ivsd(x, y, z) > _pdata->ivsd_static_tissue_threshold)
                        { continue; }

                        sx += x;
                        sy += y;
                        sxx += x * x;
                        syy += y * y;
                        sxy += x * y;
                        sn += 1;

                        sxz += x * ff(x, y, z, _pdata->end_diastolic_time_point)[v];
                        syz += y * ff(x, y, z, _pdata->end_diastolic_time_point)[v];
                        sz += ff(x, y, z, _pdata->end_diastolic_time_point)[v];
                    } // for x
                } // for y

                Mat3d A;
                A(0, 0) = sxx;
                A(1, 0) = sxy;
                A(2, 0) = sx;
                A(0, 1) = sxy;
                A(1, 1) = syy;
                A(2, 1) = sy;
                A(0, 2) = sx;
                A(1, 2) = sy;
                A(2, 2) = sn;

                Vec3d b;
                b[0] = sxz;
                b[1] = syz;
                b[2] = sz;

                auto res = A.qr().solve(b);

                if (!res)
                { std::cerr << "VelocityOffsetCorrector3DT::init - qr.solve(b) error" << std::endl; }

                _pdata->plane_coeffs_per_slice[v][z] = res.value_or(Vec3d(0, 0, 0));
            } // for v : 0-3

            #ifdef BK_EMIT_PROGRESS
                #pragma omp critical(VelocityOffsetCorrector3DT_init)
            { prog.increment(1); }
            #endif
        } // for z

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        clock.stop();

        _pdata->is_initialized = true;

        return clock;
    }

    bool VelocityOffsetCorrector3DT::apply(FlowImage3DT& ff) const
    {
        if (!this->is_initialized())
        {
            std::cerr << "VelocityOffsetCorrector3DT::save - was not initialized!" << std::endl;
            return false;
        }

        #pragma omp parallel for
        for (unsigned int t = 0; t < ff.geometry().size(3); ++t)
        {
            for (unsigned int z = 0; z < ff.geometry().size(2); ++z)
            {
                for (unsigned int v = 0; v < 3; ++v)
                {
                    const Vec3d& planeCoeffs = _pdata->plane_coeffs_per_slice[v][z];

                    for (unsigned int y = 0; y < ff.geometry().size(1); ++y)
                    {
                        for (unsigned int x = 0; x < ff.geometry().size(0); ++x)
                        { ff(x, y, z, t)[v] -= planeCoeffs[0] * x + planeCoeffs[1] * y + planeCoeffs[2]; } // for x
                    } // for y
                } // for v : 0-3
            } // for z    
        } // for t

        return true;
    }

    //====================================================================================================
    //===== I/O
    //====================================================================================================
    bool VelocityOffsetCorrector3DT::save(std::string_view filepath) const
    {
        if (!this->is_initialized())
        {
            std::cerr << "VelocityOffsetCorrector3DT::save - was not initialized!" << std::endl;
            return false;
        }

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(2, ___("Saving velocity offset correction (3D+T)"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".voc";
        if (fname.empty())
        { fname = "veloff" + suffix; }
        else if (!string_utils::ends_with(fname, suffix))
        { fname.append(suffix); }

        //------------------------------------------------------------------------------------------------------
        // create file
        //------------------------------------------------------------------------------------------------------
        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
        if (!file.good())
        {
            #ifdef BK_EMIT_PROGRESS
            prog.set_finished();
            #endif

            return false;
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // end diastolic time point
        const std::uint32_t time_id = _pdata->end_diastolic_time_point;
        file.write(reinterpret_cast<const char*>(&time_id), sizeof(std::uint32_t));

        // ivsd threshold
        file.write(reinterpret_cast<const char*>(&_pdata->ivsd_static_tissue_threshold), sizeof(double));

        for (unsigned int v = 0; v < 3; ++v)
        {
            // number of slices
            const std::uint32_t numSlices = _pdata->plane_coeffs_per_slice[v].size();
            file.write(reinterpret_cast<const char*>(&numSlices), sizeof(std::uint32_t));

            for (unsigned int z = 0; z < numSlices; ++z)
            {
                const Vec3d& planeCoeffs = _pdata->plane_coeffs_per_slice[v][z];

                for (unsigned int k = 0; k < 3; ++k)
                {
                    double dtemp = planeCoeffs[k];
                    file.write(reinterpret_cast<const char*>(&dtemp), sizeof(double));
                }
            } // for z : numSlices
        } // for v

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool VelocityOffsetCorrector3DT::load(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(2, ___("Loading velocity offset correction (3D+T)"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        const std::string suffix = ".voc";
        if (!string_utils::ends_with(filepath, suffix))
        {
            #ifdef BK_EMIT_PROGRESS
            prog.set_finished();
            #endif

            return false;
        }

        //------------------------------------------------------------------------------------------------------
        // open file
        //------------------------------------------------------------------------------------------------------
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);
        if (!file.good())
        {
            #ifdef BK_EMIT_PROGRESS
            prog.set_finished();
            #endif

            return false;
        }

        _pdata->plane_coeffs_per_slice.clear();
        _pdata->plane_coeffs_per_slice.resize(3);

        // end diastolic time point
        std::uint32_t time_id = 0;
        file.read(reinterpret_cast< char*>(&time_id), sizeof(std::uint32_t));
        _pdata->end_diastolic_time_point = time_id;

        // ivsd threshold
        file.read(reinterpret_cast< char*>(&_pdata->ivsd_static_tissue_threshold), sizeof(double));

        for (unsigned int v = 0; v < 3; ++v)
        {
            // number of slices
            std::uint32_t numSlices = 0;
            file.read(reinterpret_cast< char*>(&numSlices), sizeof(std::uint32_t));
            _pdata->plane_coeffs_per_slice[v].resize(numSlices);

            std::vector<double> buf(numSlices * 3);
            file.read(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(double));

            unsigned int bufIdCnt = 0;

            for (unsigned int z = 0; z < numSlices; ++z)
            {
                Vec3d& planeCoeffs = _pdata->plane_coeffs_per_slice[v][z];

                for (unsigned int k = 0; k < 3; ++k)
                { planeCoeffs[k] = buf[bufIdCnt++]; }
            } // for z : numSlices
        } // for v

        file.close();

        return true;
    }
  } // inline namespace cmr
} // namespace bk


