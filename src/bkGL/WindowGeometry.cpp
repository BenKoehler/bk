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

#include <algorithm>

#include <bkGL/WindowGeometry.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  WindowGeometry::WindowGeometry()
      : WindowGeometry(1280, 720)
  { /* do nothing */ }

  WindowGeometry::WindowGeometry(GLuint w, GLuint h)
      : _width(std::max(static_cast<GLuint>(1), w)),
        _height(std::max(static_cast<GLuint>(1), h))
  { /* do nothing */ }

  WindowGeometry::WindowGeometry(const self_type&) = default;
  WindowGeometry::WindowGeometry(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  WindowGeometry::~WindowGeometry() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET WIDTH
  auto WindowGeometry::width() const -> GLuint
  { return _width; }
  /// @}

  /// @{ -------------------------------------------------- GET HEIGHT
  auto WindowGeometry::height() const -> GLuint
  { return _height; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto WindowGeometry::operator=(const self_type&) -> self_type& = default;
  auto WindowGeometry::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET WIDTH
  void WindowGeometry::set_width(GLuint w)
  { _width = std::max(static_cast<GLuint>(1), w); }
  /// @}

  /// @{ -------------------------------------------------- SET HEIGHT
  void WindowGeometry::set_height(GLuint h)
  { _height = std::max(static_cast<GLuint>(1), h); }
  /// @}

  /// @{ -------------------------------------------------- SET
  void WindowGeometry::set(GLuint w, GLuint h)
  {
      set_width(w);
      set_height(h);
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- ASPECT RATIO
  GLfloat WindowGeometry::aspect_ratio() const
  { return static_cast<GLfloat>(_width) / static_cast<GLfloat>(_height); }
  /// @}

  /// @{ -------------------------------------------------- COMPARE
  bool WindowGeometry::operator==(const self_type& other) const
  { return _width == other._width && _height == other._height; }

  bool WindowGeometry::operator!=(const self_type& other) const
  { return !operator==(other); }
  /// @}
} // namespace bk