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

#ifndef BKGL_UBOGLOBAL_H
#define BKGL_UBOGLOBAL_H

#include <string>

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/lib/bkGL_export.h>
#include <bkGL/gl_definitions.h>

namespace bk::details
{
  class BKGL_EXPORT UBOGlobal : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOGlobal;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOGlobal();
      #else
      UBOGlobal(qt_gl_functions* gl);
      #endif
      UBOGlobal(const self_type&) = delete;
      UBOGlobal(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOGlobal();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

    private:
      using base_type::set_buffer_name;
      using base_type::set_buffer_base;
    public:

      //====================================================================================================
      //===== VALUES
      //====================================================================================================
      BK_UBO_SPECIALIZATION_DECLARE(modelview_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_DECLARE(projection_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_DECLARE(modelview_projection_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_DECLARE(rotation_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_DECLARE(lookat_matrix, GL_FLOAT_MAT4)
      BK_UBO_SPECIALIZATION_DECLARE(cam_pos_x, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(cam_pos_y, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(cam_pos_z, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(cam_znear, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(cam_zfar, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(window_width, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(window_height, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(ssaa_factor, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(animation_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(animation_current_time, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(animation_max_time, GL_FLOAT)
  }; // class UBOGlobal
} // namespace bk::details

#endif //BKGL_UBOGLOBAL_H
