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

/*
 * USAGE:
 *
 * RGBDICOMSliceView isv;
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

#ifndef BK_RGBDICOMSLICEVIEW_H
#define BK_RGBDICOMSLICEVIEW_H

#ifdef BK_LIB_GDCM_AVAILABLE

    #include <bk/CopyablePIMPL>
    #include <bk/Image>
    #include <bkGL/renderable/image/AbstractRGBSliceView.h>

namespace bk
{
  // ------ forward declaration -------
  class DicomDirImporter;
  // ------ forward declaration -------

  class BKGL_EXPORT RGBDICOMSliceView

      : public details::AbstractRGBSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RGBDICOMSliceView;
      using base_type = details::AbstractRGBSliceView;
    public:
      using rgb_type = typename base_type::rgb_type;
      using image_type = RegularImage<rgb_type, 2>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:

      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      RGBDICOMSliceView();
      #else
      RGBDICOMSliceView(bk::qt_gl_functions* gl);
      #endif
      RGBDICOMSliceView(const self_type& other) = delete;

      RGBDICOMSliceView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~RGBDICOMSliceView();
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
      bool set_images(unsigned int dcm_image_r_id, unsigned int dcm_image_g_id, unsigned int dcm_image_b_id);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) noexcept -> self_type&;
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
  }; // class RGBDICOMSliceView

} // namespace bk


#endif // BK_LIB_GDCM_AVAILABLE
#endif //BK_RGBDICOMSLICEVIEW_H


