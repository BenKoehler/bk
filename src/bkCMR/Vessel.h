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
//#pragma once
//
//#ifndef BKCMR_VESSEL_H
//#define BKCMR_VESSEL_H
//
//#include <fstream>
//#include <memory>
//#include <string>
//#include <string_view>
//#include <utility>
//#include <vector>
//
//#include <bk/Matrix>
//#include <bk/KDTree>
//#include <bk/Image>
//#include <bk/Mesh>
//#include <bk/Line>
//
//#include "Segmentation3DInfo.h"
//#include "lib/export.h"
//
//namespace bk
//{
//  inline namespace cmr
//  {
//    // -------------------- forward declaration
//    //class DicomSegmentation3D;
//
//    //class FlowJetSet_CMR;
//
//    //class FlowJet_CMR;
//
//    //class LineSet3;
//
//    //class MeasuringPlane_CMR;
//
//    //class TriangularMesh3D;
//    // -------------------- forward declaration END
//
//    class BKCMR_EXPORT Vessel
//    {
//        //====================================================================================================
//        //===== DEFINITIONS
//        //====================================================================================================
//      public:
//        using segmentation3d_type = bk::DicomImage<unsigned char, 3>;
//        using mesh_type = bk::TriangularMesh3D;
//        using pathline_type = bk::Line3D;
//        using pathlineset_type = std::vector<pathline_type>;
//        using centerline_type = bk::Line3D; // todo branching centerline
//        using centerlineset_type = std::vector<centerline_type>;
//        //using   measuring_plane_type = MeasuringPlane_CMR;
//        //using        flowjetset_type = FlowJetSet_CMR;
//
//        //====================================================================================================
//        //===== MEMBERS
//        //====================================================================================================
//      private:
//        class Impl;
//
//        std::unique_ptr<Impl> _pdata;
//
//        //====================================================================================================
//        //===== CONSTRUCTORS & DESTRUCTOR
//        //====================================================================================================
//      public:
//        Vessel ();
//        Vessel (const Vessel&);
//        Vessel (Vessel&&) noexcept;
//        ~Vessel ();
//
//        //====================================================================================================
//        //===== GETTER
//        //====================================================================================================
//        [[nodiscard]] const std::string& name () const;
//
//        [[nodiscard]] bool has_segmentation3D () const;
//        [[nodiscard]] const segmentation3d_type& segmentation3D () const;
//        [[nodiscard]] segmentation3d_type& segmentation3D ();
//        [[nodiscard]] const std::pair<Segmentation3DInfo, unsigned int>& segmentation3D_info () const;
//        [[nodiscard]] std::vector<unsigned int>& segmentation3D_inside_ids ();
//        [[nodiscard]] const std::vector<unsigned int>& segmentation3D_inside_ids () const;
//        [[nodiscard]] std::vector<unsigned int>& segmentation3D_outside_ids ();
//        [[nodiscard]] const std::vector<unsigned int>& segmentation3D_outside_ids () const;
//
//        [[nodiscard]] bool has_mesh () const;
//        [[nodiscard]] mesh_type& mesh ();
//        [[nodiscard]] const mesh_type& mesh () const;
//
//        [[nodiscard]] bool has_pathlines () const;
//        [[nodiscard]] unsigned int num_pathlines () const;
//        [[nodiscard]] const pathlineset_type& pathlines () const;
//        [[nodiscard]] pathlineset_type& pathlines ();
//
//        [[nodiscard]] unsigned int num_centerlines () const;
//        [[nodiscard]] const centerlineset_type& centerlines () const;
//        [[nodiscard]] centerlineset_type& centerlines ();
//
//        [[nodiscard]] bool has_centerline_ids () const;
//        [[nodiscard]] unsigned int centerline_seed_id () const;
//        [[nodiscard]] const std::vector<unsigned int>& centerline_tarids () const;
//
//        //[[nodiscard]] auto measuring_planes() -> std::vector<measuring_plane_type>&;
//        //[[nodiscard]] auto measuring_planes() const -> const std::vector<measuring_plane_type>&;
//        //[[nodiscard]] auto measuring_plane(unsigned int i) const -> const measuring_plane_type*;
//        //[[nodiscard]] auto measuring_plane(unsigned int i) -> measuring_plane_type*;
//        //[[nodiscard]] bool has_measuring_planes() const;
//        //[[nodiscard]] unsigned int num_measuring_planes() const;
//        //
//        //[[nodiscard]] auto flowjets() -> flowjetset_type&;
//        //[[nodiscard]] auto flowjets() const -> const flowjetset_type&;
//        //[[nodiscard]] unsigned int num_flowjets() const;
//        //[[nodiscard]] bool has_flowjets();
//        //[[nodiscard]] FlowJet* flowjet(unsigned int i);
//        //[[nodiscard]] const FlowJet* flowjet(unsigned int i) const;
//
//        [[nodiscard]] std::pair<int, bk::KDPointInfo<bk::Vec3d>> closest_centerline_and_point_id (const bk::Vec3d& pt);
//
//        //====================================================================================================
//        //===== SETTER
//        //====================================================================================================
//        [[maybe_unused]] Vessel& operator= (const Vessel&);
//        [[maybe_unused]] Vessel& operator= (Vessel&&) noexcept;
//
//        [[noreturn]] void clear ();
//        [[noreturn]] void clear_segmentation ();
//        [[noreturn]] void clear_mesh ();
//        [[noreturn]] void clear_pathlines ();
//        [[noreturn]] void clear_centerlines ();
//        [[noreturn]] void clear_centerline_ids ();
//        [[noreturn]] void clear_measuring_planes ();
//        [[noreturn]] void clear_flowjets ();
//
//        [[noreturn]] void set_name (const std::string& name);
//
//        [[noreturn]] void set_seg3d_was_performed_on_magnitude_TMIP ();
//        [[noreturn]] void set_seg3d_was_performed_on_LPC ();
//        [[noreturn]] void set_seg3d_was_performed_on_IVSD ();
//        [[noreturn]] void set_seg3d_was_performed_on_3D_anatomical_image (unsigned int id);
//        [[noreturn]] void set_seg3d_was_performed_on_4D_anatomical_image_TMIP (unsigned int id);
//        [[noreturn]] void set_seg3d_was_performed_on_4D_signal_intensity_image_TMIP ();
//
//        [[noreturn]] void set_centerline_seed_id (unsigned int seedid);
//        [[noreturn]] void set_centerline_target_ids (std::vector<unsigned int> targetids);
//        [[noreturn]] void add_centerline_target_id (unsigned int targetid);
//
//        //====================================================================================================
//        //===== FUNCTIONS
//        //====================================================================================================
//        [[noreturn]] void calc_pathlines_angles_to_centerline ();
//
//        //====================================================================================================
//        //===== I/O
//        //====================================================================================================
//        [[maybe_unused]] bool save_mesh (std::string_view filepath) const;
//        [[maybe_unused]] bool load_mesh (std::string_view filepath);
//        [[maybe_unused]] bool save_segmentation3D (std::string_view filepath) const;
//        [[maybe_unused]] bool load_segmentation3D (std::string_view filepath);
//        [[maybe_unused]] bool save_centerline_ids (std::string_view filepath) const;
//        [[maybe_unused]] bool load_centerline_ids (std::string_view filepath);
//        [[maybe_unused]] bool save_centerlines (std::string_view filepath) const;
//        [[maybe_unused]] bool load_centerlines (std::string_view filepath);
//        [[maybe_unused]] bool save_measuringplanes (std::string_view filepath) const;
//        [[maybe_unused]] bool load_measuringplanes (std::string_view filepath);
//        [[maybe_unused]] bool save_flowjets (std::string_view filepath) const;
//        [[maybe_unused]] bool load_flowjets (std::string_view filepath);
//
//        //void save(const std::string& filepath) const; // todo
//        //void save_pathlines(const std::string& filepath) const; // todo
//        //void load(const std::string& filepath); // todo
//        //void load_pathlines(const std::string& filepath); // todo
//
//    }; // class Vessel
//  } // inline namespace cmr
//} // namespace bk
//
//#endif //BKCMR_VESSEL_H
//
//
