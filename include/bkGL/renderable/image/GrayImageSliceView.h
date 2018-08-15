/*
 * USAGE:
 *
 * GrayImageSliceView isv;
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
#ifndef BK_GRAYIMAGESLICEVIEW_H
#define BK_GRAYIMAGESLICEVIEW_H

#include <limits>

#include "AbstractGraySliceView.h"

#include "../../../bkDataset/Image"

namespace bk
{
  class BKGL_EXPORT GrayImageSliceView : public details::AbstractGraySliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GrayImageSliceView;
      using base_type = details::AbstractGraySliceView;
    public:
      using image_type = RegularImage<GLfloat, 4>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      image_type _image;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      GrayImageSliceView();
      #else
      GrayImageSliceView(bk::qt_gl_functions* gl);
      #endif
      GrayImageSliceView(const self_type&) = delete;
      GrayImageSliceView(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~GrayImageSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET IMAGE SIZE
      GLuint image_size(GLuint id) const final;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SCALE
      GLfloat image_scale(GLuint id) const final;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET IMAGE
      template<typename Image_>
      bool set_image(const Image_& img);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type&) = delete;
      self_type& operator=(self_type&&);
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
  }; // class GrayImageSliceView
} // namespace bk

#include "GrayImageSliceView.hpp"

#endif //BK_GRAYIMAGESLICEVIEW_H


