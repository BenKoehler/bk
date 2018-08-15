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

#include <bkGL/renderable/image/AbstractRGBSliceView.h>

#include <bkGL/shader/ShaderLibrary.h>
#include <bk/Signal>

namespace bk::details
{
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct AbstractRGBSliceView::Impl
    {
        rgb_type rgbcurrent;
        Signal<rgb_type> s_rgbcurrent_changed;

        Impl()
            : rgbcurrent(0, 0, 0)
        { /* do nothing */ }
    };

    #ifndef BK_LIB_QT_AVAILABLE

    AbstractRGBSliceView::AbstractRGBSliceView()
        : base_type(),
    #else

    AbstractRGBSliceView::AbstractRGBSliceView(bk::qt_gl_functions* gl) : base_type(gl),
    #endif
          _pdata(std::make_unique<Impl>())
    {
    }

    AbstractRGBSliceView::AbstractRGBSliceView(self_type&&) = default;

    AbstractRGBSliceView::~AbstractRGBSliceView()
    { _pdata->s_rgbcurrent_changed.disconnect_all(); }

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    auto AbstractRGBSliceView::rgb_current() -> rgb_type&
    { return _pdata->rgbcurrent; }

    auto AbstractRGBSliceView::rgb_current() const -> const rgb_type&
    { return _pdata->rgbcurrent; }

    auto AbstractRGBSliceView::signal_rgbcurrent_changed() -> Signal<rgb_type>&
    { return _pdata->s_rgbcurrent_changed; }

    std::string AbstractRGBSliceView::vertex_shader_source() const
    { return ShaderLibrary::slice_view::rgb::vert(); }

    std::string AbstractRGBSliceView::fragment_shader_source() const
    { return ShaderLibrary::slice_view::rgb::frag(); }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    auto AbstractRGBSliceView::operator=(self_type&& other) -> self_type& = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    GLuint AbstractRGBSliceView::ssbo_intensity_num_elements_per_pixel() const 
    { return 3; }

    void AbstractRGBSliceView::clear()
    {
        base_type::clear();
        _pdata->s_rgbcurrent_changed.emit_signal(rgb_type(0, 0, 0));
    }

    void AbstractRGBSliceView::determine_currentIntensity_impl()
    {
        _pdata->rgbcurrent = rgb_at_xyzt_current();
        _pdata->s_rgbcurrent_changed.emit_signal(_pdata->rgbcurrent);
    }
} // namespace bk::details


