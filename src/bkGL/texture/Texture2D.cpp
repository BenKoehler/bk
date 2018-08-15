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

#include <bkGL/texture/Texture2D.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  Texture2D::Texture2D(GLuint w, GLuint h)
      : base_type(w, h)
  { set_target_TEXTURE2D(); }

  #else

  Texture2D::Texture2D(bk::qt_gl_functions* gl) : Texture2D(1, 1, gl)
  { /* do nothing */ }

  Texture2D::Texture2D(GLuint w, GLuint h, bk::qt_gl_functions* gl) : base_type(w, h, gl)
  { set_target_TEXTURE2D(); }

  #endif

  Texture2D::Texture2D(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Texture2D::~Texture2D() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto Texture2D::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET SIZE
  void Texture2D::set_size(GLuint w, GLuint h)
  {
      this->set_width(w);
      this->set_height(h);
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  void Texture2D::init_impl(GLvoid* pixel_data)
  { BK_QT_GL glTexImage2D(GL_TEXTURE_2D, mipmap_level(), internal_format(), width(), height(), border_width(), format(), pixel_data_type(), pixel_data); }
  /// @}
} // namespace bk


