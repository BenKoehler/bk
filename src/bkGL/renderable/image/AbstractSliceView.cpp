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

#include <bkGL/renderable/image/AbstractSliceView.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include <bkTools/color/WindowingTransferFunction.h>
#include <bk/Matrix>
#include <bk/Signal>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Progress>
    #include <bk/Localization>

#endif

#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/UBOSliceView.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/Mouse.h>
#include <bkGL/WindowGeometry.h>
#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>
#include <bkGL/renderable/ScreenQuad.h>

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AbstractSliceView::Impl
  {
      VBO                           vbo;
      IBO                           ibo;
      VAO                           vao;
      details::UBOSliceView         ubo;
      SSBO                          ssbo_intensity;
      Shader                        shader;
      // transfer function
      WindowingTransferFunction     tf;
      WindowingTransferFunctionView tf_view;
      bool                          show_tf;
      // general
      GLsizei                       sizeInd;
      WindowGeometry                window_geometry;
      // image
      Vec2<GLfloat>                 voxel_scale;
      Vec4<GLuint>                  xyzt_max;
      Vec4<GLuint>                  xyzt_current;
      GLfloat                       intensitymax;
      GLfloat                       intensitymin;
      // signals
      bk::Signal<GLuint>            s_xmax_changed;
      bk::Signal<GLuint>            s_xcurrent_changed;
      bk::Signal<GLuint>            s_ymax_changed;
      bk::Signal<GLuint>            s_ycurrent_changed;
      bk::Signal<GLuint>            s_zmax_changed;
      bk::Signal<GLuint>            s_zcurrent_changed;
      bk::Signal<GLuint>            s_tmax_changed;
      bk::Signal<GLuint>            s_tcurrent_changed;
      //
      Mouse                         mouse;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          : tf_view(&tf),
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            ibo(gl),
            vao(gl),
            ubo(gl),
            ssbo_intensity(gl),
            shader(gl),
            tf_view(&tf, gl),
          #endif
            show_tf(false),
            sizeInd(0),
            voxel_scale(MatrixFactory::One_Vec_2D<GLfloat>()),
            xyzt_max(MatrixFactory::Zero_Vec_4D<GLuint>()),
            xyzt_current(MatrixFactory::Zero_Vec_4D<GLuint>()),
            intensitymax(0),
            intensitymin(0)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  AbstractSliceView::AbstractSliceView()
      : base_type()
  #else

  AbstractSliceView::AbstractSliceView(bk::qt_gl_functions* gl)
        : base_type(gl),
          _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ibo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->ssbo_intensity.set_usage_DYNAMIC_DRAW();
  }

  AbstractSliceView::AbstractSliceView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractSliceView::~AbstractSliceView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET XYZT MAX
  GLuint AbstractSliceView::xmax() const
  { return _pdata->xyzt_max[0]; }

  GLuint AbstractSliceView::ymax() const
  { return _pdata->xyzt_max[1]; }

  GLuint AbstractSliceView::zmax() const
  { return _pdata->xyzt_max[2]; }

  GLuint AbstractSliceView::tmax() const
  { return _pdata->xyzt_max[3]; }
  /// @}

  /// @{ -------------------------------------------------- GET XYZT CURRENT
  GLuint AbstractSliceView::xcurrent() const
  { return _pdata->xyzt_current[0]; }

  GLuint AbstractSliceView::ycurrent() const
  { return _pdata->xyzt_current[1]; }

  GLuint AbstractSliceView::zcurrent() const
  { return _pdata->xyzt_current[2]; }

  GLuint AbstractSliceView::tcurrent() const
  { return _pdata->xyzt_current[3]; }
  /// @}

  /// @{ -------------------------------------------------- GET INTENSITY
  GLfloat AbstractSliceView::intensitymin() const
  { return _pdata->intensitymin; }

  GLfloat AbstractSliceView::intensitymax() const
  { return _pdata->intensitymax; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNAL
  bk::Signal<GLuint>& AbstractSliceView::signal_xmax_changed()
  { return _pdata->s_xmax_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_xcurrent_changed()
  { return _pdata->s_xcurrent_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_ymax_changed()
  { return _pdata->s_ymax_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_ycurrent_changed()
  { return _pdata->s_ycurrent_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_zmax_changed()
  { return _pdata->s_zmax_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_zcurrent_changed()
  { return _pdata->s_zcurrent_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_tmax_changed()
  { return _pdata->s_tmax_changed; }

  bk::Signal<GLuint>& AbstractSliceView::signal_tcurrent_changed()
  { return _pdata->s_tcurrent_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET MEMBERS
  GLuint& AbstractSliceView::_xmax()
  { return _pdata->xyzt_max[0]; }

  GLuint& AbstractSliceView::_ymax()
  { return _pdata->xyzt_max[1]; }

  GLuint& AbstractSliceView::_zmax()
  { return _pdata->xyzt_max[2]; }

  GLuint& AbstractSliceView::_tmax()
  { return _pdata->xyzt_max[3]; }

  GLuint& AbstractSliceView::_xcurrent()
  { return _pdata->xyzt_current[0]; }

  GLuint& AbstractSliceView::_ycurrent()
  { return _pdata->xyzt_current[1]; }

  GLuint& AbstractSliceView::_zcurrent()
  { return _pdata->xyzt_current[2]; }

  GLuint& AbstractSliceView::_tcurrent()
  { return _pdata->xyzt_current[3]; }

  GLfloat& AbstractSliceView::_intensitymin()
  { return _pdata->intensitymin; }

  GLfloat& AbstractSliceView::_intensitymax()
  { return _pdata->intensitymax; }

  SSBO& AbstractSliceView::_ssbo_intensity()
  { return _pdata->ssbo_intensity; }

  VAO& AbstractSliceView::_vao()
  { return _pdata->vao; }

  UBOSliceView& AbstractSliceView::_ubo()
  { return _pdata->ubo; }

  GLsizei AbstractSliceView::_sizeInd() const
  { return _pdata->sizeInd; }

  Shader& AbstractSliceView::_shader()
  { return _pdata->shader; }

  WindowingTransferFunctionView& AbstractSliceView::_tf_view()
  { return _pdata->tf_view; }

  bool AbstractSliceView::_show_tf() const
  { return _pdata->show_tf; }

  const Mouse& AbstractSliceView::_mouse() const
  { return _pdata->mouse; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool AbstractSliceView::is_initialized() const
  { return _pdata->vao.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractSliceView::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET SCREEN SIZE
  void AbstractSliceView::set_screen_size(GLint w, GLint h)
  {
      _pdata->window_geometry.set(w, h);

      if (this->is_initialized())
      {
          _pdata->ubo.set_window_width(_pdata->window_geometry.width());
          _pdata->ubo.set_window_height(_pdata->window_geometry.height());
          _pdata->ubo.release();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET SCALE
  void AbstractSliceView::set_scale(GLfloat vx, GLfloat vy)
  {
      _pdata->voxel_scale[0] = std::max(vx, static_cast<GLfloat>(0.0001));
      _pdata->voxel_scale[1] = std::max(vy, static_cast<GLfloat>(0.0001));
      _pdata->ubo.set_voxel_scale0(_pdata->voxel_scale[0]);
      _pdata->ubo.set_voxel_scale1(_pdata->voxel_scale[1]);
      _pdata->ubo.release();
  }
  /// @}

  /// @{ -------------------------------------------------- SET SLICE/TIME
  void AbstractSliceView::set_slice(unsigned int z, GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized())
      {
          _pdata->xyzt_current[2] = std::min(z, _pdata->xyzt_max[2]);
          _pdata->s_zcurrent_changed.emit_signal(_pdata->xyzt_current[2]);
          if (lastMouseX >= 0 && lastMouseY >= 0)
          { determine_currentIntensity(lastMouseX, lastMouseY); }
          update_ssbo_intensity_and_determine_intensity_min_max();
          set_slice_impl(z);
      }
  }

  void AbstractSliceView::set_slice_impl(unsigned int /*z*/)
  { /* do nothing */ }

  void AbstractSliceView::set_time(unsigned int t, GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized())
      {
          _pdata->xyzt_current[3] = std::min(t, _pdata->xyzt_max[3]);
          _pdata->s_tcurrent_changed.emit_signal(_pdata->xyzt_current[3]);
          if (lastMouseX >= 0 && lastMouseY >= 0)
          { determine_currentIntensity(lastMouseX, lastMouseY); }
          update_ssbo_intensity_and_determine_intensity_min_max();
          set_time_impl(t);
      }
  }

  void AbstractSliceView::set_time_impl(unsigned int /*t*/)
  { /* do nothing */ }

  void AbstractSliceView::previousTime(GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized() && _pdata->xyzt_current[3] > 0)
      { set_time(_pdata->xyzt_current[3] - 1, lastMouseX, lastMouseY); }
  }

  void AbstractSliceView::nextTime(GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized() && _pdata->xyzt_current[3] < _pdata->xyzt_max[3])
      { set_time(_pdata->xyzt_current[3] + 1, lastMouseX, lastMouseY); }
  }

  void AbstractSliceView::previousSlice(GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized() && _pdata->xyzt_current[2] > 0)
      { set_slice(_pdata->xyzt_current[2] - 1, lastMouseX, lastMouseY); }
  }

  void AbstractSliceView::nextSlice(GLint lastMouseX, GLint lastMouseY)
  {
      if (this->is_initialized() && _pdata->xyzt_current[2] < _pdata->xyzt_max[2])
      { set_slice(_pdata->xyzt_current[2] + 1, lastMouseX, lastMouseY); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
  void AbstractSliceView::reset_transfer_function(bool tolerant)
  {
      _pdata->tf.reset(tolerant);
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.set_tf_width(_pdata->tf.width());
      _pdata->ubo.release();
      _pdata->tf_view.update_tf();
      this->emit_signal_update_required();
  }

  void AbstractSliceView::transfer_function_shift_center_left(double percent)
  {
      _pdata->tf.shift_center_left(percent);
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.release();
      _pdata->tf_view.update_tf();
      this->emit_signal_update_required();
  }

  void AbstractSliceView::transfer_function_shift_center_right(double percent)
  {
      _pdata->tf.shift_center_right(percent);
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.release();
      _pdata->tf_view.update_tf();
      this->emit_signal_update_required();
  }

  void AbstractSliceView::transfer_function_increase_width(double percent)
  {
      _pdata->tf.increase_width(percent);
      _pdata->ubo.set_tf_width(_pdata->tf.width());
      _pdata->ubo.release();
      _pdata->tf_view.update_tf();
      this->emit_signal_update_required();
  }

  void AbstractSliceView::transfer_function_decrease_width(double percent)
  {
      _pdata->tf.decrease_width(percent);
      _pdata->ubo.set_tf_width(_pdata->tf.width());
      _pdata->ubo.release();
      _pdata->tf_view.update_tf();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- SHOW TRANSFER FUNCTION
  void AbstractSliceView::show_transfer_function()
  { _pdata->show_tf = true; }

  void AbstractSliceView::hide_transfer_function()
  { _pdata->show_tf = false; }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void AbstractSliceView::clear_ubo()
  {
      _pdata->ubo.clear();
  }

  void AbstractSliceView::clear_imageBuffers()
  {
      _pdata->vbo.clear();
      _pdata->ibo.clear();
      _pdata->vao.clear();
      _pdata->ssbo_intensity.clear();
  }

  void AbstractSliceView::clear_shader()
  { _pdata->shader.clear(); }

  void AbstractSliceView::clear_image()
  { /* do nothing */ }

  void AbstractSliceView::clear()
  {
      clear_ubo();
      clear_imageBuffers();
      clear_shader();
      clear_image();
      _pdata->s_xcurrent_changed.emit_signal(_pdata->xyzt_current[0]);
      _pdata->s_xmax_changed.emit_signal(_pdata->xyzt_max[0]);
      _pdata->s_ycurrent_changed.emit_signal(_pdata->xyzt_current[1]);
      _pdata->s_ymax_changed.emit_signal(_pdata->xyzt_max[1]);
      _pdata->s_zcurrent_changed.emit_signal(_pdata->xyzt_current[2]);
      _pdata->s_zmax_changed.emit_signal(_pdata->xyzt_max[2]);
      _pdata->s_tcurrent_changed.emit_signal(_pdata->xyzt_current[3]);
      _pdata->s_tmax_changed.emit_signal(_pdata->xyzt_max[3]);
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void AbstractSliceView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();
  }

  bool AbstractSliceView::init_shader()
  { return _pdata->shader.init_from_sources(vertex_shader_source(), fragment_shader_source()); }

  void AbstractSliceView::init_image()
  {
      #ifdef BK_EMIT_PROGRESS
      Progress& prog = bk_progress.emplace_task(5, ___("Initializing slice viewer"));
      #endif
      clear_imageBuffers();

      /*
       * determine image size
       */
      for (unsigned int i = 0; i < 4; ++i)
      { _pdata->xyzt_max[i] = image_size(i) > 0 ? image_size(i) - 1 : 0; }

      /*
       * abort if image is empty
       */
      if ((_pdata->xyzt_max[0] + 1) * (_pdata->xyzt_max[1] + 1) * (_pdata->xyzt_max[2] + 1) * (_pdata->xyzt_max[3] + 1) == 0)
      {
          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          std::cerr << "AbstractSliceView::init_image - image is empty; abort" << std::endl;

          return;
      }

      /*
       * determine voxel scale
       */
      for (unsigned int i = 0; i < 2; ++i)
      { _pdata->voxel_scale[i] = image_scale(i); }

      /*
       * setup vbo
       */
      std::vector<GLfloat> vertices;
      vertices.reserve(2 * _pdata->xyzt_max[0] * _pdata->xyzt_max[1]);
      for (GLuint y = 0; y < _pdata->xyzt_max[1]; ++y) // todo < oder <= ?
      {
          for (GLuint x = 0; x < _pdata->xyzt_max[0]; ++x) // todo < oder <= ?
          {
              vertices.push_back(static_cast<GLfloat>(x));
              vertices.push_back(static_cast<GLfloat>(y));
          }
      }
      _pdata->vbo.init(vertices);
      #ifdef BK_EMIT_PROGRESS
      prog.set_current(1);
      #endif

      /*
       * setup ibo
       */
      std::vector<GLuint> indices;
      indices.reserve((2 * _pdata->xyzt_max[0] - 2) * _pdata->xyzt_max[1] + _pdata->xyzt_max[1]);

      // TRIANGLE STRIPS
      for (GLuint y = 0; y < (_pdata->xyzt_max[1] == 0 ? 0 : _pdata->xyzt_max[1] - 1); ++y) // todo: -1 oder nicht?
      {
          for (GLuint x = 0; x < _pdata->xyzt_max[0]; ++x)
          {
              indices.push_back(static_cast<GLuint>(y * _pdata->xyzt_max[0] + x));
              indices.push_back(static_cast<GLuint>(y * _pdata->xyzt_max[0] + x + _pdata->xyzt_max[0]));
          }
          indices.push_back(std::numeric_limits<GLuint>::max());
      }
      _pdata->sizeInd = static_cast<GLsizei>(indices.size());
      _pdata->ibo.init(indices);
      #ifdef BK_EMIT_PROGRESS
      prog.set_current(1);
      #endif

      /*
       * setup vao
       */
      _pdata->vao.init(_pdata->vbo, _pdata->ibo);

      /*
       * set start slice and time
       */
      _pdata->xyzt_current[2] = _pdata->xyzt_max[2] / 2;
      _pdata->xyzt_current[3] = 0;

      /*
       * setup ssbo
       */
      //init_pdata->ssbo_intensity();
      //_pdata->ssbo_intensity.init(nullptr, (_pdata->xyzt_max[0]+1)*(_pdata->xyzt_max[1]+1)*sizeof(GLfloat));
      _pdata->ssbo_intensity.init(nullptr, (_pdata->xyzt_max[0] + 1) * (_pdata->xyzt_max[1] + 1) * sizeof(GLfloat) * ssbo_intensity_num_elements_per_pixel());
      update_ssbo_intensity_and_determine_intensity_min_max();
      #ifdef BK_EMIT_PROGRESS
      prog.set_current(3);
      #endif

      /*
       * setup transfer function
       */
      _pdata->tf.set_intensity_min_max(_pdata->intensitymin, _pdata->intensitymax);
      _pdata->tf.reset();
      _pdata->tf_view.update_tf();
      #ifdef BK_EMIT_PROGRESS
      prog.set_current(4);
      #endif

      /*
       * setup ubo
       */
      _pdata->ubo.set_xyzt_max0(_pdata->xyzt_max[0]);
      _pdata->ubo.set_xyzt_max1(_pdata->xyzt_max[1]);
      //GLuint _orientation = 0;
      //_pdata->ubo.write_registered_value("_orientation", &_orientation); // todo
      _pdata->ubo.set_window_width(_pdata->window_geometry.width());
      _pdata->ubo.set_window_height(_pdata->window_geometry.height());
      _pdata->ubo.set_voxel_scale0(_pdata->voxel_scale[0]);
      _pdata->ubo.set_voxel_scale1(_pdata->voxel_scale[1]);
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.set_tf_width(_pdata->tf.width());

      //itemp = 4;
      //_pdata->ubo.write_registered_value("nDims", &itemp);

      _pdata->ubo.release();
      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      /*
       * emit signals
       */
      _pdata->s_xmax_changed.emit_signal(_pdata->xyzt_max[0]);
      _pdata->s_ymax_changed.emit_signal(_pdata->xyzt_max[1]);
      _pdata->s_zmax_changed.emit_signal(_pdata->xyzt_max[2]);
      _pdata->s_tmax_changed.emit_signal(_pdata->xyzt_max[3]);
      _pdata->s_xcurrent_changed.emit_signal(_pdata->xyzt_current[0]);
      _pdata->s_ycurrent_changed.emit_signal(_pdata->xyzt_current[1]);
      _pdata->s_zcurrent_changed.emit_signal(_pdata->xyzt_current[2]);
      _pdata->s_tcurrent_changed.emit_signal(_pdata->xyzt_current[3]);
  }

  void AbstractSliceView::update_ssbo_intensity_and_determine_intensity_min_max()
  {
      update_ssbo_intensity_and_determine_intensity_min_max_impl(_pdata->xyzt_current[2], _pdata->xyzt_current[3]);
      _pdata->tf.set_intensity_min_max(_pdata->intensitymin, _pdata->intensitymax);
      //_pdata->tf.reset();
      _pdata->tf_view.update_tf();
      _pdata->ubo.set_tf_center(_pdata->tf.center());
      _pdata->ubo.set_tf_width(_pdata->tf.width());
      _pdata->ubo.release();
      this->emit_signal_update_required();
  }

  std::string AbstractSliceView::geometry_shader_source() const
  { return ""; }

  void AbstractSliceView::init()
  {
      _pdata->tf_view.init(&_pdata->tf);
      init_shader();
      init_ubo();
      init_image();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- CURRENT INTENSITY
  void AbstractSliceView::mouseXY_to_imageXY(int mouseX, int mouseY, unsigned int& imgX, unsigned int& imgY) const
  {
      const GLuint  w                = _pdata->window_geometry.width();
      const GLuint  h                = _pdata->window_geometry.height();
      const GLfloat qqi_ratio        = static_cast<GLfloat>(h) / static_cast<GLfloat>(w);
      const GLfloat voxelscale_ratio = _pdata->voxel_scale[0] / _pdata->voxel_scale[1];
      if (mouseX < 0)
      { imgX = 0; }
      else if (mouseX >= static_cast<int>(_pdata->window_geometry.width()))
      { imgX = _pdata->xyzt_max[0]; }
      else
      {
          const GLfloat xratio  = std::min(static_cast<GLfloat>(1), voxelscale_ratio * qqi_ratio * static_cast<GLfloat>(_pdata->xyzt_max[0]) / static_cast<GLfloat>(_pdata->xyzt_max[1]));
          const GLfloat screenX = static_cast<GLfloat>(2 * mouseX) / static_cast<GLfloat>(w) - static_cast<GLfloat>(1);
          const GLfloat tempx   = (static_cast<GLfloat>(screenX) + xratio) * (static_cast<GLfloat>(_pdata->xyzt_max[0] - 1)) / (static_cast<GLfloat>(2) * xratio);
          imgX = static_cast<unsigned int>(std::max(0, std::min(static_cast<int>(std::floor(tempx + 0.5f)), static_cast<int>(_pdata->xyzt_max[0]))));
      }
      if (mouseY < 0)
      { imgY = 0; }
      else if (mouseY >= static_cast<int>(_pdata->window_geometry.width()))
      { imgY = _pdata->xyzt_max[1]; }
      else
      {
          const GLfloat yratio  = std::min(static_cast<GLfloat>(1), static_cast<GLfloat>(_pdata->xyzt_max[1]) / (voxelscale_ratio * qqi_ratio * static_cast<GLfloat>(_pdata->xyzt_max[0])));
          const GLfloat screenY = static_cast<GLfloat>(2 * mouseY) / static_cast<GLfloat>(h) - static_cast<GLfloat>(1);
          const GLfloat tempy   = (static_cast<GLfloat>(screenY) + yratio) * (static_cast<GLfloat>(_pdata->xyzt_max[1] - 1)) / (static_cast<GLfloat>(2) * yratio);
          imgY = static_cast<unsigned int>(std::max(0, std::min(static_cast<int>(std::floor(tempy + 0.5f)), static_cast<int>(_pdata->xyzt_max[1]))));
      }
  }
  /// @}

  /// @{ -------------------------------------------------- CURRENT INTENSITY
  void AbstractSliceView::determine_currentIntensity(int mouseX, int mouseY)
  {
      // TODO orientation

      mouseXY_to_imageXY(mouseX, mouseY, _pdata->xyzt_current[0], _pdata->xyzt_current[1]);
      _pdata->s_xcurrent_changed.emit_signal(_pdata->xyzt_current[0]);
      _pdata->s_ycurrent_changed.emit_signal(_pdata->xyzt_current[1]);
      determine_currentIntensity_impl();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void AbstractSliceView::on_resize(GLint w, GLint h)
  {
      set_screen_size(w, h);
      _pdata->tf_view.on_resize(w, h);
  }

  void AbstractSliceView::on_oit_enabled(bool b)
  { _pdata->tf_view.set_oit_available(b); }

  void AbstractSliceView::on_animation_enabled(bool b)
  { _pdata->tf_view.set_animation_is_enabled(b); }

  void AbstractSliceView::on_modelview_changed(bool)
  { /* do nothing */ }

  void AbstractSliceView::on_visible_changed(bool)
  { /* do nothing */ }

  void AbstractSliceView::on_mouse_pos_changed(GLint x, GLint y)
  {
      _pdata->mouse.set_pos(x, y);

      if (on_mouse_pos_changed_impl(x, y))
      {
          if (_pdata->mouse.middle_button_is_pressed())
          {
              if (_pdata->mouse.last_move_was_down())
              { previousSlice(x, y); }
              else if (_pdata->mouse.last_move_was_up())
              { nextSlice(x, y); }

              if (_pdata->mouse.last_move_was_left())
              { previousTime(x, y); }
              else if (_pdata->mouse.last_move_was_right())
              { nextTime(x, y); }
          }

          if (_pdata->mouse.right_button_is_pressed())
          {
              static constexpr const GLfloat percent = 0.4; // todo: something more advanced?

              if (_pdata->mouse.last_move_was_down())
              { transfer_function_decrease_width(percent * std::abs(_pdata->mouse.dy())); }
              else if (_pdata->mouse.last_move_was_up())
              { transfer_function_increase_width(percent * std::abs(_pdata->mouse.dy())); }

              if (_pdata->mouse.last_move_was_left())
              { transfer_function_shift_center_left(percent * std::abs(_pdata->mouse.dx())); }
              else if (_pdata->mouse.last_move_was_right())
              { transfer_function_shift_center_right(percent * std::abs(_pdata->mouse.dx())); }
          }
      }

      determine_currentIntensity(x, y);
  }

  void AbstractSliceView::on_mouse_button_pressed(MouseButton_ btn)
  {
      _pdata->mouse.set_button_pressed(btn, true);

      if (on_mouse_button_pressed_impl(btn))
      {_pdata->show_tf = _pdata->mouse.right_button_is_pressed();}
  }

  void AbstractSliceView::on_mouse_button_released(MouseButton_ btn)
  {
      _pdata->mouse.set_button_pressed(btn, false);

      if (on_mouse_button_released_impl(btn))
      {_pdata->show_tf = _pdata->mouse.right_button_is_pressed();}
  }

  void AbstractSliceView::on_key_pressed(Key_ k)
  {
      if (k == Key_LeftArrow)
      { previousTime(); }
      else if (k == Key_RightArrow)
      { nextTime(); }
      else if (k == Key_UpArrow)
      { nextSlice(); }
      else if (k == Key_DownArrow)
      { previousSlice(); }
  }

  void AbstractSliceView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void AbstractSliceView::on_mouse_wheel_up()
  { nextSlice(); }

  void AbstractSliceView::on_mouse_wheel_down()
  { previousSlice(); }

  void AbstractSliceView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  bool AbstractSliceView::on_mouse_pos_changed_impl(GLint /*x*/, GLint /*y*/)
  { return true; }

  bool AbstractSliceView::on_mouse_button_pressed_impl(MouseButton_ /*btn*/)
  { return true; }

  bool AbstractSliceView::on_mouse_button_released_impl(MouseButton_ /*btn*/)
  { return true; }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void AbstractSliceView::draw_impl()
  {
      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->ubo.bind_to_default_base();
      _pdata->ssbo_intensity.bind_to_base(2);

      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
      _pdata->shader.release();
      _pdata->vao.release();

      _pdata->ubo.release_from_base();
      _pdata->ssbo_intensity.release_from_base();

      BK_QT_GL glPopAttrib();

      if (_pdata->show_tf)
      { _pdata->tf_view.draw(); }
  }
  /// @}
} // namespace bk::details


