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

#include <bkGL/renderable/image/AbstractGraySliceView.h>

#include <bkGL/shader/ShaderLibrary.h>
#include <bk/Signal>

namespace bk::details
{
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct AbstractGraySliceView::Impl
    {
        GLfloat intensitycurrent;
        Signal<GLfloat> s_intensitycurrent_changed;

        Impl()
            : intensitycurrent(0)
        { /* do nothing */ }
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    #ifndef BK_LIB_QT_AVAILABLE

    AbstractGraySliceView::AbstractGraySliceView()
        : base_type(),
    #else

    AbstractGraySliceView::AbstractGraySliceView(bk::qt_gl_functions* gl) : base_type(gl),
    #endif
          _pdata(std::make_unique<Impl>())
    {
    }

    AbstractGraySliceView::AbstractGraySliceView(self_type&&) noexcept = default;

    AbstractGraySliceView::~AbstractGraySliceView()
    { _pdata->s_intensitycurrent_changed.disconnect_all(); }

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    auto AbstractGraySliceView::intensity_current() -> GLfloat
    { return _pdata->intensitycurrent; }

    auto AbstractGraySliceView::signal_intensitycurrent_changed() -> Signal<GLfloat>&
    { return _pdata->s_intensitycurrent_changed; }

    std::string AbstractGraySliceView::vertex_shader_source() const
    { return details::ShaderLibrary::slice_view::vert(); }

    std::string AbstractGraySliceView::fragment_shader_source() const
    { return details::ShaderLibrary::slice_view::frag(); }

    auto AbstractGraySliceView::intensitycurrent() -> GLfloat&
    { return _pdata->intensitycurrent; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    auto AbstractGraySliceView::operator=(self_type&&) noexcept -> self_type& = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    GLuint AbstractGraySliceView::ssbo_intensity_num_elements_per_pixel() const
    { return 1; }

    void AbstractGraySliceView::clear()
    {
        base_type::clear();
        _pdata->s_intensitycurrent_changed.emit_signal(0);
    }

    void AbstractGraySliceView::determine_currentIntensity_impl()
    {
        _pdata->intensitycurrent = intensity_at_xyzt_current();
        _pdata->s_intensitycurrent_changed.emit_signal(_pdata->intensitycurrent);
    }
} // namespace bk::details


