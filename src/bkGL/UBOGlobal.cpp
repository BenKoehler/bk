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

#include <bkGL/UBOGlobal.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOGlobal::UBOGlobal()
      : base_type("UBOGlobal", 0)
  #else
  UBOGlobal::UBOGlobal(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOGlobal", 0)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(modelview_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_REGISTER(projection_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_REGISTER(modelview_projection_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_REGISTER(rotation_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_REGISTER(lookat_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_REGISTER(cam_pos_x, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(cam_pos_y, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(cam_pos_z, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(cam_znear, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(cam_zfar, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(window_width, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(window_height, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(ssaa_factor, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(animation_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(animation_current_time, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(animation_max_time, GL_FLOAT)
  }

  UBOGlobal::UBOGlobal(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOGlobal::~UBOGlobal() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOGlobal::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, modelview_matrix, GL_FLOAT_MAT4)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, projection_matrix, GL_FLOAT_MAT4)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, modelview_projection_matrix, GL_FLOAT_MAT4)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, rotation_matrix, GL_FLOAT_MAT4)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, lookat_matrix, GL_FLOAT_MAT4)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, cam_pos_x, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, cam_pos_y, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, cam_pos_z, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, cam_znear, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, cam_zfar, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, window_width, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, window_height, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, ssaa_factor, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, animation_enabled, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, animation_current_time, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOGlobal, animation_max_time, GL_FLOAT)
} // namespace bk::details