#include "../../../../include/bkGL/renderable/image/AbstractRGBSliceView.h"
#include "../../../../include/bkGL/shader/ShaderLibrary.h"
#include "../../../../include/bkTools/Signal"

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


