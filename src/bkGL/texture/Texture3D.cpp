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

#include <bkGL/texture/Texture3D.h>

#include <algorithm>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  Texture3D::Texture3D(GLuint w, GLuint h, GLuint d)
      : base_type(w, h),
        _depth(d)
  { set_target_TEXTURE3D(); }

  #else

  Texture3D::Texture3D(bk::qt_gl_functions* gl) : Texture3D(1, 1, 1, gl)
  { /* do nothing */ }

  Texture3D::Texture3D(GLuint w, GLuint h, GLuint d, bk::qt_gl_functions* gl)
      : base_type(w, h, gl),
        _depth(d)
  { set_target_TEXTURE3D(); }

  #endif

  Texture3D::Texture3D(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Texture3D::~Texture3D() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET DEPTH
  GLsizei Texture3D::depth() const
  { return _depth; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Texture3D::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET DEPTH
  void Texture3D::set_depth(GLsizei d)
  { _depth = std::max(1, d); }
  /// @}

  /// @{ -------------------------------------------------- SET SIZE
  void Texture3D::set_size(GLuint w, GLuint h, GLuint d)
  {
      this->set_width(w);
      this->set_height(h);
      this->set_depth(d);
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  void Texture3D::init_impl(GLvoid* pixel_data)
  { BK_QT_GL glTexImage3D(GL_TEXTURE_3D, mipmap_level(), internal_format(), width(), height(), _depth, border_width(), format(), pixel_data_type(), pixel_data); }
  /// @}
} // namespace bk


