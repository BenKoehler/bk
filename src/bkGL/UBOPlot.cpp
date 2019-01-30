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

#include <bkGL/UBOPlot.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOPlot::UBOPlot()
      : base_type("UBOPlot", 1)
  #else
  UBOPlot::UBOPlot(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOPlot", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(xmin, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(xmax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ymin, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ymax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(border_width_x_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(border_width_y_in_percent, GL_FLOAT)
  }

  UBOPlot::UBOPlot(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOPlot::~UBOPlot() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOPlot::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, xmin, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, xmax, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, ymin, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, ymax, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, border_width_x_in_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPlot, border_width_y_in_percent, GL_FLOAT)
} // namespace bk::details
