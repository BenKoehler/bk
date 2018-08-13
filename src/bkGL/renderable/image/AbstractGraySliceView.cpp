#include "../../../../include/bkGL/shader/ShaderLibrary.h"
#include "../../../../include/bkGL/renderable/image/AbstractGraySliceView.h"

#include "../../../../include/bkTools/Signal"

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

    AbstractGraySliceView::AbstractGraySliceView(self_type&&) = default;

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
    auto AbstractGraySliceView::operator=(self_type&&) -> self_type& = default;

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


