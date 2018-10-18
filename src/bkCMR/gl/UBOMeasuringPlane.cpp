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

#include <bkCMR/gl/UBOMeasuringPlane.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    UBOMeasuringPlane::UBOMeasuringPlane()
        : base_type("UBOMeasuringPlane", 1)
    #else
    UBOMeasuringPlane::UBOMeasuringPlane(bk::qt_gl_functions* gl)
    : base_type(gl, "UBOMeasuringPlane", 1)
    #endif
    {
        this->set_usage_STATIC_DRAW();

        BK_UBO_SPECIALIZATION_REGISTER(center_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(center_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(center_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nx_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nx_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nx_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(ny_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(ny_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(ny_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nz_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nz_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(nz_z, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(scale_x, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(scale_y, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(grid_size_x, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(grid_size_y, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(color_r, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(color_g, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(color_b, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(values_initialized, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(vmin, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(vmax, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(num_colors, GL_INT)
    }

    UBOMeasuringPlane::UBOMeasuringPlane(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    UBOMeasuringPlane::~UBOMeasuringPlane() = default;
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    auto UBOMeasuringPlane::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== VALUES
    //====================================================================================================
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, center_x, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, center_y, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, center_z, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nx_x, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nx_y, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nx_z, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, ny_x, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, ny_y, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, ny_z, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nz_x, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nz_y, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, nz_z, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, scale_x, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, scale_y, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, grid_size_x, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, grid_size_y, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, color_r, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, color_g, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, color_b, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, values_initialized, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, vmin, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, vmax, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlane, num_colors, GL_INT)
  } // inline namespace cmr
} // namespace bk