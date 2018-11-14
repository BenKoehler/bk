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

#include <bkGL/UBOPhong.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOPhong::UBOPhong()
      : base_type("UBOPhong", 1)
  #else
  UBOPhong::UBOPhong(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOPhong", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(meshcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(meshcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(meshcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(lightcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(shininess, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ghostFalloff, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(ghostCutoff, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(silhouette_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(color_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(num_colors, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(min_attribute_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(max_attribute_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(temporal_resolution, GL_FLOAT)
  }

  UBOPhong::UBOPhong(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOPhong::~UBOPhong() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOPhong::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, meshcol_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, meshcol_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, meshcol_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, lightcol_r, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, lightcol_g, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, lightcol_b, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, shininess, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, ghostFalloff, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, ghostCutoff, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, silhouette_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, color_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, num_colors, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, min_attribute_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, max_attribute_value, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOPhong, temporal_resolution, GL_FLOAT)
} // namespace bk::details