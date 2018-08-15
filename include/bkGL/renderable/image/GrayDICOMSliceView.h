/*
 * USAGE:
 *
 * GrayDICOMSliceView isv;
 * - wheel up/down -> isv.nextSlice(); / isv.previousSlice();
 * - window resize (resizeGL) -> isv.set_screen_size(w, h);
 * - mouse move event -> isv.determine_currentIntensity(_mouse.x(), _mouse.y());
 *      -> sends signal_intensitycurrent_changed(value of hovered position)
 * - paint function -> isv.draw()
 * - set image -> isv.set_image(); // performs initialization on success
 *
 * - signals: {xyzt}{max,current}_changed
 */

#pragma once

#ifndef BK_GRAYDICOMSLICEVIEW_H
#define BK_GRAYDICOMSLICEVIEW_H

#ifdef BK_LIB_GDCM_AVAILABLE

    #include <memory>
    #include <type_traits>
    #include "AbstractGraySliceView.h"

    #include <bkDataset/Image>

namespace bk
{
  // ------ forward declaration -------
  class DicomDirImporter;
  // ------ forward declaration -------

  class BKGL_EXPORT GrayDICOMSliceView : public details::AbstractGraySliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GrayDICOMSliceView;
      using base_type = details::AbstractGraySliceView;
    public:
      using image_type = RegularImage<GLfloat, 2>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      GrayDICOMSliceView();
      #else
      GrayDICOMSliceView(bk::qt_gl_functions* gl);
      #endif
      GrayDICOMSliceView(const self_type&) = delete;
      GrayDICOMSliceView(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~GrayDICOMSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DICOMDIR IMPORTER
      const DicomDirImporter* dicom_dir_importer() const;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SIZE
      GLuint image_size(GLuint id) const final;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SCALE
      GLfloat image_scale(GLuint id) const final;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET DICOMDIR IMPORTER
      void set_dicom_dir_importer(DicomDirImporter* d);
      /// @}

      /// @{ -------------------------------------------------- SET IMAGE
      bool set_image(unsigned int dcm_image_id);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type&) = delete;
      auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear_image() final;
      /// @}

      /// @{ -------------------------------------------------- UPDATE SSBO INTENSITY
    protected:
      void update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t) final;
    public:
      /// @}

      /// @{ -------------------------------------------------- CURRENT INTENSITY
    protected:
      GLfloat intensity_at_xyzt_current() const final;
    public:
      /// @}
  }; // class GrayDICOMSliceView

} // namespace bk

#endif // BK_LIB_GDCM_AVAILABLE
#endif //BK_GRAYDICOMSLICEVIEW_H


