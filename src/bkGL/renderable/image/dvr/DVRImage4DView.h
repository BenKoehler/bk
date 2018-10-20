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

#ifndef BKGL_DVRIMAGE4DVIEW_H
#define BKGL_DVRIMAGE4DVIEW_H

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bkGL/lib/bkGL_export.h>
#include <bkGL/renderable/image/dvr/DVRImageView.h>

namespace bk
{
  class BKGL_EXPORT DVRImage4DView : public DVRImageView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DVRImage4DView;
      using base_type = DVRImageView;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE

      DVRImage4DView();
      #else
      DVRImage4DView(bk::qt_gl_functions* gl);
      #endif
      DVRImage4DView(const self_type&) = delete;
      DVRImage4DView(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~DVRImage4DView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET BUFFERS/TEXTURES
    protected:
      [[nodiscard]] Texture3D& image_tex(unsigned int i);
      [[nodiscard]] RegularImage<double, 4>& img();
    public:
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    protected:
      virtual void clear_shader_impl() override;
      virtual void clear_buffers_and_textures_impl() override;
    public:
      /// @}

      /// @{ -------------------------------------------------- INIT
    protected:
      virtual void init_shader_impl() override;
    public:

      template<typename Image4D_>
      void init_image(const Image4D_& img);

      template<typename Image4D_>
      void init(const Image4D_& img, GLuint window_width, GLuint window_height);
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_animation_time_changed(GLfloat d) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void bind_image_textures() override;
      virtual void release_image_textures() override;
      [[nodiscard]] virtual bool is_up_to_date() const override;
    public:
      /// @}
  }; // class DVRImage4DView

} // namespace bk

#include <bkGL/renderable/image/dvr/DVRImage4DView.hpp>

#endif //BKGL_DVRIMAGE4DVIEW_H


