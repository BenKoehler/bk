///*
// * MIT License
// *
// * Copyright (c) 2018 Benjamin Köhler
// *
// * Permission is hereby granted, free of charge, to any person obtaining a copy
// * of this software and associated documentation files (the "Software"), to deal
// * in the Software without restriction, including without limitation the rights
// * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// * copies of the Software, and to permit persons to whom the Software is
// * furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice shall be included in all
// * copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// * SOFTWARE.
// */
//
//#include <bkCMR/Vessel.h>
//
//#include <algorithm>
//#include <cstdint>
//
//#include <bkCMR/rotation_angle_2d.h>
//#include <bkTools/BitVector>
//#include <bkTools/Clock>
//#include <bkTools/Localization>
//#include <bkDataset/Line>
//#include <bkDataset/Image>
//#include <bkDataset/Mesh>
//#include <bkTools/ThreadPool>
//
//#ifdef BK_EMIT_PROGRESS
//
//    #include <bkTools/Progress>
//
//#endif
//
////#include "FlowJetSet_CMR.h"
////#include "FlowJet_CMR.h"
//
//
//namespace bk
//{
//  inline namespace cmr
//  {
//    //====================================================================================================
//    //===== MEMBERS
//    //====================================================================================================
//    struct Vessel::Impl
//    {
//        std::string                                 name;
//        segmentation3d_type                         seg3;
//        std::pair<Segmentation3DInfo, unsigned int> seg3info;
//        std::vector<unsigned int>                   seg3insideids;
//        std::vector<unsigned int>                   seg3outsideids;
//        mesh_type                                   mesh;
//        pathlineset_type                            pathlines;
//        unsigned int                                centerline_seed_id;
//        std::vector<unsigned int>                   centerline_target_ids;
//        centerlineset_type                          centerlines;
//        //std::vector<measuring_plane_type> measuring_planes;
//        //std::vector<FlowJet_CMR> flowjets;
//        //FlowJetSet_CMR flowjets;
//
//        Impl()
//            : name("vessel"),
//              seg3(std::make_unique<segmentation3d_type>()),
//              mesh(std::make_unique<mesh_type>())
//        {}
//
//        Impl(const Impl& other)
//            : name(other.name),
//              seg3(std::make_unique<segmentation3d_type>(*other.seg3)),
//              seg3info(other.seg3info),
//              seg3insideids(other.seg3insideids),
//              seg3outsideids(other.seg3outsideids),
//              mesh(std::make_unique<mesh_type>(*other.mesh)),
//              pathlines(other.pathlines),
//              centerline_seed_id(other.centerline_seed_id),
//              centerline_target_ids(other.centerline_target_ids),
//              centerlines(other.centerlines)/*,
//              measuring_planes(other.measuring_planes),
//              flowjets(other.flowjets)*/
//        {}
//
//        Impl(Impl&&) = default;
//        ~Impl() = default;
//        Impl& operator=(const Impl&) = default;
//        Impl& operator=(Impl&&) noexcept = default;
//    };
//
//    //====================================================================================================
//    //===== CONSTRUCTORS & DESTRUCTOR
//    //====================================================================================================
//    Vessel::Vessel()
//        : _pdata(std::make_unique<Impl>())
//    {}
//
//    Vessel::Vessel(const Vessel& other)
//        : _pdata(std::make_unique<Impl>(*other._pdata.get()))
//    {}
//
//    Vessel::Vessel(Vessel&&) noexcept = default;
//    Vessel::~Vessel() = default;
//
//    //====================================================================================================
//    //===== GETTER
//    //====================================================================================================
//    const std::string& Vessel::name() const
//    { return _pdata->name; }
//
//    auto Vessel::segmentation3D() const -> const segmentation3d_type&
//    { return _pdata->seg3; }
//
//    auto Vessel::segmentation3D() -> segmentation3d_type&
//    { return _pdata->seg3; }
//
//    const std::pair<Segmentation3DInfo, unsigned int>& Vessel::segmentation3D_info() const
//    { return _pdata->seg3info; }
//
//    std::vector<unsigned int>& Vessel::segmentation3D_inside_ids()
//    { return _pdata->seg3insideids; }
//
//    const std::vector<unsigned int>& Vessel::segmentation3D_inside_ids() const
//    { return _pdata->seg3insideids; }
//
//    std::vector<unsigned int>& Vessel::segmentation3D_outside_ids()
//    { return _pdata->seg3outsideids; }
//
//    const std::vector<unsigned int>& Vessel::segmentation3D_outside_ids() const
//    { return _pdata->seg3outsideids; }
//
//    auto Vessel::mesh() const -> const mesh_type&
//    { return _pdata->mesh; }
//
//    auto Vessel::mesh() -> mesh_type&
//    { return _pdata->mesh; }
//
//    auto Vessel::pathlines() const -> const pathlineset_type&
//    { return _pdata->pathlines; }
//
//    auto Vessel::pathlines() -> pathlineset_type&
//    { return _pdata->pathlines; }
//
//    unsigned int Vessel::num_pathlines() const
//    { return _pdata->pathlines.size(); }
//
//    auto Vessel::centerlines() const -> const centerlineset_type&
//    { return _pdata->centerlines; }
//
//    auto Vessel::centerlines() -> centerlineset_type&
//    { return _pdata->centerlines; }
//
//    unsigned int Vessel::num_centerlines() const
//    { return _pdata->centerlines.size(); }
//
//    unsigned int Vessel::centerline_seed_id() const
//    { return _pdata->centerline_seed_id; }
//
//    const std::vector<unsigned int>& Vessel::centerline_target_ids() const
//    { return _pdata->centerline_target_ids; }
//
//    bool Vessel::has_centerline_ids() const
//    { return !_pdata->centerline_target_ids.empty(); }
//
//    //auto Vessel::measuring_planes() -> std::vector<measuring_plane_type>&
//    //{ return _pdata->measuring_planes; }
//    //
//    //auto Vessel::measuring_planes() const -> const std::vector<measuring_plane_type>&
//    //{ return _pdata->measuring_planes; }
//    //
//    //auto Vessel::measuring_plane(unsigned int i) const -> const measuring_plane_type*
//    //{ return i < _pdata->measuring_planes.size() ? &_pdata->measuring_planes[i] : nullptr; }
//    //
//    //auto Vessel::measuring_plane(unsigned int i) -> measuring_plane_type*
//    //{ return i < _pdata->measuring_planes.size() ? &_pdata->measuring_planes[i] : nullptr; }
//    //
//    //bool Vessel::has_measuring_planes() const
//    //{ return !_pdata->measuring_planes.empty(); }
//    //
//    //unsigned int Vessel::num_measuring_planes() const
//    //{ return _pdata->measuring_planes.size(); }
//    //
//    //auto Vessel::flowjets() -> flowjetset_type&
//    //{ return _pdata->flowjets; }
//    //
//    //auto Vessel::flowjets() const -> const flowjetset_type&
//    //{ return _pdata->flowjets; }
//    //
//    //unsigned int Vessel::num_flowjets() const
//    //{ return _pdata->flowjets.num_flowjets(); }
//    //
//    //bool Vessel::has_flowjets()
//    //{ return num_flowjets() != 0; }
//    //
//    //FlowJet_CMR* Vessel::flowjet(unsigned int i)
//    //{ return _pdata->flowjets[i]; }
//    //
//    //const FlowJet_CMR* Vessel::flowjet(unsigned int i) const
//    //{ return _pdata->flowjets[i]; }
//
//    //====================================================================================================
//    //===== SETTER
//    //====================================================================================================
//    void Vessel::clear()
//    {
//        _pdata->name = "";
//
//        clear_mesh();
//        clear_segmentation();
//        clear_pathlines();
//        clear_centerlines();
//        clear_centerline_ids();
//        clear_measuring_planes();
//        clear_flowjets();
//    }
//
//    void Vessel::clear_segmentation()
//    {
//        _pdata->seg3            = std::make_unique<segmentation3d_type>();
//        _pdata->seg3info.first  = Segmentation3DInfo::Magnitude4DTMIP;
//        _pdata->seg3info.second = 0;
//        _pdata->seg3insideids.clear();
//        _pdata->seg3outsideids.clear();
//    }
//
//    void Vessel::clear_mesh()
//    { _pdata->mesh = std::make_unique<mesh_type>(); }
//
//    void Vessel::clear_pathlines()
//    { _pdata->pathlines.clear(); }
//
//    void Vessel::clear_centerlines()
//    { _pdata->centerlines.clear(); }
//
//    void Vessel::clear_measuring_planes()
//    { _pdata->measuring_planes.clear(); }
//
//    void Vessel::clear_flowjets()
//    { _pdata->flowjets.clear(); }
//
//    void Vessel::set_name(const std::string& name)
//    { _pdata->name = name; }
//
//    void Vessel::set_seg3d_was_performed_on_magnitude_TMIP()
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::Magnitude4DTMIP;
//        _pdata->seg3info.second = 0;
//    }
//
//    void Vessel::set_seg3d_was_performed_on_LPC()
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::LPC;
//        _pdata->seg3info.second = 0;
//    }
//
//    void Vessel::set_seg3d_was_performed_on_IVSD()
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::IVSD;
//        _pdata->seg3info.second = 0;
//    }
//
//    void Vessel::set_seg3d_was_performed_on_3D_anatomical_image(unsigned int id)
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::Anatomical3D;
//        _pdata->seg3info.second = id;
//    }
//
//    void Vessel::set_seg3d_was_performed_on_4D_anatomical_image_TMIP(unsigned int id)
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::Anatomical4DTMIP;
//        _pdata->seg3info.second = id;
//    }
//
//    void Vessel::set_seg3d_was_performed_on_4D_signal_intensity_image_TMIP()
//    {
//        _pdata->seg3info.first  = Segmentation3DInfo::SignalIntensity4DTMIP;
//        _pdata->seg3info.second = 0;
//    }
//
//    void Vessel::set_centerline_seed_id(unsigned int seedid)
//    { _pdata->centerline_seed_id = seedid; }
//
//    void Vessel::set_centerline_target_ids(std::vector<unsigned int> targetids)
//    { _pdata->centerline_target_ids = targetids; }
//
//    void Vessel::add_centerline_target_id(unsigned int targetid)
//    {
//        _pdata->centerline_target_ids.push_back(targetid);
//
//        #if defined(__GNUC__) && defined(_OPENMP)
//        __gnu_parallel
//        #else
//        std
//        #endif
//        ::sort(_pdata->centerline_target_ids.begin(), _pdata->centerline_target_ids.end());
//        _pdata->centerline_target_ids.erase(std::unique(_pdata->centerline_target_ids.begin(), _pdata->centerline_target_ids.end()), _pdata->centerline_target_ids.end());
//    }
//
//    void Vessel::clear_centerline_ids()
//    {
//        _pdata->centerline_seed_id = 0;
//        _pdata->centerline_target_ids.clear();
//    }
//
//    auto Vessel::operator=(const Vessel& other) -> Vessel&
//    {
//        _pdata.reset(new Impl(*other._pdata.get()));
//        return *this;
//    }
//
//    auto Vessel::operator=(Vessel&& other) -> Vessel& = default;
//
//    //====================================================================================================
//    //===== FUNCTIONS
//    //====================================================================================================
//    #if defined(BK_LIB_VTK_AVAILABLE) && defined(BK_LIB_VMTK_AVAILABLE)
//    void CMRVessel::extract_centerlines()
//  {
//      if (mesh() != nullptr)
//      { _pdata->centerlines.add_lines(std::move(mesh()->extract_centerline(_pdata->centerline_seed_id, _pdata->centerline_target_ids))); }
//  }
//    #endif
//
//    void Vessel::calc_pathlines_angles_to_centerline()
//    {
//        using pathline_type = pathlineset_type::line_type;
//
//        if (!_pdata->centerlines.has_lines() || !_pdata->pathlines.has_lines())
//        { return; }
//
//        Clock clock;
//        clock.start();
//
//        const unsigned int numLines       = num_pathlines();
//        unsigned int       numPointsTotal = 0;
//        for (unsigned int  pid            = 0; pid < numLines; ++pid)
//        { numPointsTotal += _pdata->pathlines.line(pid)->geometry().num_points(); }
//
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, numPointsTotal, ___("analyzing pathlines/centerlines relations"));
//        #endif
//
//        /*
//         * construct centerline kd trees
//         */
//        #pragma omp parallel for
//        for (unsigned int cid = 0; cid < _pdata->centerlines.num_lines(); ++cid)
//        { _pdata->centerlines.line(cid)->geometry().construct_kd_tree(); }
//
//        /*
//         * calculate attributes
//         */
//        #pragma omp parallel for schedule(dynamic, 10)
//        for (unsigned int pid = 0; pid < numLines; ++pid)
//        {
//            pathline_type* pln = _pdata->pathlines.line(pid);
//            const unsigned int nPoints = pln->geometry().num_points();
//
//            for (unsigned int vid = 0; vid < nPoints; ++vid)
//            {
//                //const auto plnvertex = pln->geometry().point_in_world_coordinates(vid);
//                const Vec3d& plnvertex = pln->geometry().point(vid); // has no transformation
//                auto plntangent = pln->tangent_at_point(vid);
//                plntangent.normalize_internal();
//
//                double                                                                     angle = 0;
//                decltype(_pdata->centerlines.line(0)->geometry().closest_point(plnvertex)) cp;
//                cp.distance_to_query = std::numeric_limits<double>::max();
//                unsigned int clid = 0;
//
//                for (unsigned int cid = 0; cid < _pdata->centerlines.num_lines(); ++cid)
//                {
//                    auto current_cp = _pdata->centerlines.line(cid)->geometry().closest_point(plnvertex);
//
//                    if (current_cp.distance_to_query < cp.distance_to_query)
//                    {
//                        cp   = std::move(current_cp);
//                        clid = cid;
//
//                        const auto* clntangent_ptr = _pdata->centerlines.line(cid)->point_attributes().get_local_coord_sys3d_z_value(cp.point_id);
//
//                        if (clntangent_ptr != nullptr)
//                        { angle = clntangent_ptr->dot(plntangent); }
//                        else
//                        {
//                            auto clntangent = _pdata->centerlines.line(cid)->tangent_at_point(cp.point_id);
//                            clntangent.normalize_internal();
//                            angle = clntangent.dot(plntangent);
//                        }
//                    } // if closer
//                } // for cid: num centerlines
//
//                const double deg = bk::radians_to_degrees(std::acos(std::abs(angle)));
//
//                pln->point_attributes().set_relative_flow_direction_value(vid, angle);
//                pln->point_attributes().set_angle_to_centerline_value(vid, deg);
//
//                /*
//                 * determine turning direction
//                 */
//                if (vid == nPoints - 1)
//                { pln->point_attributes().set_rotation_direction_value(vid, 0.0); }
//                else //if (vid < nPoints - 1)
//                {
//                    const auto * cl      = _pdata->centerlines.line(clid);
//                    const auto & clpoint = cl->geometry().point(cp.point_id);
//                    const Vec3d* nx_ptr  = cl->point_attributes().get_local_coord_sys3d_x_value(cp.point_id);
//                    const Vec3d* ny_ptr  = cl->point_attributes().get_local_coord_sys3d_y_value(cp.point_id);
//
//                    if (nx_ptr != nullptr && ny_ptr != nullptr)
//                    {
//                        const double rotangle_deg = bk::rotation_angle_deg_2d_of_3d_points_projected_onto_plane(plnvertex, pln->geometry().point(vid + 1), clpoint, *nx_ptr, *ny_ptr);
//                        pln->point_attributes().set_rotation_direction_value(vid, rotangle_deg);
//                    }
//                    else
//                    { pln->point_attributes().set_rotation_direction_value(vid, 0.0); }
//                } // if (vid < nPoints - 1)
//            } // for vid: num points
//
//            /*
//             * smooth attributes
//             */
//            pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::relative_flow_direction_str(), 25, 0.25, 0.25);
//            pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::angle_to_centerline_str(), 25, 0.25, 0.25);
//            pln->smooth_scalar_point_attribute_lambda_mu(attribute_info::rotation_direction_str(), 25, 0.25, 0.25);
//            //
//                #ifdef BK_EMIT_PROGRESS
//                    #pragma omp critical
//            { prog.increment(nPoints); }
//                #endif
//        } // for pid: num pathlines
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        /*
//         * clamp rotation direction values
//         */
//        std::vector<double> rotdirvalues;
//        rotdirvalues.reserve(numPointsTotal);
//
//        for (unsigned int pid = 0; pid < numLines; ++pid)
//        {
//            pathline_type* pln = _pdata->pathlines.line(pid);
//            for (unsigned int vid = 0; vid < pln->geometry().num_points(); ++vid)
//            {
//                auto rotptr = pln->point_attributes().get_rotation_direction_value(vid);
//                if (rotptr != nullptr)
//                {
//                    const double rotdir = static_cast<double>(*rotptr);
//                    rotdirvalues.push_back(rotdir);
//                }
//            }
//        }
//
//        #if defined(__GNUC__) && defined(_OPENMP)
//        __gnu_parallel
//        #else
//        std
//        #endif
//        ::sort(rotdirvalues.begin(), rotdirvalues.end());
//
//        if (!rotdirvalues.empty())
//        {
//            const double rotdirclampmin = rotdirvalues[2 * rotdirvalues.size() / 100]; // todo options
//            const double rotdirclampmax = rotdirvalues[98 * rotdirvalues.size() / 100]; // todo options
//            const double rotdirclamp    = std::max(std::abs(rotdirclampmin), std::abs(rotdirclampmax));
//
//            #pragma omp parallel for
//            for (unsigned int pid = 0; pid < numLines; ++pid)
//            {
//                pathline_type* pln = _pdata->pathlines.line(pid);
//                for (unsigned int vid = 0; vid < pln->geometry().num_points(); ++vid)
//                {
//                    const double rotdir = static_cast<double>(*pln->point_attributes().get_rotation_direction_value(vid));
//                    pln->point_attributes().set_rotation_direction_value(vid, std::min(rotdirclamp, std::max(-rotdirclamp, rotdir)));
//                }
//            }
//        }
//
//        clock.stop();
//        std::cout << "Calculated Advanced Attributes (" << numLines << " lines; " << numPointsTotal << " points) in " << clock.time_in_sec() << " seconds" << std::endl;
//    }
//
//    std::pair<int /*centerlineId*/, bk::KDPointInfo<bk::Vec3d>> Vessel::closest_centerline_and_point_id(const bk::Vec3d& pt)
//    {
//        for (unsigned int centerlineId = 0; centerlineId < centerlines().num_lines(); ++centerlineId)
//        {
//            auto cl = centerlines().line(centerlineId);
//
//            #pragma omp critical
//            {
//                if (!cl->geometry().has_kdtree())
//                { cl->geometry().construct_kd_tree(); }
//            };
//        } // for centerlineId
//
//        int                        closestCenterlineId = -1;
//        bk::KDPointInfo<bk::Vec3d> closest;
//        closest.distance_to_query = std::numeric_limits<double>::max();
//
//        for (unsigned int clid = 0; clid < centerlines().num_lines(); ++clid)
//        {
//            auto& cl = *centerlines().line(clid);
//            bk::KDPointInfo<bk::Vec3d> cp = cl.geometry().closest_point(pt);
//
//            if (cp.distance_to_query < closest.distance_to_query)
//            {
//                closest             = std::move(cp);
//                closestCenterlineId = clid;
//            }
//        } // for clid
//
//        return std::make_pair(closestCenterlineId, std::move(closest));
//    }
//
//    //====================================================================================================
//    //===== I/O
//    //====================================================================================================
//    bool Vessel::save_mesh(const std::string& filepath) const
//    { return _pdata->mesh->save(filepath); }
//
//    bool Vessel::load_mesh(const std::string& filepath)
//    { return _pdata->mesh->load(filepath); }
//
//    bool Vessel::save_segmentation3D(const std::string& filepath) const
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog           = bk_progress.emplace_task(0, 5, ___("saving 3d segmentation"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check filename
//        //------------------------------------------------------------------------------------------------------
//        std::string       fname(filepath);
//        const std::string suffix = ".seg3";
//        if (fname.empty())
//        { fname = "segmentation3d" + suffix; }
//        else if (!string_utils::ends_with(fname, suffix))
//        { fname.append(suffix); }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint16_t ui16temp = 0;
//        std::uint32_t ui32temp = 0;
//        double        dtemp    = 0;
//
//        // grid size
//        const auto& gs = _pdata->seg3->geometry().size();
//        for (unsigned int i = 0; i < 3; ++i)
//        {
//            ui16temp = static_cast<std::uint16_t>(gs[i]);
//            file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
//        }
//
//        // world matrix
//        const auto& wmat = _pdata->seg3->geometry().transformation().world_matrix();
//        for (unsigned int i = 0; i < 16; ++i)
//        {
//            dtemp = static_cast<double>(wmat[i]);
//            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//        }
//
//        // segmentation bits
//        BitVectorX segbits;
//        segbits.set_size(_pdata->seg3->num_values());
//        for (unsigned int i = 0; i < _pdata->seg3->num_values(); ++i)
//        { segbits[i] = _pdata->seg3->operator[](i) != 0 ? 1 : 0; }
//        segbits.save(file);
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // inside ids
//        ui32temp = static_cast<std::uint32_t>(_pdata->seg3insideids.size());
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        for (unsigned int i = 0; i < _pdata->seg3insideids.size(); ++i)
//        {
//            ui32temp = static_cast<std::uint32_t>(_pdata->seg3insideids[i]);
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // outside ids
//        ui32temp = static_cast<std::uint32_t>(_pdata->seg3outsideids.size());
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        for (unsigned int i = 0; i < _pdata->seg3outsideids.size(); ++i)
//        {
//            ui32temp = static_cast<std::uint32_t>(_pdata->seg3outsideids[i]);
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // info
//        ui16temp = static_cast<std::uint16_t>(_pdata->seg3info.first);
//        file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
//
//        ui16temp = static_cast<std::uint16_t>(_pdata->seg3info.second);
//        file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::load_segmentation3D(const std::string& filepath)
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, 5, ___("loading 3d segmentation"));
//        #endif
//
//        clear_segmentation();
//
//        //------------------------------------------------------------------------------------------------------
//        // check file ending
//        //------------------------------------------------------------------------------------------------------
//        if (!string_utils::ends_with(filepath, ".seg3"))
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        //------------------------------------------------------------------------------------------------------
//        // open file
//        //------------------------------------------------------------------------------------------------------
//        std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint16_t ui16temp = 0;
//
//        // grid size
//        std::uint16_t gs[3];
//        file.read(reinterpret_cast<char*>(gs), 3 * sizeof(std::uint16_t));
//        _pdata->seg3->set_size(gs[0], gs[1], gs[2]);
//
//        auto sz             = _pdata->seg3->geometry().size();
//
//        // world matrix
//        double wmat[16];
//        file.read(reinterpret_cast<char*>(wmat), 16 * sizeof(double));
//        Mat4d             w;
//        for (unsigned int i = 0; i < 16; ++i)
//        { w[i] = wmat[i]; }
//        _pdata->seg3->geometry().transformation().set_world_matrix(w);
//
//        // segmentation bits
//        BitVectorX segbits;
//        segbits.load(file);
//
//        //_pdata->seg3->set(segbits);
//
//        for (unsigned int i = 0; i < _pdata->seg3->num_values(); ++i)
//        { _pdata->seg3->operator[](i) = segbits[i]; }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // inside ids
//        std::uint32_t numInsideIds = 0;
//        file.read(reinterpret_cast<char*>(&numInsideIds), sizeof(std::uint32_t));
//        std::vector<std::uint32_t> insideids(numInsideIds);
//        file.read(reinterpret_cast<char*>(insideids.data()), numInsideIds * sizeof(std::uint32_t));
//
//        auto fut_insideids = bk_threadpool.enqueue([&]()
//                                                   {
//                                                       this->_pdata->seg3insideids.resize(numInsideIds);
//                                                       std::copy(insideids.begin(), insideids.end(), this->_pdata->seg3insideids.begin());
//
//                                                       #ifdef BK_EMIT_PROGRESS
//                                                       prog.increment(1);
//                                                       #endif
//                                                   });
//
//        // outside ids
//        std::uint32_t numOutsideIds = 0;
//        file.read(reinterpret_cast<char*>(&numOutsideIds), sizeof(std::uint32_t));
//        std::vector<std::uint32_t> outsideids(numOutsideIds);
//        file.read(reinterpret_cast<char*>(outsideids.data()), numOutsideIds * sizeof(std::uint32_t));
//
//        auto fut_outsideids = bk_threadpool.enqueue([&]()
//                                                    {
//                                                        this->_pdata->seg3outsideids.resize(numOutsideIds);
//                                                        std::copy(outsideids.begin(), outsideids.end(), this->_pdata->seg3outsideids.begin());
//
//                                                        #ifdef BK_EMIT_PROGRESS
//                                                        prog.increment(1);
//                                                        #endif
//                                                    });
//
//        // info
//        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
//        _pdata->seg3info.first = static_cast<Segmentation3DInfo>(ui16temp);
//
//        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
//        _pdata->seg3info.second = ui16temp;
//
//        fut_insideids.get();
//        fut_outsideids.get();
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::save_centerline_ids(const std::string& filepath) const
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog           = bk_progress.emplace_task(0, 3, ___("saving centerline end points"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check filename
//        //------------------------------------------------------------------------------------------------------
//        std::string       fname(filepath);
//        const std::string suffix = ".clids";
//        if (fname.empty())
//        { fname = "centerlineIDs" + suffix; }
//        else if (!string_utils::ends_with(fname, suffix))
//        { fname.append(suffix); }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint32_t ui32temp = 0;
//
//        // seed id
//        ui32temp = static_cast<std::uint32_t>(_pdata->centerline_seed_id);
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // num target ids
//        ui32temp = _pdata->centerline_target_ids.size();
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        // target ids
//        for (unsigned int i = 0; i < _pdata->centerline_target_ids.size(); ++i)
//        {
//            ui32temp = static_cast<std::uint32_t>(_pdata->centerline_target_ids[i]);
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//        }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        //prog.increment(1);
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::load_centerline_ids(const std::string& filepath)
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, 3, ___("loading centerline end points"));
//        #endif
//
//        _pdata->centerline_target_ids.clear();
//        _pdata->centerline_seed_id = 0;
//
//        //------------------------------------------------------------------------------------------------------
//        // check file ending
//        //------------------------------------------------------------------------------------------------------
//        if (!string_utils::ends_with(filepath, ".clids"))
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint32_t ui32temp = 0;
//
//        // seed id
//        file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//        _pdata->centerline_seed_id = ui32temp;
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // num target ids
//        std::uint32_t numTargetIds = 0;
//        file.read(reinterpret_cast<char*>(&numTargetIds), sizeof(std::uint32_t));
//        _pdata->centerline_target_ids.resize(numTargetIds);
//
//        // target ids
//        for (unsigned int i = 0; i < numTargetIds; ++i)
//        {
//            file.read(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//            _pdata->centerline_target_ids[i] = ui32temp;
//        }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        //prog.increment(1);
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::save_centerlines(const std::string& filepath) const
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog           = bk_progress.emplace_task(0, 3, ___("saving centerlines"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check filename
//        //------------------------------------------------------------------------------------------------------
//        std::string       fname(filepath);
//        const std::string suffix = ".cl";
//        if (fname.empty())
//        { fname = "centerlines" + suffix; }
//        else if (!string_utils::ends_with(fname, suffix))
//        { fname.append(suffix); }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint32_t ui32temp = 0;
//        double        dtemp    = 0;
//
//        // num centerlines
//        ui32temp = static_cast<std::uint32_t>(centerlines().num_lines());
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        // centerline
//        for (unsigned int i = 0; i < centerlines().num_lines(); ++i)
//        {
//            const auto& l = *centerlines().line(i);
//
//            // num points of centerline
//            ui32temp = static_cast<std::uint32_t>(l.geometry().num_points());
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//            for (unsigned int p = 0; p < l.geometry().num_points(); ++p)
//            {
//                const auto& pt = l.geometry().point(p);
//
//                // 3d point coordinates
//                for (unsigned int k = 0; k < 3; ++k)
//                {
//                    dtemp = pt[k];
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                }
//
//                // MaximumInscribedSphereRadius
//                auto misr = l.point_attributes().get_MaximumInscribedSphereRadius_value(p);
//                dtemp = misr != nullptr ? *misr : 0;
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//        }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        //prog.increment(1);
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::load_centerlines(const std::string& filepath)
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, 3, ___("loading centerlines"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check file ending
//        //------------------------------------------------------------------------------------------------------
//        const std::string suffix = ".cl";
//        if (!string_utils::ends_with(filepath, suffix))
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        //------------------------------------------------------------------------------------------------------
//        // open file
//        //------------------------------------------------------------------------------------------------------
//        std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // num centerlines
//        std::uint32_t numCenterlines = 0;
//        file.read(reinterpret_cast<char*>(&numCenterlines), sizeof(std::uint32_t));
//
//        centerlines().lines().resize(numCenterlines);
//
//        std::vector<std::future<void>> fut;
//        fut.reserve(numCenterlines);
//
//        // centerline
//        for (unsigned int i = 0; i < numCenterlines; ++i)
//        {
//            auto& l = *centerlines().line(i);
//
//            // num points of centerline
//            std::uint32_t numPoints = 0;
//            file.read(reinterpret_cast<char*>(&numPoints), sizeof(std::uint32_t));
//
//            l.geometry().reserve(numPoints);
//
//            for (unsigned int p = 0; p < numPoints; ++p)
//            {
//                // 3d point coordinates
//                double dtemp0 = 0;
//                double dtemp1 = 0;
//                double dtemp2 = 0;
//
//                file.read(reinterpret_cast<char*>(&dtemp0), sizeof(double));
//                file.read(reinterpret_cast<char*>(&dtemp1), sizeof(double));
//                file.read(reinterpret_cast<char*>(&dtemp2), sizeof(double));
//
//                l.geometry().emplace_back(dtemp0, dtemp1, dtemp2);
//
//                // MaximumInscribedSphereRadius
//                double misr = 0;
//                file.read(reinterpret_cast<char*>(&misr), sizeof(double));
//                l.point_attributes().set_MaximumInscribedSphereRadius_value(p, misr);
//            }
//
//            fut.push_back(bk_threadpool.enqueue([=]()
//                                                {
//                                                    //constexpr const unsigned int binomial_smooth_iterations = 25; // todo options
//                                                    //constexpr const unsigned int binomial_smooth_kernel_size = 5; // todo options
//                                                    //centerlines().line(i)->calc_consistent_local_coordinate_systems(binomial_smooth_iterations, binomial_smooth_kernel_size);
//
//                                                    centerlines().line(i)->geometry().construct_kd_tree();
//                                                    centerlines().line(i)->calc_consistent_local_coordinate_systems();
//                                                }));
//        }
//
//        for (auto& f : fut)
//        { f.get(); }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        //prog.increment(1);
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::save_measuringplanes(const std::string& filepath) const
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog           = bk_progress.emplace_task(0, 3, ___("saving measuring planes"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check filename
//        //------------------------------------------------------------------------------------------------------
//        std::string       fname(filepath);
//        const std::string suffix = ".mp";
//        if (fname.empty())
//        { fname = "measuringplanes" + suffix; }
//        else if (!string_utils::ends_with(fname, suffix))
//        { fname.append(suffix); }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint32_t ui32temp = 0;
//        double        dtemp    = 0;
//        std::uint8_t  ui8temp  = 0;
//
//        // num measuring planes
//        ui32temp = static_cast<std::uint32_t>(measuring_planes().size());
//        file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//        // measuring planes
//        for (unsigned int i = 0; i < measuring_planes().size(); ++i)
//        {
//            const auto& mp = *measuring_plane(i);
//
//            // vessel id
//            ui8temp = mp.vesselID();
//            file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));
//
//            // grid size
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                ui32temp = mp.geometry().size(s);
//                file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//            }
//
//            // center
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                dtemp = mp.geometry().transformation().center()[s];
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // scale
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                dtemp = mp.geometry().transformation().scale(s);
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // nx
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                dtemp = mp.geometry().transformation().nx()[s];
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // ny
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                dtemp = mp.geometry().transformation().ny()[s];
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // nz
//            for (unsigned int s = 0; s < 3; ++s)
//            {
//                dtemp = mp.geometry().transformation().nz()[s];
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // diameter
//            dtemp = mp.diameter_mm();
//            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//
//            // velocity vectors (N = grid_size[0] * grid_size[1] * grid_size[2])
//            for (unsigned int k = 0; k < mp.num_values(); ++k)
//            {
//                // vvec
//                for (unsigned int s = 0; s < 3; ++s)
//                {
//                    dtemp = mp[k][s];
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                }
//            }
//
//            // through-plane velocity (N = grid_size[0] * grid_size[1] * grid_size[2])
//            for (unsigned int k = 0; k < mp.num_values(); ++k)
//            {
//                const auto vtp_ptr = mp.point_attributes().get_velocity_throughplane_value(k);
//                dtemp = vtp_ptr != nullptr ? *vtp_ptr : 0;
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//            }
//
//            // segmentation
//            for (unsigned int k = 0; k < mp.num_values(); ++k)
//            {
//                ui8temp = mp.segmentation_cross_section()[k] != 0 ? 1 : 0;
//                file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));
//            }
//        }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::load_measuringplanes(const std::string& filepath)
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, 3, ___("loading measuring planes"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check file ending
//        //------------------------------------------------------------------------------------------------------
//        const std::string suffix = ".mp";
//        if (!string_utils::ends_with(filepath, suffix))
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        //------------------------------------------------------------------------------------------------------
//        // open file
//        //------------------------------------------------------------------------------------------------------
//        std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // num measuring planes
//        std::uint32_t numMeasuringPlanes = 0;
//        file.read(reinterpret_cast<char*>(&numMeasuringPlanes), sizeof(std::uint32_t));
//        _pdata->measuring_planes.resize(numMeasuringPlanes);
//
//        // measuring planes
//        for (unsigned int i = 0; i < numMeasuringPlanes; ++i)
//        {
//            auto& mp = *measuring_plane(i);
//
//            // vessel id
//            std::uint8_t id = 0;
//            file.read(reinterpret_cast<char*>(&id), sizeof(std::uint8_t));
//            mp.set_vesselID(id);
//
//            // grid size
//            std::uint32_t gs[3] = {0, 0, 0};
//            file.read(reinterpret_cast<char*>(gs), 3 * sizeof(std::uint32_t));
//            mp.set_size(gs[0], gs[1], gs[2]);
//            mp.geometry().transformation().set_grid_size(gs[0], gs[1], gs[2]);
//
//            const unsigned int  N = 5 * 3 + mp.num_values() * 4 + 1;
//            std::vector<double> dbuf(N);
//
//            file.read(reinterpret_cast<char*>(dbuf.data()), N * sizeof(double));
//
//            auto fut_set_mp = bk_threadpool.enqueue([&]()
//                                                    {
//                                                        unsigned int cnt = 0;
//
//                                                        // center
//                                                        mp.geometry().transformation().set_center(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                        cnt += 3;
//
//                                                        // scale
//                                                        mp.geometry().transformation().set_scale(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                        cnt += 3;
//
//                                                        // nx
//                                                        mp.geometry().transformation().set_nx(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                        cnt += 3;
//
//                                                        // ny
//                                                        mp.geometry().transformation().set_ny(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                        cnt += 3;
//
//                                                        // nz
//                                                        mp.geometry().transformation().set_nz(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                        cnt += 3;
//
//                                                        // diameter
//                                                        mp.set_diameter_mm(dbuf[cnt++]);
//
//                                                        // velocity vectors (N = grid_size[0] * grid_size[1] * grid_size[2])
//                                                        for (unsigned int k = 0; k < mp.num_values(); ++k)
//                                                        {
//                                                            mp[k].set(dbuf[cnt], dbuf[cnt + 1], dbuf[cnt + 2]);
//                                                            cnt += 3;
//                                                        }
//
//                                                        // through-plane velocity (N = grid_size[0] * grid_size[1] * grid_size[2])
//                                                        for (unsigned int k = 0; k < mp.num_values(); ++k)
//                                                        { mp.point_attributes().set_velocity_throughplane_value(k, dbuf[cnt++]); }
//                                                    });
//
//            // segmentation
//            CartesianImage<std::uint8_t, 3> seg;
//            seg.set_size(gs[0], gs[1], gs[2]);
//
//            std::vector<std::uint8_t> ui8buf(mp.num_values());
//            file.read(reinterpret_cast<char*>(ui8buf.data()), mp.num_values() * sizeof(std::uint8_t));
//
//            #pragma omp parallel for
//            for (unsigned int k = 0; k < mp.num_values(); ++k)
//            { seg[k] = ui8buf[k]; }
//
//            mp.set_measuring_plane_segmentation(seg);
//
//            fut_set_mp.get();
//        }
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::save_flowjets(const std::string& filepath) const
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog           = bk_progress.emplace_task(0, 3, ___("saving flow jets"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check filename
//        //------------------------------------------------------------------------------------------------------
//        std::string       fname(filepath);
//        const std::string suffix = ".fj";
//        if (fname.empty())
//        { fname = "flowjets" + suffix; }
//        else if (!string_utils::ends_with(fname, suffix))
//        { fname.append(suffix); }
//
//        //------------------------------------------------------------------------------------------------------
//        // create file
//        //------------------------------------------------------------------------------------------------------
//        std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        // temp
//        std::uint8_t  ui8temp  = 0;
//        std::uint32_t ui32temp = 0;
//        double        dtemp    = 0;
//
//        // num jets
//        ui8temp = num_flowjets();
//        file.write(reinterpret_cast<char*>(&ui8temp), sizeof(std::uint8_t));
//
//        constexpr const double _invalid = std::numeric_limits<double>::max();
//
//        for (const FlowJet_CMR& fj: _pdata->flowjets.flowjets())
//        {
//            // num planes
//            ui32temp = fj.num_positions();
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//            // num times
//            ui32temp = fj.num_times();
//            file.write(reinterpret_cast<char*>(&ui32temp), sizeof(std::uint32_t));
//
//            for (unsigned int pid = 0; pid < fj.num_positions(); ++pid)
//            {
//                // vessel center
//                const auto        vcenter = fj.vessel_center(pid);
//                for (unsigned int i: {0, 1, 2})
//                {
//                    dtemp = vcenter != nullptr ? (*vcenter)[i] : _invalid;
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                }
//
//                // vessel radius
//                dtemp = *fj.vessel_radius(pid);
//                file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//
//                // jet lcs x
//                const auto        jlcs = fj.jet_local_coord_sys(pid);
//                for (unsigned int i: {0, 1, 2})
//                {
//                    dtemp = jlcs != nullptr ? jlcs->first[i] : _invalid;
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                }
//
//                // jet lcs y
//                for (unsigned int i: {0, 1, 2})
//                {
//                    dtemp = jlcs != nullptr ? jlcs->second[i] : _invalid;
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                }
//
//                for (unsigned int tid = 0; tid < fj.num_times(); ++tid)
//                {
//                    // jet center
//                    const auto        jcenter = fj.jet_position(pid, tid);
//                    for (unsigned int i: {0, 1, 2})
//                    {
//                        dtemp = jcenter != nullptr ? (*jcenter)[i] : _invalid;
//                        file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                    }
//
//                    // jet velocity
//                    dtemp = *fj.jet_velocity(pid, tid);
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//
//                    // area center
//                    const auto        acenter = fj.area_center(pid, tid);
//                    for (unsigned int i: {0, 1, 2})
//                    {
//                        dtemp = acenter != nullptr ? (*acenter)[i] : _invalid;
//                        file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                    }
//
//                    // area radii
//                    const auto aradii = fj.area_radii(pid, tid);
//                    dtemp = aradii != nullptr ? aradii->first : _invalid;
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                    dtemp = aradii != nullptr ? aradii->second : _invalid;
//                    file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//
//                    // area dirs
//                    const auto        adirs = fj.area_dirs(pid, tid);
//                    for (unsigned int i: {0, 1, 2})
//                    {
//                        dtemp = adirs != nullptr ? adirs->first[i] : _invalid;
//                        file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                    }
//
//                    for (unsigned int i: {0, 1, 2})
//                    {
//                        dtemp = adirs != nullptr ? adirs->second[i] : _invalid;
//                        file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
//                    }
//                } // for tid : fj.num_times()
//            } // for pid : fj.num_positions()
//        } // for fjid : num_flowjets()
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//
//    bool Vessel::load_flowjets(const std::string& filepath)
//    {
//        #ifdef BK_EMIT_PROGRESS
//        Progress& prog = bk_progress.emplace_task(0, 3, ___("loading flow jets"));
//        #endif
//
//        //------------------------------------------------------------------------------------------------------
//        // check file ending
//        //------------------------------------------------------------------------------------------------------
//        const std::string suffix = ".fj";
//        if (!string_utils::ends_with(filepath, suffix))
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        //------------------------------------------------------------------------------------------------------
//        // open file
//        //------------------------------------------------------------------------------------------------------
//        std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
//        if (!file.good())
//        {
//            #ifdef BK_EMIT_PROGRESS
//            prog.set_finished();
//            #endif
//            return false;
//        }
//
//        clear_flowjets();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.increment(1);
//        #endif
//
//        std::uint8_t numFlowjets = 0;
//        file.read(reinterpret_cast<char*>(&numFlowjets), sizeof(std::uint8_t));
//
//        _pdata->flowjets.set_num_flowjets(numFlowjets);
//
//        for (unsigned int fjid = 0; fjid < numFlowjets; ++fjid)
//        {
//            FlowJet_CMR& fj = *_pdata->flowjets[fjid];
//
//            // num planes
//            std::uint32_t numPlanes = 0;
//            file.read(reinterpret_cast<char*>(&numPlanes), sizeof(std::uint32_t));
//
//            // num times
//            std::uint32_t numTimes = 0;
//            file.read(reinterpret_cast<char*>(&numTimes), sizeof(std::uint32_t));
//
//            fj.set_num_positions_and_times(numPlanes, numTimes);
//
//            for (unsigned int pid = 0; pid < fj.num_positions(); ++pid)
//            {
//                // vessel center
//                double buf_vcenter[3] = {0, 0, 0};
//                file.read(reinterpret_cast<char*>(buf_vcenter), 3 * sizeof(double));
//
//                // vessel radius
//                double vradius = *fj.vessel_radius(pid);
//                file.read(reinterpret_cast<char*>(&vradius), sizeof(double));
//
//                // jet lcs
//                double buf_lcsx[3] = {0, 0, 0};
//                file.read(reinterpret_cast<char*>(buf_lcsx), 3 * sizeof(double));
//
//                double buf_lcsy[3] = {0, 0, 0};
//                file.read(reinterpret_cast<char*>(buf_lcsy), 3 * sizeof(double));
//
//                for (unsigned int tid = 0; tid < fj.num_times(); ++tid)
//                {
//                    // jet center
//                    double buf_jcenter[3] = {0, 0, 0};
//                    file.read(reinterpret_cast<char*>(buf_jcenter), 3 * sizeof(double));
//
//                    // jet velocity
//                    double jvelocity = 0;
//                    file.read(reinterpret_cast<char*>(&jvelocity), sizeof(double));
//
//                    // area center
//                    double buf_acenter[3] = {0, 0, 0};
//                    file.read(reinterpret_cast<char*>(buf_acenter), 3 * sizeof(double));
//
//                    // area radii
//                    double arad0 = 0;
//                    file.read(reinterpret_cast<char*>(&arad0), sizeof(double));
//
//                    double arad1 = 0;
//                    file.read(reinterpret_cast<char*>(&arad1), sizeof(double));
//
//                    // area dirs
//                    double buf_adir0[3] = {0, 0, 0};
//                    file.read(reinterpret_cast<char*>(buf_adir0), 3 * sizeof(double));
//
//                    double buf_adir1[3] = {0, 0, 0};
//                    file.read(reinterpret_cast<char*>(buf_adir1), 3 * sizeof(double));
//
//                    fj.set_center(pid, tid, Vec3d(buf_jcenter[0], buf_jcenter[1], buf_jcenter[2]), jvelocity, Vec3d(buf_lcsx[0], buf_lcsx[1], buf_lcsx[2]), Vec3d(buf_lcsy[0], buf_lcsy[1], buf_lcsy[2]), Vec3d(buf_acenter[0], buf_acenter[1], buf_acenter[2]), arad0, arad1, Vec3d(buf_adir0[0], buf_adir0[1], buf_adir0[2]), Vec3d(buf_adir1[0], buf_adir1[1], buf_adir1[2]), Vec3d(buf_vcenter[0], buf_vcenter[1], buf_vcenter[2]), vradius);
//                } // for tid : fj.num_times()
//            } // for pid : fj.num_positions()
//        } // for fjid: numFlowhjets
//
//        file.close();
//
//        #ifdef BK_EMIT_PROGRESS
//        prog.set_finished();
//        #endif
//
//        return true;
//    }
//  } // inline namespace cmr
//} // namespace bk
//
//
