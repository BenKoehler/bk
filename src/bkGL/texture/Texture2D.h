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

#ifndef BKGL_TEXTURE2D_H
#define BKGL_TEXTURE2D_H

#include <bkGL/texture/AbstractTexture.h>

namespace bk
{
  class BKGL_EXPORT Texture2D : public details::AbstractTexture
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Texture2D;
      using base_type = details::AbstractTexture;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D(GLuint w = 1, GLuint h = 1);
      #else
      explicit Texture2D(bk::qt_gl_functions* gl);
      explicit Texture2D(GLuint w, GLuint h, bk::qt_gl_functions* gl);
      #endif
      Texture2D(const self_type& other) = delete;
      Texture2D(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~Texture2D();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      void set_size(GLuint w, GLuint h);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    protected:
      void init_impl(GLvoid* pixel_data = nullptr) override;
    public:
      /// @}
  }; // class Texture2D
} // namespace bk

#endif //BKGL_TEXTURE2D_H


