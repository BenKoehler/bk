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

#include <bkGL/UBODVR.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBODVR::UBODVR()
      : base_type("UBODVR", 1)
  #else
  UBODVR::UBODVR(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBODVR", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(num_ray_samples, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(tf_center, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(tf_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(num_times, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(temporal_resolution, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(current_t0, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(minVal, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(maxVal, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(image_size_x, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(image_size_y, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(image_size_z, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(image_scale_x, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(image_scale_y, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(image_scale_z, GL_FLOAT)
  }

  UBODVR::UBODVR(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBODVR::~UBODVR() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBODVR::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, num_ray_samples, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, tf_center, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, tf_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, num_times, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, temporal_resolution, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, current_t0, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, minVal, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, maxVal, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_size_x, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_size_y, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_size_z, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_scale_x, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_scale_y, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBODVR, image_scale_z, GL_FLOAT)
} // namespace bk::details