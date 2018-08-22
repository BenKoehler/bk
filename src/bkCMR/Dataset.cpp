#include <filesystem>

#include <bkCMR/Dataset.h>
#include <bkCMR/Vessel.h>
#include <bkDicom/DicomDirImporter_CMR.h>
#include <bk/StringUtils>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct Dataset::Impl
    {
        std::string          project_path;
        DicomDirImporter_CMR importer;
        DicomImage<Vec3d, 4> flow_image_3dt;
        std::vector<Vessel>  vessels;

        // todo flow dir correction
        // todo phase unwrapping
        // todo eddy current correction
    };

    //====================================================================================================
    //===== DEFINITIONS
    //====================================================================================================
    const std::string Dataset::dcmbytes = "dcmbytes";

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    Dataset::Dataset() = default;
    Dataset::Dataset(const Dataset&) = default;
    Dataset::Dataset(Dataset&&) = default;
    Dataset::~Dataset() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const std::string& Dataset::project_path() const
    { return _pdata->project_path; }

    DicomDirImporter_CMR& Dataset::importer()
    { return _pdata->importer; }

    const DicomDirImporter_CMR& Dataset::importer() const
    { return _pdata->importer; }

    bool Dataset::has_magnitude_images() const
    { return !_pdata->importer.class_3dt_magnitude_images().empty(); }

    bool Dataset::has_signal_intensity_image() const
    { return !_pdata->importer.class_3dt_signal_intensity_images().empty(); }

    bool Dataset::has_local_image_copies() const
    {
        bool              hasLocalImageCopies               = true;
        const std::string proj_path                         = project_path();
        const auto        class_3dt_flow_images             = _pdata->importer.class_3dt_flow_images();
        const auto        class_3dt_anatomical_images       = _pdata->importer.class_3dt_anatomical_images();
        const auto        class_3dt_magnitude_images        = _pdata->importer.class_3dt_magnitude_images();
        const auto        class_3dt_signal_intensity_images = _pdata->importer.class_3dt_signal_intensity_images();
        const auto        class_2dt_flow_images             = _pdata->importer.class_2dt_flow_images();
        const auto        class_2dt_anatomical_images       = _pdata->importer.class_2dt_anatomical_images();
        const auto        class_2d_anatomical_images        = _pdata->importer.class_2d_anatomical_images();
        const auto        class_3d_anatomical_images        = _pdata->importer.class_3d_anatomical_images();

        const std::string filepath_base = string_utils::append(_pdata->project_path, dcmbytes);

        //------------------------------------------------------------------------------------------------------
        // 3dt flow
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_3dt_flow_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 3dt anatomical
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_3dt_anatomical_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 3dt magnitude
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_3dt_magnitude_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 3dt signal intensity
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_3dt_signal_intensity_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 2dt flow
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_2dt_flow_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 2dt anatomical
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_2dt_anatomical_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 2d anatomical
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_2d_anatomical_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
        // 3d anatomical
        //------------------------------------------------------------------------------------------------------
        if (hasLocalImageCopies)
        {
            for (unsigned int imgId: class_3d_anatomical_images)
            {
                if (!std::filesystem::exists(string_utils::append(filepath_base, string_utils::from_number(imgId))))
                {
                    hasLocalImageCopies = false;
                    break;
                }
            }
        }

        // remove corrupted local copy if exists
        if (!hasLocalImageCopies)
        { remove_local_dcm_image_copies(); }

        return hasLocalImageCopies;
    }

    bool Dataset::has_flow_dir_correction() const;

    bool Dataset::is_flow_image_3dt_loaded() const
    { return _pdata->flow_image_3dt.num_values() > 1; }

    bk::DicomImage<bk::Vec3d, 4>* Dataset::flow_image_3dt()
    {
        if (!flow_image_3dt_is_loaded())
        { load_flow_image_3dt(); }

        if (!flow_image_3dt_is_loaded())
        { return nullptr; }

        return &_pdata->flow_image_3dt;
    }

    const bk::DicomImage<bk::Vec3d, 4>* Dataset::flow_image_3dt() const
    { return &_pdata->flow_image_3dt; }

    unsigned int Dataset::num_vessels() const;
    Vessel* Dataset::vessel(unsigned int i);
    Vessel* Dataset::vessel(const std::string& name, bool case_sensitive = true);
    const Vessel* Dataset::vessel(unsigned int i) const;
    const Vessel* Dataset::vessel(const std::string& name, bool case_sensitive = true) const;

    bool Dataset::vessel_has_centerline_ids(const Vessel* v) const;
    bool Dataset::vessel_has_centerlines(const Vessel* v) const;
    bool Dataset::vessels_have_centerline_ids() const;
    bool Dataset::vessels_have_centerlines() const;
  } // inline namespace cmr
} // namespace bk

