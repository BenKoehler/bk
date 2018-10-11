#include "CenterlineExtractor.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include <bkAlgorithm/smooth.h>
#include <bkCMR/Dataset.h>
#include <bkCMR/Vessel.h>
#include <bk/Clock>
#include <bkDataset/attributes/attribute_info.h>
#include <bk/Matrix>
#include <bkMath/functions/list_grid_id_conversion.h>

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
    struct CenterlineExtractor::Impl
    {
        unsigned int image_upscale;
        int distance_penalty_exponent;
        unsigned int num_smooth_iterations;
        unsigned int smooth_kernel_size;
        double smooth_relaxation;

        Impl()
            : image_upscale(3),
              distance_penalty_exponent(5),
              num_smooth_iterations(500),
              smooth_kernel_size(3),
              smooth_relaxation(0.1)
        { /*do nothing*/ }

        Impl(const Impl&) = default;
        Impl(Impl&&) noexcept = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    CenterlineExtractor::CenterlineExtractor() = default;
    CenterlineExtractor::CenterlineExtractor(const CenterlineExtractor&) = default;
    CenterlineExtractor::CenterlineExtractor(CenterlineExtractor&&) noexcept = default;
    CenterlineExtractor::~CenterlineExtractor() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    unsigned int CenterlineExtractor::image_upscale() const
    { return _pdata->image_upscale; }

    int CenterlineExtractor::distance_penalty_exponent() const
    { return _pdata->distance_penalty_exponent; }

    unsigned int CenterlineExtractor::num_smooth_iterations() const
    { return _pdata->num_smooth_iterations; }

    unsigned int CenterlineExtractor::smooth_kernel_size() const
    { return _pdata->smooth_kernel_size; }

    double CenterlineExtractor::smooth_relaxation() const
    { return _pdata->smooth_relaxation; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    void CenterlineExtractor::set_image_upscale(unsigned int image_upscale)
    { _pdata->image_upscale = std::max(image_upscale, 1U); }

    void CenterlineExtractor::set_distance_penalty_exponent(int distance_penalty_exponent)
    { _pdata->distance_penalty_exponent = distance_penalty_exponent; }

    void CenterlineExtractor::set_num_smooth_iterations(unsigned int num_smooth_iterations)
    { _pdata->num_smooth_iterations = std::max(num_smooth_iterations, 1U); }

    void CenterlineExtractor::set_smooth_kernel_size(unsigned int smooth_kernel_size)
    { _pdata->smooth_kernel_size = std::max(smooth_kernel_size, 1U); }

    void CenterlineExtractor::set_smooth_relaxation(double smooth_relaxation)
    { _pdata->smooth_relaxation = smooth_relaxation; }

    CenterlineExtractor& CenterlineExtractor::operator=(const CenterlineExtractor&) = default;
    CenterlineExtractor& CenterlineExtractor::operator=(CenterlineExtractor&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    std::pair<std::vector<Line3D> /*centerlines*/, bool /*success*/> CenterlineExtractor::extract_centerlines(TriangularMesh3D& mesh, const bk::DicomImage<double,3>& seg, unsigned int seed_meshVertexId, const std::vector<unsigned int>& target_meshVertexIds) const
    {
        std::vector<Line<3>> result_centerlines;

        if (target_meshVertexIds.empty())
        {
            std::cerr << "centerline input incomplete (no target points)!" << std::endl;
            return std::make_pair(result_centerlines, false);
        }

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(5 + target_meshVertexIds.size(), ___("Extracting centerline(s)"));
        #endif

        bk::Clock clock;
        clock.start();

        const Vec3ui size = seg.geometry().size();
        const Vec3ui dmUpscale = MatrixFactory::Constant_Vec_3D<unsigned int>(_pdata->image_upscale);

        //------------------------------------------------------------------------------------------------------
        // calculate distance map w.r.t. vessel segmentation boundary
        //------------------------------------------------------------------------------------------------------
        // create distance map
        const Vec3ui dmSize = size.mult_cwise(dmUpscale);
        bk::DicomImage<double, 3> dm;
        dm.set_size(dmSize);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // create upscaled world matrix
        auto wmat = seg.geometry().transformation().world_matrix();
        wmat.col_ref(0) /= dmUpscale[0];
        wmat.col_ref(1) /= dmUpscale[1];
        wmat.col_ref(2) /= dmUpscale[2];
        dm.geometry().transformation().set_world_matrix(wmat);
        dm.geometry().transformation().set_dicom_image_type_3d();

        //double maxdst = dm.num_values();
        double maxdst = 0;
        mesh.geometry().construct_kd_tree();

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        //maxdst = 0;

        #ifdef BK_EMIT_PROGRESS
        Progress& prog_dmap = bk_progress.emplace_task(dm.num_values() + dmSize[0], ___("Intravascular distance map"));
        #endif

        // calc distance map by finding closest point on surface mesh
        #pragma omp parallel for schedule(dynamic, 1) reduction(max:maxdst)
        for (unsigned int x = 0; x < dmSize[0]; ++x)
        {
            double tempMaxdst = 0;

            for (unsigned int y = 0; y < dmSize[1]; ++y)
            {
                for (unsigned int z = 0; z < dmSize[2]; ++z)
                {
                    // inside vessel segmentation?
                    if (seg(x / dmUpscale[0], y / dmUpscale[1], z / dmUpscale[2]) == 0)
                    { continue; }

                    // distance map point in world coordinates
                    const auto pt = dm.geometry().transformation().to_world_coordinates(Vec3d(x, y, z));
                    // distance to closest mesh point
                    const double d = mesh.geometry().closest_point(pt).distance_to_query;
                    dm(x, y, z) = d;

                    // determine global max for later normalization
                    tempMaxdst = std::max(tempMaxdst, d);
                }
            }

            maxdst = std::max(maxdst, tempMaxdst);

            #ifdef BK_EMIT_PROGRESS
            #pragma omp critical(prog_dmap)
            { prog_dmap.increment(1); }
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // - normalize distance map
        // - apply exponential behavior
        #pragma omp parallel for
        for (unsigned int i = 0; i < dm.num_values(); ++i)
        {
            dm[i] = (maxdst - dm[i]) / maxdst;
            dm[i] = std::pow(dm[i], _pdata->distance_penalty_exponent);

            #ifdef BK_EMIT_PROGRESS
            #pragma omp critical(prog_dmap)
            { prog_dmap.increment(1); }
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog_dmap.set_finished();
        #endif

        const double maxdst_old = maxdst;
        maxdst = 1;

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        //------------------------------------------------------------------------------------------------------
        //
        //------------------------------------------------------------------------------------------------------
        auto clseed_voxel = seg.geometry().transformation().to_object_coordinates(mesh.geometry().point(seed_meshVertexId));
        bk::CartesianImage<Vec3i, 3> parent;
        parent.set_size(dmSize);

        bk::CartesianImage<double, 3> cost;
        cost.set_size(dmSize);

        std::vector<std::vector<std::vector<bool>>> visited(dmSize[0], std::vector<std::vector<bool>>(dmSize[1], std::vector<bool>(dmSize[2], false)));

        for (unsigned int i = 0; i < 3; ++i)
        {
            clseed_voxel[i] = std::max(clseed_voxel[i], 0.0);
            clseed_voxel[i] = std::min(static_cast<unsigned int>(std::round(clseed_voxel[i])), size[i] - 1);
        }

        clseed_voxel = clseed_voxel.mult_cwise(dmUpscale);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        for (unsigned int tid = 0; tid < target_meshVertexIds.size(); ++tid)
        {
            #ifdef BK_EMIT_PROGRESS
            Progress& prog_cl = bk_progress.emplace_task(19, ___("Calculating centerline @0 of @1",  tid+1, target_meshVertexIds.size()));
            #endif

            auto cltarget_voxel = seg.geometry().transformation().to_object_coordinates(mesh.geometry().point(target_meshVertexIds[tid]));

            for (unsigned int i = 0; i < 3; ++i)
            {
                cltarget_voxel[i] = std::max(cltarget_voxel[i], 0.0);
                cltarget_voxel[i] = std::min(static_cast<unsigned int>(std::round(cltarget_voxel[i])), size[i] - 1);
            }

            cltarget_voxel = cltarget_voxel.mult_cwise(dmUpscale);

            if (dm(cltarget_voxel) == maxdst)
            { dm(cltarget_voxel) -= 0.1; }

            //------------------------------------------------------------------------------------------------------
            // determine max score path from seed to target
            //------------------------------------------------------------------------------------------------------
            #pragma omp parallel for
            for (unsigned int i = 0; i < cost.num_values(); ++i)
            {
                cost[i] = std::numeric_limits<double>::max();
                parent[i].set_constant(-1);
            }

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(1);
            #endif

            #pragma omp parallel for
            for (unsigned int x = 0; x < dmSize[0]; ++x)
            {
                for (unsigned int y = 0; y < dmSize[1]; ++y)
                {
                    for (unsigned int z = 0; z < dmSize[2]; ++z)
                    { visited[x][y][z] = false; }
                }
            }

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(1);
            #endif

            const unsigned int lid_seed = grid_to_list_id(dmSize, clseed_voxel, 3);
            const unsigned int lid_target = grid_to_list_id(dmSize, cltarget_voxel, 3);
            std::queue<unsigned int> active;
            active.push(lid_seed);

            cost[lid_seed] = dm[lid_seed];
            visited[clseed_voxel[0]][clseed_voxel[1]][clseed_voxel[2]] = true;

            Vec3i neighbor;

            while (!active.empty())
            {
                const unsigned int lid = active.front();
                active.pop();
                const Vec3i current = list_to_grid_id(dmSize, lid);
                const double cost_current = cost[lid];

                for (int dx = -1; dx <= 1; ++dx)
                {
                    neighbor[0] = current[0] + dx;

                    if (neighbor[0] < 0 || neighbor[0] >= static_cast<int>(dmSize[0]))
                    { continue; }

                    for (int dy = -1; dy <= 1; ++dy)
                    {
                        neighbor[1] = current[1] + dy;

                        if (neighbor[1] < 0 || neighbor[1] >= static_cast<int>(dmSize[1]))
                        { continue; }

                        for (int dz = -1; dz <= 1; ++dz)
                        {
                            if (dx == 0 && dy == 0 && dz == 0)
                            { continue; }

                            neighbor[2] = current[2] + dz;

                            // neighbor is within image bounds
                            if (neighbor[2] < 0 || neighbor[2] >= static_cast<int>(dmSize[2]))
                            { continue; }

                            const unsigned int lid_neighbor = grid_to_list_id(dmSize, neighbor, 3);

                            // neighbor is in segmentation
                            if (dm[lid_neighbor] != maxdst)
                            {
                                const double cost_neighbor = dm[lid_neighbor];
                                const double cost_accum = cost_current + cost_neighbor;

                                if (cost_accum < cost[lid_neighbor])
                                {
                                    cost[lid_neighbor] = cost_accum;
                                    parent[lid_neighbor] = current;

                                    if (visited[neighbor[0]][neighbor[1]][neighbor[2]] == 0)
                                    { active.push(lid_neighbor); }
                                }

                                visited[neighbor[0]][neighbor[1]][neighbor[2]] = 1;
                            }
                        } // for dz
                    } // for dy
                } // for dx
            } // while

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(5);
            #endif

            /*
             * was the target point not reached?
             */
            //if (parent[lid_target] == Vec3i::Constant(-1))
            if (parent[lid_target] == MatrixFactory::Constant_Vec_3D<int>(-1))
            {
                for (unsigned int x = 0; x < dmSize[0]; ++x)
                {
                    for (unsigned int y = 0; y < dmSize[1]; ++y)
                    {
                        for (unsigned int z = 0; z < dmSize[2]; ++z)
                        {
                            if (visited[x][y][z] == false)
                            { continue; }

                            bool addToActive = false;

                            for (int dx = -1; dx <= 1; ++dx)
                            {
                                neighbor[0] = x + dx;

                                if (neighbor[0] < 0 || neighbor[0] >= static_cast<int>(dmSize[0]))
                                { continue; }

                                for (int dy = -1; dy <= 1; ++dy)
                                {
                                    neighbor[1] = y + dy;

                                    if (neighbor[1] < 0 || neighbor[1] >= static_cast<int>(dmSize[1]))
                                    { continue; }

                                    for (int dz = -1; dz <= 1; ++dz)
                                    {
                                        if (dx == 0 && dy == 0 && dz == 0)
                                        { continue; }

                                        neighbor[2] = z + dz;

                                        // neighbor is within image bounds
                                        if (neighbor[2] < 0 || neighbor[2] >= static_cast<int>(dmSize[2]))
                                        { continue; }

                                        if (visited[neighbor[0]][neighbor[1]][neighbor[2]] == false)
                                        {
                                            addToActive = true;
                                            break;
                                        }
                                    } // for dz
                                } // for dy
                            } // for dx

                            if (addToActive)
                            { active.push(grid_to_list_id(dmSize, x, y, z)); }
                        }
                    }
                }

                while (!active.empty())
                {
                    const unsigned int lid = active.front();
                    active.pop();
                    const Vec3i current = list_to_grid_id(dmSize, lid);
                    const double cost_current = cost[lid];

                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        neighbor[0] = current[0] + dx;

                        if (neighbor[0] < 0 || neighbor[0] >= static_cast<int>(dmSize[0]))
                        { continue; }

                        for (int dy = -1; dy <= 1; ++dy)
                        {
                            neighbor[1] = current[1] + dy;

                            if (neighbor[1] < 0 || neighbor[1] >= static_cast<int>(dmSize[1]))
                            { continue; }

                            for (int dz = -1; dz <= 1; ++dz)
                            {
                                if (dx == 0 && dy == 0 && dz == 0)
                                { continue; }

                                neighbor[2] = current[2] + dz;

                                // neighbor is within image bounds
                                if (neighbor[2] < 0 || neighbor[2] >= static_cast<int>(dmSize[2]))
                                { continue; }

                                const unsigned int lid_neighbor = grid_to_list_id(dmSize, neighbor, 3);

                                // neighbor is in segmentation
                                const double cost_neighbor = dm[lid_neighbor];
                                const double cost_accum = cost_current + cost_neighbor;

                                if (cost_accum < cost[lid_neighbor])
                                {
                                    cost[lid_neighbor] = cost_accum;
                                    parent[lid_neighbor] = current;

                                    if (visited[neighbor[0]][neighbor[1]][neighbor[2]] == 0)
                                    { active.push(lid_neighbor); }
                                }

                                visited[neighbor[0]][neighbor[1]][neighbor[2]] = 1;
                            } // for dz
                        } // for dy
                    } // for dx
                } // while
            }

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(5);
            #endif

            //------------------------------------------------------------------------------------------------------
            // trace parents back starting from target
            //------------------------------------------------------------------------------------------------------
            std::vector<bk::Vec3d> centerline;
            centerline.reserve(1'000);

            Vec3i current = cltarget_voxel;

            while (current != clseed_voxel)
            {
                centerline.push_back(seg.geometry().transformation().to_world_coordinates(Vec3d(current).div_cwise(dmUpscale)));
                current = parent(current);
            }

            centerline.push_back(seg.geometry().transformation().to_world_coordinates(Vec3d(clseed_voxel).div_cwise(dmUpscale)));

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(1);
            #endif

            const unsigned int N = centerline.size();

            //------------------------------------------------------------------------------------------------------
            // smooth centerline points
            //------------------------------------------------------------------------------------------------------
            //bk::smooth_lambda_mu(centerline.begin(), centerline.end(), centerline.begin(), _pdata->num_smooth_iterations, _pdata->smooth_kernel_size, _pdata->smooth_relaxation, _pdata->smooth_relaxation, Vec3d::Zero());
            bk::smooth_lambda_mu(centerline.begin(), centerline.end(), _pdata->num_smooth_iterations, _pdata->smooth_kernel_size, _pdata->smooth_relaxation, _pdata->smooth_relaxation, MatrixFactory::Zero_Vec_3D<double>());

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(2);
            #endif

            //------------------------------------------------------------------------------------------------------
            // replace vessel centerline
            //------------------------------------------------------------------------------------------------------
            result_centerlines.emplace_back(Line<3>());
            auto& vcl = result_centerlines.back();
            vcl.geometry().set_num_points(N);
            std::vector<double>& rad = vcl.template add_point_attribute_vector<attribute_info::radius()>();

            /*
             * right now the centerline is sorted from target to seed
             *    -> add in reversed order
             */
            #pragma omp parallel for
            for (unsigned int i = 0; i < N; ++i)
            { vcl.geometry().point(i) = centerline[N - 1 - i]; }

            //vcl.geometry().construct_kd_tree(); // not necessary here

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(1);
            #endif

            for (unsigned int i = 0; i < N; ++i)
            {
                const double radius = (maxdst - std::pow(dm(dm.geometry().transformation().to_object_coordinates(centerline[N - 1 - i])), 1.0 / _pdata->distance_penalty_exponent)) * maxdst_old;
                rad[i] = radius;
            }

            #ifdef BK_EMIT_PROGRESS
            prog_cl.increment(1);
            #endif

            vcl.calc_consistent_local_coordinate_systems();

            #ifdef BK_EMIT_PROGRESS
            prog_cl.set_finished();
            #endif

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        } // for tid: num target ids

        clock.stop();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        std::cout << "Extracted " << target_meshVertexIds.size() << " centerlines in " << clock.time_in_sec() << " s (" << clock.time_in_milli_sec() << " ms)" << std::endl;

        return std::make_pair(result_centerlines, true);
    }
  } // inline namespace cmr
} // namespace bk



