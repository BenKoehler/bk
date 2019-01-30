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

#include <bkGL/UBOVectorView.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOVectorView::UBOVectorView()
      : base_type("UBOVectorView", 1)
  #else
  UBOVectorView::UBOVectorView(bk::qt_gl_functions* gl)
      : base_type(gl, "UBOVectorView", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(col_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(col_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(col_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(isl_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(shininess, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(halo_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(halo_width_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(num_colors, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(min_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(max_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_transparency_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(color_alpha_correction, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(scale_attrib_to_colorbar, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(num_times, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(current_t0, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(temporal_resolution, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(vector_scale, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(arrow_head_length_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(arrow_head_width_factor, GL_FLOAT)
  }

  UBOVectorView::UBOVectorView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOVectorView::~UBOVectorView() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOVectorView::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, line_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, col_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, col_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, col_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, isl_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, lightcol_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, lightcol_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, lightcol_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, shininess, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, halo_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, halo_width_in_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, color_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, num_colors, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, min_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, max_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, color_transparency_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, color_alpha_correction, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, scale_attrib_to_colorbar, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, num_times, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, current_t0, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, temporal_resolution, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, vector_scale, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, arrow_head_length_percent, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOVectorView, arrow_head_width_factor, GL_FLOAT)
} // namespace bk::details