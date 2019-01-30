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

#pragma once

#ifndef BK_TEXTURE2DVIEW_H
#define BK_TEXTURE2DVIEW_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk
{
  class Texture2DView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Texture2DView;
      using base_type = details::AbstractRenderable;

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

      Texture2DView();
      #else
      Texture2DView(bk::qt_gl_functions* gl);
      #endif
      Texture2DView(const self_type& other) = delete;
      Texture2DView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~Texture2DView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TEXTURE UNIT
      GLuint texture_unit() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}
      
      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET TEXTURE UNIT
      void set_texture_unit(GLuint i);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear_shader();
      void clear_buffers();
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init_shader();
      void init_ubo();
      void init();
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw_impl() override;
      /// @}
  }; // class Texture2DView
} // namespace bk

#endif //BK_TEXTURE2DVIEW_H


