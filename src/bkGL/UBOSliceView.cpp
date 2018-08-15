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

#include <bkGL/UBOSliceView.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOSliceView::UBOSliceView()
      : base_type("UBOSliceView", 0)
  #else
  UBOSliceView::UBOSliceView(bk::qt_gl_functions* gl) 
      : base_type(gl, "UBOSliceView", 1)
  #endif
  {
      this->set_usage_STATIC_DRAW();

      BK_UBO_SPECIALIZATION_REGISTER(xyzt_max0, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(xyzt_max1, GL_INT)
      //BK_UBO_SPECIALIZATION_REGISTER(orientation, GL_INT) // todo
      BK_UBO_SPECIALIZATION_REGISTER(image_width, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(image_height, GL_INT)
      BK_UBO_SPECIALIZATION_REGISTER(voxel_scale0, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(voxel_scale1, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(tf_center, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(tf_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_REGISTER(threshold, GL_FLOAT)
  }

  UBOSliceView::UBOSliceView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOSliceView::~UBOSliceView() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOSliceView::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== VALUES
  //====================================================================================================
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, xyzt_max0, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, xyzt_max1, GL_INT)
  //BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, orientation, GL_INT) // todo
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, image_width, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, image_height, GL_INT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, voxel_scale0, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, voxel_scale1, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, tf_center, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, tf_width, GL_FLOAT)
  BK_UBO_SPECIALIZATION_DEFINE(UBOSliceView, threshold, GL_FLOAT)
} // namespace bk::details