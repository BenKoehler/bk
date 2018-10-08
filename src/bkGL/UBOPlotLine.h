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

#ifndef BK_UBOPLOTLINE_H
#define BK_UBOPLOTLINE_H

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/lib/bkGL_export.h>
#include <bkGL/gl_definitions.h>

namespace bk::details
{
  class BKGL_EXPORT UBOPlotLine : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOPlotLine;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOPlotLine();
      #else
      UBOPlotLine(qt_gl_functions* gl);
      #endif
      UBOPlotLine(const self_type&) = delete;
      UBOPlotLine(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOPlotLine();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

    private:
      using base_type::set_buffer_name;
      using base_type::set_buffer_base;
    public:

      //====================================================================================================
      //===== VALUES
      //====================================================================================================
      BK_UBO_SPECIALIZATION_DECLARE(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_a, GL_FLOAT)
  }; // class UBOPlotLine
} // namespace bk::details

#endif //BK_UBOPLOTLINE_H
