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

#include "PlotLine.h"

#include <algorithm>
#include <limits>

#include "PlotDataVector.h"
#include "tools/color/ColorRGBA.h"
#include "gl/buffer/VBO.h"
#include "gl/buffer/UBO.h"
#include "gl/vao/VAO.h"
#include "gl/Shader.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotLine::Impl
  {
      VAO vao;
      VBO vbo;
      UBO ubo;
      Shader shader;
      PlotDataVector data;
      color_type color;
      value_type lineWidth;
      GLsizei primitiveCount;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : vao(gl),
            vbo(gl),
            ubo(gl),
            shader(gl),
          #endif
          //color(0.25, 0.5, 1, 1),
            color(1, 0.5, 0, 1),
            lineWidth(3),
            primitiveCount(0)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR 
  //====================================================================================================

  #ifndef BK_LIB_QT_AVAILABLE
  PlotLine::PlotLine()
      : base_type(),
      _pdata(std::make_unique<Impl>())
  #else

  PlotLine::PlotLine(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();

      _pdata->ubo.set_usage_STATIC_DRAW();
      _pdata->ubo.register_value_of_size("lineWidth", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorr", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorg", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorb", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colora", sizeof(value_type));
  }

  PlotLine::PlotLine(self_type&&) = default;

  PlotLine::~PlotLine()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  auto PlotLine::get_color() const -> const color_type&
  { return _pdata->color; }

  auto PlotLine::get_line_width() const -> value_type
  { return _pdata->lineWidth; }

  auto PlotLine::get_data_vector() const -> const data_vector_type&
  { return _pdata->data; }

  auto PlotLine::get_data_vector() -> data_vector_type&
  { return _pdata->data; }

  auto PlotLine::get_x_min() const -> value_type
  {
      const auto& v = get_data_vector().get_x_value_vector();
      return v.empty() ? std::numeric_limits<value_type>::max() : *std::min_element(v.begin(), v.end());
  }

  auto PlotLine::get_x_max() const -> value_type
  {
      const auto& v = get_data_vector().get_x_value_vector();
      return v.empty() ? -std::numeric_limits<value_type>::max() : *std::max_element(get_data_vector().get_x_value_vector().begin(), get_data_vector().get_x_value_vector().end());
  }

  auto PlotLine::get_y_min() const -> value_type
  {
      const auto& v = get_data_vector().get_y_value_vector();
      return v.empty() ? std::numeric_limits<value_type>::max() : *std::min_element(get_data_vector().get_y_value_vector().begin(), get_data_vector().get_y_value_vector().end());
  }

  auto PlotLine::get_y_max() const -> value_type
  {
      const auto& v = get_data_vector().get_y_value_vector();
      return v.empty() ? -std::numeric_limits<value_type>::max() : *std::max_element(get_data_vector().get_y_value_vector().begin(), get_data_vector().get_y_value_vector().end());
  }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto PlotLine::operator=(self_type&& other) -> self_type& = default;

  void PlotLine::set_color(const color_type& col)
  { set_color(col[0], col[1], col[2], col[3]); }

  void PlotLine::set_color(value_type r, value_type g, value_type b, value_type a)
  { _pdata->color.set(r, g, b, a); }

  void PlotLine::set_line_width(value_type w)
  {
      _pdata->lineWidth = std::max(w, static_cast<value_type>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.write_registered_value("lineWidth", &_pdata->lineWidth);
          _pdata->ubo.release();
      }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotLine::init_shader()
  {
      clear_shader();
      return _pdata->shader.init("shader/plot-new/plotline.vert", "shader/plot-new/plotline.frag", "shader/plot-new/plotline.geom");
  }

  bool PlotLine::init_vbo_vao()
  {
      clear_vbo_vao();

      const unsigned int N = _pdata->data.get_num_values();

      if (N == 0)
      { return false; }

      _pdata->primitiveCount = N + 2;

      const std::vector<value_type>& x = _pdata->data.get_x_value_vector();
      const std::vector<value_type>& y = _pdata->data.get_y_value_vector();
      std::vector<value_type> vert;
      vert.reserve(2 * _pdata->primitiveCount);

      // insert first point twice for line adjacency
      vert.push_back(x[0]);
      vert.push_back(y[0]);

      for (unsigned int i = 0; i < N; ++i)
      {
          vert.push_back(x[i]);
          vert.push_back(y[i]);
      }

      // insert last point twice for line adjacency
      vert.push_back(x[N - 1]);
      vert.push_back(y[N - 1]);

      _pdata->vbo.init(vert);
      _pdata->vao.init(_pdata->vbo);

      return true;
  }

  bool PlotLine::init_ubo()
  {
      clear_ubo();

      if (!_pdata->ubo.init_from_registered_values_size())
      { return false; }

      _pdata->ubo.write_registered_value("lineWidth", &_pdata->lineWidth);
      value_type ftemp = _pdata->color[0];
      _pdata->ubo.write_registered_value("colorr", &ftemp);
      ftemp = _pdata->color[1];
      _pdata->ubo.write_registered_value("colorg", &ftemp);
      ftemp = _pdata->color[2];
      _pdata->ubo.write_registered_value("colorb", &ftemp);
      ftemp = _pdata->color[3];
      _pdata->ubo.write_registered_value("colora", &ftemp);
      _pdata->ubo.release();

      return true;
  }

  bool PlotLine::init()
  {
      clear();

      const bool success = init_shader() && init_ubo() && init_vbo_vao();
      this->set_initialized(success);

      if (!success)
      { clear(); }

      return success;
  }

  void PlotLine::clear_shader()
  { _pdata->shader.clear(); }

  void PlotLine::clear_vbo_vao()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }

  void PlotLine::clear_ubo()
  { _pdata->ubo.clear(); }

  void PlotLine::clear()
  {
      clear_shader();
      clear_vbo_vao();
      clear_ubo();

      this->set_initialized(false);
  }

  void PlotLine::draw_impl()
  {
      _pdata->ubo.bind_to_base(2);

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
      BK_QT_GL glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, _pdata->primitiveCount);
      _pdata->shader.release();
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


