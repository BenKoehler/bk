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

namespace bk
{
  // -------------------- forward declaration
  class DicomDirImporter_CMR;
  class Vessel;
  // -------------------- forward declaration END

  inline namespace cmr
  {
    class Dataset
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        static const std::string dcmbytes;

      public:

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

        [[nodiscard]] DicomDirImporter_CMR& importer();
        [[nodiscard]] const DicomDirImporter_CMR& importer() const;

        [[nodiscard]] bool has_magnitude_images() const;
        [[nodiscard]] bool has_signal_intensity_image() const;
        [[nodiscard]] bool has_local_image_copies() const;
        [[nodiscard]] bool has_flow_dir_correction() const;
        [[nodiscard]] bool is_flow_image_3dt_loaded() const;

        [[nodiscard]] bk::DicomImage<bk::Vec3d, 4>* flow_image_3dt();
        [[nodiscard]] const bk::DicomImage<bk::Vec3d, 4>* flow_image_3dt() const;

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

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] Dataset& operator=(const Dataset&);
        [[maybe_unused]] Dataset& operator=(Dataset&&);

        void set_project_path(std::string_view path);

        void set_flow_image_3dt_dir_correction(int cx, int cy, int cz);

        [[maybe_unused]] Vessel* add_vessel(std::string_view name);
        [[maybe_unused]] Vessel* add_vessel(Vessel&& v);
        [[maybe_unused]] bool remove_vessel(Vessel* v);

        //====================================================================================================
        //===== IMAGES
        //====================================================================================================
        void load_flow_image_3dt(bool unwrap_phases_if_available = true, bool correct_eddy_currents_if_available = true);

        [[nodiscard]] std::vector<std::unique_ptr<DicomImage<double>>> flow_images_2dt(bool unwrap_phases_if_available = true, bool correct_eddy_currents_if_available = true);
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> flow_image_2dt(unsigned int dcm_id, bool unwrap_phases_if_available = true, bool correct_eddy_currents_if_available = true);

        [[nodiscard]] int anatomical_2dt_image_id_of_flow_image_2dt(unsigned int flowimg_dcm_id);

        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> lpc() const;
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> ivsd() const;
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> tmip_magnitude_3dt() const;
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> tmip_signal_intensity_3dt() const;
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 4>> pressure_map() const;
        [[nodiscard]] std::unique_ptr<bk::DicomImage<double, 3>> vessel_segmentation_in_flow_field_3dt_size(const Vessel_4DPCMRI* v) const;

        // todo: 2dt centerline cuts

        //====================================================================================================
        //===== FILTERS
        //====================================================================================================
        //[[maybe_unused]] bool determine_phase_wraps_3dt();

        // todo phase unwrapping 3dt
        // todo phase unwrapping 2dt
        // todo eddy current 3dt
        // todo eddy current 2dt

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();

    }; // class Dataset
  } // inline namespace cmr
} // namespace bk

#endif //BK_DATASET_H
