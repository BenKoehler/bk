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

#include <bkCMR/gl/UBOFlowJetView.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    UBOFlowJetView::UBOFlowJetView()
        : base_type("UBOFlowJetView", 1)
    #else
    UBOFlowJetView::UBOFlowJetView(bk::qt_gl_functions* gl)
    : base_type(gl, "UBOFlowJetView", 1)
    #endif
    {
        this->set_usage_STATIC_DRAW();

        BK_UBO_SPECIALIZATION_REGISTER(num_times, GL_INT)
        BK_UBO_SPECIALIZATION_REGISTER(temporal_scale, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(relative_velocity_visibility_threshold, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(transparency_exponent, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_color_r, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_color_g, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_color_b, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_line_width, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_halo_width_in_percent, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(area_line_shininess, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(jet_shininess, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(jet_light_color_r, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(jet_light_color_g, GL_FLOAT)
        BK_UBO_SPECIALIZATION_REGISTER(jet_light_color_b, GL_FLOAT)
    }

    UBOFlowJetView::UBOFlowJetView(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    UBOFlowJetView::~UBOFlowJetView() = default;
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    auto UBOFlowJetView::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== VALUES
    //====================================================================================================
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, num_times, GL_INT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, temporal_scale, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, relative_velocity_visibility_threshold, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, transparency_exponent, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_color_r, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_color_g, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_color_b, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_line_width, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_halo_width_in_percent, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, area_line_shininess, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, jet_shininess, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, jet_light_color_r, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, jet_light_color_g, GL_FLOAT)
    BK_UBO_SPECIALIZATION_DEFINE(UBOFlowJetView, jet_light_color_b, GL_FLOAT)
  } // inline namespace cmr
} // namespace bk