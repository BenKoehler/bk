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

#ifndef BK_UBOMEASURINGPLANE_H
#define BK_UBOMEASURINGPLANE_H

#include <string>

#include <bkGL/buffer/UBOSpecialization.h>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    class BKCMR_EXPORT UBOMeasuringPlane : public details::UBOSpecialization
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = UBOMeasuringPlane;
        using base_type = details::UBOSpecialization;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CTOR
        #ifndef BK_LIB_QT_AVAILABLE
        UBOMeasuringPlane();
        #else
        UBOMeasuringPlane(qt_gl_functions* gl);
        #endif
        UBOMeasuringPlane(const self_type&) = delete;
        UBOMeasuringPlane(self_type&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DTOR
        ~UBOMeasuringPlane();
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
        BK_UBO_SPECIALIZATION_DECLARE(nx_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(nx_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(nx_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(ny_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(ny_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(ny_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(nz_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(nz_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(nz_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(scale_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(scale_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(grid_size_x, GL_INT)
        BK_UBO_SPECIALIZATION_DECLARE(grid_size_y, GL_INT)
        BK_UBO_SPECIALIZATION_DECLARE(color_r, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(color_g, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(color_b, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(values_initialized, GL_INT)
        BK_UBO_SPECIALIZATION_DECLARE(vmin, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(vmax, GL_FLOAT)
        BK_UBO_SPECIALIZATION_DECLARE(num_colors, GL_INT)
    }; // class UBOMeasuringPlane
  } // inline namespace cmr
} // namespace bk

#endif //BK_UBOMEASURINGPLANE_H
