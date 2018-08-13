#include "../../../../../include/bkGL/renderable/image/texture/Texture2DView.h"

#include "../../../../../include/bkGL/renderable/ScreenQuad.h"
#include "../../../../../include/bkGL/buffer/UBO.h"
#include "../../../../../include/bkGL/shader/Shader.h"
#include "../../../../../include/bkGL/shader/ShaderLibrary.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct Texture2DView::Impl
  {
      ScreenQuad screenquad;
      UBO ubo;
      Shader shader;
      GLuint tex_unit;

      #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
      #else
      explicit Impl(bk::qt_gl_functions* gl)
          : screenquad(gl),
            ubo(gl),
            shader(gl),
      #endif
          tex_unit(0)
      {}
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  Texture2DView::Texture2DView()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  Texture2DView::Texture2DView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->ubo.set_usage_STATIC_DRAW();
      _pdata->ubo.register_value("tex_unit", _pdata->tex_unit);
  }

  Texture2DView::Texture2DView(self_type&&) = default;

  Texture2DView::~Texture2DView()
  {}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  GLuint Texture2DView::texture_unit() const
  { return _pdata->tex_unit; }

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool Texture2DView::is_initialized() const
  {return _pdata->screenquad.is_initialized();}
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void Texture2DView::set_texture_unit(GLuint i)
  {
      if (_pdata->tex_unit != i)
      {
          _pdata->tex_unit = i;

          if (this->is_initialized())
          {
              _pdata->ubo.write_registered_value("tex_unit", &_pdata->tex_unit);
              _pdata->ubo.release();

              this->emit_signal_update_required();
          }
      }
  }

  auto Texture2DView::operator=(self_type&& other) -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void Texture2DView::clear_shader()
  { _pdata->shader.clear(); }

  void Texture2DView::clear_buffers()
  {
      _pdata->screenquad.clear();
      _pdata->ubo.clear();
  }

  void Texture2DView::clear()
  {
      clear_shader();
      clear_buffers();
  }

  void Texture2DView::init_shader()
  {
      clear_shader();
      _pdata->shader.init("shader/texture/render_texture.vert", "shader/texture/render_texture_rgb.frag");
  }

  void Texture2DView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();
      _pdata->ubo.write_registered_value("tex_unit", &_pdata->tex_unit);
      _pdata->ubo.release();
  }

  void Texture2DView::init()
  {
      _pdata->screenquad.init();
      init_shader();
      init_ubo();
      this->emit_signal_update_required();
  }

  /// @{ -------------------------------------------------- EVENTS
  void Texture2DView::on_resize(GLint /*w*/, GLint /*h*/) { /* do nothing */ }
  void Texture2DView::on_oit_enabled(bool /*b*/) { /* do nothing */ }
  void Texture2DView::on_animation_enabled(bool /*b*/) { /* do nothing */ }
  void Texture2DView::on_modelview_changed(bool /*b*/) { /* do nothing */ }
  void Texture2DView::on_visible_changed(bool /*b*/) { /* do nothing */ }
  void Texture2DView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/) { /* do nothing */ }
  void Texture2DView::on_mouse_button_pressed(MouseButton /*btn*/) { /* do nothing */ }
  void Texture2DView::on_mouse_button_released(MouseButton /*btn*/) { /* do nothing */ }
  void Texture2DView::on_ssaa_factor_changed(GLint /*ssaa_factor*/) { /* do nothing */ }
  /// @}

  void Texture2DView::draw_impl()
  {
      _pdata->ubo.bind_to_base(1);

      _pdata->shader.bind();
      _pdata->screenquad.draw();
      _pdata->shader.release();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


