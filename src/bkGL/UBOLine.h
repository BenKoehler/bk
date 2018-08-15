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

#ifndef BKGL_UBOLINE_H
#define BKGL_UBOLINE_H

#include <string>

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk::details
{
  class BKGL_EXPORT UBOLine : public UBOSpecialization
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOLine;
      using base_type = UBOSpecialization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOLine();
      #else
      UBOLine(qt_gl_functions* gl);
      #endif
      UBOLine(const self_type&) = delete;
      UBOLine(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOLine();
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
      BK_UBO_SPECIALIZATION_DECLARE(line_width, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(linecol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(linecol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(linecol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(isl_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_r, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_g, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lightcol_b, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(shininess, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(halo_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(halo_width_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(halo_depth_dependent_dmax, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(trail_length_in_ms, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(trail_opaque_part_in_percent, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(num_colors, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(min_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(max_value, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(color_transparency_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(color_alpha_correction, GL_FLOAT)
      BK_UBO_SPECIALIZATION_DECLARE(lineao_enabled, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(lineao_anisotropic, GL_INT)
      BK_UBO_SPECIALIZATION_DECLARE(scale_attrib_to_colorbar, GL_INT)
  }; // class UBOLine
} // namespace bk::details

#endif //BKGL_UBOLINE_H
