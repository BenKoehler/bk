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

#include <algorithm>
#include <cassert>
#include <cmath>

#include <bkCMR/PressureMapImageFilter.h>

#include <bkAlgorithm/quantile.h>
#include <bk/Clock>
#include <bkCMR/FlowImage3DT.h>
#include <bkCMR/Vessel.h>
#include <bkCMR/VesselSegmentationInFlowFieldSizeImageFilter.h>
#include <bkMath/functions/list_grid_id_conversion.h>
#include <bk/Matrix>

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
    struct PressureMapImageFilter::Impl
    {
        double       density;
        double       viscosity;
        unsigned int max_iterations;
        bool         convert_to_mmhg;
        Clock        clock;

        Impl()
            : density(1060),
              viscosity(0.0035),
              max_iterations(1000),
              convert_to_mmhg(true)
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
    PressureMapImageFilter::PressureMapImageFilter() = default;
    PressureMapImageFilter::PressureMapImageFilter(const PressureMapImageFilter&) = default;
    PressureMapImageFilter::PressureMapImageFilter(PressureMapImageFilter&&) noexcept = default;
    PressureMapImageFilter::~PressureMapImageFilter() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    double PressureMapImageFilter::density() const
    { return _pdata->density; }

    double PressureMapImageFilter::viscosity() const
    { return _pdata->viscosity; }

    unsigned int PressureMapImageFilter::max_iterations() const
    { return _pdata->max_iterations; }

    bool PressureMapImageFilter::convert_to_mmhg() const
    { return _pdata->convert_to_mmhg; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    PressureMapImageFilter& PressureMapImageFilter::operator=(const PressureMapImageFilter&) = default;
    PressureMapImageFilter& PressureMapImageFilter::operator=(PressureMapImageFilter&&) noexcept = default;

    void PressureMapImageFilter::set_density(double dens)
    { _pdata->density = dens; }

    void PressureMapImageFilter::set_viscosity(double viscos)
    { _pdata->viscosity = viscos; }

    void PressureMapImageFilter::set_max_iterations(unsigned int maxIter)
    { _pdata->max_iterations = maxIter; }

    void PressureMapImageFilter::set_convert_to_mmhg(bool useMmhg)
    { _pdata->convert_to_mmhg = useMmhg; }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    std::unique_ptr<DicomImage<double, 4>> PressureMapImageFilter::apply(const FlowImage3DT& ff, const std::vector<std::reference_wrapper<const Vessel>>& vessels)
    {
        if (vessels.empty())
        { return nullptr; }

        constexpr const double PA_TO_MMHG      = 0.0075006156130264;
        constexpr const double ALPHA           = 0.5; // linear combination weight
        constexpr const double ONE_MINUS_ALPHA = 1.0 - ALPHA;
        constexpr const double ALPHA_SIXTH     = ALPHA / 6.0;

        const auto& gs    = ff.geometry().size();
        const auto& scale = ff.geometry().transformation().scale();

        auto res = std::make_unique<DicomImage<double, 4>>();
        res->set_size(gs);
        res->geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix_with_time());
        res->geometry().transformation().set_dicom_image_type_3dt();

        const unsigned int numVessels = vessels.size();

        const auto scale_spatial         = scale.sub_vector<0, 2>();
        const auto scale_spatial_twice   = scale_spatial * 2;
        const auto scale_spatial_squared = scale_spatial.sqr_cwise();

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(numVessels * (gs[0] + 10 + _pdata->max_iterations + 10), ___("Calculating relative pressure"));
        #endif

        _pdata->clock.start();

        DicomImage<Vec3d, 4> presGrad;
        presGrad.geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix_with_time());
        presGrad.set_size(gs);

        DicomImage<double, 4> pk;
        pk.geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix_with_time());
        pk.set_size(gs);

        DicomImage<double, 4> pk2;
        pk2.geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix_with_time());
        pk2.set_size(gs);

        for (unsigned int i = 0; i < numVessels; ++i)
        {
            const auto& v = vessels[i].get();
            const auto seg = VesselSegmentationInFlowFieldSizeImageFilter::apply(ff, v);

            /*
             * generate pressure gradient
             */
            #pragma omp parallel for schedule(dynamic, 1)
            for (int x = 0; x < static_cast<int>(gs[0]); ++x)
            {
                for (int y = 0; y < static_cast<int>(gs[1]); ++y)
                {
                    for (int z = 0; z < static_cast<int>(gs[2]); ++z)
                    {
                        if ((*seg)(x, y, z) == 0)
                        {
                            for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                            { presGrad(x, y, z, t).set_zero(); }

                            continue;
                        }

                        for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                        {
                            const unsigned int lid   = grid_to_list_id(gs, x, y, z, t);
                            const unsigned int lidx0 = grid_to_list_id(gs, (x - 1) % gs[0], y, z, t);
                            const unsigned int lidx1 = grid_to_list_id(gs, (x + 1) % gs[0], y, z, t);
                            const unsigned int lidy0 = grid_to_list_id(gs, x, (y - 1) % gs[1], z, t);
                            const unsigned int lidy1 = grid_to_list_id(gs, x, (y + 1) % gs[1], z, t);
                            const unsigned int lidz0 = grid_to_list_id(gs, x, y, (z - 1) % gs[2], t);
                            const unsigned int lidz1 = grid_to_list_id(gs, x, y, (z + 1) % gs[2], t);
                            const unsigned int lidt0 = grid_to_list_id(gs, x, y, z, (t - 1) % gs[3]);
                            const unsigned int lidt1 = grid_to_list_id(gs, x, y, z, (t + 1) % gs[3]);

                            /// temporal velocity derivative
                            const Vec3d dvdt    = (ff[lidt1] - ff[lidt0]) / (2 * scale[3]);

                            /// velocity 6p laplace
                            Vec3d             lapV;
                            for (unsigned int k = 0; k < 3; ++k)
                            {
                                lapV[k] = (ff[lidx0][k] + ff[lidx1][k] - 2 * ff[lid][k]) / (scale_spatial_squared[0]); // v[k] in x direction
                                lapV[k] += (ff[lidy0][k] + ff[lidy1][k] - 2 * ff[lid][k]) / (scale_spatial_squared[1]); // v[k] in y direction
                                lapV[k] += (ff[lidz0][k] + ff[lidz1][k] - 2 * ff[lid][k]) / (scale_spatial_squared[2]); // v[k] in z direction
                            }

                            // https://www.physicsforums.com/threads/notation-v-gradient-operator-v-in-navier-stokes.511659/
                            // https://www.plymouth.ac.uk/uploads/production/document/path/3/3744/PlymouthUniversity_MathsandStats_the_Laplacian.pdf
                            const Vec3d dx = (ff[lidx1] - ff[lidx0]) / scale_spatial_twice[0];
                            const Vec3d dy = (ff[lidy1] - ff[lidy0]) / scale_spatial_twice[1];
                            const Vec3d dz = (ff[lidz1] - ff[lidz0]) / scale_spatial_twice[2];

                            Vec3d vdv(0, 0, 0);

                            for (unsigned int k = 0; k < 3; ++k)
                            { vdv[k] = ff[lid].dot(Vec3d(dx[k], dy[k], dz[k])); }

                            /// Navier-Stokes
                            //presGrad[lid] = (dvdt + vdv) * (-DENSITY) + lapV * VISCOSITY;
                            presGrad[lid] = (dvdt + vdv) * _pdata->density - lapV * _pdata->viscosity;
                        } // for t: gs[3]
                    } // for z: gs[2]
                } // for y: gs[1]

                #ifdef BK_EMIT_PROGRESS
                    #pragma omp critical(PressureMapImageFilter)
                prog.increment(1);
                #endif
            } // for x: gs[0]

            /*
             * pressure calculation
             */
            #pragma omp parallel for
            for (unsigned int k = 0; k < pk.num_values(); ++k)
            {
                pk[k]  = 0;
                pk2[k] = 0;
            }

            #ifdef BK_EMIT_PROGRESS
            prog.increment(10);
            #endif

            DicomImage<double, 4>* read  = nullptr;
            DicomImage<double, 4>* write = nullptr;

            //double delta = 100; //The maximum difference between after and before (two iterations)
            //for (; iter < MAX_ITERATION && delta > EPSILON; ++iter)
            unsigned int iter = 0;
            for (; iter < _pdata->max_iterations; ++iter)
            {
                //delta = 0;

                if (iter % 2 == 0)
                {
                    read  = &pk;
                    write = &pk2;
                }
                else
                {
                    read  = &pk2;
                    write = &pk;
                }

                #pragma omp parallel for schedule(dynamic, 1)
                for (int x = 0; x < static_cast<int>(gs[0]); ++x)
                {
                    for (int y = 0; y < static_cast<int>(gs[1]); ++y)
                    {
                        for (int z = 0; z < static_cast<int>(gs[2]); ++z)
                        {
                            if ((*seg)(x, y, z) == 0)
                            { continue; }

                            for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                            {
                                const unsigned int lid   = grid_to_list_id(gs, x, y, z, t);
                                const unsigned int lidx0 = grid_to_list_id(gs, (x - 1) % gs[0], y, z, t);
                                const unsigned int lidx1 = grid_to_list_id(gs, (x + 1) % gs[0], y, z, t);
                                const unsigned int lidy0 = grid_to_list_id(gs, x, (y - 1) % gs[1], z, t);
                                const unsigned int lidy1 = grid_to_list_id(gs, x, (y + 1) % gs[1], z, t);
                                const unsigned int lidz0 = grid_to_list_id(gs, x, y, (z - 1) % gs[2], t);
                                const unsigned int lidz1 = grid_to_list_id(gs, x, y, (z + 1) % gs[2], t);

                                (*write)[lid] = ONE_MINUS_ALPHA * (*read)[lid] + ALPHA_SIXTH * ((*read)[lidx0] + (*read)[lidx1] + (*read)[lidy0] + (*read)[lidy1] + (*read)[lidz0] + (*read)[lidz1] + scale[0] * (presGrad[lidx1][0] - presGrad[lidx0][0]) + scale[1] * (presGrad[lidy1][1] - presGrad[lidy0][1]) + scale[2] * (presGrad[lidz1][2] - presGrad[lidz0][2]));

                                //Remember that if X1, r.grad(p) is -r * grad(p) for each component (that why the minus)
                                //Visually we can say that the "sum" of the two gradient will be 0
                                //And that the 1/6 is here because we have 6 times pk.

                                //#pragma omp critical
                                //{ delta = std::max(std::abs((*write)[lid] - (*read)[lid]), delta); };
                            } // for t: gs[3]
                        } // for z: gs[2]
                    } // for y: gs[1]
                } // for x: gs[0]

                #ifdef BK_EMIT_PROGRESS
                prog.increment(1);
                #endif
            } // for iter: iterations

            #ifdef BK_EMIT_PROGRESS
            prog.increment(_pdata->max_iterations - iter);
            #endif

            /// 3x3x3 binomial smoothing
            //constexpr const unsigned int iterations_binom = 0;
            constexpr const unsigned int iterations_binom = 1;
            constexpr const double       norm             = 64.0;
            constexpr const double       one              = 1.0 / norm;
            constexpr const double       two              = 2.0 / norm;
            constexpr const double       four             = 4.0 / norm;
            constexpr const double       eight            = 8.0 / norm;
            const int                    stride_x         = stride_of_dim(gs, 0, 4);
            const int                    stride_y         = stride_of_dim(gs, 1, 4);
            const int                    stride_z         = stride_of_dim(gs, 2, 4);
            const int                    stride_t         = stride_of_dim(gs, 3, 4);
            DicomImage<double, 4>* read_binom  = write;
            DicomImage<double, 4>* write_binom = read;

            for (unsigned int iter_binom = 0; iter_binom < iterations_binom; ++iter_binom)
            {
                if (iter_binom % 2 == 0)
                {
                    read_binom  = write;
                    write_binom = read;
                }
                else
                {
                    read_binom  = read;
                    write_binom = write;
                }

                #pragma omp parallel for schedule(dynamic, 1)
                for (int x = 1; x < static_cast<int>(gs[0]) - 1; ++x)
                {
                    for (int y = 1; y < static_cast<int>(gs[1]) - 1; ++y)
                    {
                        for (int z = 1; z < static_cast<int>(gs[2]) - 1; ++z)
                        {
                            if ((*seg)(x, y, z) == 0)
                            { continue; }

                            unsigned int lidz0  = grid_to_list_id(gs, x, y, z, 0);
                            unsigned int lidzm1 = lidz0 - stride_z;
                            unsigned int lidzp1 = lidz0 + stride_z;

                            for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                            {
                                (*write_binom)[lidz0] = one * ((*read_binom)[lidzm1 - stride_x - stride_y] + (*read_binom)[lidzm1 + stride_x - stride_y] + (*read_binom)[lidzm1 - stride_x + stride_y] + (*read_binom)[lidzm1 + stride_x + stride_y] + (*read_binom)[lidzp1 - stride_x - stride_y] + (*read_binom)[lidzp1 + stride_x - stride_y] + (*read_binom)[lidzp1 - stride_x + stride_y] + (*read_binom)[lidzp1 + stride_x + stride_y]);
                                (*write_binom)[lidz0] += two * ((*read_binom)[lidzm1 - stride_x] + (*read_binom)[lidz0 - stride_x - stride_y] + (*read_binom)[lidz0 + stride_x - stride_y] + (*read_binom)[lidz0 - stride_x + stride_y] + (*read_binom)[lidz0 + stride_x + stride_y] + (*read_binom)[lidzp1 - stride_x] + (*read_binom)[lidzp1 + stride_x] + (*read_binom)[lidzp1 - stride_y] + (*read_binom)[lidzp1 + stride_y] + (*read_binom)[lidzm1 + stride_x] + (*read_binom)[lidzm1 - stride_y] + (*read_binom)[lidzm1 + stride_y]);
                                (*write_binom)[lidz0] += four * ((*read_binom)[lidzm1] + (*read_binom)[lidz0 - stride_x] + (*read_binom)[lidz0 + stride_x] + (*read_binom)[lidz0 - stride_y] + (*read_binom)[lidz0 + stride_y] + (*read_binom)[lidzp1]);
                                (*write_binom)[lidz0] += eight * (*read_binom)[lidz0];

                                lidz0 += stride_t;
                                lidzm1 += stride_t;
                                lidzp1 += stride_t;
                            } // for t: gs[3]
                        } // for z: gs[2]
                    } // for y: gs[1]
                } // for x: gs[0]

            } // for iter_binom

            #ifdef BK_EMIT_PROGRESS
            prog.increment(5);
            #endif

            // ===================================================================================================
            // determine quantiles

            std::vector<double> values;
            values.reserve(gs[0] * gs[1] * gs[2] * gs[3] / 4);
            for (int x = 0; x < static_cast<int>(gs[0]); ++x)
            {
                for (int y = 0; y < static_cast<int>(gs[1]); ++y)
                {
                    for (int z = 0; z < static_cast<int>(gs[2]); ++z)
                    {
                        if ((*seg)(x, y, z) == 0)
                        { continue; }

                        unsigned int lid = grid_to_list_id(gs, x, y, z, 0);

                        for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                        {
                            values.push_back((*write_binom)[lid]);
                            lid += stride_t;
                        } // for t: gs[3]
                    } // for z: gs[2]
                } // for y: gs[1]
            } // for x: gs[0]

            std::sort(values.begin(), values.end());

            const double qlow  = bk::quantile(values.begin(), values.end(), 0.01);
            const double qhigh = bk::quantile(values.begin(), values.end(), 0.99);


            // - clamp values to quantiles
            // - determine average

            double       avg = 0;
            unsigned int cnt = 0;

            #pragma omp parallel for reduction(+:cnt)
            for (unsigned int x = 0; x < gs[0]; ++x)
            {
                for (unsigned int y = 0; y < gs[1]; ++y)
                {
                    for (unsigned int z = 0; z < gs[2]; ++z)
                    {
                        if ((*seg)(x, y, z) == 0)
                        { continue; }

                        unsigned int lid = grid_to_list_id(gs, x, y, z, 0);

                        for (unsigned int t = 0; t < gs[3]; ++t)
                        {
                            (*write_binom)[lid] = std::clamp((*write_binom)[lid], qlow, qhigh);

                            #pragma omp critical(PressureMapImageFilter)
                            { avg += (*write_binom)[lid]; };

                            lid += stride_t;
                        } // for t: gs[3]

                        cnt += gs[3];
                    } // for z: gs[2]
                } // for y: gs[1]
            } // for x: gs[0]

            avg /= cnt != 0 ? cnt : 1;


            // set mean as reference value

            #pragma omp parallel for
            for (int x = 0; x < static_cast<int>(gs[0]); ++x)
            {
                for (int y = 0; y < static_cast<int>(gs[1]); ++y)
                {
                    for (int z = 0; z < static_cast<int>(gs[2]); ++z)
                    {
                        if ((*seg)(x, y, z) == 0)
                        { continue; }

                        unsigned int lid = grid_to_list_id(gs, x, y, z, 0);

                        for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                        {
                            (*write_binom)[lid] -= avg;
                            lid += stride_t;
                        } // for t: gs[3]
                    } // for z: gs[2]
                } // for y: gs[1]
            } // for x: gs[0]

            // ===================================================================================================

            /// copy result

            #pragma omp parallel for schedule(dynamic, 1)
            for (int x = 0; x < static_cast<int>(gs[0]); ++x)
            {
                for (int y = 0; y < static_cast<int>(gs[1]); ++y)
                {
                    for (int z = 0; z < static_cast<int>(gs[2]); ++z)
                    {
                        /*
                         * segmentation + 1 (dilation)
                         */
                        bool isValid = false;

                        for (int dx = -1; dx <= +1 && !isValid; ++dx)
                        {
                            for (int dy = -1; dy <= +1 && !isValid; ++dy)
                            {
                                for (int dz = -1; dz <= +1 && !isValid; ++dz)
                                { isValid = (*seg)(x + dx, y + dy, z + dz) != 0 || isValid; } // for dz
                            } // for dy
                        } // for dx

                        if (!isValid)
                        { continue; }

                        /*
                         * copy
                         */
                        unsigned int lid = grid_to_list_id(gs, x, y, z, 0);

                        for (int t = 0; t < static_cast<int>(gs[3]); ++t)
                        {
                            (*res)[lid] = PA_TO_MMHG * (*write_binom)[lid];
                            lid += stride_t;
                        } // for t: gs[3]
                    } // for z: gs[2]
                } // for y: gs[1]
            } // for x: gs[0]

            #ifdef BK_EMIT_PROGRESS
            prog.increment(5);
            #endif
        } // for v: vessels

        _pdata->clock.stop();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return res;
    }
  } // inline namespace cmr
} // namespace bk
