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

#pragma once

#ifndef BKGL_UBOSLICEVIEW_H
#define BKGL_UBOSLICEVIEW_H

#include <string>

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk::details
{
  class BKGL_EXPORT UBOSliceView : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOSliceView;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOSliceView();
      #else
      UBOSliceView(qt_gl_functions* gl);
      #endif
      UBOSliceView(const self_type&) = delete;
      UBOSliceView(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOSliceView();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

    private:
      using base_type::set_buffer_name;
      using base_type::set_buffer_base;
    public:

      //====================================================================================================
      //===== VALUES
      //====================================================================================================
      BK_UBO_SPECIALIZATION_DECLARE(xyzt_max0, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(xyzt_max1, GL_INT)
      //BK_UBO_SPECIALIZATION_DECLARE(orientation, GL_INT) // todo
      BK_UBO_SPECIALIZATION_DECLARE(window_width, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(window_height, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(voxel_scale0, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(voxel_scale1, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(tf_center, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(tf_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(threshold, GL_FLOAT)
  }; // class UBOSliceView
} // namespace bk::details

#endif //BKGL_UBOSLICEVIEW_H
