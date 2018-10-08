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

#include <bkGL/UBOPlotLine.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOPlotLine::UBOPlotLine()
      : base_type("UBOPlotLine", 2)
  #else
  UBOPlotLine::UBOPlotLine(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOPlotLine", 2)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_a, GL_FLOAT)
  }

  UBOPlotLine::UBOPlotLine(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOPlotLine::~UBOPlotLine() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOPlotLine::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotLine, line_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotLine, color_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotLine, color_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotLine, color_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotLine, color_a, GL_FLOAT)
} // namespace bk::details
