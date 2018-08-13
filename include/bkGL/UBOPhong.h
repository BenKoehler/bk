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

#ifndef BKGL_UBOPHONG_H
#define BKGL_UBOPHONG_H

#include <string>

#include "buffer/UBOSpecialization.h"
#include "lib/export.h"

namespace bk::details
{
  class BK_GL_EXPORT UBOPhong : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOPhong;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOPhong();
      #else
      UBOPhong(qt_gl_functions* gl);
      #endif
      UBOPhong(const self_type&) = delete;
      UBOPhong(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOPhong();
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
      BK_UBO_SPECIALIZATION_DECLARE(meshcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(meshcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(meshcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(shininess, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(ghostFalloff, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(ghostCutoff, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(silhouette_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(num_colors, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(min_attribute_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(max_attribute_value, GL_FLOAT)
  }; // class UBOPhong
} // namespace bk::details

#endif //BKGL_UBOPHONG_H
