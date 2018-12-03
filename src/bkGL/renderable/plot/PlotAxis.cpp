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
#include <iostream>
#include <sstream>
#include <vector>

#include <bkTools/color/ColorRGBA.h>
#include <bkGL/UBOPlotLine.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/renderable/text/TextView.h>

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
      TextView textview_label;
      std::vector<TextView> textview_ticks;
      GLsizei sizeInd_ticks;
      GLfloat xmin;
      GLfloat xmax;
      GLfloat ymin;
      GLfloat ymax;
      GLfloat border_width_x_in_percent;
      GLfloat border_width_y_in_percent;
      GLfloat window_width;
      GLfloat window_height;
      GLint ssaa;
      ColorRGBA color;
      GLfloat lineWidth;
      GLuint numTicks;
      std::string label;
      PlotAxisOrientation orientation;
      unsigned int tickPrecision;
      bool draw_ticks_manually;

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
            textview_label(gl),
          #endif
          sizeInd_ticks(0),
          xmin(0),
          xmax(0),
          ymin(0),
          ymax(0),
          border_width_x_in_percent(0),
          border_width_y_in_percent(0),
          window_width(0),
          window_height(0),
          ssaa(1),
          color(0.5, 0.5, 0.5, 1),
          lineWidth(1),
          numTicks(3),
          label(""),
          orientation(PlotAxisOrientation_Horizontal),
          tickPrecision(1),
          draw_ticks_manually(false)
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

      _pdata->textview_label.set_position_mode_relative();
      _pdata->textview_label.set_background_enabled(true);
      _pdata->textview_label.set_color_background(0, 0, 0, 0.25);
      this->forward_signals(&_pdata->textview_label);
  }

  PlotAxis::PlotAxis(PlotAxis&&) noexcept = default;

  PlotAxis::~PlotAxis() = default;

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

  GLfloat PlotAxis::border_width_x_in_percent() const
  { return _pdata->border_width_x_in_percent; }

  GLfloat PlotAxis::border_width_y_in_percent() const
  { return _pdata->border_width_y_in_percent; }

  PlotAxisOrientation PlotAxis::orientation() const
  { return _pdata->orientation; }

  bool PlotAxis::orientation_is_horizontal() const
  { return _pdata->orientation == PlotAxisOrientation_Horizontal; }

  bool PlotAxis::orientation_is_vertical() const
  { return _pdata->orientation == PlotAxisOrientation_Vertical; }

  unsigned int PlotAxis::tick_precision() const
  { return _pdata->tickPrecision; }

  TextView& PlotAxis::text_view_label()
  { return _pdata->textview_label; }

  const TextView& PlotAxis::text_view_label() const
  { return _pdata->textview_label; }

  bool PlotAxis::is_initialized() const
  { return _pdata->vao.is_initialized() && _pdata->ubo.is_initialized(); }

  bool PlotAxis::draw_ticks_manually() const
  { return _pdata->draw_ticks_manually; }

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
  {
      _pdata->label = label;

      if (this->is_initialized())
      {
          init_textview();
          this->emit_signal_update_required();
      }
  }

  void PlotAxis::set_border_width_x_in_percent(GLfloat p)
  { _pdata->border_width_x_in_percent = p; }

  void PlotAxis::set_border_width_y_in_percent(GLfloat p)
  { _pdata->border_width_y_in_percent = p; }

  void PlotAxis::set_orientation(PlotAxisOrientation orientation)
  {
      _pdata->orientation = orientation;

      if (_pdata->orientation == PlotAxisOrientation_Horizontal)
      {
          _pdata->textview_label.set_orientation_horizontal();

          for (auto& t: _pdata->textview_ticks)
          { t.set_orientation_horizontal(); }
      }
      else // if (_pdata->orientation == PlotAxisOrientation_Vertical)
      {
          _pdata->textview_label.set_orientation_vertical();

          for (auto& t: _pdata->textview_ticks)
          { t.set_orientation_vertical(); }
      }

      if (this->is_initialized())
      {
          init_vbo_vao();
          this->emit_signal_update_required();
      }
  }

  void PlotAxis::set_orientation_horizontal()
  { set_orientation(PlotAxisOrientation_Horizontal); }

  void PlotAxis::set_orientation_vertical()
  { set_orientation(PlotAxisOrientation_Vertical); }

  void PlotAxis::set_tick_precision(unsigned int tickPrecision)
  { _pdata->tickPrecision = tickPrecision; }

  void PlotAxis::set_draw_ticks_manually(bool b)
  {
      if (_pdata->draw_ticks_manually != b)
      {
          _pdata->draw_ticks_manually = b;

          if (this->is_initialized())
          { emit_signal_update_required(); }
      }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotAxis::init_shader()
  {
      using SL = details::ShaderLibrary::plot;

      bool success = true;
      success &= _pdata->shader.init_from_sources(SL::axis::vert(), SL::axis::frag(), SL::axis::geom());
      success &= _pdata->shader_ticks.init_from_sources(SL::ticks::vert(), SL::ticks::frag(), SL::ticks::geom());

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
          const GLfloat xTickValueDelta = (_pdata->xmax - _pdata->xmin) / (_pdata->numTicks + 1);
          GLfloat xval = _pdata->xmin;
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
          const GLfloat yTickValueDelta = (_pdata->ymax - _pdata->ymin) / (_pdata->numTicks + 1);
          GLfloat yval = _pdata->ymin;
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
      {
          std::cerr << "PlotAxis::init_ubo - failed" << std::endl;
          return false;
      }

      _pdata->ubo.set_line_width(_pdata->lineWidth);
      _pdata->ubo.set_color_r(_pdata->color[0]);
      _pdata->ubo.set_color_g(_pdata->color[1]);
      _pdata->ubo.set_color_b(_pdata->color[2]);
      _pdata->ubo.set_color_a(_pdata->color[3]);
      _pdata->ubo.release();

      return true;
  }

  bool PlotAxis::init_textview()
  {
      const bool success = _pdata->textview_label.init(_pdata->label);
      constexpr GLfloat margin = 0.01f;
      constexpr double scalescale = 0.75;

      /*
       * label
       */
      if (orientation_is_horizontal())
      {
          const GLfloat w = _pdata->textview_label.text_pixel_width_relative();
          const GLfloat x = _pdata->border_width_x_in_percent + 0.5 * (1.0f - _pdata->border_width_x_in_percent - w);
          const GLfloat y = margin;
          _pdata->textview_label.set_position(x, y);
          //_pdata->textview_label.set_position(1.0f - margin - w, _pdata->border_width_y_in_percent + margin); // bottom right, above axis
      }
      else // if (orientation_is_vertical())
      {
          const GLfloat fw = orientation_is_horizontal() ? 1 : _pdata->window_width / _pdata->window_height;

          const GLfloat w = fw * _pdata->textview_label.text_pixel_width_relative();
          //const GLfloat h = _pdata->textview_label.text_pixel_height_relative();
          //const GLfloat x = h + margin;
          const GLfloat x = margin;
          const GLfloat y = _pdata->border_width_y_in_percent + 0.5 * (1.0f - _pdata->border_width_y_in_percent - w);
          _pdata->textview_label.set_position(x, y);
          //_pdata->textview_label.set_position(_pdata->border_width_x_in_percent + margin, 1.0f - h - margin); // top left; right next to axis
      }

      /*
       * ticks
       */
      _pdata->textview_ticks.clear();

      std::stringstream ss;
      ss << std::fixed;
      ss.precision(tick_precision());

      const GLfloat range = orientation_is_horizontal() ? _pdata->xmax - _pdata->xmin : _pdata->ymax - _pdata->ymin;
      const GLfloat deltaValue = range / (_pdata->numTicks + 1);
      const GLfloat deltaRel = (1.0f - (orientation_is_horizontal() ? _pdata->border_width_x_in_percent : _pdata->border_width_y_in_percent)) / (_pdata->numTicks + 1);
      GLfloat val = orientation_is_horizontal() ? _pdata->xmin : _pdata->ymin;
      GLfloat rel = orientation_is_horizontal() ? _pdata->border_width_x_in_percent : _pdata->border_width_y_in_percent;

      std::vector<GLfloat> values;
      values.reserve(num_ticks() + 2);

      std::vector<GLfloat> rels;
      rels.reserve(num_ticks() + 2);

      values.push_back(val);
      rels.push_back(rel);
      for (unsigned int i = 0; i < num_ticks() + 1; ++i)
      {
          val += deltaValue;
          values.push_back(val);

          rel += deltaRel;
          rels.push_back(rel);
      }

      for (unsigned int i = 0; i < values.size(); ++i)
      {
          val = values[i];
          rel = rels[i];

          TextView tv
          #ifdef BK_LIB_QT_AVAILABLE
          (this->_gl)
          #endif
          ;

          ss.str("");
          ss << val;

          if (_pdata->textview_label.position_mode_is_absolute())
          { tv.set_position_mode_absolute(); }
          else
          { tv.set_position_mode_relative(); }

          tv.on_resize(_pdata->window_width, _pdata->window_height);
          tv.on_ssaa_factor_changed(_pdata->ssaa);
          tv.set_scale(scalescale * _pdata->textview_label.scale_x(), scalescale * _pdata->textview_label.scale_y());
          tv.set_color_background(_pdata->textview_label.color_background());
          tv.set_color_text(_pdata->textview_label.color_text());
          tv.set_background_enabled(_pdata->textview_label.background_is_enabled());
          tv.set_text(ss.str());

          const GLfloat w = tv.text_pixel_width_relative();
          const GLfloat h = tv.text_pixel_height_relative();

          if (orientation_is_horizontal())
          {
              if (i == 0)
              { tv.set_position(rel, _pdata->border_width_y_in_percent - h - margin); }
              else if (i != values.size() - 1)
              { tv.set_position(rel - 0.5 * w, _pdata->border_width_y_in_percent - h - margin); }
              else
              { tv.set_position(1.0f - w, _pdata->border_width_y_in_percent - h - margin); }
          }
          else
          {
              if (i == 0)
              { tv.set_position(_pdata->border_width_x_in_percent - w - margin, rel); }
              else if (i != values.size() - 1)
              { tv.set_position(_pdata->border_width_x_in_percent - w - margin, rel - 0.5 * h); }
              else
              { tv.set_position(_pdata->border_width_x_in_percent - w - margin, 1.0f - h); }
          }

          tv.init();

          _pdata->textview_ticks.emplace_back(std::move(tv));
      } // for x

      return success;
  }

  bool PlotAxis::init()
  {
      bool success = true;

      if (!(success &= init_shader()))
      { std::cerr << "PlotAxis::init - shader failed" << std::endl; }

      if (!(success &= init_ubo()))
      { std::cerr << "PlotAxis::init - ubo failed" << std::endl; }

      if (!(success &= init_vbo_vao()))
      { std::cerr << "PlotAxis::init - vbo/vao failed" << std::endl; }

      if (!(success &= init_textview()))
      { std::cerr << "PlotAxis::init - textview_label failed" << std::endl; }

      if (!success)
      { clear(); }

      return success;
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

  void PlotAxis::on_resize(GLint w, GLint h)
  {
      _pdata->window_width = w;
      _pdata->window_height = h;

      _pdata->textview_label.on_resize(w, h);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_resize(w, h); }

      if (this->is_initialized())
      {
          init_textview();
          emit_signal_update_required();
      }
  }

  void PlotAxis::on_oit_enabled(bool b)
  {
      _pdata->textview_label.on_oit_enabled(b);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_oit_enabled(b); }
  }

  void PlotAxis::on_animation_enabled(bool b)
  {
      _pdata->textview_label.on_animation_enabled(b);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_animation_enabled(b); }
  }

  void PlotAxis::on_modelview_matrix_changed(bool b)
  {
      _pdata->textview_label.on_modelview_matrix_changed(b);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_modelview_matrix_changed(b); }
  }

  void PlotAxis::on_new_modelview_matrix(const ColMat4<GLfloat>& m)
  {
      _pdata->textview_label.on_new_modelview_matrix(m);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_new_modelview_matrix(m); }
  }

  void PlotAxis::on_new_projection_matrix(const ColMat4<GLfloat>& p)
  {
      _pdata->textview_label.on_new_projection_matrix(p);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_new_projection_matrix(p); }
  }

  void PlotAxis::on_visible_changed(bool b)
  {
      _pdata->textview_label.on_visible_changed(b);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_visible_changed(b); }
  }

  void PlotAxis::on_mouse_pos_changed(GLint x, GLint y)
  {
      _pdata->textview_label.on_mouse_pos_changed(x, y);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_mouse_pos_changed(x, y); }
  }

  void PlotAxis::on_mouse_button_pressed(MouseButton_ btn)
  {
      _pdata->textview_label.on_mouse_button_pressed(btn);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_mouse_button_pressed(btn); }
  }

  void PlotAxis::on_mouse_button_released(MouseButton_ btn)
  {
      _pdata->textview_label.on_mouse_button_released(btn);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_mouse_button_released(btn); }
  }

  void PlotAxis::on_key_pressed(Key_ k)
  {
      _pdata->textview_label.on_key_pressed(k);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_key_pressed(k); }
  }

  void PlotAxis::on_key_released(Key_ k)
  {
      _pdata->textview_label.on_key_released(k);

      for (TextView& t: _pdata->textview_ticks)
      { t.on_key_released(k); }
  }

  void PlotAxis::on_mouse_wheel_up()
  {
      _pdata->textview_label.on_mouse_wheel_up();

      for (TextView& t: _pdata->textview_ticks)
      { t.on_mouse_wheel_up(); }
  }

  void PlotAxis::on_mouse_wheel_down()
  {
      _pdata->textview_label.on_mouse_wheel_down();

      for (TextView& t: _pdata->textview_ticks)
      { t.on_mouse_wheel_down(); }
  }

  void PlotAxis::on_ssaa_factor_changed(GLint ssaa_factor)
  {
      _pdata->textview_label.on_ssaa_factor_changed(ssaa_factor);
      _pdata->ssaa = ssaa_factor;

      for (TextView& t: _pdata->textview_ticks)
      { t.on_ssaa_factor_changed(ssaa_factor); }

      if (this->is_initialized())
      {
          init_textview();
          emit_signal_update_required();
      }
  }

  void PlotAxis::on_animation_time_changed(GLfloat t)
  {
      _pdata->textview_label.on_animation_time_changed(t);

      for (TextView& tv: _pdata->textview_ticks)
      { tv.on_animation_time_changed(t); }
  }

  void PlotAxis::draw_ticks()
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
       * ticks
       */
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->shader_ticks.bind();
      _pdata->vao_ticks.bind();
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
      if (!_pdata->draw_ticks_manually)
      {
          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

          _pdata->shader_ticks.bind();
          _pdata->vao_ticks.bind();
          BK_QT_GL glDrawElements(GL_LINE_STRIP, _pdata->sizeInd_ticks, GL_UNSIGNED_INT, nullptr);
          _pdata->vao_ticks.release();
          _pdata->shader_ticks.release();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
      }

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();

      /*
       * label
       */
      _pdata->textview_label.draw();

      for (TextView& t: _pdata->textview_ticks)
      { t.draw(); }
  }
} // namespace bk


