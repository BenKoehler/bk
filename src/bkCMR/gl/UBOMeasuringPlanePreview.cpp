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

#include <bkCMR/gl/UBOMeasuringPlanePreview.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    UBOMeasuringPlanePreview::UBOMeasuringPlanePreview()
        : base_type("UBOMeasuringPlanePreview", 1)
    #else
    UBOMeasuringPlanePreview::UBOMeasuringPlanePreview(bk::qt_gl_functions* gl)
    : base_type(gl, "UBOMeasuringPlanePreview", 1)
    #endif
    {
        this->set_usage_STATIC_DRAW();

        BK_UBO_SPECIALIZATION_REGISTER(color_r, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(color_g, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(color_b, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(color_a, GL_FLOAT)
    }

    UBOMeasuringPlanePreview::UBOMeasuringPlanePreview(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    UBOMeasuringPlanePreview::~UBOMeasuringPlanePreview() = default;
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    auto UBOMeasuringPlanePreview::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== VALUES
    //====================================================================================================
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlanePreview, color_r, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlanePreview, color_g, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlanePreview, color_b, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOMeasuringPlanePreview, color_a, GL_FLOAT)
  } // inline namespace cmr
} // namespace bk