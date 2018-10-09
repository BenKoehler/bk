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

#include <bkGL/renderable/image/dvr/DVRImageView.h>

#include <algorithm>
#include <vector>

#include <bkGL/UBODVR.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>
#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture3D.h>
#include <bkGL/texture/Texture2D.h>
#include <bkGL/vao/VAO.h>
#include <bkTools/color/WindowingTransferFunction.h>

namespace bk
{
  namespace details
  {
    enum class DVRMode : int
    {
        Default = 0, MIP = 1, MinIP = 2
    };
  } // namespace details

  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct DVRImageView::Impl
  {
      // cuboid faces
      VBO vbo_cuboid;
      IBO ibo_cuboid;
      VAO vao_cuboid;
      details::UBODVR ubo;
      Shader shader_cuboid;
      Shader shader_dvr;
      Shader shader_screenquad_texture;
      FBO fbo;
      FBO fbo_result;
      ScreenQuad screenquad;
      WindowingTransferFunction tf;
      WindowingTransferFunctionView tf_view;
      bool show_tf;
      GLsizei sizeInd; // cuboid faces
      bk::Vec3<GLfloat> center;
      GLuint num_ray_samples;
      details::DVRMode mode;
      bk::Vec3<GLuint> image_size;
      bk::Vec3<GLfloat> image_scale;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : vbo_cuboid(gl),
            ibo_cuboid(gl),
            vao_cuboid(gl),
            ubo(gl),
            shader_cuboid(gl),
            shader_dvr(gl),
            shader_screenquad_texture(gl),
            fbo(gl),
            fbo_result(gl),
            screenquad(gl),
            tf_view(&tf, gl),
          #endif
          show_tf(false),
          sizeInd(0),
          center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
          num_ray_samples(100), // todo options
          mode(details::DVRMode::MIP)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  DVRImageView::DVRImageView()
      : base_type()
  #else

  DVRImageView::DVRImageView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo_cuboid.set_usage_STATIC_DRAW();
      _pdata->ibo_cuboid.set_usage_STATIC_DRAW();
      _pdata->vao_cuboid.add_default_attribute_position_3xfloat();
      _pdata->vao_cuboid.add_default_attribute_colorRGB_3xfloat();
  }

  DVRImageView::DVRImageView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DVRImageView::~DVRImageView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET CENTER
  bk::Vec3<GLfloat> DVRImageView::center() const
  { return _pdata->center; }
  /// @}

  /// @{ -------------------------------------------------- GET DVR SETTINGS
  GLuint DVRImageView::num_ray_samples() const
  { return _pdata->num_ray_samples; }

  bool DVRImageView::mode_is_default() const
  { return _pdata->mode == details::DVRMode::Default; }

  bool DVRImageView::mode_is_maximum_intensity_projection() const
  { return _pdata->mode == details::DVRMode::MIP; }

  bool DVRImageView::mode_is_minimum_intensity_projection() const
  { return _pdata->mode == details::DVRMode::MinIP; }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: GET MEMBERS
  bk::Vec3<GLfloat>& DVRImageView::_center()
  { return _pdata->center; }

  VBO& DVRImageView::vbo_cuboid()
  { return _pdata->vbo_cuboid; }

  IBO& DVRImageView::ibo_cuboid()
  { return _pdata->ibo_cuboid; }

  GLsizei& DVRImageView::size_ind()
  { return _pdata->sizeInd; }

  VAO& DVRImageView::vao_cuboid()
  { return _pdata->vao_cuboid; }

  GLsizei& DVRImageView::sizeInd()
  { return _pdata->sizeInd; }

  details::UBODVR& DVRImageView::ubo()
  { return _pdata->ubo; }

  FBO& DVRImageView::fbo()
  { return _pdata->fbo; }

  FBO& DVRImageView::fbo_result()
  { return _pdata->fbo_result; }

  Shader& DVRImageView::shader_cuboid()
  { return _pdata->shader_cuboid; }

  Shader& DVRImageView::shader_dvr()
  { return _pdata->shader_dvr; }

  Shader& DVRImageView::shader_screenquad_texture()
  { return _pdata->shader_screenquad_texture; }

  WindowingTransferFunction& DVRImageView::tf()
  { return _pdata->tf; }

  WindowingTransferFunctionView& DVRImageView::tf_view()
  { return _pdata->tf_view; }

  ScreenQuad& DVRImageView::screenquad()
  { return _pdata->screenquad; }

  bool DVRImageView::show_tf() const
  { return _pdata->show_tf; }

  bk::Vec3<GLuint>& DVRImageView::image_size()
  { return _pdata->image_size; }

  bk::Vec3<GLfloat>& DVRImageView::image_scale()
  { return _pdata->image_scale; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool DVRImageView::is_initialized() const
  { return _pdata->vao_cuboid.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DVRImageView::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NUM RAY SAMPLES
  void DVRImageView::set_num_ray_samples(GLuint n)
  {
      _pdata->num_ray_samples = std::max(n, static_cast<GLuint>(1));

      if (this->is_initialized())
      {
          _pdata->ubo.set_num_ray_samples(_pdata->num_ray_samples);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET MODE
  void DVRImageView::set_mode_default()
  {
      if (_pdata->mode != details::DVRMode::Default)
      {
          _pdata->mode = details::DVRMode::Default;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void DVRImageView::set_mode_maximum_intensity_projection()
  {
      if (_pdata->mode != details::DVRMode::MIP)
      {
          _pdata->mode = details::DVRMode::MIP;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void DVRImageView::set_mode_minimum_intensity_projection()
  {
      if (_pdata->mode != details::DVRMode::MinIP)
      {
          _pdata->mode = details::DVRMode::MinIP;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
  void DVRImageView::reset_transfer_function(bool tolerant)
  {
      _pdata->tf.reset(tolerant);

      if (this->is_initialized())
      {
          _pdata->ubo.set_tf_center(_pdata->tf.center());
          _pdata->ubo.set_tf_width(_pdata->tf.width());
          _pdata->ubo.release();
          _pdata->tf_view.update_tf();

          this->emit_signal_update_required();
      }
  }

  void DVRImageView::transfer_function_shift_center_left(double percent)
  {
      _pdata->tf.shift_center_left(percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_tf_center(_pdata->tf.center());
          _pdata->ubo.release();
          _pdata->tf_view.update_tf();

          this->emit_signal_update_required();
      }
  }

  void DVRImageView::transfer_function_shift_center_right(double percent)
  {
      _pdata->tf.shift_center_right(percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_tf_center(_pdata->tf.center());
          _pdata->ubo.release();
          _pdata->tf_view.update_tf();

          this->emit_signal_update_required();
      }
  }

  void DVRImageView::transfer_function_increase_width(double percent)
  {
      _pdata->tf.increase_width(percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_tf_width(_pdata->tf.width());
          _pdata->ubo.release();
          _pdata->tf_view.update_tf();

          this->emit_signal_update_required();
      }
  }

  void DVRImageView::transfer_function_decrease_width(double percent)
  {
      _pdata->tf.decrease_width(percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_tf_width(_pdata->tf.width());
          _pdata->ubo.release();
          _pdata->tf_view.update_tf();

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SHOW TRANSFER FUNCTION
  void DVRImageView::show_transfer_function()
  { _pdata->show_tf = true; }

  void DVRImageView::hide_transfer_function()
  { _pdata->show_tf = false; }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void DVRImageView::clear_shader()
  {
      _pdata->shader_dvr.clear();
      _pdata->shader_cuboid.clear();
      _pdata->shader_screenquad_texture.clear();

      clear_shader_impl();
  }

  void DVRImageView::clear_buffers_and_textures()
  {
      _pdata->vbo_cuboid.clear();
      _pdata->ibo_cuboid.clear();
      _pdata->vao_cuboid.clear();

      _pdata->ubo.clear();
      _pdata->sizeInd = 0;

      _pdata->fbo.clear();
      _pdata->fbo_result.clear();
      _pdata->screenquad.clear();

      clear_buffers_and_textures_impl();
  }

  void DVRImageView::clear()
  {
      clear_shader();
      clear_buffers_and_textures();
      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void DVRImageView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary;

      _pdata->shader_cuboid.init_from_sources(SL::dvr::cuboid::vert(), SL::dvr::cuboid::frag());
      _pdata->shader_screenquad_texture.init_from_sources(SL::render_texture::vert(), SL::render_texture::frag(3, true));

      init_shader_impl();
  }

  void DVRImageView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_num_ray_samples(_pdata->num_ray_samples);
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.set_tf_width(_pdata->tf.width());
      _pdata->ubo.set_num_times(static_cast<GLint>(1));
      _pdata->ubo.set_temporal_resolution(static_cast<GLfloat>(1));
      _pdata->ubo.set_current_t0(static_cast<GLint>(1));
      _pdata->ubo.set_minVal(_pdata->tf.intensity_min());
      _pdata->ubo.set_maxVal(_pdata->tf.intensity_max());
      _pdata->ubo.set_image_size_x(_pdata->image_size[0]);
      _pdata->ubo.set_image_size_y(_pdata->image_size[1]);
      _pdata->ubo.set_image_size_z(_pdata->image_size[2]);
      _pdata->ubo.set_image_scale_x(_pdata->image_scale[0]);
      _pdata->ubo.set_image_scale_y(_pdata->image_scale[1]);
      _pdata->ubo.set_image_scale_z(_pdata->image_scale[2]);

      _pdata->ubo.release();

      _pdata->tf_view.update_tf();
  }

  void DVRImageView::init_fbo(GLuint window_width, GLuint window_height)
  {
      _pdata->fbo.clear();
      _pdata->fbo_result.clear();
      _pdata->screenquad.clear();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D entry_tex;
      #else
      Texture2D entry_tex(this->_gl);
      #endif
      entry_tex.set_width(window_width);
      entry_tex.set_height(window_height);
      entry_tex.set_default_config_rgb_tex();
      entry_tex.set_texture_unit_number(1);
      entry_tex.init();

      entry_tex.bind();
      entry_tex.set_interpolation_LINEAR();
      entry_tex.set_texture_coordinates_CLAMP();
      entry_tex.release();

      _pdata->fbo.set_texture_COLOR_ATTACHMENT(std::move(entry_tex), 0);
      _pdata->fbo.init();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D result_tex;
      #else
      Texture2D result_tex(this->_gl);
      #endif
      result_tex.set_width(window_width);
      result_tex.set_height(window_height);
      result_tex.set_default_config_rgba_tex();
      result_tex.set_texture_unit_number(3);
      result_tex.init();

      result_tex.bind();
      result_tex.set_interpolation_LINEAR();
      result_tex.set_texture_coordinates_CLAMP();
      result_tex.release();

      _pdata->fbo_result.set_texture_COLOR_ATTACHMENT(std::move(result_tex), 0);
      _pdata->fbo_result.init();

      _pdata->screenquad.init();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void DVRImageView::on_resize(GLint w, GLint h)
  {
      init_fbo(w, h);
      _pdata->tf_view.on_resize(w, h);
      this->set_modelview_matrix_changed(true);
  }

  void DVRImageView::on_oit_enabled(bool b)
  { _pdata->tf_view.set_oit_available(b); }

  void DVRImageView::on_animation_enabled(bool b)
  { _pdata->tf_view.set_animation_is_enabled(b); }

  void DVRImageView::on_modelview_changed(bool b)
  { _pdata->tf_view.set_modelview_matrix_changed(b); }

  void DVRImageView::on_visible_changed(bool b)
  {
      if (b)
      { this->set_modelview_matrix_changed(true); }
  }

  void DVRImageView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }
  //void DVRImageView::on_mouse_pos_changed(GLint x, GLint y)
  //{ _pdata->tf_view.on_mouse_pos_changed(x, y); }

  void DVRImageView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }
  //void DVRImageView::on_mouse_button_pressed(MouseButton_ btn)
  //{
  //    _pdata->tf_view.on_mouse_button_pressed(btn);
  //
  //    if (btn == MouseButton_Right)
  //    { show_transfer_function(); }
  //}

  void DVRImageView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }
  //void DVRImageView::on_mouse_button_released(MouseButton_ btn)
  //{
  //    _pdata->tf_view.on_mouse_button_released(btn);
  //
  //    if (btn == MouseButton_Right)
  //    { hide_transfer_function(); }
  //}

  void DVRImageView::on_key_pressed(Key_ /*k*/){ /* do nothing */ }
  void DVRImageView::on_key_released(Key_ /*k*/){ /* do nothing */ }
  void DVRImageView::on_mouse_wheel_up(){ /* do nothing */ }
  void DVRImageView::on_mouse_wheel_down(){ /* do nothing */ }

  void DVRImageView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void DVRImageView::draw_opaque_impl()
  {
      if (this->modelview_matrix_changed() || _pdata->show_tf || !is_up_to_date())
      {
          BK_QT_GL glPushAttrib(GL_POLYGON_BIT);

          // ubo 0 must be global ubo with modelview/projection matrices

          BK_QT_GL glDepthMask(GL_FALSE);

          BK_QT_GL glEnable(GL_CULL_FACE);
          BK_QT_GL glCullFace(GL_BACK);

          /*
           * first pass: render cuboid front faces with position colors
           */
          _pdata->ubo.bind_to_default_base();

          _pdata->fbo.bind();
          BK_QT_GL glClear(GL_COLOR_BUFFER_BIT);

          _pdata->vao_cuboid.bind();
          _pdata->shader_cuboid.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_cuboid.release();
          _pdata->vao_cuboid.release();

          _pdata->fbo.release();
          //this->emit_signal_bind_default_fbo();

          /*
           * second pass: render cuboid back faces, determine directions via entry_tex,
           * and perform ray casting
           */
          BK_QT_GL glCullFace(GL_FRONT);

          _pdata->fbo_result.bind(); // color tex bound to tex unit 3
          BK_QT_GL glClearColor(0, 0, 0, 0);
          BK_QT_GL glClear(GL_COLOR_BUFFER_BIT);

          _pdata->fbo.color_attachment(0)->bind(); // bound to tex unit 1
          bind_image_textures(); //_pdata->image_tex.bind(); // bound to tex unit 2

          _pdata->vao_cuboid.bind();
          _pdata->shader_dvr.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_dvr.release();
          _pdata->vao_cuboid.release();

          _pdata->fbo_result.release();
          _pdata->fbo.color_attachment(0)->release();
          release_image_textures(); //_pdata->image_tex.release();
          _pdata->ubo.release_from_base();

          this->emit_signal_bind_default_fbo();

          BK_QT_GL glDepthMask(GL_TRUE);

          BK_QT_GL glPopAttrib();
      }

      _pdata->fbo_result.color_attachment(0)->bind(); // bound to tex unit 3
      _pdata->shader_screenquad_texture.bind();
      _pdata->screenquad.draw();
      _pdata->shader_screenquad_texture.release();
      _pdata->fbo_result.color_attachment(0)->release();

      if (_pdata->show_tf)
      { _pdata->tf_view.draw(); }
  }

  void DVRImageView::draw_transparent_impl()
  { /* do nothing */ }
  /// @}
} // namespace bk


