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

#include "PlotAxis.h"

#include <algorithm>

#include <bkTools/color/ColorRGBA.h>
#include <bkGL/UBOPlotLine.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotAxis::Impl
  {
      VAO vao;
      VBO vbo;
      details::UBOPlotLine ubo;
      Shader shader;
      VBO vbo_ticks;
      IBO ibo_ticks;
      VAO vao_ticks;
      Shader shader_ticks;
      GLsizei sizeInd_ticks;
      GLfloat xmin;
      GLfloat xmax;
      GLfloat ymin;
      GLfloat ymax;
      ColorRGBA color;
      GLfloat lineWidth;
      GLuint numTicks;
      std::string label;
      details::PlotAxisOrientation_ orientation;
      unsigned int tickPrecision;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : vao(gl),
            vbo(gl),
            ubo(gl),
            shader(gl),
            vbo_ticks(gl),
            ibo_ticks(gl),
            vao_ticks(gl),
            shader_ticks(gl),
          #endif
          sizeInd_ticks(0),
          xmin(0),
          xmax(0),
          ymin(0),
          ymax(0),
          color(0, 0, 0, 1),
          lineWidth(1),
          numTicks(3),
          label(""),
          orientation(details::PlotAxisOrientation_Horizontal),
          tickPrecision(1)
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

  #ifndef BK_LIB_QT_AVAILABLE

  PlotAxis::PlotAxis()
      : base_type()
  #else

  PlotAxis::PlotAxis(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();

      _pdata->vbo_ticks.set_usage_STATIC_DRAW();
      _pdata->ibo_ticks.set_usage_STATIC_DRAW();
      _pdata->vao_ticks.add_default_attribute_position_2xfloat();
  }

  PlotAxis::PlotAxis(PlotAxis&&) noexcept = default;

  PlotAxis::~PlotAxis()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  const ColorRGBA& PlotAxis::color() const
  { return _pdata->color; }

  GLfloat PlotAxis::line_width() const
  { return _pdata->lineWidth; }

  GLfloat PlotAxis::x_min() const
  { return _pdata->xmin; }

  GLfloat PlotAxis::x_max() const
  { return _pdata->xmax; }

  GLfloat PlotAxis::y_min() const
  { return _pdata->ymin; }

  GLfloat PlotAxis::y_max() const
  { return _pdata->ymax; }

  GLuint PlotAxis::num_ticks() const
  { return _pdata->numTicks; }

  const std::string& PlotAxis::label() const
  { return _pdata->label; }

  const details::PlotAxisOrientation_& PlotAxis::orientation() const
  { return _pdata->orientation; }

  bool PlotAxis::orientation_is_horizontal() const
  { return _pdata->orientation == details::PlotAxisOrientation_Horizontal; }

  bool PlotAxis::orientation_is_vertical() const
  { return _pdata->orientation == details::PlotAxisOrientation_Vertical; }

  unsigned int PlotAxis::tick_precision() const
  { return _pdata->tickPrecision; }

  bool PlotAxis::is_initialized() const
  { return _pdata->vao.is_initialized() && _pdata->ubo.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotAxis& PlotAxis::operator=(PlotAxis&&) noexcept = default;

  void PlotAxis::set_color(const ColorRGBA& col)
  { set_color(col[0], col[1], col[2], col[3]); }

  void PlotAxis::set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  { _pdata->color.set(r, g, b, a); }

  void PlotAxis::set_line_width(GLfloat w)
  {
      _pdata->lineWidth = std::max(w, static_cast<GLfloat>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.set_line_width(_pdata->lineWidth);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotAxis::set_x_min(GLfloat xmin)
  { _pdata->xmin = xmin; }

  void PlotAxis::set_x_max(GLfloat xmax)
  { _pdata->xmax = xmax; }

  void PlotAxis::set_y_min(GLfloat ymin)
  { _pdata->ymin = ymin; }

  void PlotAxis::set_y_max(GLfloat ymax)
  { _pdata->ymax = ymax; }

  void PlotAxis::set_num_ticks(GLuint numTicks)
  { _pdata->numTicks = numTicks; }

  void PlotAxis::set_label(std::string_view label)
  { _pdata->label = label; }

  void PlotAxis::set_orientation(details::PlotAxisOrientation_ orientation)
  {
      _pdata->orientation = orientation;

      if (this->is_initialized())
      {
          init_vbo_vao();
          this->emit_signal_update_required();
      }
  }

  void PlotAxis::set_orientation_horizontal()
  { set_orientation(details::PlotAxisOrientation_Horizontal); }

  void PlotAxis::set_orientation_vertical()
  { set_orientation(details::PlotAxisOrientation_Vertical); }

  void PlotAxis::set_tick_precision(unsigned int tickPrecision)
  { _pdata->tickPrecision = tickPrecision; }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotAxis::init_shader()
  {
      using SL = details::ShaderLibrary::plot;

      bool success = _pdata->shader.init_from_sources(SL::axis::vert(), SL::axis::frag(), SL::axis::geom());
      success = _pdata->shader_ticks.init_from_sources(SL::ticks::vert(), SL::ticks::frag(), SL::ticks::geom()) && success;

      return success;
  }

  bool PlotAxis::init_vbo_vao()
  {
      // axis
      std::vector<GLfloat> vert;
      vert.reserve(4);
      vert.push_back(_pdata->xmin); // axis starts in (0,0)
      vert.push_back(_pdata->ymin);

      if (orientation_is_horizontal())
      {
          /*
           * x axis
           */
          vert.push_back(_pdata->xmax); // x axis ends in (xmax,0)
          vert.push_back(_pdata->ymin);

          /*
           * x ticks (vertical lines)
           */
          const auto xTickValueDelta = (_pdata->xmax - _pdata->xmin) / (_pdata->numTicks + 1);
          auto xval = _pdata->xmin;
          std::vector<GLfloat> vert_ticks_x;
          vert_ticks_x.reserve(2 * 2 * _pdata->numTicks);

          std::vector<GLuint> indices_ticks_x;
          indices_ticks_x.reserve(2 * _pdata->numTicks);
          unsigned int cnt = 0;

          //for (unsigned int i = 0; i < _pdata->numTicks + 1; ++i)
          for (unsigned int i = 0; i <= _pdata->numTicks + 1; ++i)
          {
              vert_ticks_x.push_back(xval);
              vert_ticks_x.push_back(_pdata->ymin);
              indices_ticks_x.push_back(cnt++);

              vert_ticks_x.push_back(xval);
              vert_ticks_x.push_back(_pdata->ymax);
              indices_ticks_x.push_back(cnt++);

              indices_ticks_x.push_back(std::numeric_limits<GLuint>::max());

              xval += xTickValueDelta;
          }

          _pdata->sizeInd_ticks = indices_ticks_x.size();

          _pdata->vbo_ticks.init(vert_ticks_x);
          _pdata->ibo_ticks.init(indices_ticks_x);
          _pdata->vao_ticks.init(_pdata->vbo_ticks, _pdata->ibo_ticks);
      }
      else // if (orientation_is_vertical())
      {
          /*
           * y axis
           */
          vert.push_back(_pdata->xmin); // y axis ends in (0,ymax)
          vert.push_back(_pdata->ymax);

          /*
           * y ticks (horizontal lines)
           */
          const auto yTickValueDelta = (_pdata->ymax - _pdata->ymin) / (_pdata->numTicks + 1);
          auto yval = _pdata->ymin;
          std::vector<GLfloat> vert_ticks_y;
          vert_ticks_y.reserve(2 * 2 * (_pdata->numTicks + 1));

          std::vector<GLuint> indices_ticks_y;
          indices_ticks_y.reserve(2 * (_pdata->numTicks + 1));
          unsigned int cnt = 0;

          //for (unsigned int i = 0; i < _pdata->numTicks + 1; ++i)
          for (unsigned int i = 0; i <= _pdata->numTicks + 1; ++i)
          {
              vert_ticks_y.push_back(_pdata->xmin);
              vert_ticks_y.push_back(yval);
              indices_ticks_y.push_back(cnt++);

              vert_ticks_y.push_back(_pdata->xmax);
              vert_ticks_y.push_back(yval);
              indices_ticks_y.push_back(cnt++);

              indices_ticks_y.push_back(std::numeric_limits<GLuint>::max());

              yval += yTickValueDelta;
          }

          _pdata->sizeInd_ticks = indices_ticks_y.size();

          _pdata->vbo_ticks.init(vert_ticks_y);
          _pdata->ibo_ticks.init(indices_ticks_y);
          _pdata->vao_ticks.init(_pdata->vbo_ticks, _pdata->ibo_ticks);
      }

      _pdata->vbo.init(vert);
      _pdata->vao.init(_pdata->vbo);

      return true;
  }

  bool PlotAxis::init_ubo()
  {
      if (!_pdata->ubo.init_from_registered_values_size())
      { return false; }

      _pdata->ubo.set_line_width(_pdata->lineWidth);
      _pdata->ubo.set_color_r(_pdata->color[0]);
      _pdata->ubo.set_color_g(_pdata->color[1]);
      _pdata->ubo.set_color_b(_pdata->color[2]);
      _pdata->ubo.set_color_a(_pdata->color[3]);
      _pdata->ubo.release();

      return true;
  }

  bool PlotAxis::init()
  {
      if (!(init_shader() && init_ubo() && init_vbo_vao()))
      {
          clear();
          return false;
      }
      else
      { return true; }
  }

  void PlotAxis::clear_shader()
  {
      _pdata->shader.clear();
      _pdata->shader_ticks.clear();
  }

  void PlotAxis::clear_vbo_vao()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();

      _pdata->vbo_ticks.clear();
      _pdata->vao_ticks.clear();
      _pdata->ibo_ticks.clear();
  }

  void PlotAxis::clear_ubo()
  { _pdata->ubo.clear(); }

  void PlotAxis::clear()
  {
      clear_shader();
      clear_vbo_vao();
      clear_ubo();
  }

  void PlotAxis::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void PlotAxis::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void PlotAxis::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void PlotAxis::on_modelview_changed(bool /*b*/)
  { /* do nothing */ }

  void PlotAxis::on_visible_changed(bool /*b*/)
  { /* do nothing */ }

  void PlotAxis::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void PlotAxis::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotAxis::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotAxis::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void PlotAxis::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void PlotAxis::on_mouse_wheel_up()
  { /* do nothing */ }

  void PlotAxis::on_mouse_wheel_down()
  { /* do nothing */ }

  void PlotAxis::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  void PlotAxis::draw_impl()
  {
      _pdata->ubo.bind_to_default_base();

      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_BLEND);
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();
      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      /*
       * axis
       */
      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawArrays(GL_LINE_STRIP, 0, 2);
      _pdata->shader.release();
      _pdata->vao.release();

      /*
       * ticks
       */
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->shader_ticks.bind();
      _pdata->vao_ticks.bind();
      //BK_QT_GL glDrawElements(GL_LINE_STRIP, 3 * (_pdata->numTicks + 1), GL_UNSIGNED_INT, nullptr);
      BK_QT_GL glDrawElements(GL_LINE_STRIP, _pdata->sizeInd_ticks, GL_UNSIGNED_INT, nullptr);
      _pdata->vao_ticks.release();
      _pdata->shader_ticks.release();

      BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


