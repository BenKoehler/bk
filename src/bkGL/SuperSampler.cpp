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

#include <bkGL/SuperSampler.h>

#include <algorithm>
#include <utility>

#include <bk/Signal>
#include <bkGL/UBOGlobal.h>
#include <bkGL/WindowGeometry.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture2D.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class SuperSampler::Impl
  {
    public:
      FBO fbo;
      ScreenQuad screenquad;
      Shader shader;
      //ShaderText shaderTextVert;
      //ShaderText shaderTextFrag;
      GLuint factor;
      WindowGeometry window_geometry;
      bk::Signal<GLuint> s_factor_changed;
      bk::Signal<GLuint, GLuint> s_window_size_changed;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : fbo(gl),
            screenquad(gl),
            shader(gl),
          #endif
          factor(static_cast<GLuint>(2))
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;

      ~Impl()
      {
          s_factor_changed.disconnect_all();
          s_window_size_changed.disconnect_all();
      }

      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  SuperSampler::SuperSampler()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else
  SuperSampler::SuperSampler(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  { /* do nothing */ }

  SuperSampler::SuperSampler(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  SuperSampler::~SuperSampler() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET SSAA FACTOR
  GLuint SuperSampler::factor() const
  { return _pdata->factor; }
  /// @}

  /// @{ -------------------------------------------------- GET WINDOW SIZE
  WindowGeometry& SuperSampler::window()
  { return _pdata->window_geometry; }

  const WindowGeometry& SuperSampler::window() const
  { return _pdata->window_geometry; }

  GLuint SuperSampler::width() const
  { return _pdata->window_geometry.width(); }

  GLuint SuperSampler::height() const
  { return _pdata->window_geometry.height(); }
  /// @}

  /// @{ -------------------------------------------------- GET WINDOW SIZE UPSAMPLED
  //! the factor is multiplied to each window dimension 
  WindowGeometry SuperSampler::window_upsampled() const
  { return WindowGeometry(width_upsampled(), height_upsampled()); }

  GLuint SuperSampler::width_upsampled() const
  { return width() * _pdata->factor; }

  GLuint SuperSampler::height_upsampled() const
  { return height() * _pdata->factor; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<GLuint>& SuperSampler::signal_factor_changed()
  { return _pdata->s_factor_changed; }

  const bk::Signal<GLuint>& SuperSampler::signal_factor_changed() const
  { return _pdata->s_factor_changed; }

  bk::Signal<GLuint, GLuint>& SuperSampler::signal_window_size_changed()
  { return _pdata->s_window_size_changed; }

  const bk::Signal<GLuint, GLuint>& SuperSampler::signal_window_size_changed() const
  { return _pdata->s_window_size_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET ID OF FBO
  [[nodiscard]] GLuint SuperSampler::fbo_id() const
  { return _pdata->fbo.id(); }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool SuperSampler::is_initialized() const
  { return _pdata->fbo.is_initialized() && _pdata->screenquad.is_initialized() && _pdata->shader.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto SuperSampler::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET FACTOR
  void SuperSampler::set_factor(GLuint x)
  {
      const GLuint y = std::clamp(x, static_cast<GLuint>(1), static_cast<GLuint>(4));

      if (y != _pdata->factor)
      {
          _pdata->factor = y;

          init();

          _pdata->s_factor_changed.emit_signal(y);
          emit_signal_window_size_changed();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET WINDOW SIZE
  void SuperSampler::set_window_size(const WindowGeometry& window)
  {
      if (_pdata->window_geometry != window)
      {
          _pdata->window_geometry = window;
          emit_signal_window_size_changed();
      }
  }

  void SuperSampler::set_window_size(GLuint w, GLuint h)
  {
      if (_pdata->window_geometry.width() != w || _pdata->window_geometry.height() != h)
      {
          _pdata->window_geometry.set(w, h);
          emit_signal_window_size_changed();
      }
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- EMIT SIGNALS
  void SuperSampler::emit_signal_window_size_changed() const
  { _pdata->s_window_size_changed.emit_signal(width_upsampled(), height_upsampled()); }
  /// @}

  /// @{ -------------------------------------------------- CLEAR
  void SuperSampler::clear()
  {
      _pdata->fbo.clear();
      _pdata->shader.clear();
      //_pdata->shaderTextVert.clear();
      //_pdata->shaderTextFrag.clear();
      _pdata->screenquad.clear();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void SuperSampler::init()
  {
      clear();

      _pdata->screenquad.init();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D color_tex;
      #else
      Texture2D color_tex(this->_gl);
      #endif
      color_tex.set_width(width_upsampled());
      color_tex.set_height(height_upsampled());
      color_tex.set_default_config_rgba_tex();
      color_tex.set_texture_unit_number(texture_unit_number_color_tex());
      color_tex.init();

      color_tex.bind();
      color_tex.set_interpolation_LINEAR();
      color_tex.set_texture_coordinates_CLAMP();
      color_tex.release();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D depth_tex;
      #else
      Texture2D depth_tex(this->_gl);
      #endif
      depth_tex.set_width(width_upsampled());
      depth_tex.set_height(height_upsampled());
      depth_tex.set_default_config_depth_tex();
      depth_tex.set_texture_unit_number(texture_unit_number_depth_tex());
      depth_tex.init();

      depth_tex.bind();
      depth_tex.set_interpolation_LINEAR();
      depth_tex.set_texture_coordinates_CLAMP();
      depth_tex.release();

      _pdata->fbo.set_texture_COLOR_ATTACHMENT(std::move(color_tex), 0);
      _pdata->fbo.set_texture_DEPTH_ATTACHMENT(std::move(depth_tex));
      _pdata->fbo.init();

      /*
       * shader
       */
      using SL = details::ShaderLibrary;
      _pdata->shader.init_from_sources(SL::ssaa::vert(), SL::ssaa::frag());
  }
  /// @}

  /// @{ -------------------------------------------------- CONNECT SIGNALS
  void SuperSampler::connect_signals(std::shared_ptr<details::AbstractRenderable>& r)
  {
      _pdata->s_window_size_changed.connect([=](GLuint w, GLuint h)
                                            { r->on_resize(w, h); });

      _pdata->s_factor_changed.connect([=](GLuint f)
                                       { r->on_ssaa_factor_changed(f); });
  }

  void SuperSampler::connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r)
  {
      _pdata->s_window_size_changed.connect([=](GLuint w, GLuint h)
                                            { r->on_resize(w, h); });

      _pdata->s_factor_changed.connect([=](GLuint f)
                                       { r->on_ssaa_factor_changed(f); });
  }
  /// @}

  /// @{ -------------------------------------------------- BIND/RELEASE FBO
  void SuperSampler::bind_fbo()
  { _pdata->fbo.bind(); }

  void SuperSampler::release_fbo()
  { _pdata->fbo.release(); }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void SuperSampler::draw_init()
  {
      BK_QT_GL glViewport(0, 0, width_upsampled(), height_upsampled());

      bind_fbo();
      BK_QT_GL glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void SuperSampler::draw_finish()
  {
      BK_QT_GL glViewport(0, 0, _pdata->window_geometry.width(), _pdata->window_geometry.height());

      _pdata->fbo.color_attachment(0)->bind();
      _pdata->shader.bind();
      _pdata->screenquad.draw();
      _pdata->shader.release();
      _pdata->fbo.color_attachment(0)->release();
  }
  /// @}
} // namespace bk