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

#include <bkCMR/filters/FlowJetExtractor.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include <bk/Clock>
#include <bk/Line>
#include <bk/Matrix>
#include <bk/StringUtils>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

#include <bkMath/functions/list_grid_id_conversion.h>
#include <bkCMR/dataset/Dataset.h>
#include <bkCMR/dataset/FlowImage3DT.h>
#include <bkCMR/dataset/FlowJet.h>
#include <bkCMR/dataset/FlowJetPoint.h>
#include <bkCMR/dataset/MeasuringPlane.h>
#include <bkCMR/dataset/Vessel.h>

namespace bk
{
  inline namespace cmr
  {
    struct FlowJetExtractor::Impl
    {
        double maxVelocity_clamp_quantile_p;
        double percentaged_velocity_threshold;
        unsigned int measuring_plane_size;
        double equiDistance_in_mm_of_measuring_planes;
        unsigned int num_smooth_iterations;
        unsigned int smooth_kernel_size;

        Impl()
            : maxVelocity_clamp_quantile_p(0.99),
              percentaged_velocity_threshold(0.75),
              measuring_plane_size(50),
              equiDistance_in_mm_of_measuring_planes(2.5),
              num_smooth_iterations(15),
              smooth_kernel_size(3)
        { /* do nothing */ }

        Impl(const Impl&) = default;
        Impl(Impl&&) noexcept = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    FlowJetExtractor::FlowJetExtractor() = default;
    FlowJetExtractor::FlowJetExtractor(const FlowJetExtractor&) = default;
    FlowJetExtractor::FlowJetExtractor(FlowJetExtractor&&) noexcept = default;
    FlowJetExtractor::~FlowJetExtractor() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    double FlowJetExtractor::maxVelocity_clamp_quantile() const
    { return _pdata->maxVelocity_clamp_quantile_p; }

    double FlowJetExtractor::percentaged_velocity_threshold() const
    { return _pdata->percentaged_velocity_threshold; }

    unsigned int FlowJetExtractor::measuring_plane_gridSize() const
    { return _pdata->measuring_plane_size; }

    double FlowJetExtractor::equiDistance_in_mm_of_measuring_planes() const
    { return _pdata->equiDistance_in_mm_of_measuring_planes; }

    unsigned int FlowJetExtractor::num_smooth_iterations() const
    { return _pdata->num_smooth_iterations; }

    unsigned int FlowJetExtractor::smooth_kernel_size() const
    { return _pdata->smooth_kernel_size; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    void FlowJetExtractor::set_maxVelocity_clamp_quantile(double percentage_between_zero_and_one)
    { _pdata->maxVelocity_clamp_quantile_p = std::clamp(percentage_between_zero_and_one, static_cast<double>(0), static_cast<double>(1)); }

    void FlowJetExtractor::set_percentaged_velocity_threshold(double percentage_between_zero_and_one)
    { _pdata->percentaged_velocity_threshold = std::clamp(percentage_between_zero_and_one, static_cast<double>(0), static_cast<double>(1)); }

    void FlowJetExtractor::set_measuring_plane_gridSize(unsigned int gs)
    { _pdata->measuring_plane_size = std::max(gs, 1U); }

    void FlowJetExtractor::set_equiDistance_in_mm_of_measuring_planes(double mm)
    { _pdata->equiDistance_in_mm_of_measuring_planes = std::max(mm, 1e-10); }

    void FlowJetExtractor::set_num_smooth_iterations(unsigned int it)
    { _pdata->num_smooth_iterations = std::max(it, 1U); }

    void FlowJetExtractor::set_smooth_kernel_size(unsigned int ks)
    { _pdata->smooth_kernel_size = std::max(ks, 1U); }

    FlowJetExtractor& FlowJetExtractor::operator=(const FlowJetExtractor&) = default;
    FlowJetExtractor& FlowJetExtractor::operator=(FlowJetExtractor&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void FlowJetExtractor::calculate_flowjets(Dataset& ds) const
    {
        if (!ds.is_flow_image_3dt_loaded())
        { ds.load_flow_image_3dt(); }

        if (!ds.is_flow_image_3dt_loaded())
        {
            std::cerr << "FlowJetExtractor::calculate_flowjets - dataset has no flow image" << std::endl;
            return;
        }

        FlowImage3DT* ff = ds.flow_image_3dt();

        const unsigned int numVessels = ds.num_vessels();
        std::vector<bool> hasFlowJets(numVessels, false);

        for (unsigned int i = 0; i < numVessels; ++i)
        {
            if (std::filesystem::exists(ds.filepath_flowjet_of_vessel(ds.vessel(i))))
            { hasFlowJets[i] = true; }
        }

        unsigned int cnt = 0;
        for (bool b: hasFlowJets)
        {
            if (b)
            { ++cnt; }
        }

        if (cnt == numVessels)
        { return; }

        const unsigned int numTimes = ff->geometry().size(3);
        const double msPerTime = ff->geometry().transformation().scale(3);

        #ifdef BK_EMIT_PROGRESS
        unsigned int numClPointsTotal = 0;
        for (unsigned int vid = 0; vid < numVessels; ++vid)
        {
            if (hasFlowJets[vid])
            { continue; }

            const auto& v = *ds.vessel(vid);

            for (unsigned int clid = 0; clid < v.centerlines().size(); ++clid)
            { numClPointsTotal += v.centerlines()[clid].geometry().num_points(); }
        }

        Progress& prog = bk_progress.emplace_task(numClPointsTotal, ___("Extracting flow jet(s)"));
        unsigned int progCnt = 0;
        #endif

        const double sqEquiDist = _pdata->equiDistance_in_mm_of_measuring_planes;
        const Vec3ui size(_pdata->measuring_plane_size, _pdata->measuring_plane_size, numTimes);
        const unsigned int numel = _pdata->measuring_plane_size * _pdata->measuring_plane_size * numTimes;
        bk::Clock clock;
        clock.start();

        MeasuringPlane mp;
        mp.set_size(_pdata->measuring_plane_size, _pdata->measuring_plane_size, numTimes);

        std::vector<double> mpvelo(numel, 0);

        for (unsigned int vid = 0; vid < numVessels; ++vid)
        {
            if (hasFlowJets[vid])
            { continue; }

            Vessel* vptr = ds.vessel(vid);
            if (vptr == nullptr)
            { continue; }

            Vessel& v = *vptr;

            v.clear_flowjets();
            const unsigned int numCenterlines = v.num_centerlines();

            mp.set_vesselID(vid);

            for (unsigned int fjid = 0; fjid < numCenterlines; ++fjid)
            { v.flowjets().emplace_back(); }

            for (unsigned int clid = 0; clid < numCenterlines; ++clid)
            {
                const Line3D& cl = v.centerlines()[clid];

                /*
                 * count num flow jet positions
                 */
                const unsigned int numPoints = cl.geometry().num_points();
                double currentSqDist = sqEquiDist;
                unsigned int numFlowJetPositions = 0;

                for (unsigned int pid = 1; pid < numPoints; ++pid)
                {
                    if (currentSqDist < sqEquiDist)
                    {
                        currentSqDist += cl.geometry().point(pid).distance(cl.geometry().point(pid - 1));
                        continue;
                    }
                    else
                    {
                        while (currentSqDist >= sqEquiDist)
                        { currentSqDist -= sqEquiDist; }
                    }

                    ++numFlowJetPositions;
                }

                currentSqDist = sqEquiDist;
                FlowJet& flowjet = v.flowjets()[clid];
                flowjet.resize(numFlowJetPositions, numTimes);

                bool firstFlowJetPosition = true;
                unsigned int flowJetPositionID = 0;

                for (unsigned int pid = 1; pid < numPoints; ++pid)
                {
                    if (currentSqDist < sqEquiDist)
                    {
                        currentSqDist += cl.geometry().point(pid).distance(cl.geometry().point(pid - 1));

                        #ifdef BK_EMIT_PROGRESS
                        ++progCnt;

                        if (pid == numPoints - 1)
                        {
                            prog.increment(progCnt);
                            progCnt = 0;
                        }
                        #endif

                        continue;
                    }
                    else
                    {
                        while (currentSqDist >= sqEquiDist)
                        { currentSqDist -= sqEquiDist; }
                    }

                    if (firstFlowJetPosition)
                    { firstFlowJetPosition = false; }
                    else
                    { ++flowJetPositionID; }

                    Mat3d lcs = cl.local_coordinate_system_at_point(pid);

                    const Vec3d nx = lcs.col(0);;
                    const Vec3d ny = lcs.col(1);;
                    const Vec3d nz = lcs.col(2);;
                    const Vec3d& clpoint = cl.geometry().point(pid);
                    const double radius = cl.point_attribute_value<attribute_info::radius()>(pid);
                    const double scl = 2.5 * radius / _pdata->measuring_plane_size;

                    mp.set_diameter_mm(radius);
                    mp.geometry().transformation().set_center(clpoint);
                    mp.geometry().transformation().set_scale(Vec3d(scl, scl, msPerTime));
                    mp.geometry().transformation().set_nx(nx);
                    mp.geometry().transformation().set_ny(ny);
                    mp.geometry().transformation().set_nz(nz);

                    mp.sample_segmentation_from_vessel_mesh(v);

                    const auto& seg = mp.segmentation_cross_section();
                    //seg.connected_component_analysis_keep_largest_fast3D_internal(); // todo remove (should not be necessary)

                    mp.sample_from_flow_field(*ds.flow_image_3dt());

                    // --------------- veloq for each time separately

                    std::vector<double> veloq_max(numTimes, 0);
                    std::vector<double> veloq_min(numTimes, 0);
                    double maxVelo = 0; // for velocity normalization

                    #pragma omp parallel for reduction(max: maxVelo)
                    for (unsigned int t = 0; t < numTimes; ++t)
                    {
                        std::vector<double> velo_abs;
                        velo_abs.reserve(_pdata->measuring_plane_size * _pdata->measuring_plane_size);

                        for (unsigned int x = 0; x < _pdata->measuring_plane_size; ++x)
                        {
                            for (unsigned int y = 0; y < _pdata->measuring_plane_size; ++y)
                            {
                                if (seg(x, y, 0) == 0)
                                { continue; }

                                const unsigned int lid = grid_to_list_id(size, x, y, t);
                                const double velo = mp[lid].norm();

                                if (mp[lid].dot(nz) > 0) // forward flow
                                {
                                    mpvelo[lid] = velo;
                                    velo_abs.push_back(velo);
                                }
                                else //if (mp[lid].dot(nz) <= 0) // backward flow
                                { mpvelo[lid] = 0; }
                            } // for y : _pdata->measuring_plane_size
                        } // for x : _pdata->measuring_plane_size

                        velo_abs.erase(std::remove_if(velo_abs.begin(), velo_abs.end(), [](double x)
                        { return x == 0; }), velo_abs.end());

                        std::sort(velo_abs.begin(), velo_abs.end());

                        veloq_max[t] = velo_abs.empty() ? 0 : velo_abs[static_cast<unsigned int>(std::round(_pdata->maxVelocity_clamp_quantile_p * (velo_abs.size() - 1)))];
                        veloq_min[t] = velo_abs.empty() ? 0 : velo_abs[static_cast<unsigned int>(std::round((1.0 - _pdata->maxVelocity_clamp_quantile_p) * (velo_abs.size() - 1)))];

                        maxVelo = std::max(maxVelo, veloq_max[t]);
                    } // for t : numTimes

                    #pragma omp parallel for
                    for (unsigned int t = 0; t < numTimes; ++t)
                    {
                        veloq_max[t] /= maxVelo;
                        veloq_min[t] /= maxVelo;
                    }

                    #pragma omp parallel for
                    for (unsigned int i = 0; i < mpvelo.size(); ++i)
                    { mpvelo[i] /= maxVelo; }

                    /*
                     * - calc weighted center
                     * - calc weighted PCA
                     */
                    auto weight_fn_jet = [](double x) -> double
                    { return std::pow(x, 10); };

                    auto weight_fn_area = [&](double x, int t) -> double
                    { return std::max(0.0, x / veloq_max[t] - _pdata->percentaged_velocity_threshold) != 0 ? 1 : 0; };

                    for (int t = 0; t < static_cast<int>(numTimes); ++t)
                    {
                        Vec3d center_jet(0, 0, 0);
                        double center_jet_w = 0;
                        Vec3d center_area(0, 0, 0);
                        double center_area_w = 0;

                        for (int x = 0; x < static_cast<int>(_pdata->measuring_plane_size); ++x)
                        {
                            for (int y = 0; y < static_cast<int>(_pdata->measuring_plane_size); ++y)
                            {
                                if (seg(x, y, 0) == 0)
                                { continue; }

                                const unsigned int lid = grid_to_list_id(size, x, y, t);
                                const Vec3d wp = mp.geometry().transformation().to_world_coordinates(x, y, 0).sub_vector<0, 2>();

                                // forward
                                const double w_jet = weight_fn_jet(mpvelo[lid]);

                                center_jet += wp * w_jet;
                                center_jet_w += w_jet;

                                const double w_area = weight_fn_area(mpvelo[lid], t);

                                if (w_area != 0)
                                {
                                    center_area += wp * w_area;
                                    center_area_w += w_area;
                                }
                            } // for y
                        } // for x


                        if (center_jet_w == 0 || center_area_w == 0)
                        {
                            center_jet = clpoint;
                            center_jet_w = 1;
                            center_area = clpoint;
                            center_area_w = 1;
                        }

                        const Vec3d center_jet_weighted = center_jet / center_jet_w;
                        Vec3d center_area_weighted = center_area / center_area_w;

                        Mat3d A;
                        A.set_zero();

                        #pragma omp parallel for schedule(dynamic, 1)
                        for (int x = 0; x < static_cast<int>(_pdata->measuring_plane_size); ++x)
                        {
                            for (int y = 0; y < static_cast<int>(_pdata->measuring_plane_size); ++y)
                            {
                                if (seg(x, y, 0) == 0)
                                { continue; }

                                const unsigned int lid = bk::grid_to_list_id(size, x, y, t);

                                const double w_area = weight_fn_area(mpvelo[lid], t);

                                if (w_area != 0)
                                {
                                    const Vec3d wp = mp.geometry().transformation().to_world_coordinates(x, y, 0).sub_vector<0, 2>();
                                    const Vec3d diff = wp - center_area_weighted;
                                    const Mat3d addA = (diff * diff.transpose()) * w_area;

                                    #pragma omp critical
                                    { A += addA; }
                                }
                            } // for y
                        } // for x

                        A /= center_area_w != 0 ? center_area_w : 1;

                        const auto eig = A.eigenanalysis_symmetric();
                        const auto eigval = eig.eigenvalues();
                        const auto eigvec = eig.eigenvectors();

                        // ----------
                        /*
                         * use measuring planes' local coordinate system instead of PCA eigenvectors
                         */
                        std::pair<double, Vec3d> dir0;
                        std::pair<double, Vec3d> dir1;
                        if (std::abs(eigvec.value().col(0).angle(nx)) < std::abs(eigvec.value().col(0).angle(ny)))
                        {
                            dir0 = std::make_pair(eigval.value()[0] / 2, nx);
                            dir1 = std::make_pair(eigval.value()[1] / 2, ny);
                        }
                        else
                        {
                            dir0 = std::make_pair(eigval.value()[1] / 2, nx);
                            dir1 = std::make_pair(eigval.value()[0] / 2, ny);
                        }

                        /*
                         * use PCA eigenvectors
                         */
                        /*std::pair<double, Vec3d> dir0;
                        std::pair<double, Vec3d> dir1;
                        if (std::abs(eigvec.col(0).angle(nx)) < std::abs(eigvec.col(0).angle(ny)))
                        {
                            dir0 = std::make_pair(eigval[0], eigvec.col(0));
                            dir1 = std::make_pair(eigval[1], eigvec.col(1));
                        }
                        else
                        {
                            dir0 = std::make_pair(eigval[1], eigvec.col(0));
                            dir1 = std::make_pair(eigval[0], eigvec.col(1));
                        }*/
                        // ----------

                        /*
                         * area must not be larger than vessel
                         */
                        Vec3d areaToVesselCenter = mp.geometry().transformation().center() - center_area_weighted;
                        double ndist[2] = {std::abs(areaToVesselCenter.dot(dir0.second)), std::abs(areaToVesselCenter.dot(dir1.second))};
                        double maxEig0 = radius - ndist[0];
                        dir0.first = std::min(maxEig0, dir0.first);

                        double maxEig1 = radius - ndist[1];
                        dir1.first = std::min(maxEig1, dir1.first);

                        /*
                         * area must contain flow jet
                         */
                        /*const Vec3d jetToVesselCenter = mp.geometry().transformation().center() - center_jet_weighted;
                        const Vec3d areaCenterToJet = center_jet_weighted - center_area_weighted;
                        Vec3d tempd0 = dir0.second;
                        if (jetToVesselCenter.dot(tempd0) > 0)
                        { tempd0 *= -1; }

                        const double jetd0 = jetToVesselCenter.dot(tempd0);
                        const double aread0 = areaToVesselCenter.dot(tempd0);
                        const double areatojetd0 = std::abs(areaCenterToJet.dot(tempd0));

                        if (areatojetd0 > dir0.first) // jet is not in area
                        { center_area_weighted += tempd0 * std::abs(std::abs(jetd0 - aread0) - dir0.first); }

                        Vec3d tempd1 = dir1.second;
                        if (jetToVesselCenter.dot(tempd1) > 0)
                        { tempd1 *= -1; }

                        const double jetd1 = jetToVesselCenter.dot(tempd1);
                        const double aread1 = areaToVesselCenter.dot(tempd1);
                        const double areatojetd1 = std::abs(areaCenterToJet.dot(tempd1));

                        if (areatojetd1 > dir1.first) // jet is not in area
                        { center_area_weighted += tempd1 * std::abs(std::abs(jetd1 - aread1) - dir1.first); }*/

                        /*
                         * area must not be larger than vessel
                         */
                        /*areaToVesselCenter = mp.geometry().transformation().center() - center_area_weighted;
                        ndist[0] = std::abs(areaToVesselCenter.dot(dir0.second));
                        ndist[1] = std::abs(areaToVesselCenter.dot(dir1.second));

                        maxEig0 = radius - ndist[0];
                        dir0.first = std::min(maxEig0, dir0.first);

                        maxEig1 = radius - ndist[1];
                        dir1.first = std::min(maxEig1, dir1.first);*/

                        // add to flow jet

                        FlowJetPoint fjp;
                        fjp.vessel_center = mp.geometry().transformation().center();
                        fjp.vessel_radius = radius;
                        fjp.peak_velocity = veloq_max[t];
                        fjp.peak_velocity_position = center_jet_weighted;
                        fjp.local_coord_sys_x = nx;
                        fjp.local_coord_sys_y = ny;
                        fjp.area_center = center_area_weighted;
                        fjp.area_dir_x = dir0.second;
                        fjp.area_dir_y = dir1.second;
                        fjp.area_radius_x = dir0.first;
                        fjp.area_radius_y = dir1.first;

                        flowjet.point(flowJetPositionID, t) = std::move(fjp);
                    } // for t: numTimes

                    #ifdef BK_EMIT_PROGRESS
                    ++progCnt;
                    prog.increment(progCnt);

                    progCnt = 0;
                    #endif
                } // for pid : numPoints

                flowjet.smooth_spatial_binomial(_pdata->num_smooth_iterations, _pdata->smooth_kernel_size);
            } // for clid : numCenterlines
        } // for vid : numVessels

        clock.stop();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        unsigned int numFlowJets = 0;
        for (unsigned int i = 0; i < numVessels; ++i)
        {
            if (hasFlowJets[i])
            { continue; }

            const Vessel& v = *ds.vessel(i);
            numFlowJets += v.num_flowjets();
        }

        std::cout << "Extracted " << numFlowJets << " flow jet(s) in " << clock.time_in_sec() << " s" << std::endl;
    }
  } // inline namespace cmr
} // namespace bk