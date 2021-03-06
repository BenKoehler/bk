/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#include <bk/Image>
#include <bkGL/renderable/image/AbstractGraySliceView.h>

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
      GrayImageSliceView(self_type&&) noexcept;
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
      self_type& operator=(self_type&&) noexcept;
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

#include <bkGL/renderable/image/GrayImageSliceView.hpp>

#endif //BK_GRAYIMAGESLICEVIEW_H


