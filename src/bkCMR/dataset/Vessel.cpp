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

#include <bkCMR/dataset/Vessel.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

#include <bk/BitVector>
#include <bk/Clock>
#include <bk/Line>
#include <bk/Image>
#include <bk/Mesh>
#include <bk/ThreadPool>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

#include <bkCMR/dataset/FlowJet.h>
#include <bkCMR/dataset/FlowJetPoint.h>
#include <bkCMR/dataset/MeasuringPlane.h>
#include <bkCMR/rotation_angle_2d.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct Vessel::Impl
    {
        std::string name;
        segmentation3d_type seg3;
        std::pair<Segmentation3DInfo_, unsigned int> seg3info;
        std::vector<unsigned int> seg3insideids;
        std::vector<unsigned int> seg3outsideids;
        VesselSemantic semantic;
        bk::TriangularMesh3D mesh;
        std::vector<bk::Line3D> pathlines;
        unsigned int centerline_seed_id;
        std::vector<unsigned int> centerline_target_ids;
        std::vector<Line3D> centerlines;
        std::vector<LandMark> land_marks;
        std::vector<MeasuringPlane> measuring_planes;
        std::vector<FlowJet> flowjets;

        Impl()
            : name(""),
              semantic(VesselSemantic_None),
              centerline_seed_id(0)
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
    Vessel::Vessel() = default;

    Vessel::Vessel(const Vessel&) = default;

    Vessel::Vessel(Vessel&&) noexcept = default;
    Vessel::~Vessel() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const std::string& Vessel::name() const
    { return _pdata->name; }

    VesselSemantic Vessel::semantic() const
    { return _pdata->semantic; }

    bool Vessel::is_semantic(VesselSemantic sem) const
    { return _pdata->semantic & sem; }

    bool Vessel::is_semantic_aorta() const
    { return is_semantic(VesselSemantic_LeftHeart_Aorta); }

    bool Vessel::is_semantic_left_ventricle() const
    { return is_semantic(VesselSemantic_LeftHeart_Ventricle); }

    bool Vessel::is_semantic_left_atrium() const
    { return is_semantic(VesselSemantic_LeftHeart_Atrium); }

    bool Vessel::is_semantic_pulmonary_veins() const
    { return is_semantic(VesselSemantic_LeftHeart_PulmonaryVeins); }

    bool Vessel::is_semantic_left_heart() const
    { return is_semantic_aorta() || is_semantic_left_ventricle() || is_semantic_left_atrium() || is_semantic_pulmonary_veins(); }

    bool Vessel::is_semantic_pulmonary_artery() const
    { return is_semantic(VesselSemantic_RightHeart_PulmonaryArtery); }

    bool Vessel::is_semantic_right_ventricle() const
    { return is_semantic(VesselSemantic_RightHeart_Ventricle); }

    bool Vessel::is_semantic_right_atrium() const
    { return is_semantic(VesselSemantic_RightHeart_Atrium); }

    bool Vessel::is_semantic_vena_cava() const
    { return is_semantic(VesselSemantic_RightHeart_VenaCava); }

    bool Vessel::is_semantic_right_heart() const
    { return is_semantic_pulmonary_artery() || is_semantic_right_ventricle() || is_semantic_right_atrium() || is_semantic_vena_cava(); }

    bool Vessel::has_land_marks() const
    { return !_pdata->land_marks.empty(); }

    bool Vessel::has_land_mark(LandMarkSemantic sem) const
    {
        return std::find_if(_pdata->land_marks.begin(), _pdata->land_marks.end(), [&](const LandMark& lm)
        { return lm.semantic == sem; }) != _pdata->land_marks.end();
    }

    int Vessel::id_of_land_mark(LandMarkSemantic sem) const
    {
        const auto it = std::find_if(_pdata->land_marks.begin(), _pdata->land_marks.end(), [&](const LandMark& lm)
        { return lm.semantic == sem; });

        return it != _pdata->land_marks.end() ? std::abs(std::distance(_pdata->land_marks.begin(), it)) : -1;
    }

    unsigned int Vessel::num_land_marks() const
    { return _pdata->land_marks.size(); }

    const std::vector<LandMark>& Vessel::land_marks() const
    { return _pdata->land_marks; }

    const LandMark& Vessel::land_mark(unsigned int id) const
    { return _pdata->land_marks[id]; }

    bool Vessel::has_segmentation3D() const
    { return _pdata->seg3.num_values() > 1; }

    auto Vessel::segmentation3D()& -> segmentation3d_type&
    { return _pdata->seg3; }

    auto Vessel::segmentation3D() const& -> const segmentation3d_type&
    { return _pdata->seg3; }

    auto Vessel::segmentation3D()&& -> segmentation3d_type&&
    { return std::move(_pdata->seg3); }

    const std::pair<Segmentation3DInfo_, unsigned int>& Vessel::segmentation3D_info() const
    { return _pdata->seg3info; }

    std::vector<unsigned int>& Vessel::segmentation3D_inside_ids()
    { return _pdata->seg3insideids; }

    const std::vector<unsigned int>& Vessel::segmentation3D_inside_ids() const
    { return _pdata->seg3insideids; }

    std::vector<unsigned int>& Vessel::segmentation3D_outside_ids()
    { return _pdata->seg3outsideids; }

    const std::vector<unsigned int>& Vessel::segmentation3D_outside_ids() const
    { return _pdata->seg3outsideids; }

    bool Vessel::has_mesh() const
    { return _pdata->mesh.geometry().num_points() > 1 && _pdata->mesh.topology().num_cells() > 1; }

    bk::TriangularMesh3D& Vessel::mesh()&
    { return _pdata->mesh; }

    const bk::TriangularMesh3D& Vessel::mesh() const&
    { return _pdata->mesh; }

    bk::TriangularMesh3D&& Vessel::mesh()&&
    { return std::move(_pdata->mesh); }

    std::pair<double, double> Vessel::min_max_scalar_mesh_point_attribute(std::string_view attribute_name) const
    {
        std::pair minmax(std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest());

        if (_pdata->mesh.point_attribute_map().has_attribute(attribute_name))
        {
            const std::vector<double>& avec = _pdata->mesh.point_attribute_vector_of_type<double>(attribute_name);
            const auto itMinMax = std::minmax_element(avec.begin(), avec.end());

            minmax.first = std::min(minmax.first, *itMinMax.first);
            minmax.second = std::max(minmax.second, *itMinMax.second);
        }

        return minmax;
    }

    bool Vessel::has_pathlines() const
    { return num_pathlines() != 0; }

    unsigned int Vessel::num_pathlines() const
    { return _pdata->pathlines.size(); }

    const std::vector<bk::Line3D>& Vessel::pathlines() const
    { return _pdata->pathlines; }

    std::vector<bk::Line3D>& Vessel::pathlines()
    { return _pdata->pathlines; }

    std::pair<double, double> Vessel::min_max_scalar_pathline_point_attribute(std::string_view attribute_name) const
    {
        std::pair minmax(std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest());

        #pragma omp parallel for
        for (unsigned int i = 0; i < num_pathlines(); ++i)
        {
            const Line3D& p = _pdata->pathlines[i];

            if (p.point_attribute_map().has_attribute(attribute_name))
            {
                const std::vector<double>& avec = p.point_attribute_vector_of_type<double>(attribute_name);
                const auto itMinMax = std::minmax_element(avec.begin(), avec.end());

                #pragma omp critical(min_max_scalar_pathline_point_attribute)
                {
                    minmax.first = std::min(minmax.first, *itMinMax.first);
                    minmax.second = std::max(minmax.second, *itMinMax.second);
                }
            }
        }

        return minmax;
    }

    std::vector<Line3D>& Vessel::centerlines()&
    { return _pdata->centerlines; }

    const std::vector<Line3D>& Vessel::centerlines() const&
    { return _pdata->centerlines; }

    std::vector<Line3D>&& Vessel::centerlines()&&
    { return std::move(_pdata->centerlines); }

    unsigned int Vessel::num_centerlines() const
    { return _pdata->centerlines.size(); }

    bool Vessel::has_centerline_ids() const
    { return !_pdata->centerline_target_ids.empty(); }

    unsigned int Vessel::centerline_seed_id() const
    { return _pdata->centerline_seed_id; }

    const std::vector<unsigned int>& Vessel::centerline_target_ids() const
    { return _pdata->centerline_target_ids; }

    bool Vessel::has_centerlines() const
    { return !_pdata->centerlines.empty(); }

    std::pair<int /*centerlineId*/, bk::KDPointInfo<bk::Vec3d>> Vessel::closest_centerline_and_point_id(const bk::Vec3d& pt)
    {
        for (unsigned int centerlineId = 0; centerlineId < num_centerlines(); ++centerlineId)
        {
            Line3D& cl = _pdata->centerlines[centerlineId];

            #pragma omp critical
            {
                if (!cl.geometry().has_kdtree())
                { cl.geometry().construct_kd_tree(); }
            };
        } // for centerlineId

        int closestCenterlineId = -1;
        bk::KDPointInfo<bk::Vec3d> closest;
        closest.distance_to_query = std::numeric_limits<double>::max();

        for (unsigned int clid = 0; clid < num_centerlines(); ++clid)
        {
            Line3D& cl = _pdata->centerlines[clid];
            bk::KDPointInfo<bk::Vec3d> cp = cl.geometry().closest_point(pt);

            if (cp.distance_to_query < closest.distance_to_query)
            {
                closest = std::move(cp);
                closestCenterlineId = clid;
            }
        } // for clid

        return std::make_pair(closestCenterlineId, std::move(closest));
    }

    std::vector<MeasuringPlane>& Vessel::measuring_planes()
    { return _pdata->measuring_planes; }

    const std::vector<MeasuringPlane>& Vessel::measuring_planes() const
    { return _pdata->measuring_planes; }

    bool Vessel::has_measuring_planes() const
    { return num_measuring_planes() != 0; }

    unsigned int Vessel::num_measuring_planes() const
    { return _pdata->measuring_planes.size(); }

    std::pair<double, double> Vessel::min_max_scalar_measuring_plane_point_attribute(std::string_view attribute_name) const
    {
        std::pair minmax(std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest());

        #pragma omp parallel for
        for (unsigned int i = 0; i < num_measuring_planes(); ++i)
        {
            const MeasuringPlane& mp = _pdata->measuring_planes[i];

            if (mp.point_attribute_map().has_attribute(attribute_name))
            {
                const NDVector<double>& avec = mp.point_attribute_vector_of_type<double>(attribute_name);
                const auto itMinMax = std::minmax_element(avec.begin(), avec.end());

                #pragma omp critical(min_max_scalar_pathline_point_attribute)
                {
                    minmax.first = std::min(minmax.first, *itMinMax.first);
                    minmax.second = std::max(minmax.second, *itMinMax.second);
                }
            }
        }

        return minmax;
    }

    std::vector<FlowJet>& Vessel::flowjets()
    { return _pdata->flowjets; }

    const std::vector<FlowJet>& Vessel::flowjets() const
    { return _pdata->flowjets; }

    unsigned int Vessel::num_flowjets() const
    { return _pdata->flowjets.size(); }

    bool Vessel::has_flowjets() const
    { return num_flowjets() != 0; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    void Vessel::clear()
    {
        clear_semantic();
        clear_mesh();
        clear_segmentation();
        clear_pathlines();
        clear_centerlines();
        clear_centerline_ids();
        clear_measuring_planes();
        clear_flowjets();
    }

    void Vessel::clear_semantic()
    { _pdata->semantic = VesselSemantic_None; }

    void Vessel::clear_segmentation()
    {
        //_pdata->seg3            = std::make_unique<segmentation3d_type>();
        _pdata->seg3.set_size(1, 1, 1);
        _pdata->seg3info.first = Segmentation3DInfo_Magnitude4DTMIP;
        _pdata->seg3info.second = 0;
        _pdata->seg3insideids.clear();
        _pdata->seg3outsideids.clear();
    }

    void Vessel::clear_mesh()
    { _pdata->mesh.clear(); }

    void Vessel::clear_pathlines()
    { _pdata->pathlines.clear(); }

    void Vessel::clear_centerlines()
    { _pdata->centerlines.clear(); }

    void Vessel::clear_centerline_ids()
    {
        _pdata->centerline_seed_id = 0;
        _pdata->centerline_target_ids.clear();
    }

    void Vessel::clear_measuring_planes()
    {
        //_pdata->measuring_planes.clear();
        // todo
    }

    void Vessel::clear_flowjets()
    { _pdata->flowjets.clear(); }

    void Vessel::set_name(std::string_view name)
    { _pdata->name = name; }

    void Vessel::set_name_from_semantic()
    { _pdata->name = Name_from_semantic(_pdata->semantic); }

    std::string Vessel::Name_from_semantic(VesselSemantic s)
    {
        std::string name = "";

        auto append_to_name = [&](bool semantic, std::string_view str)
        {
            if (semantic)
            {
                if (!name.empty())
                { name += "-"; }

                name += str;
            }
        };

        append_to_name(s & VesselSemantic_LeftHeart_Aorta, "Aorta");
        append_to_name(s & VesselSemantic_LeftHeart_Ventricle, "LeftVentricle");
        append_to_name(s & VesselSemantic_LeftHeart_Atrium, "LeftAtrium");
        append_to_name(s & VesselSemantic_LeftHeart_PulmonaryVeins, "PulmonaryVeins");
        append_to_name(s & VesselSemantic_RightHeart_PulmonaryArtery, "PulmonaryArtery");
        append_to_name(s & VesselSemantic_RightHeart_Ventricle, "RightVentricle");
        append_to_name(s & VesselSemantic_RightHeart_Atrium, "RightAtrium");
        append_to_name(s & VesselSemantic_RightHeart_VenaCava, "VenaCava");

        return name;
    }

    void Vessel::set_semantic(VesselSemantic sem)
    { _pdata->semantic = sem; }

    void Vessel::add_semantic(VesselSemantic sem)
    { _pdata->semantic = static_cast<VesselSemantic>(static_cast<unsigned int>(_pdata->semantic) | static_cast<unsigned int>(sem)); }

    void Vessel::add_land_mark(LandMarkSemantic sem, unsigned int centerline_id, unsigned int point_id)
    {
        assert(centerline_id < num_centerlines());
        assert(point_id < _pdata->centerlines[centerline_id].geometry().num_points());

        auto it = std::find_if(_pdata->land_marks.begin(), _pdata->land_marks.end(), [&](const LandMark& l)
        { return l.semantic == sem; });

        if (it == _pdata->land_marks.end())
        {
            LandMark lm;
            lm.semantic = sem;
            lm.centerline_id = centerline_id;
            lm.point_id = point_id;

            _pdata->land_marks.emplace_back(std::move(lm));
        }
        else
        {
            it->point_id = point_id;
            it->centerline_id = centerline_id;
        }
    }

    void Vessel::set_seg3d_was_performed_on_magnitude_TMIP()
    {
        _pdata->seg3info.first = Segmentation3DInfo_Magnitude4DTMIP;
        _pdata->seg3info.second = 0;
    }

    void Vessel::set_seg3d_was_performed_on_LPC()
    {
        _pdata->seg3info.first = Segmentation3DInfo_LPC;
        _pdata->seg3info.second = 0;
    }

    void Vessel::set_seg3d_was_performed_on_IVSD()
    {
        _pdata->seg3info.first = Segmentation3DInfo_IVSD;
        _pdata->seg3info.second = 0;
    }

    void Vessel::set_seg3d_was_performed_on_3D_anatomical_image(unsigned int id)
    {
        _pdata->seg3info.first = Segmentation3DInfo_Anatomical3D;
        _pdata->seg3info.second = id;
    }

    void Vessel::set_seg3d_was_performed_on_4D_anatomical_image_TMIP(unsigned int id)
    {
        _pdata->seg3info.first = Segmentation3DInfo_Anatomical4DTMIP;
        _pdata->seg3info.second = id;
    }

    void Vessel::set_seg3d_was_performed_on_4D_signal_intensity_image_TMIP()
    {
        _pdata->seg3info.first = Segmentation3DInfo_SignalIntensity4DTMIP;
        _pdata->seg3info.second = 0;
    }

    void Vessel::set_centerline_seed_id(unsigned int seedid)
    { _pdata->centerline_seed_id = seedid; }

    void Vessel::set_centerline_target_ids(std::vector<unsigned int> targetids)
    { _pdata->centerline_target_ids = targetids; }

    void Vessel::add_centerline_target_id(unsigned int targetid)
    {
        _pdata->centerline_target_ids.push_back(targetid);

        std::sort(_pdata->centerline_target_ids.begin(), _pdata->centerline_target_ids.end());
        _pdata->centerline_target_ids.erase(std::unique(_pdata->centerline_target_ids.begin(), _pdata->centerline_target_ids.end()), _pdata->centerline_target_ids.end());
    }

    void Vessel::add_centerline(const Line3D& cl)
    { _pdata->centerlines.push_back(cl); }

    void Vessel::add_centerline(Line3D&& cl)
    { _pdata->centerlines.emplace_back(std::move(cl)); }

    void Vessel::add_centerlines(std::vector<Line3D>::iterator first, std::vector<Line3D>::iterator last)
    { _pdata->centerlines.insert(_pdata->centerlines.end(), first, last); }

    Vessel& Vessel::operator=(const Vessel&) = default;
    Vessel& Vessel::operator=(Vessel&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    #if defined(BK_LIB_VTK_AVAILABLE) && defined(BK_LIB_VMTK_AVAILABLE)
    void CMRVessel::extract_centerlines()
  {
      if (mesh() != nullptr)
      { _pdata->centerlines.add_lines(std::move(mesh()->extract_centerline(_pdata->centerline_seed_id, _pdata->centerline_target_ids))); }
  }
    #endif

    //void Vessel::calc_pathlines_angles_to_centerline()
    //{
    //    using pathline_type = pathlineset_type::line_type;
    //
    //    if (!_pdata->centerlines.has_lines() || !_pdata->pathlines.has_lines())
    //    { return; }
    //
    //    Clock clock;
    //    clock.start();
    //
    //    const unsigned int numLines       = num_pathlines();
    //    unsigned int       numPointsTotal = 0;
    //    for (unsigned int  pid            = 0; pid < numLines; ++pid)
    //    { numPointsTotal += _pdata->pathlines.line(pid)->geometry().num_points(); }
    //
    //    #ifdef BK_EMIT_PROGRESS
    //    Progress& prog = bk_progress.emplace_task(0, numPointsTotal, ___("analyzing pathlines/centerlines relations"));
    //    #endif
    //
    //    /*
    //     * construct centerline kd trees
    //     */
    //    #pragma omp parallel for
    //    for (unsigned int cid = 0; cid < _pdata->centerlines.num_lines(); ++cid)
    //    { _pdata->centerlines.line(cid)->geometry().construct_kd_tree(); }
    //
    //    /*
    //     * calculate attributes
    //     */
    //    #pragma omp parallel for schedule(dynamic, 10)
    //    for (unsigned int pid = 0; pid < numLines; ++pid)
    //    {
    //        pathline_type* pln = _pdata->pathlines.line(pid);
    //        const unsigned int nPoints = pln->geometry().num_points();
    //
    //        for (unsigned int vid = 0; vid < nPoints; ++vid)
    //        {
    //            //const auto plnvertex = pln->geometry().point_in_world_coordinates(vid);
    //            const Vec3d& plnvertex = pln->geometry().point(vid); // has no transformation
    //            auto plntangent = pln->tangent_at_point(vid);
    //            plntangent.normalize_internal();
    //
    //            double                                                                     angle = 0;
    //            decltype(_pdata->centerlines.line(0)->geometry().closest_point(plnvertex)) cp;
    //            cp.distance_to_query = std::numeric_limits<double>::max();
    //            unsigned int clid = 0;
    //
    //            for (unsigned int cid = 0; cid < _pdata->centerlines.num_lines(); ++cid)
    //            {
    //                auto current_cp = _pdata->centerlines.line(cid)->geometry().closest_point(plnvertex);
    //
    //                if (current_cp.distance_to_query < cp.distance_to_query)
    //                {
    //                    cp   = std::move(current_cp);
    //                    clid = cid;
    //
    //                    const auto* clntangent_ptr = _pdata->centerlines.line(cid)->point_attributes().get_local_coord_sys3d_z_value(cp.point_id);
    //
    //                    if (clntangent_ptr != nullptr)
    //                    { angle = clntangent_ptr->dot(plntangent); }
    //                    else
    //                    {
    //                        auto clntangent = _pdata->centerlines.line(cid)->tangent_at_point(cp.point_id);
    //                        clntangent.normalize_internal();
    //                        angle = clntangent.dot(plntangent);
    //                    }
    //                } // if closer
    //            } // for cid: num centerlines
    //
    //            const double deg = bk::radians_to_degrees(std::acos(std::abs(angle)));
    //
    //            pln->point_attributes().set_relative_flow_direction_value(vid, angle);
    //            pln->point_attributes().set_angle_to_centerline_value(vid, deg);
    //
    //            /*
    //             * determine turning direction
    //             */
    //            if (vid == nPoints - 1)
    //            { pln->point_attributes().set_rotation_direction_value(vid, 0.0); }
    //            else //if (vid < nPoints - 1)
    //            {
    //                const auto * cl      = _pdata->centerlines.line(clid);
    //                const auto & clpoint = cl->geometry().point(cp.point_id);
    //                const Vec3d* nx_ptr  = cl->point_attributes().get_local_coord_sys3d_x_value(cp.point_id);
    //                const Vec3d* ny_ptr  = cl->point_attributes().get_local_coord_sys3d_y_value(cp.point_id);
    //
    //                if (nx_ptr != nullptr && ny_ptr != nullptr)
    //                {
    //                    const double rotangle_deg = bk::rotation_angle_deg_2d_of_3d_points_projected_onto_plane(plnvertex, pln->geometry().point(vid + 1), clpoint, *nx_ptr, *ny_ptr);
    //                    pln->point_attributes().set_rotation_direction_value(vid, rotangle_deg);
    //                }
    //                else
    //                { pln->point_attributes().set_rotation_direction_value(vid, 0.0); }
    //            } // if (vid < nPoints - 1)
    //        } // for vid: num points
    //
    //        /*
    //         * smooth attributes
    //         */
    //        pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::relative_flow_direction_str(), 25, 0.25, 0.25);
    //        pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::angle_to_centerline_str(), 25, 0.25, 0.25);
    //        pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::rotation_direction_str(), 25, 0.25, 0.25);
    //        //
    //            #ifdef BK_EMIT_PROGRESS
    //                #pragma omp critical
    //        { prog.increment(nPoints); }
    //            #endif
    //    } // for pid: num pathlines
    //
    //    #ifdef BK_EMIT_PROGRESS
    //    prog.set_finished();
    //    #endif
    //
    //    /*
    //     * clamp rotation direction values
    //     */
    //    std::vector<double> rotdirvalues;
    //    rotdirvalues.reserve(numPointsTotal);
    //
    //    for (unsigned int pid = 0; pid < numLines; ++pid)
    //    {
    //        pathline_type* pln = _pdata->pathlines.line(pid);
    //        for (unsigned int vid = 0; vid < pln->geometry().num_points(); ++vid)
    //        {
    //            auto rotptr = pln->point_attributes().get_rotation_direction_value(vid);
    //            if (rotptr != nullptr)
    //            {
    //                const double rotdir = static_cast<double>(*rotptr);
    //                rotdirvalues.push_back(rotdir);
    //            }
    //        }
    //    }
    //
    //    #if defined(__GNUC__) && defined(_OPENMP)
    //    __gnu_parallel
    //    #else
    //    std
    //    #endif
    //    ::sort(rotdirvalues.begin(), rotdirvalues.end());
    //
    //    if (!rotdirvalues.empty())
    //    {
    //        const double rotdirclampmin = rotdirvalues[2 * rotdirvalues.size() / 100]; // todo options
    //        const double rotdirclampmax = rotdirvalues[98 * rotdirvalues.size() / 100]; // todo options
    //        const double rotdirclamp    = std::max(std::abs(rotdirclampmin), std::abs(rotdirclampmax));
    //
    //        #pragma omp parallel for
    //        for (unsigned int pid = 0; pid < numLines; ++pid)
    //        {
    //            pathline_type* pln = _pdata->pathlines.line(pid);
    //            for (unsigned int vid = 0; vid < pln->geometry().num_points(); ++vid)
    //            {
    //                const double rotdir = static_cast<double>(*pln->point_attributes().get_rotation_direction_value(vid));
    //                pln->point_attributes().set_rotation_direction_value(vid, std::min(rotdirclamp, std::max(-rotdirclamp, rotdir)));
    //            }
    //        }
    //    }
    //
    //    clock.stop();
    //    std::cout << "Calculated Advanced Attributes (" << numLines << " lines; " << numPointsTotal << " points) in " << clock.time_in_sec() << " seconds" << std::endl;
    //}





    //====================================================================================================
    //===== I/O
    //====================================================================================================
    bool Vessel::save_mesh(std::string_view filepath) const
    { return _pdata->mesh.save(filepath); }

    bool Vessel::load_mesh(std::string_view filepath)
    { return _pdata->mesh.load(filepath); }

    bool Vessel::save_segmentation3D(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(5, ___("Saving 3D segmentation"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".seg3";
        if (fname.empty())
        { fname = "segmentation3d" + suffix; }
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

        // temp
        std::uint16_t ui16temp = 0;
        std::uint32_t ui32temp = 0;
        double dtemp = 0;

        // grid size
        const auto& gs = _pdata->seg3.geometry().size();
        for (unsigned int i = 0; i < 3; ++i)
        {
            ui16temp = static_cast<std::uint16_t>(gs[i]);
            file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        }

        // world matrix
        const auto& wmat = _pdata->seg3.geometry().transformation().world_matrix();
        for (unsigned int i = 0; i < 16; ++i)
        {
            dtemp = static_cast<double>(wmat[i]);
            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
        }

        // segmentation bits
        BitVectorX segbits;
        segbits.set_size(_pdata->seg3.num_values());
        for (unsigned int i = 0; i < _pdata->seg3.num_values(); ++i)
        { segbits[i] = _pdata->seg3.operator[](i) != 0 ? 1 : 0; }
        segbits.save(file);

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // inside ids
        ui32temp = static_cast<std::uint32_t>(_pdata->seg3insideids.size());
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        for (unsigned int i = 0; i < _pdata->seg3insideids.size(); ++i)
        {
            ui32temp = static_cast<std::uint32_t>(_pdata->seg3insideids[i]);
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // outside ids
        ui32temp = static_cast<std::uint32_t>(_pdata->seg3outsideids.size());
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        for (unsigned int i = 0; i < _pdata->seg3outsideids.size(); ++i)
        {
            ui32temp = static_cast<std::uint32_t>(_pdata->seg3outsideids[i]);
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // info
        ui16temp = static_cast<std::uint16_t>(_pdata->seg3info.first);
        file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));

        ui16temp = static_cast<std::uint16_t>(_pdata->seg3info.second);
        file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));

        // semantic
        ui32temp = static_cast<std::uint32_t>(_pdata->semantic);
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_segmentation3D(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(5, ___("Loading 3D segmentation"));
        #endif

        clear_segmentation();

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        if (!string_utils::ends_with(filepath, ".seg3"))
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

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // temp
        std::uint16_t ui16temp = 0;
        std::uint32_t ui32temp = 0;

        // grid size
        std::uint16_t gs[3];
        file.read(reinterpret_cast<char*>(gs), 3 * sizeof(std::uint16_t));
        _pdata->seg3.set_size(gs[0], gs[1], gs[2]);

        //auto sz             = _pdata->seg3.geometry().size(); // todo: unused; is this correct?

        // world matrix
        double wmat[16];
        file.read(reinterpret_cast<char*>(wmat), 16 * sizeof(double));
        Mat4d w;
        for (unsigned int i = 0; i < 16; ++i)
        { w[i] = wmat[i]; }
        _pdata->seg3.geometry().transformation().set_world_matrix(w);
        _pdata->seg3.geometry().transformation().set_dicom_image_type_3d();

        // segmentation bits
        BitVectorX segbits;
        segbits.load(file);

        //_pdata->seg3.set(segbits);

        for (unsigned int i = 0; i < _pdata->seg3.num_values(); ++i)
        { _pdata->seg3.operator[](i) = segbits[i]; }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // inside ids
        std::uint32_t numInsideIds = 0;
        file.read(reinterpret_cast<char*>(&numInsideIds), sizeof(std::uint32_t));
        std::vector<std::uint32_t> insideids(numInsideIds);
        file.read(reinterpret_cast<char*>(insideids.data()), numInsideIds * sizeof(std::uint32_t));

        auto fut_insideids = bk_threadpool.enqueue([&]()
                                                   {
                                                       this->_pdata->seg3insideids.resize(numInsideIds);
                                                       std::copy(insideids.begin(), insideids.end(), this->_pdata->seg3insideids.begin());

                                                       #ifdef BK_EMIT_PROGRESS
                                                       prog.increment(1);
                                                       #endif
                                                   });

        // outside ids
        std::uint32_t numOutsideIds = 0;
        file.read(reinterpret_cast<char*>(&numOutsideIds), sizeof(std::uint32_t));
        std::vector<std::uint32_t> outsideids(numOutsideIds);
        file.read(reinterpret_cast<char*>(outsideids.data()), numOutsideIds * sizeof(std::uint32_t));

        auto fut_outsideids = bk_threadpool.enqueue([&]()
                                                    {
                                                        this->_pdata->seg3outsideids.resize(numOutsideIds);
                                                        std::copy(outsideids.begin(), outsideids.end(), this->_pdata->seg3outsideids.begin());

                                                        #ifdef BK_EMIT_PROGRESS
                                                        prog.increment(1);
                                                        #endif
                                                    });

        // info
        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->seg3info.first = static_cast<Segmentation3DInfo_>(ui16temp);

        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->seg3info.second = ui16temp;

        // semantic
        file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        _pdata->semantic = static_cast<VesselSemantic>(ui32temp);

        fut_insideids.get();
        fut_outsideids.get();

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::save_centerline_ids(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("Saving centerline end points"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".clids";
        if (fname.empty())
        { fname = "centerlineIDs" + suffix; }
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

        // temp
        std::uint32_t ui32temp = 0;

        // seed id
        ui32temp = static_cast<std::uint32_t>(_pdata->centerline_seed_id);
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // num target ids
        ui32temp = _pdata->centerline_target_ids.size();
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        // target ids
        for (unsigned int i = 0; i < _pdata->centerline_target_ids.size(); ++i)
        {
            ui32temp = static_cast<std::uint32_t>(_pdata->centerline_target_ids[i]);
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_centerline_ids(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("Loading centerline end points"));
        #endif

        _pdata->centerline_target_ids.clear();
        _pdata->centerline_seed_id = 0;

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        if (!string_utils::ends_with(filepath, ".clids"))
        {
            #ifdef BK_EMIT_PROGRESS
            prog.set_finished();
            #endif
            return false;
        }

        //------------------------------------------------------------------------------------------------------
        // create file
        //------------------------------------------------------------------------------------------------------
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);
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

        // temp
        std::uint32_t ui32temp = 0;

        // seed id
        file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        _pdata->centerline_seed_id = ui32temp;

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // num target ids
        std::uint32_t numTargetIds = 0;
        file.read(reinterpret_cast<char*>(&numTargetIds), sizeof(std::uint32_t));
        _pdata->centerline_target_ids.resize(numTargetIds);

        // target ids
        for (unsigned int i = 0; i < numTargetIds; ++i)
        {
            file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
            _pdata->centerline_target_ids[i] = ui32temp;
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::save_centerlines(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("Saving centerlines"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".cl";
        if (fname.empty())
        { fname = "centerlines" + suffix; }
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

        // temp
        std::uint32_t ui32temp = 0;
        double dtemp = 0;

        // num centerlines
        ui32temp = static_cast<std::uint32_t>(num_centerlines());
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        // centerline
        for (unsigned int i = 0; i < num_centerlines(); ++i)
        {
            const Line3D& l = _pdata->centerlines[i];

            // num points of centerline
            ui32temp = static_cast<std::uint32_t>(l.geometry().num_points());
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            const bool hasRadius = l.point_attribute_map().has_attribute(attribute_info::radius());

            for (unsigned int p = 0; p < l.geometry().num_points(); ++p)
            {
                const auto& pt = l.geometry().point(p);

                // 3d point coordinates
                for (unsigned int k = 0; k < 3; ++k)
                {
                    dtemp = pt[k];
                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
                }

                // radius
                dtemp = 0;
                if (hasRadius)
                { dtemp = l.template point_attribute_value<attribute_info::radius()>(p); }

                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_centerlines(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("Loading centerlines"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        const std::string suffix = ".cl";
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

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // num centerlines
        std::uint32_t numCenterlines = 0;
        file.read(reinterpret_cast<char*>(&numCenterlines), sizeof(std::uint32_t));

        _pdata->centerlines.resize(numCenterlines);

        std::vector<std::future<void>> fut;
        fut.reserve(numCenterlines);

        // centerline
        for (unsigned int i = 0; i < numCenterlines; ++i)
        {
            Line3D& l = _pdata->centerlines[i];

            // num points of centerline
            std::uint32_t numPoints = 0;
            file.read(reinterpret_cast<char*>(&numPoints), sizeof(std::uint32_t));

            l.geometry().reserve(numPoints);
            std::vector<double>& rad = l.template add_point_attribute_vector<attribute_info::radius()>();
            rad.resize(numPoints, 0);

            for (unsigned int p = 0; p < numPoints; ++p)
            {
                // 3d point coordinates
                double dtemp0 = 0;
                double dtemp1 = 0;
                double dtemp2 = 0;

                file.read(reinterpret_cast<char*>(&dtemp0), sizeof(double));
                file.read(reinterpret_cast<char*>(&dtemp1), sizeof(double));
                file.read(reinterpret_cast<char*>(&dtemp2), sizeof(double));

                l.geometry().emplace_back(dtemp0, dtemp1, dtemp2);

                // radius
                file.read(reinterpret_cast<char*>(&rad[p]), sizeof(double));
            }

            fut.push_back(bk_threadpool.enqueue([&, i]()
                                                {
                                                    _pdata->centerlines[i].geometry().construct_kd_tree();
                                                    _pdata->centerlines[i].calc_consistent_local_coordinate_systems();
                                                }));
        }

        for (auto& f : fut)
        { f.get(); }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::save_land_marks(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(2, ___("Saving land marks"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".lm";
        if (fname.empty())
        { fname = "landmarks" + suffix; }
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

        // temp
        std::uint32_t ui32temp = 0;

        // num land marks
        ui32temp = static_cast<std::uint32_t>(num_land_marks());
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        // land marks
        for (unsigned int i = 0; i < num_land_marks(); ++i)
        {
            const LandMark& l = _pdata->land_marks[i];

            ui32temp = l.semantic;
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            ui32temp = l.centerline_id;
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            ui32temp = l.point_id;
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_land_marks(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(2, ___("Loading land marks"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        const std::string suffix = ".lm";
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

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // num centerlines
        std::uint32_t numLandMarks = 0;
        file.read(reinterpret_cast<char*>(&numLandMarks), sizeof(std::uint32_t));

        _pdata->land_marks.resize(numLandMarks);

        // land mark
        for (unsigned int i = 0; i < numLandMarks; ++i)
        {
            LandMark& l = _pdata->land_marks[i];

            file.read(reinterpret_cast<char*>(&l.semantic), sizeof(std::uint32_t));
            file.read(reinterpret_cast<char*>(&l.centerline_id), sizeof(std::uint32_t));
            file.read(reinterpret_cast<char*>(&l.point_id), sizeof(std::uint32_t));
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::save_measuringplanes(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(num_measuring_planes() + 3, ___("Saving measuring planes"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".mp";
        if (fname.empty())
        { fname = "measuringplanes" + suffix; }
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

        // temp
        std::uint32_t ui32temp = 0;
        double dtemp = 0;
        std::uint8_t ui8temp = 0;

        // num measuring planes
        ui32temp = static_cast<std::uint32_t>(num_measuring_planes());
        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

        // measuring planes
        for (unsigned int i = 0; i < num_measuring_planes(); ++i)
        {
            const MeasuringPlane& mp = _pdata->measuring_planes[i];

            // vessel id
            ui8temp = mp.vesselID();
            file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));

            // grid size
            for (unsigned int s = 0; s < 3; ++s)
            {
                ui32temp = mp.geometry().size(s);
                file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
            }

            // center
            for (unsigned int s = 0; s < 3; ++s)
            {
                dtemp = mp.geometry().transformation().center()[s];
                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }

            // scale
            for (unsigned int s = 0; s < 3; ++s)
            {
                dtemp = mp.geometry().transformation().scale(s);
                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }

            // nx
            for (unsigned int s = 0; s < 3; ++s)
            {
                dtemp = mp.geometry().transformation().nx()[s];
                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }

            // ny
            for (unsigned int s = 0; s < 3; ++s)
            {
                dtemp = mp.geometry().transformation().ny()[s];
                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }

            // nz
            for (unsigned int s = 0; s < 3; ++s)
            {
                dtemp = mp.geometry().transformation().nz()[s];
                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            }

            // diameter
            dtemp = mp.diameter_mm();
            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));

            // velocity vectors (N = grid_size[0] * grid_size[1] * grid_size[2])
            for (unsigned int k = 0; k < mp.num_values(); ++k)
            {
                // vvec
                for (unsigned int s = 0; s < 3; ++s)
                {
                    dtemp = mp[k][s];
                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
                }
            }

            // through-plane velocity (N = grid_size[0] * grid_size[1] * grid_size[2])
            //for(unsigned int x = 0; x < mp.size(0); ++x)
            //{
            //    for(unsigned int y = 0; y < mp.size(1); ++y)
            //    {
            //        for(unsigned int z = 0; z < mp.size(2); ++z)
            //        {
            //            dtemp = mp.velocity_through_plane(x,y,z);
            //            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
            //        }
            //    }
            //}

            // segmentation
            const auto& seg = mp.segmentation_cross_section();
            for (unsigned int k = 0; k < mp.num_values(); ++k)
            {
                ui8temp = seg[k] != 0 ? 1 : 0;
                file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));
            }

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        }

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_measuringplanes(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("loading measuring planes"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        const std::string suffix = ".mp";
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

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // num measuring planes
        std::uint32_t numMeasuringPlanes = 0;
        file.read(reinterpret_cast<char*>(&numMeasuringPlanes), sizeof(std::uint32_t));
        _pdata->measuring_planes.resize(numMeasuringPlanes);

        #ifdef BK_EMIT_PROGRESS
        prog.set_max(prog.max() + numMeasuringPlanes);
        #endif

        std::vector<std::future<void>> fut_calc_stats;
        fut_calc_stats.reserve(numMeasuringPlanes);

        // measuring planes
        for (unsigned int i = 0; i < numMeasuringPlanes; ++i)
        {
            MeasuringPlane& mp = _pdata->measuring_planes[i];

            // vessel id
            std::uint8_t id = 0;
            file.read(reinterpret_cast<char*>(&id), sizeof(std::uint8_t));
            mp.set_vesselID(id);

            // grid size
            std::uint32_t gs[3] = {0, 0, 0};
            file.read(reinterpret_cast<char*>(gs), 3 * sizeof(std::uint32_t));
            mp.set_size(gs[0], gs[1], gs[2]);
            mp.geometry().transformation().set_grid_size(gs[0], gs[1], gs[2]);

            const unsigned int N = 5 * 3 + mp.num_values() * 4 + 1;
            std::vector<double> dbuf(N);

            file.read(reinterpret_cast<char*>(dbuf.data()), N * sizeof(double));

            auto fut_set_mp = bk_threadpool.enqueue([&]()
                                                    {
                                                        unsigned int cnt = 0;

                                                        // center
                                                        mp.geometry().transformation().set_center(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                        cnt += 3;

                                                        // scale
                                                        mp.geometry().transformation().set_scale(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                        cnt += 3;

                                                        // nx
                                                        mp.geometry().transformation().set_nx(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                        cnt += 3;

                                                        // ny
                                                        mp.geometry().transformation().set_ny(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                        cnt += 3;

                                                        // nz
                                                        mp.geometry().transformation().set_nz(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                        cnt += 3;

                                                        // diameter
                                                        mp.set_diameter_mm(dbuf[cnt++]);

                                                        // velocity vectors (N = grid_size[0] * grid_size[1] * grid_size[2])
                                                        for (unsigned int k = 0; k < mp.num_values(); ++k)
                                                        {
                                                            mp[k].set(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
                                                            cnt += 3;
                                                        }

                                                        // through-plane velocity (N = grid_size[0] * grid_size[1] * grid_size[2])
                                                        //for (unsigned int k = 0; k < mp.num_values(); ++k)
                                                        //{ mp.point_attributes().set_velocity_throughplane_value(k, dbuf[cnt++]); }
                                                        //
                                                        //for(unsigned int x = 0; x < mp.size(0); ++x)
                                                        //{
                                                        //    for(unsigned int y = 0; y < mp.size(1); ++y)
                                                        //    {
                                                        //        for(unsigned int z = 0; z < mp.size(2); ++z)
                                                        //        {mp.velocity_through_plane(x,y,z) = dbuf[cnt++];}
                                                        //    }
                                                        //}
                                                    });

            // segmentation
            CartesianImage<std::uint8_t, 3> seg;
            seg.set_size(gs[0], gs[1], gs[2]);

            std::vector<std::uint8_t> ui8buf(mp.num_values());
            file.read(reinterpret_cast<char*>(ui8buf.data()), mp.num_values() * sizeof(std::uint8_t));

            #pragma omp parallel for
            for (unsigned int k = 0; k < mp.num_values(); ++k)
            { seg[k] = ui8buf[k]; }

            fut_set_mp.get();

            mp.set_measuring_plane_segmentation(seg);
            fut_calc_stats.emplace_back(bk_threadpool.enqueue([&]()
                                                              { mp.calc_statistics(); }));
        }

        file.close();

        for (std::future<void>& f: fut_calc_stats)
        { f.get(); }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::save_flowjets(std::string_view filepath) const
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(num_flowjets() + 3, ___("Saving flow jets"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check filename
        //------------------------------------------------------------------------------------------------------
        std::string fname(filepath);
        const std::string suffix = ".fj";
        if (fname.empty())
        { fname = "flowjets" + suffix; }
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

        // temp
        std::uint8_t ui8temp = 0;
        std::uint32_t ui32temp = 0;

        // num jets
        ui8temp = num_flowjets();
        file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));

        for (const FlowJet& fj: _pdata->flowjets)
        {
            // num planes
            ui32temp = fj.num_positions();
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            // num times
            ui32temp = fj.num_times();
            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));

            for (unsigned int pid = 0; pid < fj.num_positions(); ++pid)
            {
                for (unsigned int tid = 0; tid < fj.num_times(); ++tid)
                {
                    const FlowJetPoint& fjp = fj.point(pid, tid);

                    // vessel center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.vessel_center[i]), sizeof(double)); }

                    // vessel radius
                    file.write(reinterpret_cast<const char*>(&fjp.vessel_radius), sizeof(double));

                    // jet lcs x
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.local_coord_sys_x[i]), sizeof(double)); }

                    // jet lcs y
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.local_coord_sys_y[i]), sizeof(double)); }

                    // jet center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.peak_velocity_position[i]), sizeof(double)); }

                    // jet velocity
                    file.write(reinterpret_cast<const char*>(&fjp.peak_velocity), sizeof(double));

                    // area center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.area_center[i]), sizeof(double)); }

                    // area radii
                    file.write(reinterpret_cast<const char*>(&fjp.area_radius_x), sizeof(double));
                    file.write(reinterpret_cast<const char*>(&fjp.area_radius_y), sizeof(double));

                    // area dir x
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.area_dir_x[i]), sizeof(double)); }

                    // area dir y
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.write(reinterpret_cast<const char*>(&fjp.area_dir_y[i]), sizeof(double)); }
                } // for tid : fj.num_times()
            } // for pid : fj.num_positions()

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        } // for fjid : num_flowjets()

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Vessel::load_flowjets(std::string_view filepath)
    {
        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(3, ___("Loading flow jets"));
        #endif

        //------------------------------------------------------------------------------------------------------
        // check file ending
        //------------------------------------------------------------------------------------------------------
        const std::string suffix = ".fj";
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

        clear_flowjets();

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        std::uint8_t numFlowjets = 0;
        file.read(reinterpret_cast<char*>(&numFlowjets), sizeof(std::uint8_t));

        _pdata->flowjets.resize(numFlowjets);

        #ifdef BK_EMIT_PROGRESS
        prog.set_max(prog.max() + static_cast<int>(numFlowjets));
        #endif

        for (unsigned int fjid = 0; fjid < numFlowjets; ++fjid)
        {
            FlowJet& fj = _pdata->flowjets[fjid];

            // num planes
            std::uint32_t numPlanes = 0;
            file.read(reinterpret_cast<char*>(&numPlanes), sizeof(std::uint32_t));

            // num times
            std::uint32_t numTimes = 0;
            file.read(reinterpret_cast<char*>(&numTimes), sizeof(std::uint32_t));

            fj.resize(numPlanes, numTimes);

            for (unsigned int pid = 0; pid < fj.num_positions(); ++pid)
            {
                for (unsigned int tid = 0; tid < fj.num_times(); ++tid)
                {
                    FlowJetPoint& fjp = fj.point(pid, tid);

                    // vessel center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.vessel_center[i]), sizeof(double)); }

                    // vessel radius
                    file.read(reinterpret_cast<char*>(&fjp.vessel_radius), sizeof(double));

                    // jet lcs x
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.local_coord_sys_x[i]), sizeof(double)); }

                    // jet lcs y
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.local_coord_sys_y[i]), sizeof(double)); }

                    // jet center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.peak_velocity_position[i]), sizeof(double)); }

                    // jet velocity
                    file.read(reinterpret_cast<char*>(&fjp.peak_velocity), sizeof(double));

                    // area center
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.area_center[i]), sizeof(double)); }

                    // area radii
                    file.read(reinterpret_cast<char*>(&fjp.area_radius_x), sizeof(double));
                    file.read(reinterpret_cast<char*>(&fjp.area_radius_y), sizeof(double));

                    // area dir x
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.area_dir_x[i]), sizeof(double)); }

                    // area dir y
                    for (unsigned int i = 0; i < 3; ++i)
                    { file.read(reinterpret_cast<char*>(&fjp.area_dir_y[i]), sizeof(double)); }
                } // for tid : fj.num_times()
            } // for pid : fj.num_positions()

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        } // for fjid: numFlowhjets

        file.close();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }
  } // inline namespace cmr
} // namespace bk


