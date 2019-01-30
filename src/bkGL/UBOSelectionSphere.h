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

#pragma once

#ifndef BKGL_UBOPICKING_H
#define BKGL_UBOPICKING_H

#include <string>

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk::details
{
  class BKGL_EXPORT UBOSelectionSphere : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOSelectionSphere;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOSelectionSphere();
      #else
      UBOSelectionSphere(qt_gl_functions* gl);
      #endif
      UBOSelectionSphere(const self_type&) = delete;
      UBOSelectionSphere(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOSelectionSphere();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      using base_type::set_buffer_base;
    private:
      using base_type::set_buffer_name;
    public:

      //====================================================================================================
      //===== VALUES
      //====================================================================================================
      BK_UBO_SPECIALIZATION_DECLARE(center_x, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(center_y, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(center_z, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_b, GL_FLOAT)
  }; // class UBOSelectionSphere
} // namespace bk::details

#endif //BKGL_UBOPICKING_H
