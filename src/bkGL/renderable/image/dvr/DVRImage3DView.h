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

#ifndef BKGL_DVRIMAGE3DVIEW_H
#define BKGL_DVRIMAGE3DVIEW_H

#include <bk/CopyablePIMPL>
#include <bkGL/lib/bkGL_export.h>
#include <bkGL/renderable/image/dvr/DVRImageView.h>

namespace bk
{
  class BKGL_EXPORT DVRImage3DView : public DVRImageView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DVRImage3DView;
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
      DVRImage3DView();
      #else
      DVRImage3DView(bk::qt_gl_functions* gl);
      #endif
      DVRImage3DView(const self_type& other) = delete;
      DVRImage3DView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~DVRImage3DView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET BUFFERS/TEXTURES
    private:
      [[nodiscard]] Texture3D& image_tex();
    public:
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
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

      template<typename Image3D_>
      void init_image(const Image3D_& img);

      template<typename Image3D_>
      void init(const Image3D_& img, GLuint window_width, GLuint window_height);
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void bind_image_textures() override;
      virtual void release_image_textures() override;
      [[nodiscard]] virtual bool is_up_to_date() const override;
    public:
      /// @}
  }; // class DVRImage3DView

} // namespace bk

#include <bkGL/renderable/image/dvr/DVRImage3DView.hpp>

#endif //BKGL_DVRIMAGE3DVIEW_H


