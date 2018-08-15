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

#ifndef BKGL_WINDOWGEOMETRY_H
#define BKGL_WINDOWGEOMETRY_H

#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  class BKGL_EXPORT WindowGeometry
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = WindowGeometry;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      GLuint _width;
      GLuint _height;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      WindowGeometry();
      WindowGeometry(GLuint w, GLuint h);
      WindowGeometry(const self_type&);
      WindowGeometry(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~WindowGeometry();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET WIDTH
      [[nodiscard]] GLuint width() const;
      /// @}

      /// @{ -------------------------------------------------- GET HEIGHT
      [[nodiscard]] GLuint height() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET WIDTH
      void set_width(GLuint w);
      /// @}

      /// @{ -------------------------------------------------- SET HEIGHT
      void set_height(GLuint h);
      /// @}

      /// @{ -------------------------------------------------- SET
      void set(GLuint w, GLuint h);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- ASPECT RATIO
      [[nodiscard]] GLfloat aspect_ratio() const;
      /// @}

      /// @{ -------------------------------------------------- COMPARE
      [[nodiscard]] bool operator==(const self_type& other) const;
      [[nodiscard]] bool operator!=(const self_type& other) const;
      /// @}
  }; // class WindowGeometry

} // namespace bk

#endif //BKGL_WINDOWGEOMETRY_H


