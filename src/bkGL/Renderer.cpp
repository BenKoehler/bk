/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#include <bkGL/Renderer.h>

#include <algorithm>
#include <filesystem>
#include <future>
#include <memory>
#include <vector>

#include <bk/Algorithm>
#include <bk/Clock>
#include <bk/Signal>
#include <bk/ThreadPool>
#include <bkMath/functions/equals_approx.h>
#include <bkMath/functions/num_digits.h>

#ifdef BK_LIB_PNG_AVAILABLE

    #include <bk/Image>
    #include <bk/StringUtils>

#endif

#ifdef BK_EMIT_PROGRESS

    #include <bk/Progress>
    #include <bk/Localization>

#endif

#include <bkGL/Animator.h>
#include <bkGL/Interactor.h>
#include <bkGL/OrderIndependentTransparency.h>
#include <bkGL/SuperSampler.h>
#include <bkGL/UBOGlobal.h>
#include <bkGL/gl_definitions.h>
#include <bkGL/gl_error.h>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/renderable/background/GradientBackground.h>
#include <bkGL/Camera.h>
#include <bkGL/Mouse.h>
#include <bkGL/WindowGeometry.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/texture/Texture2D.h>
#include <bkGL/trackball/TrackBall.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct Renderer::Impl
  {
      //std::shared_ptr<GradientBackground> background;
      std::shared_ptr<details::AbstractRenderable> background;
      details::UBOGlobal ubo_global;
      OrderIndependentTransparency oit;
      SuperSampler supersampler;
      WindowGeometry window_geometry;
      Interactor interactor;
      Camera camera;
      ColMat4<GLfloat> modelview_matrix;
      bool modelview_matrix_changed;
      GLuint fbo_default_id;
      bk::Vec3<GLfloat> center_translation;
      std::vector<std::shared_ptr<details::AbstractSceneRenderable>> scene_renderables;
      std::vector<bk::Vec3<GLfloat>> centers;
      std::vector<std::shared_ptr<details::AbstractRenderable>> renderables;
      Animator animator;
      bk::Signal<> s_update_required;
      bk::Signal<const ColMat4<GLfloat>&> s_new_modelview_matrix;
      bk::Signal<std::string, std::shared_ptr<bk::CartesianImage<bk::Vec<3, double>, 2>>> s_video_rendering_new_frame;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          : background(std::make_shared<GradientBackground>()),
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : background(std::make_shared<GradientBackground>(gl)),
            ubo_global(gl),
            oit(gl),
            supersampler(gl),
          #endif
            modelview_matrix_changed(true),
            fbo_default_id(0)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  Renderer::Renderer()
      : base_type()
  #else

  Renderer::Renderer(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->renderables.reserve(10);
      _pdata->scene_renderables.reserve(10);

      //------------------------------------------------------------------------------------------------------
      // animator signals
      //------------------------------------------------------------------------------------------------------
      _pdata->animator.signal_enabled_changed().connect([&](bool enabled)
                                                        {
                                                            if (this->is_initialized())
                                                            {
                                                                this->_pdata->ubo_global.set_animation_enabled(enabled ? 1 : 0);
                                                                this->_pdata->ubo_global.release();
                                                                this->emit_signal_update_required();
                                                            }
                                                        });
      _pdata->animator.signal_current_time_changed().connect([&](GLfloat currentTime)
                                                             {
                                                                 if (this->is_initialized())
                                                                 {
                                                                     this->_pdata->ubo_global.set_animation_current_time(currentTime);
                                                                     this->_pdata->ubo_global.release();
                                                                     this->emit_signal_update_required();
                                                                 }
                                                             });
      _pdata->animator.signal_max_time_changed().connect([&](GLfloat maxTime)
                                                         {
                                                             if (this->is_initialized())
                                                             {
                                                                 this->_pdata->ubo_global.set_animation_max_time(maxTime);
                                                                 this->_pdata->ubo_global.release();
                                                                 this->emit_signal_update_required();
                                                             }
                                                         });
      _pdata->animator.signal_paused_changed().connect([&](bool)
                                                       {
                                                           if (this->is_initialized())
                                                           { this->emit_signal_update_required(); }
                                                       });
      _pdata->animator.signal_speed_settings_changed().connect([&]()
                                                               {
                                                                   if (this->is_initialized())
                                                                   { this->emit_signal_update_required(); }
                                                               });

      //------------------------------------------------------------------------------------------------------
      // oit signals
      //------------------------------------------------------------------------------------------------------
      _pdata->oit.signal_enabled_changed().connect([&](bool)
                                                   {
                                                       if (this->is_initialized())
                                                       { this->emit_signal_update_required(); }
                                                   });

      //------------------------------------------------------------------------------------------------------
      // interactor signals
      //------------------------------------------------------------------------------------------------------
      _pdata->interactor.signal_do_update().connect([&]()
                                                    { this->emit_signal_update_required(); });
      _pdata->interactor.signal_require_modelview_update().connect([&]()
                                                                   { this->set_modelview_matrix_changed(true); });

      //------------------------------------------------------------------------------------------------------
      // supersampler signals
      //------------------------------------------------------------------------------------------------------
      _pdata->supersampler.signal_factor_changed().connect([&](GLuint x)
                                                           {
                                                               if (this->is_initialized())
                                                               {
                                                                   this->_pdata->ubo_global.set_ssaa_factor(x);
                                                                   this->_pdata->ubo_global.release();

                                                                   this->_pdata->oit.on_resize(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());
                                                               }
                                                           });

      _pdata->supersampler.signal_window_size_changed().connect([&](GLuint w, GLuint h)
                                                                {
                                                                    if (this->is_initialized())
                                                                    {
                                                                        this->_pdata->ubo_global.set_window_width(w);
                                                                        this->_pdata->ubo_global.set_window_height(h);
                                                                        this->_pdata->ubo_global.release();

                                                                        //this->_pdata->oit.on_resize(w, h); // done in Renderer::resize

                                                                        this->emit_signal_update_required();
                                                                    }
                                                                });
      //------------------------------------------------------------------------------------------------------
      // camera signals
      //------------------------------------------------------------------------------------------------------
      _pdata->camera.signal_projection_matrix_changed().connect([&]()
                                                                {
                                                                    if (this->is_initialized())
                                                                    {
                                                                        this->_pdata->ubo_global.set_projection_matrix(this->_pdata->camera.projection_matrix_data_ptr());
                                                                        this->_pdata->ubo_global.release();
                                                                    }
                                                                });
      _pdata->camera.signal_lookat_matrix_changed().connect([&]()
                                                            {
                                                                if (this->is_initialized())
                                                                {
                                                                    this->_pdata->ubo_global.set_lookat_matrix(this->_pdata->camera.look_at_matrix_data_ptr());
                                                                    this->_pdata->ubo_global.release();
                                                                }
                                                            });

      connect_signals_renderable(_pdata->background);
  }

  Renderer::Renderer(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Renderer::~Renderer() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET WINDOW SIZE
  WindowGeometry& Renderer::window()
  { return _pdata->window_geometry; }

  const WindowGeometry& Renderer::window() const
  { return _pdata->window_geometry; }

  unsigned int Renderer::width() const
  { return _pdata->window_geometry.width(); }

  unsigned int Renderer::height() const
  { return _pdata->window_geometry.height(); }
  /// @}

  /// @{ -------------------------------------------------- GET BACKGROUND
  //GradientBackground& Renderer::background()
  //{ return *_pdata->background; }
  //
  //const GradientBackground& Renderer::background() const
  //{ return *_pdata->background; }

  GradientBackground& Renderer::background()
  { return *static_cast<GradientBackground*>(_pdata->background.get()); }

  const GradientBackground& Renderer::background() const
  { return *static_cast<GradientBackground*>(_pdata->background.get()); }
  /// @}

  /// @{ -------------------------------------------------- GET CAMERA
  Camera& Renderer::camera()
  { return _pdata->camera; }

  const Camera& Renderer::camera() const
  { return _pdata->camera; }

  bk::Vec3<GLfloat> Renderer::center_translation() const
  { return _pdata->center_translation; }
  /// @}

  /// @{ -------------------------------------------------- GET MATRICES
  //const bk::ColMat4<GLfloat>& Renderer::modelview_matrix() const
  //{ return _pdata->modelview_matrix; }

  //const bk::ColMat4<GLfloat>& Renderer::projection_matrix() const
  //{ return _pdata->camera.projection_matrix(); }
  /// @}

  /// @{ -------------------------------------------------- GET OIT
  OrderIndependentTransparency& Renderer::oit()
  { return _pdata->oit; }

  const OrderIndependentTransparency& Renderer::oit() const
  { return _pdata->oit; }
  /// @}

  /// @{ -------------------------------------------------- GET DEFAULT FBO ID
  GLuint Renderer::default_fbo_id() const
  //{ return _pdata->supersampler.factor() <= 1 ? _pdata->fbo_default_id : _pdata->fbo_ssaa.id(); }
  { return _pdata->supersampler.fbo_id(); }
  /// @}

  /// @{ -------------------------------------------------- GET INTERACTOR
  [[nodiscard]] Interactor& Renderer::interactor()
  { return _pdata->interactor; }

  [[nodiscard]] const Interactor& Renderer::interactor() const
  { return _pdata->interactor; }
  /// @}

  /// @{ -------------------------------------------------- GET ANIMATOR
  Animator& Renderer::animator()
  { return _pdata->animator; }

  const Animator& Renderer::animator() const
  { return _pdata->animator; }

  bk::Signal<GLfloat>& Renderer::signal_animation_current_time_changed()
  { return _pdata->animator.signal_current_time_changed(); }

  const bk::Signal<GLfloat>& Renderer::signal_animation_current_time_changed() const
  { return _pdata->animator.signal_current_time_changed(); }
  /// @}

  /// @{ -------------------------------------------------- GET SUPERSAMPLER
  SuperSampler& Renderer::supersampler()
  { return _pdata->supersampler; }

  const SuperSampler& Renderer::supersampler() const
  { return _pdata->supersampler; }
  /// @}

  /// @{ -------------------------------------------------- GET RENDER SETTINGS
  unsigned int Renderer::num_scene_renderables() const
  { return _pdata->scene_renderables.size(); }

  unsigned int Renderer::num_renderables() const
  { return _pdata->renderables.size(); }

  unsigned int Renderer::num_all_renderables() const
  { return num_scene_renderables() + num_renderables(); }
  /// @}

  /// @{ -------------------------------------------------- GET UBO
  [[nodiscard]] const details::UBOGlobal& Renderer::ubo_global() const
  { return _pdata->ubo_global; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<>& Renderer::signal_update_required()
  { return _pdata->s_update_required; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool Renderer::is_initialized() const
  { return _pdata->background->is_initialized() && _pdata->ubo_global.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void Renderer::set_modelview_matrix_changed(bool b)
  {
      _pdata->modelview_matrix_changed = b;

      for (auto& r: _pdata->renderables)
      { r->set_modelview_matrix_changed(_pdata->modelview_matrix_changed); }

      for (auto& r: _pdata->scene_renderables)
      { r->set_modelview_matrix_changed(_pdata->modelview_matrix_changed); }

      _pdata->background->set_modelview_matrix_changed(_pdata->modelview_matrix_changed);
  }

  void Renderer::clear_scene_renderables()
  {
      _pdata->scene_renderables.clear();
      _pdata->centers.clear();
      _pdata->center_translation.set_zero();
      this->emit_signal_update_required();
  }

  void Renderer::clear_renderables()
  {
      _pdata->renderables.clear();
      this->emit_signal_update_required();
  }

  void Renderer::add_scene_renderable(std::shared_ptr<details::AbstractSceneRenderable>&& r)
  {
      _pdata->centers.push_back(r->center());
      _pdata->center_translation = bk::mean(_pdata->centers.begin(), _pdata->centers.end()) * (-1);
      set_modelview_matrix_changed(true);
      _pdata->scene_renderables.emplace_back(std::move(r));

      connect_signals_scene_renderable(_pdata->scene_renderables.back());
      this->emit_signal_update_required();
  }

  void Renderer::add_renderable(std::shared_ptr<details::AbstractRenderable>&& r)
  {
      _pdata->renderables.emplace_back(std::move(r));

      connect_signals_renderable(_pdata->renderables.back());
      this->emit_signal_update_required();
  }

  void Renderer::set_default_fbo_id(GLuint id)
  { _pdata->fbo_default_id = id; }

  /// @{ -------------------------------------------------- OPERATOR =
  auto Renderer::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void Renderer::emit_signal_update_required()
  {
      if (this->is_initialized())
      { _pdata->s_update_required.emit_signal(); }
  }

  /// @{ -------------------------------------------------- CONNECT SIGNALS
  void Renderer::connect_signals_scene_renderable(std::shared_ptr<details::AbstractSceneRenderable>& r)
  {
      r->signal_update_required().connect([&]()
                                          { this->emit_signal_update_required(); });

      r->signal_bind_default_fbo().connect([&]()
                                           { this->bind_default_fbo(); });

      r->signal_bind_default_fbo_as_read().connect([&]()
                                                   { this->bind_default_fbo_as_read(); });

      r->signal_bind_default_fbo_as_draw().connect([&]()
                                                   { this->bind_default_fbo_as_draw(); });

      r->signal_scene_changed().connect([&]()
                                        { this->update_centers(); });

      _pdata->oit.connect_signals(r);
      r->set_oit_available(_pdata->oit.is_enabled());

      _pdata->animator.connect_signals(r);
      r->set_animation_is_enabled(_pdata->animator.is_enabled());

      _pdata->supersampler.connect_signals(r);
      r->on_resize(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());
      r->on_ssaa_factor_changed(supersampler().factor());

      _pdata->interactor.connect_signals(r);
      r->on_mouse_pos_changed(_pdata->interactor.mouse().x(), _pdata->interactor.mouse().y());

      _pdata->camera.connect_signals(r);
      r->set_new_projection_matrix(_pdata->camera.projection_matrix());

      _pdata->s_new_modelview_matrix.connect([=](const ColMat4<GLfloat>& m)
                                             { r->set_new_modelview_matrix(m); });
      r->set_new_modelview_matrix(_pdata->modelview_matrix);

      r->set_modelview_matrix_changed(_pdata->modelview_matrix_changed);
  }

  void Renderer::connect_signals_renderable(std::shared_ptr<details::AbstractRenderable>& r)
  {
      r->signal_update_required().connect([&]()
                                          { this->emit_signal_update_required(); });

      r->signal_bind_default_fbo().connect([&]()
                                           { this->bind_default_fbo(); });

      r->signal_bind_default_fbo_as_read().connect([&]()
                                                   { this->bind_default_fbo_as_read(); });

      r->signal_bind_default_fbo_as_draw().connect([&]()
                                                   { this->bind_default_fbo_as_draw(); });

      _pdata->oit.connect_signals(r);
      r->set_oit_available(_pdata->oit.is_enabled());

      _pdata->animator.connect_signals(r);
      r->set_animation_is_enabled(_pdata->animator.is_enabled());

      _pdata->supersampler.connect_signals(r);
      r->on_resize(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());
      r->on_ssaa_factor_changed(supersampler().factor());

      _pdata->interactor.connect_signals(r);
      r->on_mouse_pos_changed(_pdata->interactor.mouse().x(), _pdata->interactor.mouse().y());

      _pdata->camera.connect_signals(r);
      r->set_new_projection_matrix(_pdata->camera.projection_matrix());

      _pdata->s_new_modelview_matrix.connect([=](const ColMat4<GLfloat>& m)
                                             { r->set_new_modelview_matrix(m); });
      r->set_new_modelview_matrix(_pdata->modelview_matrix);

      r->set_modelview_matrix_changed(_pdata->modelview_matrix_changed);
  }
  /// @}

  void Renderer::clear()
  {
      //_pdata->background->clear();
      background().clear();
      _pdata->interactor.clear();
      _pdata->supersampler.clear();
      _pdata->scene_renderables.clear();
      _pdata->renderables.clear();
      _pdata->ubo_global.clear();
      _pdata->center_translation.set_zero();
      _pdata->centers.clear();
      _pdata->modelview_matrix.set_identity();

      _pdata->oit.clear();
  }

  void Renderer::init()
  {
      clear();

      #ifdef BK_DEBUG_GL
      BK_QT_GL glEnable(GL_DEBUG_OUTPUT);
      BK_QT_GL glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      BK_QT_GL glDebugMessageCallback(openglCallbackFunction, nullptr);
      #endif

      //_pdata->background->set_color_bottom_or_left(0.05, 0.05, 0.05, 1);
      //_pdata->background->set_color_top_or_right(0.15, 0.15, 0.15, 1);
      //_pdata->background->init();
      background().set_color_bottom_or_left(0.05, 0.05, 0.05, 1);
      background().set_color_top_or_right(0.15, 0.15, 0.15, 1);
      background().init();

      _pdata->camera.init();
      _pdata->ubo_global.init_from_registered_values_size();

      _pdata->ubo_global.set_modelview_matrix(&_pdata->modelview_matrix[0]);
      _pdata->ubo_global.set_projection_matrix(_pdata->camera.projection_matrix_data_ptr());
      _pdata->ubo_global.set_rotation_matrix(_pdata->interactor.trackball().rotation_matrix_ptr());
      _pdata->ubo_global.set_lookat_matrix(_pdata->camera.look_at_matrix_data_ptr());
      _pdata->ubo_global.set_cam_pos_x(_pdata->camera.position()[0]);
      _pdata->ubo_global.set_cam_pos_y(_pdata->camera.position()[1]);
      _pdata->ubo_global.set_cam_pos_z(_pdata->camera.position()[2]);
      _pdata->ubo_global.set_cam_znear(_pdata->camera.znear());
      _pdata->ubo_global.set_cam_zfar(_pdata->camera.zfar());
      _pdata->ubo_global.set_window_width(_pdata->supersampler.width_upsampled());
      _pdata->ubo_global.set_window_height(_pdata->supersampler.height_upsampled());
      _pdata->ubo_global.set_ssaa_factor(_pdata->supersampler.factor());
      _pdata->ubo_global.release();

      //_camera.set_aspect_ratio(_window_geometry.aspect_ratio());
      //_camera.calc_projection_matrix();

      _pdata->interactor.set_window_size(_pdata->window_geometry);
      _pdata->interactor.init();
      //_pdata->interactor.trackball().set_current_mouse_position(0, 0);
      //_pdata->interactor.trackball().set_radius(1);
      //_pdata->interactor.trackball().set_upvector_id(2);

      _pdata->oit.set_window_size(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());
      _pdata->oit.init_if_enabled();

      _pdata->supersampler.init();
  }

  void Renderer::update_centers()
  {
      _pdata->centers.clear();

      if (_pdata->scene_renderables.size() != 0)
      {
          for (auto& r: _pdata->scene_renderables)
          {
              if (!equals_approx(r->center().norm(), 0))
              { _pdata->centers.push_back(r->center()); }
          }

          if (!_pdata->centers.empty())
          { _pdata->center_translation = bk::mean(_pdata->centers.begin(), _pdata->centers.end()) * (-1); }

          set_modelview_matrix_changed(true);

          this->emit_signal_update_required();
      }
  }

  std::unique_ptr<CartesianImage<Vec<3, double>, 2>> Renderer::render_screenshot(int ssaafac, int sizex, int sizey)
  {
      const bk::Vec2i oldSize(_pdata->window_geometry.width(), _pdata->window_geometry.height());
      const int oldSSAAFactor = _pdata->supersampler.factor();
      const bk::Vec2i newSize(sizex > 0 ? sizex : oldSize[0], sizey > 0 ? sizey : oldSize[1]);
      const int newSSAAFactor = ssaafac > 0 ? ssaafac : oldSSAAFactor;
      const bool changeSize = newSSAAFactor != oldSSAAFactor || newSize[0] != oldSize[0] || newSize[1] != oldSize[1];

      if (changeSize)
      {
          _pdata->window_geometry.set(newSize[0], newSize[1]);
          _pdata->supersampler.set_factor(newSSAAFactor);
          this->resize(newSize[0], newSize[1]);
          this->draw();
      }

      const bk::Vec2i size_ssaa(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());

      _pdata->supersampler.bind_fbo();

      constexpr const int num_values_per_pixel = 3;
      std::vector<GLubyte> buf(size_ssaa[0] * size_ssaa[1] * num_values_per_pixel);
      BK_QT_GL glReadPixels(0, 0, size_ssaa[0], size_ssaa[1], GL_RGB, GL_UNSIGNED_BYTE, buf.data());

      auto img = std::make_unique<CartesianImage<Vec<num_values_per_pixel, double>, 2>>();

      //------------------------------------------------------------------------------------------------------

      /*img->set_size(newSize[0], newSize[1]);

      const int ssaafac2 = newSSAAFactor * newSSAAFactor;

      #pragma omp parallel for
      for (int y = 0; y < size_ssaa[1]; y += newSSAAFactor)
      {
          for (int x = 0; x < size_ssaa[0]; x += newSSAAFactor)
          {
              const unsigned int lid = grid_to_list_id(newSize, x / newSSAAFactor, y / newSSAAFactor);
              (*img)[lid].set_zero();

              for (int dx = 0; dx < newSSAAFactor; ++dx)
              {
                  for (int dy = 0; dy < newSSAAFactor; ++dy)
                  {
                      const unsigned int off = (size_ssaa[1] - 1 - (y + dy)) * size_ssaa[0] * num_values_per_pixel + (x + dx) * num_values_per_pixel;

                      for (unsigned int k = 0; k < num_values_per_pixel; ++k)
                      { (*img)[lid][k] += buf[off + k]; }
                  } // for dy
              } // for dx

              (*img)[lid] /= ssaafac2;
          } // for x
      } // for y*/

      //------------------------------------------------------------------------------------------------------

      img->set_size(size_ssaa[0], size_ssaa[1]);

      #pragma omp parallel for
      for (int y = size_ssaa[1] - 1; y >= 0; --y)
      {
          unsigned int cnt = (size_ssaa[1] - 1 - y) * size_ssaa[0] * num_values_per_pixel;

          for (int x = 0; x < size_ssaa[0]; ++x)
          {
              for (unsigned int k = 0; k < num_values_per_pixel; ++k)
              { (*img)(x, y)[k] = buf[cnt++]; }
          } // for x
      } // for y

      _bind_default_fbo();

      if (changeSize)
      {
          _pdata->window_geometry.set(oldSize[0], oldSize[1]);
          _pdata->supersampler.set_factor(oldSSAAFactor);
          this->resize(oldSize[0], oldSize[1]);
          this->draw();
      }

      return std::move(img);
  }

  void Renderer::render_video(const double fps, const double length_in_s, int ssaafac, int sizex, int sizey)
  {
      //------------------------------------------------------------------------------------------------------
      // setup new rendering image size & animation parameters
      //------------------------------------------------------------------------------------------------------
      const bk::Vec2i oldSize(_pdata->window_geometry.width(), _pdata->window_geometry.height());
      const int oldSSAAFactor = _pdata->supersampler.factor();
      const bk::Vec2i newSize(sizex > 0 ? sizex : oldSize[0], sizey > 0 ? sizey : oldSize[1]);
      const int newSSAAFactor = ssaafac > 0 ? ssaafac : oldSSAAFactor;
      const bool changeSize = newSSAAFactor != oldSSAAFactor || newSize[0] != oldSize[0] || newSize[1] != oldSize[1];

      if (changeSize)
      {
          _pdata->supersampler.set_factor(newSSAAFactor);
          this->resize(newSize[0], newSize[1]);
      }

      const bk::Vec2i size_ssaa(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());
      const GLfloat oldFPS = _pdata->animator.target_fps();
      const GLfloat oldSpeedFactor = _pdata->animator.speed_factor();
      const GLfloat newFPS = fps;
      const GLfloat newSpeedFactor = _pdata->animator.max_time()/*in ms*/ / (length_in_s * 1000);
      _pdata->animator.set_parameters(newSpeedFactor, newFPS);

      //------------------------------------------------------------------------------------------------------
      // rendering loop & image appending to video
      //------------------------------------------------------------------------------------------------------
      _pdata->animator.set_enabled(true);
      _pdata->animator.set_paused(true);

      const int N = std::floor(length_in_s * newFPS);
      constexpr const int num_values_per_pixel = 3;
      GLubyte* buf = new GLubyte[size_ssaa[0] * size_ssaa[1] * num_values_per_pixel];

      _pdata->animator.set_current_time(0);

      constexpr const unsigned int fileIndexLength = 5;

      #ifdef BK_EMIT_PROGRESS
      Progress& prog = bk_progress.emplace_task(N, ___("Rendering video frames"));
      #endif

      std::vector<std::future<void>> tasks_save_img;
      tasks_save_img.reserve(N);

      const int ssaafac2 = ssaafac * ssaafac;

      for (unsigned int i = 0; i < static_cast<unsigned int>(N); ++i)
      {
          _pdata->animator.advance_current_time();

          draw();

          _pdata->supersampler.bind_fbo();
          BK_QT_GL glReadPixels(0, 0, size_ssaa[0], size_ssaa[1], GL_RGB, GL_UNSIGNED_BYTE, buf);

          auto img = std::make_shared<CartesianImage<Vec<num_values_per_pixel, double>, 2>>();
          img->set_size(newSize[0], newSize[1]);

          #pragma omp parallel for
          for (int y = 0; y < size_ssaa[1]; y += ssaafac)
          {
              for (int x = 0; x < size_ssaa[0]; x += ssaafac)
              {
                  const unsigned int lid = grid_to_list_id(newSize, x / ssaafac, y / ssaafac);
                  (*img)[lid].set_zero();

                  for (int dx = 0; dx < ssaafac; ++dx)
                  {
                      for (int dy = 0; dy < ssaafac; ++dy)
                      {
                          const unsigned int off = (size_ssaa[1] - 1 - (y + dy)) * size_ssaa[0] * num_values_per_pixel + (x + dx) * num_values_per_pixel;

                          for (unsigned int k = 0; k < num_values_per_pixel; ++k)
                          { (*img)[lid][k] += buf[off + k]; }
                      } // for dy
                  } // for dx

                  (*img)[lid] /= ssaafac2;
              } // for x
          } // for y

          _bind_default_fbo();

          std::string filename = "";
          const unsigned int currentIndexLength = num_digits_int(i);

          for (unsigned int k = 0; k < fileIndexLength - currentIndexLength; ++k)
          { filename = bk::string_utils::append(filename, "0"); }

          filename = bk::string_utils::append(filename, bk::string_utils::from_number(i));

          this->_pdata->s_video_rendering_new_frame.emit_signal(filename, img);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif
      }

      //------------------------------------------------------------------------------------------------------
      // restore old rendering image size & animation parameters
      //------------------------------------------------------------------------------------------------------
      delete[] buf;
      _bind_default_fbo();

      _pdata->animator.set_parameters(oldSpeedFactor, oldFPS);

      if (changeSize)
      {
          _pdata->supersampler.set_factor(oldSSAAFactor);
          this->resize(oldSize[0], oldSize[1]);
          this->draw();
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif
  }

      #ifdef BK_LIB_PNG_AVAILABLE

  //std::future<void> Renderer::save_screenshot(const std::string& path, int ssaafac, int sizex, int sizey)
  //{
      //std::unique_ptr<CartesianImage<Vec<3, double>, 2>> img = render_screenshot(ssaafac, sizex, sizey);
      //
      //return bk_threadpool.enqueue([imgPtr = std::move(img), savePath = path]()
      //                             { imgPtr->save_png(savePath); });
  //}

  std::future<void> Renderer::save_screenshot(const std::string& path)
  {
      const GLuint size_ssaa[2] = {_pdata->supersampler.width_upsampled(),_pdata->supersampler.height_upsampled()};

      _pdata->supersampler.bind_fbo();

      constexpr const int num_values_per_pixel = 3;
      std::vector<GLubyte> buf(size_ssaa[0] * size_ssaa[1] * num_values_per_pixel);
      BK_QT_GL glReadPixels(0, 0, size_ssaa[0], size_ssaa[1], GL_RGB, GL_UNSIGNED_BYTE, buf.data());

      auto img = std::make_unique<CartesianImage<Vec<num_values_per_pixel, double>, 2>>();

      img->set_size(size_ssaa[0], size_ssaa[1]);

      #pragma omp parallel for
      for (int y = static_cast<int>(size_ssaa[1]) - 1; y >= 0; --y)
      {
          unsigned int cnt = (size_ssaa[1] - 1 - y) * size_ssaa[0] * num_values_per_pixel;

          for (int x = 0; x < static_cast<int>(size_ssaa[0]); ++x)
          {
              for (unsigned int k = 0; k < num_values_per_pixel; ++k)
              { (*img)(x, y)[k] = buf[cnt++]; }
          } // for x
      } // for y

      _pdata->supersampler.release_fbo(); // compatible with ImGui progress bar
      //_bind_default_fbo();

      return bk_threadpool.enqueue([imgPtr = std::move(img), savePath = path]()
                                   { imgPtr->save_png(savePath); });

  }

  void Renderer::save_video(const std::string& path, const double fps, const double length_in_s, int ssaafac, int sizex, int sizey)
  {
      std::string path_ = path;
      path_ = bk::string_utils::replace(path_, "\\", "/");
      if (!bk::string_utils::ends_with(path_, "/"))
      { path_ = bk::string_utils::append(path_, "/"); }

      std::filesystem::create_directories(path_.c_str());

      std::vector<std::future<void>> tasks_save_img;
      tasks_save_img.reserve(20);

      const unsigned int slotid = _pdata->s_video_rendering_new_frame.connect([&](std::string filename, std::shared_ptr<CartesianImage<Vec<3, double>, 2>> img)
                                                                              {
                                                                                  tasks_save_img.push_back(bk_threadpool.enqueue([=]()
                                                                                                                                 {
                                                                                                                                     img->save_png(path_ + filename);
                                                                                                                                     img->set_size(1, 1);
                                                                                                                                 }));

                                                                                  if (tasks_save_img.size() >= 15)
                                                                                  {
                                                                                      for (auto& t: tasks_save_img)
                                                                                      { t.get(); }

                                                                                      tasks_save_img.clear();
                                                                                  }
                                                                              });

      render_video(fps, length_in_s, ssaafac, sizex, sizey);

      _pdata->s_video_rendering_new_frame.disconnect(slotid);

      for (auto& t: tasks_save_img)
      { t.get(); }
  }

  bk::Signal<std::string, std::shared_ptr<CartesianImage<Vec<3, double>, 2>>>& Renderer::signal_video_rendering_new_frame()
  { return _pdata->s_video_rendering_new_frame; }

      #endif // BK_LIB_PNG_AVAILABLE

  //====================================================================================================
  //===== GL FUNCTIONS
  //====================================================================================================
  void Renderer::draw()
  {
      if (!this->is_initialized())
      { return; }

      //BK_QT_GL glPushAttrib(GL_DEPTH_TEST | GL_BLEND | GL_LIGHTING | GL_ALPHA_TEST | GL_STENCIL_TEST);
      BK_QT_GL glPushAttrib(GL_ENABLE_BIT);

      BK_QT_GL glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      BK_QT_GL glDisable(GL_LIGHTING);
      BK_QT_GL glEnable(GL_DEPTH_TEST);
      BK_QT_GL glDepthFunc(GL_LESS);

      _pdata->ubo_global.bind_to_default_base();
      BK_QT_GL glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      _pdata->oit.blendFunc();

      BK_QT_GL glEnable(GL_ALPHA_TEST);
      BK_QT_GL glAlphaFunc(GL_GREATER, static_cast<GLclampf>(0.025));

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();
      BK_QT_GL glMultMatrixf(_pdata->camera.projection_matrix_data_ptr());

      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      _pdata->ubo_global.set_window_width(_pdata->supersampler.width_upsampled());
      _pdata->ubo_global.set_window_height(_pdata->supersampler.height_upsampled());
      _pdata->ubo_global.release();

      _pdata->supersampler.draw_init();

      _pdata->background->draw();

      if (!_pdata->modelview_matrix_changed)
      { BK_QT_GL glMultMatrixf(&_pdata->modelview_matrix[0]); }
      else
      {
          BK_QT_GL glMultMatrixf(_pdata->camera.look_at_matrix_data_ptr());
          BK_QT_GL glTranslatef(-_pdata->camera.position()[0], -_pdata->camera.position()[1], -_pdata->camera.position()[2]);
          BK_QT_GL glTranslatef(_pdata->interactor.translation()[0], _pdata->interactor.translation()[1], _pdata->interactor.translation()[2]);
          BK_QT_GL glScalef(_pdata->interactor.scale_manual()[0], _pdata->interactor.scale_manual()[1], _pdata->interactor.scale_manual()[2]);
          BK_QT_GL glScalef(_pdata->interactor.scale_fixed(), _pdata->interactor.scale_fixed(), _pdata->interactor.scale_fixed());
          BK_QT_GL glMultMatrixf(_pdata->interactor.trackball().rotation_matrix_ptr());
          BK_QT_GL glTranslatef(_pdata->center_translation[0], _pdata->center_translation[1], _pdata->center_translation[2]);

          // update modelview matrix in buffer
          _pdata->modelview_matrix_changed = false;
          BK_QT_GL glGetFloatv(GL_MODELVIEW_MATRIX, &_pdata->modelview_matrix[0]);

          _pdata->s_new_modelview_matrix.emit_signal(_pdata->modelview_matrix);

          _pdata->ubo_global.set_modelview_matrix(&_pdata->modelview_matrix[0]);
          _pdata->ubo_global.set_rotation_matrix(_pdata->interactor.trackball().rotation_matrix_ptr());

          const bk::Mat4<GLfloat> MVP = _pdata->camera.projection_matrix() * _pdata->modelview_matrix;
          _pdata->ubo_global.set_modelview_projection_matrix(&MVP[0]);

          _pdata->ubo_global.set_lookat_matrix(_pdata->camera.look_at_matrix_data_ptr());
          _pdata->ubo_global.set_cam_pos_x(_pdata->camera.position()[0]);
          _pdata->ubo_global.set_cam_pos_y(_pdata->camera.position()[1]);
          _pdata->ubo_global.set_cam_pos_z(_pdata->camera.position()[2]);
          _pdata->ubo_global.release();
      }

      for (const auto& r: _pdata->scene_renderables)
      { r->draw_opaque(); }

      _pdata->oit.draw_init();

      for (const auto& r: _pdata->scene_renderables)
      { r->draw_transparent(); }

      _pdata->oit.draw_finish();

      for (const auto& r: _pdata->renderables)
      { r->draw(); }

      _pdata->ubo_global.set_window_width(_pdata->supersampler.width());
      _pdata->ubo_global.set_window_height(_pdata->supersampler.height());
      _pdata->ubo_global.release();

      _bind_default_fbo();

      _pdata->supersampler.draw_finish();

      _pdata->ubo_global.release_from_base();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->animator.update();

      if (_pdata->animator.is_enabled() && !_pdata->animator.is_paused())
      { this->emit_signal_update_required(); }
  }

  void Renderer::mouse_move(GLint x, GLint y)
  { _pdata->interactor.mouse_move(x, y); }

  void Renderer::mouse_pressed(MouseButton_ btn)
  { _pdata->interactor.mouse_pressed(btn); }

  void Renderer::mouse_released(MouseButton_ btn)
  { _pdata->interactor.mouse_released(btn); }

  /// @{ -------------------------------------------------- WHEEL EVENTS
  void Renderer::wheel_down()
  { _pdata->interactor.wheel_down(); }

  void Renderer::wheel_up()
  { _pdata->interactor.wheel_up(); }
  /// @}

  /// @{ -------------------------------------------------- KEY EVENTS
  void Renderer::key_pressed(Key_ k)
  { _pdata->interactor.key_pressed(k); }

  void Renderer::key_released(Key_ k)
  { _pdata->interactor.key_released(k); }
  /// @}

  void Renderer::resize(GLint w, GLint h)
  {
      _pdata->window_geometry.set(w, h);

      _pdata->interactor.set_window_size(_pdata->window_geometry);

      _pdata->supersampler.set_window_size(w, h);
      _pdata->supersampler.init();

      _pdata->oit.on_resize(_pdata->supersampler.width_upsampled(), _pdata->supersampler.height_upsampled());

      _pdata->camera.set_aspect_ratio(_pdata->window_geometry.aspect_ratio());
      _pdata->camera.calc_projection_matrix();
  }

  void Renderer::bind_default_fbo()
  { BK_QT_GL glBindFramebuffer(GL_FRAMEBUFFER, default_fbo_id()); }

  void Renderer::bind_default_fbo_as_read()
  { BK_QT_GL glBindFramebuffer(GL_READ_FRAMEBUFFER, default_fbo_id()); }

  void Renderer::bind_default_fbo_as_draw()
  { BK_QT_GL glBindFramebuffer(GL_DRAW_FRAMEBUFFER, default_fbo_id()); }

  void Renderer::_bind_default_fbo()
  { BK_QT_GL glBindFramebuffer(GL_FRAMEBUFFER, _pdata->fbo_default_id); }

  void Renderer::_bind_default_fbo_as_read()
  { BK_QT_GL glBindFramebuffer(GL_READ_FRAMEBUFFER, _pdata->fbo_default_id); }

  void Renderer::_bind_default_fbo_as_draw()
  { BK_QT_GL glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pdata->fbo_default_id); }
} // namespace bk


