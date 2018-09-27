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

#ifndef BK_DATASET_H
#define BK_DATASET_H

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bk/Matrix>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  // -------------------- forward declaration
  class DicomDirImporter_CMR;

  inline namespace cmr
  {
    // -------------------- forward declaration END
    class FlowDirCorrection;

    class FlowImage2DT;

    class FlowImage3DT;

    class PressureMapImageFilter;

    class Vessel;

    class PhaseUnwrapping2DT;

    class PhaseUnwrapping3DT;

    enum DatasetFilter_ : unsigned int
    {
        DatasetFilter_PhaseUnwrapping   = 1 << 0, //
        DatasetFilter_VelocityOffset    = 1 << 1, //
        DatasetFilter_FlowDirCorrection = 1 << 2, //
        //
        DatasetFilter_None              = 0, //
        DatasetFilter_All               = DatasetFilter_PhaseUnwrapping | DatasetFilter_VelocityOffset | DatasetFilter_FlowDirCorrection, //
    };

    class BKCMR_EXPORT Dataset
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        static const std::string vessel_dir;

      public:
        static const std::string dcmbytes;

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
        Dataset();
        Dataset(const Dataset&);
        Dataset(Dataset&&);
        ~Dataset();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] const std::string& project_path() const;
        [[nodiscard]] std::string project_path_without_slash_ending() const;

        [[nodiscard]] DicomDirImporter_CMR& importer();
        [[nodiscard]] const DicomDirImporter_CMR& importer() const;

        [[nodiscard]] bool has_magnitude_images() const;
        [[nodiscard]] bool has_signal_intensity_image() const;
      private:
        [[nodiscard]] std::vector<unsigned int> ids_of_local_image_copies() const;

      public:
        [[nodiscard]] bool has_local_image_copies() const;
        [[nodiscard]] bool is_flow_image_3dt_loaded() const;

        [[nodiscard]] FlowImage3DT* flow_image_3dt();
        [[nodiscard]] const FlowImage3DT* flow_image_3dt() const;

        [[nodiscard]] unsigned int num_vessels() const;
        [[nodiscard]] Vessel* vessel(unsigned int i);
        [[nodiscard]] Vessel* vessel(const std::string& name, bool case_sensitive = true);
        [[nodiscard]] const Vessel* vessel(unsigned int i) const;
        [[nodiscard]] const Vessel* vessel(const std::string& name, bool case_sensitive = true) const;

        [[nodiscard]] bool vessel_has_centerline_ids(const Vessel* v) const;
        [[nodiscard]] bool vessel_has_centerlines(const Vessel* v) const;
        [[nodiscard]] bool vessels_have_centerline_ids() const;
        [[nodiscard]] bool vessels_have_centerlines() const;

        //bool has_pathline_point_attribute(const std::string& attributeName) const;
        //bool has_pathline_cell_attribute(const std::string& attributeName) const;
        //bool has_pathline_object_attribute(const std::string& attributeName) const;
        //double pathline_scalar_point_attribute_quantile(const std::string& attributeName, double p) const;
        //std::pair<double, double> pathline_scalar_point_attribute_interquartile_range(const std::string& attributeName) const;

        [[nodiscard]] FlowDirCorrection& flow_image_3dt_dir_correction();
        [[nodiscard]] const FlowDirCorrection& flow_image_3dt_dir_correction() const;

        [[nodiscard]] const PhaseUnwrapping3DT& phase_unwrapping_3dt() const;

        [[nodiscard]] const std::map<unsigned int, PhaseUnwrapping2DT>& phase_unwrapping_2dt() const;
        [[nodiscard]] const PhaseUnwrapping2DT* phase_unwrapping_2dt(unsigned int imgId) const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] Dataset& operator=(const Dataset&);
        [[maybe_unused]] Dataset& operator=(Dataset&&);

        void set_project_path(std::string_view path);

        [[maybe_unused]] Vessel* add_vessel(std::string_view name);
        [[maybe_unused]] Vessel* add_vessel(Vessel&& v);
        [[maybe_unused]] bool remove_vessel(Vessel* v);

        //====================================================================================================
        //===== IMAGES
        //====================================================================================================
      private:
        [[nodiscard]] std::string filepath_flow_image(unsigned int v) const;
        [[nodiscard]] std::string filepath_tmip_magnitude_3dt() const;
        [[nodiscard]] std::string filepath_lpc() const;
        [[nodiscard]] std::string filepath_ivsd() const;
        [[nodiscard]] std::string filepath_tmip_signal_3dt() const;
        [[nodiscard]] std::string filepath_tmip_anatomical_3dt(unsigned int imgId) const;
        [[nodiscard]] std::vector<std::string> filepaths_of_local_image_copies() const;
        [[nodiscard]] std::string filepath_pressure_map_of_vessel(const Vessel& v) const;

        [[nodiscard]] bool has_local_image_copy(std::string_view filepath) const;
        [[nodiscard]] bool has_local_image_copy_dcmbytes(unsigned int imgId) const;

        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> load_local_image_copy(std::string_view filepath) const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, -1>> load_local_image_copy_dcmbytes(unsigned int imgId) const;
      public:

        [[maybe_unused]] bool load_flow_image_3dt(DatasetFilter_ flags = DatasetFilter_All);

        [[nodiscard]] std::vector<std::unique_ptr<FlowImage2DT>> flow_images_2dt(DatasetFilter_ flags = DatasetFilter_All);
        [[nodiscard]] std::unique_ptr<FlowImage2DT> flow_image_2dt(unsigned int dcm_id, DatasetFilter_ flags = DatasetFilter_All);

        [[nodiscard]] int anatomical_2dt_image_id_of_flow_image_2dt(unsigned int flowimg_dcm_id, bool* success = nullptr);

        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> lpc() const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> ivsd() const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> tmip_magnitude_3dt() const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> tmip_signal_intensity_3dt() const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> tmip_anatomical_3dt(unsigned int dcmImgId) const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 4>> pressure_map(PressureMapImageFilter pmf) const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 4>> pressure_map() const;
        [[nodiscard]] std::unique_ptr<DicomImage<double, 3>> vessel_segmentation_in_flow_field_3dt_size(const Vessel& v) const;

        // todo: 2dt centerline cuts

        //====================================================================================================
        //===== FILTERS
        //====================================================================================================
        [[maybe_unused]] bool determine_phase_wraps_2dt();
        [[maybe_unused]] bool determine_phase_wraps_3dt(bool reload_flow_image = true);

        // todo velocity offset correction 3dt
        // todo velocity offset correction 2dt

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();
        void delete_local_image_copies_if_incomplete() const;
        void delete_local_image_copies() const;

        //====================================================================================================
        //===== I/O
        //====================================================================================================
      private:
        [[nodiscard]] std::string filepath_importer() const;
        [[nodiscard]] std::string filepath_flow_dir_correction() const;
        [[nodiscard]] std::string filepath_phase_unwrapping_2dt() const;
        [[nodiscard]] std::string filepath_phase_unwrapping_3dt() const;
      public:

        [[maybe_unused]] bool save_local_dcmbyte_image_copies() const;
        [[maybe_unused]] bool save_local_image_copy(std::string_view filepath, const DicomImage<double, 3>& img) const;

        [[maybe_unused]] bool save_importer() const;
        [[maybe_unused]] bool load_importer();

        [[maybe_unused]] bool save_pressure_map(PressureMapImageFilter pmf) const;
        [[maybe_unused]] bool save_pressure_map() const;

        [[maybe_unused]] bool save_flow_dir_correction();
        [[maybe_unused]] bool load_flow_dir_correction();

        [[maybe_unused]] bool save_phase_unwrapping_2dt();
        [[maybe_unused]] bool load_phase_unwrapping_2dt();

        [[maybe_unused]] bool save_phase_unwrapping_3dt();
        [[maybe_unused]] bool load_phase_unwrapping_3dt();

        [[maybe_unused]] bool save_ivsd();
        [[maybe_unused]] bool save_magnitude_tmip_3dt();
        [[maybe_unused]] bool save_anatomical_tmip_3dt(unsigned int imgId);
    }; // class Dataset
  } // inline namespace cmr
} // namespace bk

#endif //BK_DATASET_H
