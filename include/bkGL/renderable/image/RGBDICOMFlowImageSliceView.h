/*
 * USAGE:
 *
 * RGBDICOMFlowImageSliceView isv;
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
#ifndef BK_RGBDICOMFLOWIMAGESLICEVIEW_H
#define BK_RGBDICOMFLOWIMAGESLICEVIEW_H
#ifdef BK_LIB_GDCM_AVAILABLE

  #include <memory>

    #include <bkDataset/Image>

#include "AbstractRGBSliceView.h"

namespace bk
{
  // ------ forward declaration -------
  class DicomDirImporter;
  // ------ forward declaration -------

  class BK_GL_EXPORT RGBDICOMFlowImageSliceView : public details::AbstractRGBSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RGBDICOMFlowImageSliceView;
      using base_type = details::AbstractRGBSliceView;
    public:
      using rgb_type = typename base_type::rgb_type;
      using image_type = bk::RegularImage<rgb_type, 2>;

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

      RGBDICOMFlowImageSliceView();
      #else
      RGBDICOMFlowImageSliceView(bk::qt_gl_functions* gl);
      #endif
      RGBDICOMFlowImageSliceView(const self_type& other) = delete;
      RGBDICOMFlowImageSliceView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~RGBDICOMFlowImageSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DICOMDIR IMPORTER
      const DicomDirImporter* dicom_dir_importer() const;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SIZE
      auto image_size(GLuint id) const -> GLuint final;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SCALE
      auto image_scale(GLuint id) const -> GLfloat final;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET DICOMDIR IMPORTER
      void set_dicom_dir_importer(DicomDirImporter* d);
      /// @}

      /// @{ -------------------------------------------------- SET IMAGE
      bool set_images(unsigned int dcm_image_r_id, unsigned int dcm_image_g_id, unsigned int dcm_image_b_id);
      void set_flip_images(bool flip_image_r, bool flip_image_g, bool flip_image_b);
      void set_use_abs(bool abs_image_r, bool abs_image_g, bool abs_image_b);
      void set_use_for_coloring(bool col_image_r, bool col_image_g, bool col_image_b);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) -> self_type&;
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
      auto rgb_at_xyzt_current() const -> rgb_type final;
    public:
      /// @}
  }; // class RGBDICOMFlowImageSliceView

} // namespace bk

#endif // BK_LIB_GDCM_AVAILABLE
#endif //BK_RGBDICOMFLOWIMAGESLICEVIEW_H


