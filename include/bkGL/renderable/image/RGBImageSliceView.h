/*
 * USAGE:
 *
 * RGBImageSliceView isv;
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
#ifndef BK_RGBIMAGESLICEVIEW_H
#define BK_RGBIMAGESLICEVIEW_H

#include <bkDataset/Image>

#include "AbstractRGBSliceView.h"

namespace bk
{
  class BKGL_EXPORT RGBImageSliceView : public details::AbstractRGBSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RGBImageSliceView;
      using base_type = details::AbstractRGBSliceView;
    public:
      using rgb_type = typename base_type::rgb_type;
      using image_type = RegularImage<rgb_type, 4>;

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

      RGBImageSliceView();
      #else
      RGBImageSliceView(bk::qt_gl_functions* gl);
      #endif
      RGBImageSliceView(const self_type& other) = delete;
      RGBImageSliceView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~RGBImageSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET IMAGE SIZE
      auto image_size(GLuint id) const -> GLuint final;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE SCALE
      auto image_scale(GLuint id) const -> GLfloat final;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET IMAGE
      template<typename Image_>
      bool set_image(const Image_& img);
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
  }; // class RGBImageSliceView

} // namespace bk

#include "RGBImageSliceView.hpp"

#endif //BK_RGBIMAGESLICEVIEW_H


