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

#ifndef BKGL_TEXTURE3D_H
#define BKGL_TEXTURE3D_H

#include "AbstractTexture.h"

namespace bk
{
  class BK_GL_EXPORT Texture3D : public details::AbstractTexture
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Texture3D;
      using base_type = details::AbstractTexture;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      GLsizei _depth;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      Texture3D(GLuint w = 1, GLuint h = 1, GLuint d = 1);
      #else
      explicit Texture3D(bk::qt_gl_functions* gl);
      explicit Texture3D(GLuint w, GLuint h, GLuint d, qt_gl_functions* gl);
      #endif
      Texture3D(const self_type& other) = delete;
      Texture3D(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~Texture3D();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DEPTH
      GLsizei depth() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET DEPTH
      void set_depth(GLsizei d);
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      void set_size(GLuint w, GLuint h, GLuint d);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    protected:
      void init_impl(GLvoid* pixel_data = nullptr) override;
    public:
      /// @}
  }; // class Texture3D
} // namespace bk

#endif //BKGL_TEXTURE3D_H


