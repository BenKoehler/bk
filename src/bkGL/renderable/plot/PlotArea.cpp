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

#include <bkGL/renderable/plot/PlotArea.h>

#include <algorithm>
#include <limits>

#include <bkGL/renderable/plot/AreaPlotDataVector.h>
#include <bkGL/renderable/plot/PlotDataVector.h>
#include <bkGL/renderable/plot/PlotLine.h>
#include <bkTools/color/ColorRGBA.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/UBOPlotArea.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotArea::Impl
  {
      VAO vao;
      VBO vbo;
      IBO ibo;
      details::UBOPlotArea ubo;
      Shader shader;
      PlotLine line_upper;
      PlotLine line_lower;
      AreaPlotDataVector data;
      ColorRGBA color_area_above;
      ColorRGBA color_area_below;
      ColorRGBA color_line;
      GLfloat lineWidth;
      GLsizei sizeInd;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : vao(gl),
            vbo(gl),
            ibo(gl),
            ubo(gl),
            shader(gl),
            line_upper(gl),
            line_lower(gl),
          #endif
          color_area_above(0, 0.75, 0, 1),
          color_area_below(0.75, 0, 0, 1),
          color_line(0.2, 0.2, 0.2, 1),
          lineWidth(2),
          sizeInd(0)
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

  PlotArea::PlotArea()
      : base_type()
  #else

  PlotArea::PlotArea(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ibo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->vao.add_default_attribute_scalar_1xfloat("diff");

      _pdata->line_lower.set_line_width(_pdata->lineWidth);
      _pdata->line_lower.set_color(_pdata->color_line);
      _pdata->line_upper.set_line_width(_pdata->lineWidth);
      _pdata->line_upper.set_color(_pdata->color_line);
  }

  PlotArea::PlotArea(PlotArea&&) noexcept = default;

  PlotArea::~PlotArea() = default;

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  const ColorRGBA& PlotArea::color_above() const
  { return _pdata->color_area_above; }

  const ColorRGBA& PlotArea::color_below() const
  { return _pdata->color_area_below; }

  const ColorRGBA& PlotArea::color_line() const
  { return _pdata->color_line; }

  GLfloat PlotArea::line_width() const
  { return _pdata->lineWidth; }

  const AreaPlotDataVector& PlotArea::data_vector() const
  { return _pdata->data; }

  AreaPlotDataVector& PlotArea::data_vector()
  { return _pdata->data; }

  GLfloat PlotArea::x_min() const
  {
      const auto& v = data_vector().x_value_vector();
      return v.empty() ? std::numeric_limits<GLfloat>::max() : *std::min_element(v.begin(), v.end());
  }

  GLfloat PlotArea::x_max() const
  {
      const auto& v = data_vector().x_value_vector();
      return v.empty() ? -std::numeric_limits<GLfloat>::max() : *std::max_element(v.begin(), v.end());
  }

  GLfloat PlotArea::y_min() const
  {
      const auto& v0 = data_vector().y0_value_vector();
      const auto& v1 = data_vector().y1_value_vector();

      if (v0.empty() || v0.size() != v1.size())
      { return std::numeric_limits<GLfloat>::max(); }

      const auto min0 = *std::min_element(v0.begin(), v0.end());
      const auto min1 = *std::min_element(v1.begin(), v1.end());

      return std::min(min0, min1);
  }

  GLfloat PlotArea::y_max() const
  {
      const auto& v0 = data_vector().y0_value_vector();
      const auto& v1 = data_vector().y1_value_vector();

      if (v0.empty() || v0.size() != v1.size())
      { return std::numeric_limits<GLfloat>::max(); }

      const auto max0 = *std::max_element(v0.begin(), v0.end());
      const auto max1 = *std::max_element(v1.begin(), v1.end());

      return std::max(max0, max1);
  }

  bool PlotArea::is_initialized() const
  { return _pdata->vao.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotArea& PlotArea::operator=(PlotArea&&) noexcept = default;

  void PlotArea::set_color_area_above(const ColorRGBA& col)
  { set_color_area_above(col[0], col[1], col[2], col[3]); }

  void PlotArea::set_color_area_above(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      _pdata->color_area_above.set(r, g, b, a);

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_area_above_r(_pdata->color_area_above[0]);
          _pdata->ubo.set_color_area_above_g(_pdata->color_area_above[1]);
          _pdata->ubo.set_color_area_above_b(_pdata->color_area_above[2]);
          _pdata->ubo.set_color_area_above_a(_pdata->color_area_above[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotArea::set_color_area_below(const ColorRGBA& col)
  { set_color_area_below(col[0], col[1], col[2], col[3]); }

  void PlotArea::set_color_area_below(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      _pdata->color_area_below.set(r, g, b, a);

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_area_below_r(_pdata->color_area_below[0]);
          _pdata->ubo.set_color_area_below_g(_pdata->color_area_below[1]);
          _pdata->ubo.set_color_area_below_b(_pdata->color_area_below[2]);
          _pdata->ubo.set_color_area_below_a(_pdata->color_area_below[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotArea::set_color_line(const ColorRGBA& col)
  { set_color_line(col[0], col[1], col[2], col[3]); }

  void PlotArea::set_color_line(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      _pdata->color_line.set(r, g, b, a);
      _pdata->line_lower.set_color(r, g, b, a);
      _pdata->line_upper.set_color(r, g, b, a);

      if (this->is_initialized())
      { this->emit_signal_update_required(); }
  }

  void PlotArea::set_line_width(GLfloat w)
  {
      _pdata->lineWidth = std::max(w, static_cast<GLfloat>(0));
      _pdata->line_lower.set_line_width(_pdata->lineWidth);
      _pdata->line_upper.set_line_width(_pdata->lineWidth);

      if (this->is_initialized())
      { this->emit_signal_update_required(); }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotArea::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::plot::area;
      return _pdata->shader.init_from_sources(SL::vert(), SL::frag());
  }

  bool PlotArea::init_vbo_vao()
  {
      const unsigned int N = _pdata->data.num_values();

      std::vector<GLfloat> vert;
      vert.reserve(N * 2 * 2);

      std::vector<GLuint> indices;
      indices.reserve(N * 4 * 3);

      GLfloat xintersection = 0;
      GLfloat yintersection = 0;

      // first point
      vert.push_back(*_pdata->data.x_value(0));
      vert.push_back(*_pdata->data.y0_value(0));
      vert.push_back(*_pdata->data.y1_value(0) - *_pdata->data.y0_value(0));
      vert.push_back(*_pdata->data.x_value(0));
      vert.push_back(*_pdata->data.y1_value(0));
      vert.push_back(*_pdata->data.y1_value(0) - *_pdata->data.y0_value(0));

      GLuint cnt = 2;

      for (unsigned int i = 1; i < N; ++i)
      {
          const bool consistent_order = (*_pdata->data.y0_value(i - 1) < *_pdata->data.y1_value(i - 1) && *_pdata->data.y0_value(i) < *_pdata->data.y1_value(i)) || (*_pdata->data.y0_value(i - 1) >= *_pdata->data.y1_value(i - 1) && *_pdata->data.y0_value(i) >= *_pdata->data.y1_value(i));

          if (consistent_order)
          {
              vert.push_back(*_pdata->data.x_value(i));
              vert.push_back(*_pdata->data.y0_value(i));
              vert.push_back(*_pdata->data.y1_value(i) - *_pdata->data.y0_value(i));

              indices.push_back(cnt - 2);
              indices.push_back(cnt - 1);
              indices.push_back(cnt++);

              vert.push_back(*_pdata->data.x_value(i));
              vert.push_back(*_pdata->data.y1_value(i));
              vert.push_back(*_pdata->data.y1_value(i) - *_pdata->data.y0_value(i));

              indices.push_back(cnt - 2);
              indices.push_back(cnt - 1);
              indices.push_back(cnt++);
          }
          else
          {
              const GLfloat dx = *_pdata->data.x_value(i) - *_pdata->data.x_value(i - 1);
              const GLfloat m0 = (*_pdata->data.y0_value(i) - *_pdata->data.y0_value(i - 1)) / dx;
              const GLfloat m1 = (*_pdata->data.y1_value(i) - *_pdata->data.y1_value(i - 1)) / dx;
              xintersection = (*_pdata->data.y1_value(i - 1) - *_pdata->data.y0_value(i - 1)) / (m0 - m1);
              yintersection = m0 * xintersection + *_pdata->data.y0_value(i - 1);

              vert.push_back(*_pdata->data.x_value(i - 1) + xintersection);
              vert.push_back(yintersection);
              vert.push_back(yintersection - *_pdata->data.y1_value(i));

              indices.push_back(cnt - 2);
              indices.push_back(cnt - 1);
              indices.push_back(cnt++);

              vert.push_back(*_pdata->data.x_value(i));
              vert.push_back(*_pdata->data.y0_value(i));
              vert.push_back(*_pdata->data.y1_value(i) - *_pdata->data.y0_value(i));
              vert.push_back(*_pdata->data.x_value(i));
              vert.push_back(*_pdata->data.y1_value(i));
              vert.push_back(*_pdata->data.y1_value(i) - *_pdata->data.y0_value(i));

              indices.push_back(cnt - 1);
              indices.push_back(cnt++);
              indices.push_back(cnt++);
          }
      }

      _pdata->sizeInd = indices.size();

      _pdata->vbo.init(vert);
      _pdata->ibo.init(indices);
      return _pdata->vao.init(_pdata->vbo, _pdata->ibo);
  }

  bool PlotArea::init_ubo()
  {
      clear_ubo();

      if (!_pdata->ubo.init_from_registered_values_size())
      { return false; }

      _pdata->ubo.set_color_area_above_r(_pdata->color_area_above[0]);
      _pdata->ubo.set_color_area_above_g(_pdata->color_area_above[1]);
      _pdata->ubo.set_color_area_above_b(_pdata->color_area_above[2]);
      _pdata->ubo.set_color_area_above_a(_pdata->color_area_above[3]);
      _pdata->ubo.set_color_area_below_r(_pdata->color_area_below[0]);
      _pdata->ubo.set_color_area_below_g(_pdata->color_area_below[1]);
      _pdata->ubo.set_color_area_below_b(_pdata->color_area_below[2]);
      _pdata->ubo.set_color_area_below_a(_pdata->color_area_below[3]);
      _pdata->ubo.release();

      return true;
  }

  bool PlotArea::init()
  {
      clear();

      bool success = init_shader();
      success &= init_ubo();
      success &= init_vbo_vao();

      /*
       * init lower/upper bound lines
       */
      const unsigned int N = _pdata->data.num_values();

      auto& dv_lower = _pdata->line_lower.data_vector();
      auto& dv_upper = _pdata->line_upper.data_vector();
      dv_lower.set_num_values(N);
      dv_upper.set_num_values(N);

      for (unsigned int i = 0; i < N; ++i)
      {
          dv_lower.set_value(i, *_pdata->data.x_value(i), *_pdata->data.y0_value(i));
          dv_upper.set_value(i, *_pdata->data.x_value(i), *_pdata->data.y1_value(i));
      }

      success &= _pdata->line_lower.init();
      success &= _pdata->line_upper.init();

      if (!success)
      { clear(); }

      return success;
  }

  void PlotArea::clear_shader()
  { _pdata->shader.clear(); }

  void PlotArea::clear_vbo_vao()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }

  void PlotArea::clear_ubo()
  { _pdata->ubo.clear(); }

  void PlotArea::clear()
  {
      clear_shader();
      clear_vbo_vao();
      clear_ubo();
  }

  void PlotArea::on_resize(GLint w, GLint h)
  {
      _pdata->line_lower.on_resize(w, h);
      _pdata->line_upper.on_resize(w, h);
  }

  void PlotArea::on_oit_enabled(bool b)
  {
      _pdata->line_lower.on_oit_enabled(b);
      _pdata->line_upper.on_oit_enabled(b);
  }

  void PlotArea::on_animation_enabled(bool b)
  {
      _pdata->line_lower.on_animation_enabled(b);
      _pdata->line_upper.on_animation_enabled(b);
  }

  void PlotArea::on_modelview_matrix_changed(bool b)
  {
      _pdata->line_lower.on_modelview_matrix_changed(b);
      _pdata->line_upper.on_modelview_matrix_changed(b);
  }

  void PlotArea::on_new_modelview_matrix(const ColMat4<GLfloat>& m)
  {
      _pdata->line_lower.on_new_modelview_matrix(m);
      _pdata->line_upper.on_new_modelview_matrix(m);
  }

  void PlotArea::on_new_projection_matrix(const ColMat4<GLfloat>& p)
  {
      _pdata->line_lower.on_new_projection_matrix(p);
      _pdata->line_upper.on_new_projection_matrix(p);
  }

  void PlotArea::on_visible_changed(bool b)
  {
      _pdata->line_lower.on_visible_changed(b);
      _pdata->line_upper.on_visible_changed(b);
  }

  void PlotArea::on_mouse_pos_changed(GLint x, GLint y)
  {
      _pdata->line_lower.on_mouse_pos_changed(x, y);
      _pdata->line_upper.on_mouse_pos_changed(x, y);
  }

  void PlotArea::on_mouse_button_pressed(MouseButton_ btn)
  {
      _pdata->line_lower.on_mouse_button_pressed(btn);
      _pdata->line_upper.on_mouse_button_pressed(btn);
  }

  void PlotArea::on_mouse_button_released(MouseButton_ btn)
  {
      _pdata->line_lower.on_mouse_button_released(btn);
      _pdata->line_upper.on_mouse_button_released(btn);
  }

  void PlotArea::on_key_pressed(Key_ k)
  {
      _pdata->line_lower.on_key_pressed(k);
      _pdata->line_upper.on_key_pressed(k);
  }

  void PlotArea::on_key_released(Key_ k)
  {
      _pdata->line_lower.on_key_released(k);
      _pdata->line_upper.on_key_released(k);
  }

  void PlotArea::on_mouse_wheel_up()
  {
      _pdata->line_lower.on_mouse_wheel_up();
      _pdata->line_upper.on_mouse_wheel_up();
  }

  void PlotArea::on_mouse_wheel_down()
  {
      _pdata->line_lower.on_mouse_wheel_down();
      _pdata->line_upper.on_mouse_wheel_down();
  }

  void PlotArea::on_ssaa_factor_changed(GLint ssaa_factor)
  {
      _pdata->line_lower.on_ssaa_factor_changed(ssaa_factor);
      _pdata->line_upper.on_ssaa_factor_changed(ssaa_factor);
  }

  void PlotArea::on_animation_time_changed(GLfloat t)
  {
      _pdata->line_lower.on_animation_time_changed(t);
      _pdata->line_upper.on_animation_time_changed(t);
  }

  void PlotArea::draw_impl()
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

      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
      _pdata->shader.release();
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();

      _pdata->line_lower.draw();
      _pdata->line_upper.draw();
  }
} // namespace bk


