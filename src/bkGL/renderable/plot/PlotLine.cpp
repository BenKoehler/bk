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

#include <bkGL/renderable/plot/PlotLine.h>

#include <algorithm>
#include <limits>

#include <bkGL/renderable/plot/PlotDataVector.h>
#include <bkTools/color/ColorRGBA.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/UBOPlotLine.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotLine::Impl
  {
      VAO vao;
      VBO vbo;
      details::UBOPlotLine ubo;
      Shader shader;
      PlotDataVector data;
      ColorRGBA color;
      GLfloat lineWidth;
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
          color(1, 0.5, 0, 1),
          lineWidth(3),
          primitiveCount(0)
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

  PlotLine::PlotLine()
      : base_type()
  #else

  PlotLine::PlotLine(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();
  }

  PlotLine::PlotLine(PlotLine&&) noexcept = default;

  PlotLine::~PlotLine() = default;

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  const ColorRGBA& PlotLine::color() const
  { return _pdata->color; }

  GLfloat PlotLine::line_width() const
  { return _pdata->lineWidth; }

  const PlotDataVector& PlotLine::data_vector() const
  { return _pdata->data; }

  PlotDataVector& PlotLine::data_vector()
  { return _pdata->data; }

  GLfloat PlotLine::x_min() const
  {
      const auto& v = data_vector().x_value_vector();
      return v.empty() ? std::numeric_limits<GLfloat>::max() : *std::min_element(v.begin(), v.end());
  }

  GLfloat PlotLine::x_max() const
  {
      const auto& v = data_vector().x_value_vector();
      return v.empty() ? -std::numeric_limits<GLfloat>::max() : *std::max_element(v.begin(), v.end());
  }

  GLfloat PlotLine::y_min() const
  {
      const auto& v = data_vector().y_value_vector();
      return v.empty() ? std::numeric_limits<GLfloat>::max() : *std::min_element(v.begin(), v.end());
  }

  GLfloat PlotLine::y_max() const
  {
      const auto& v = data_vector().y_value_vector();
      return v.empty() ? -std::numeric_limits<GLfloat>::max() : *std::max_element(v.begin(), v.end());
  }

  bool PlotLine::is_initialized() const
  { return _pdata->vao.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotLine& PlotLine::operator=(PlotLine&&) noexcept = default;

  void PlotLine::set_color(const ColorRGBA& col)
  { set_color(col[0], col[1], col[2], col[3]); }

  void PlotLine::set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  { _pdata->color.set(r, g, b, a); }

  void PlotLine::set_line_width(GLfloat w)
  {
      _pdata->lineWidth = std::max(w, static_cast<GLfloat>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.set_line_width(_pdata->lineWidth);
          _pdata->ubo.release();
      }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotLine::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::plot::line;
      return _pdata->shader.init_from_sources(SL::vert(), SL::frag(), SL::geom(true));
  }

  bool PlotLine::init_vbo_vao()
  {
      clear_vbo_vao();

      const unsigned int N = _pdata->data.num_values();

      if (N == 0)
      { return false; }

      _pdata->primitiveCount = N + 2;

      const std::vector<GLfloat>& x = _pdata->data.x_value_vector();
      const std::vector<GLfloat>& y = _pdata->data.y_value_vector();
      std::vector<GLfloat> vert;
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

      _pdata->ubo.set_line_width(_pdata->lineWidth);
      _pdata->ubo.set_color_r(_pdata->color[0]);
      _pdata->ubo.set_color_g(_pdata->color[1]);
      _pdata->ubo.set_color_b(_pdata->color[2]);
      _pdata->ubo.set_color_a(_pdata->color[3]);
      _pdata->ubo.release();

      return true;
  }

  bool PlotLine::init()
  {
      clear();

      const bool success = init_shader() && init_ubo() && init_vbo_vao();

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
  }

  void PlotLine::draw_impl()
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


