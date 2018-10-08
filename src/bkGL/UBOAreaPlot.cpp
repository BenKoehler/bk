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

#include <bkGL/UBOAreaPlot.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOAreaPlot::UBOAreaPlot()
      : base_type("UBOAreaPlot", 1)
  #else
  UBOAreaPlot::UBOAreaPlot(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOAreaPlot", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(areaColorAbove_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorAbove_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorAbove_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorAbove_a, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorBelow_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorBelow_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorBelow_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(areaColorBelow_a, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecolorr, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecolorg, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecolorb, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecolora, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(stdevcolorr, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(stdevcolorg, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(stdevcolorb, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(stdevcolora, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(xmin, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(xmax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ymin, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ymax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(border_width_horizontal_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(border_width_vertical_in_percent, GL_FLOAT)
  }

  UBOAreaPlot::UBOAreaPlot(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOAreaPlot::~UBOAreaPlot() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOAreaPlot::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorAbove_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorAbove_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorAbove_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorAbove_a, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorBelow_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorBelow_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorBelow_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, areaColorBelow_a, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, line_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, linecolorr, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, linecolorg, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, linecolorb, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, linecolora, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, stdevcolorr, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, stdevcolorg, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, stdevcolorb, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, stdevcolora, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, xmin, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, xmax, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, ymin, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, ymax, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, border_width_horizontal_in_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOAreaPlot, border_width_vertical_in_percent, GL_FLOAT)
} // namespace bk::details
