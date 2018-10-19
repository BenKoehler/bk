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

#pragma once

#ifndef BKCMR_VESSEL_H
#define BKCMR_VESSEL_H

#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bk/KDTree>
#include <bk/Image>
#include <bk/Mesh>
#include <bk/Line>

#include <bkCMR/dataset/ESegmentation3DInfo.h>
#include <bkCMR/dataset/EVesselSemantic.h>
#include <bkCMR/dataset/ELandMarkSemantic.h>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  // -------------------- forward declaration
  //class TriangularMesh3D;

  inline namespace cmr
  {
    class FlowJet;

    class MeasuringPlane;

    class LandMark;
    // -------------------- forward declaration END

    class BKCMR_EXPORT Vessel
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
      public:
        using segmentation3d_type = bk::DicomImage<unsigned char, 3>;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      private:
        class Impl;

        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        Vessel();
        Vessel(const Vessel&);
        Vessel(Vessel&&) noexcept;
        ~Vessel();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] const std::string& name() const;
        [[nodiscard]] VesselSemantic semantic() const;
        [[nodiscard]] bool is_semantic(VesselSemantic sem) const;
        [[nodiscard]] bool is_semantic_aorta() const;
        [[nodiscard]] bool is_semantic_left_ventricle() const;
        [[nodiscard]] bool is_semantic_left_atrium() const;
        [[nodiscard]] bool is_semantic_pulmonary_veins() const;
        [[nodiscard]] bool is_semantic_left_heart() const;
        [[nodiscard]] bool is_semantic_pulmonary_artery() const;
        [[nodiscard]] bool is_semantic_right_ventricle() const;
        [[nodiscard]] bool is_semantic_right_atrium() const;
        [[nodiscard]] bool is_semantic_vena_cava() const;
        [[nodiscard]] bool is_semantic_right_heart() const;

        [[nodiscard]] bool has_land_marks() const;
        [[nodiscard]] bool has_land_mark(LandMarkSemantic sem) const;
        [[nodiscard]] int id_of_land_mark(LandMarkSemantic sem) const; // returns -1 if no such land mark is found
        [[nodiscard]] unsigned int num_land_marks() const;
        [[nodiscard]] const std::vector<LandMark>& land_marks() const;
        [[nodiscard]] const LandMark& land_mark(unsigned int id) const;

        [[nodiscard]] bool has_segmentation3D() const;
        [[nodiscard]] segmentation3d_type& segmentation3D()&;
        [[nodiscard]] const segmentation3d_type& segmentation3D() const&;
        [[nodiscard]] segmentation3d_type&& segmentation3D()&&;
        [[nodiscard]] const std::pair<Segmentation3DInfo_, unsigned int>& segmentation3D_info() const;
        [[nodiscard]] std::vector<unsigned int>& segmentation3D_inside_ids();
        [[nodiscard]] const std::vector<unsigned int>& segmentation3D_inside_ids() const;
        [[nodiscard]] std::vector<unsigned int>& segmentation3D_outside_ids();
        [[nodiscard]] const std::vector<unsigned int>& segmentation3D_outside_ids() const;

        [[nodiscard]] bool has_mesh() const;
        [[nodiscard]] bk::TriangularMesh3D& mesh()&;
        [[nodiscard]] const bk::TriangularMesh3D& mesh() const&;
        [[nodiscard]] bk::TriangularMesh3D&& mesh()&&;
        [[nodiscard]] std::pair<double,double> min_max_scalar_mesh_point_attribute(std::string_view attribute_name) const;

        [[nodiscard]] bool has_pathlines() const;
        [[nodiscard]] unsigned int num_pathlines() const;
        [[nodiscard]] const std::vector<bk::Line3D>& pathlines() const;
        [[nodiscard]] std::vector<bk::Line3D>& pathlines();
        [[nodiscard]] std::pair<double,double> min_max_scalar_pathline_point_attribute(std::string_view attribute_name) const;

        [[nodiscard]] unsigned int num_centerlines() const;
        [[nodiscard]] std::vector<Line3D>& centerlines()&;
        [[nodiscard]] const std::vector<Line3D>& centerlines() const&;
        [[nodiscard]] std::vector<Line3D>&& centerlines()&&;

        [[nodiscard]] bool has_centerline_ids() const;
        [[nodiscard]] unsigned int centerline_seed_id() const;
        [[nodiscard]] const std::vector<unsigned int>& centerline_target_ids() const;
        [[nodiscard]] bool has_centerlines() const;

        [[nodiscard]] std::pair<int, bk::KDPointInfo<bk::Vec3d>> closest_centerline_and_point_id(const bk::Vec3d& pt);

        [[nodiscard]] std::vector<MeasuringPlane>& measuring_planes();
        [[nodiscard]] const std::vector<MeasuringPlane>& measuring_planes() const;
        [[nodiscard]] bool has_measuring_planes() const;
        [[nodiscard]] unsigned int num_measuring_planes() const;
        [[nodiscard]] std::pair<double,double> min_max_scalar_measuring_plane_point_attribute(std::string_view attribute_name) const;

        [[nodiscard]] std::vector<FlowJet>& flowjets();
        [[nodiscard]] const std::vector<FlowJet>& flowjets() const;
        [[nodiscard]] unsigned int num_flowjets() const;
        [[nodiscard]] bool has_flowjets() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] Vessel& operator=(const Vessel&);
        [[maybe_unused]] Vessel& operator=(Vessel&&) noexcept;

        void clear();
        void clear_semantic();
        void clear_segmentation();
        void clear_mesh();
        void clear_pathlines();
        void clear_centerlines();
        void clear_centerline_ids();
        void clear_measuring_planes();
        void clear_flowjets();

        void set_name(std::string_view name);
        void set_name_from_semantic();
        [[nodiscard]] static std::string Name_from_semantic(VesselSemantic s);

        void set_semantic(VesselSemantic sem);
        void add_semantic(VesselSemantic sem);

        void add_land_mark(LandMarkSemantic sem, unsigned int centerline_id, unsigned int point_id);

        void set_seg3d_was_performed_on_magnitude_TMIP();
        void set_seg3d_was_performed_on_LPC();
        void set_seg3d_was_performed_on_IVSD();
        void set_seg3d_was_performed_on_3D_anatomical_image(unsigned int id);
        void set_seg3d_was_performed_on_4D_anatomical_image_TMIP(unsigned int id);
        void set_seg3d_was_performed_on_4D_signal_intensity_image_TMIP();

        void set_centerline_seed_id(unsigned int seedid);
        void set_centerline_target_ids(std::vector<unsigned int> targetids);
        void add_centerline_target_id(unsigned int targetid);

        void add_centerline(const Line3D& cl);
        void add_centerline(Line3D&& cl);
        void add_centerlines(std::vector<Line3D>::iterator first, std::vector<Line3D>::iterator last);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        //void calc_pathlines_angles_to_centerline();

        //====================================================================================================
        //===== I/O
        //====================================================================================================
        [[maybe_unused]] bool save_mesh(std::string_view filepath) const;
        [[maybe_unused]] bool load_mesh(std::string_view filepath);
        [[maybe_unused]] bool save_segmentation3D(std::string_view filepath) const;
        [[maybe_unused]] bool load_segmentation3D(std::string_view filepath);
        [[maybe_unused]] bool save_centerline_ids(std::string_view filepath) const;
        [[maybe_unused]] bool load_centerline_ids(std::string_view filepath);
        [[maybe_unused]] bool save_centerlines(std::string_view filepath) const;
        [[maybe_unused]] bool load_centerlines(std::string_view filepath);
        [[maybe_unused]] bool save_land_marks(std::string_view filepath) const;
        [[maybe_unused]] bool load_land_marks(std::string_view filepath);
        [[maybe_unused]] bool save_measuringplanes(std::string_view filepath) const;
        [[maybe_unused]] bool load_measuringplanes(std::string_view filepath);
        [[maybe_unused]] bool save_flowjets(std::string_view filepath) const;
        [[maybe_unused]] bool load_flowjets(std::string_view filepath);

        //void save(const std::string& filepath) const;
        //void save_pathlines(const std::string& filepath) const;
        //void load(const std::string& filepath);
        //void load_pathlines(const std::string& filepath);
    }; // class Vessel
  } // inline namespace cmr
} // namespace bk

#endif //BKCMR_VESSEL_H


