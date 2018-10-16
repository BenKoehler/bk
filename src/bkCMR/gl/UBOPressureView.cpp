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

#include <bkCMR/gl/UBOPressureView.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    UBOPressureView::UBOPressureView()
        : base_type("UBOPressureView", 1)
    #else
    UBOPressureView::UBOPressureView(bk::qt_gl_functions* gl)
    : base_type(gl, "UBOPressureView", 1)
    #endif
    {
        this->set_usage_STATIC_DRAW();

        BK_UBO_SPECIALIZATION_REGISTER(num_ray_samples, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(num_times, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(temporal_resolution, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(current_t0, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(scaleVal, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(referenceVal, GL_FLOAT)
    }

    UBOPressureView::UBOPressureView(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    UBOPressureView::~UBOPressureView() = default;
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    auto UBOPressureView::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== VALUES
    //====================================================================================================
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, num_ray_samples, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, num_times, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, temporal_resolution, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, current_t0, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, scaleVal, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOPressureView, referenceVal, GL_FLOAT)
  } // inline namespace cmr
} // namespace bk