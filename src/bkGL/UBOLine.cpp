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

#include "../include/bkGL/UBOLine.h"

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOLine::UBOLine()
      : base_type("UBOLine", 1)
  #else
  UBOLine::UBOLine(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOLine", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(linecol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(isl_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(shininess, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(halo_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(halo_width_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(halo_depth_dependent_dmax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(trail_length_in_ms, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(trail_opaque_part_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(num_colors, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(min_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(max_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_transparency_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(color_alpha_correction, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lineao_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(lineao_anisotropic, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(scale_attrib_to_colorbar, GL_INT)
  }

  UBOLine::UBOLine(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOLine::~UBOLine() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOLine::operator=(self_type&&) -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, line_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, linecol_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, linecol_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, linecol_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, isl_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, lightcol_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, lightcol_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, lightcol_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, shininess, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, halo_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, halo_width_in_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, halo_depth_dependent_dmax, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, trail_length_in_ms, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, trail_opaque_part_in_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, color_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, num_colors, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, min_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, max_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, color_transparency_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, color_alpha_correction, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, lineao_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, lineao_anisotropic, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOLine, scale_attrib_to_colorbar, GL_INT)
} // namespace bk::details