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

#include <bkGL/UBOPlotArea.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOPlotArea::UBOPlotArea()
      : base_type("UBOPlotArea", 2)
  #else
  UBOPlotArea::UBOPlotArea(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOPlotArea", 2)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(color_area_above_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_above_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_above_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_above_a, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_below_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_below_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_below_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_area_below_a, GL_FLOAT)
  }

  UBOPlotArea::UBOPlotArea(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOPlotArea::~UBOPlotArea() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOPlotArea::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_above_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_above_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_above_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_above_a, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_below_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_below_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_below_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlotArea, color_area_below_a, GL_FLOAT)
} // namespace bk::details
