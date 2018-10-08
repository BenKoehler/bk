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

#include "AreaPlotView.h"

#include <algorithm>
#include <string>
#include <vector>

#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/UBOAreaPlot.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/gl_definitions.h>
#include <bk/Color>
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AreaPlotView::Impl
  {
      // gl
      // area graph
      VBO vbo_area;
      IBO ibo_area;
      VAO vao_area;
      Shader shader_area;
      // line0 (lower line)
      VBO vbo_line0;
      VAO vao_line0;
      // line0 (lower line) stdev bars
      VBO vbo_line0_stdev;
      VAO vao_line0_stdev;
      // line1 (upper line)
      VBO vbo_line1;
      VAO vao_line1;
      // line1 (lower line) stdev bars
      VBO vbo_line1_stdev;
      VAO vao_line1_stdev;
      Shader shader_line;
      Shader shader_stdev;
      details::UBOAreaPlot ubo;
      // axes
      VBO vbo_axis_x;
      VBO vbo_axis_y;
      VAO vao_axis_x;
      VAO vao_axis_y;
      Shader shader_axis;
      // ticks
      VBO vbo_ticks_x;
      VBO vbo_ticks_y;
      IBO ibo_ticks_x;
      IBO ibo_ticks_y;
      VAO vao_ticks_x;
      VAO vao_ticks_y;
      Shader shader_ticks;
      // gl2
      GLuint sizeInd_area;
      // plot
      std::string xlabel;
      std::string ylabel;
      std::vector<GLfloat> xvalues;
      std::vector<GLfloat> yvalues0;
      std::vector<GLfloat> ystdev0;
      std::vector<GLfloat> yvalues1;
      std::vector<GLfloat> ystdev1;
      GLfloat xmin;
      GLfloat xmax;
      GLfloat ymin;
      GLfloat ymax;
      GLuint numxticks;
      GLuint numyticks;
      ColorRGBA fillcolor_above;
      ColorRGBA fillcolor_below;
      ColorRGBA line0color;
      ColorRGBA line0stdevcolor;
      ColorRGBA line1color;
      ColorRGBA line1stdevcolor;
      // vis params
      GLfloat border_width_horizontal_in_percent;
      GLfloat border_width_vertical_in_percent;
      GLfloat line_width;


      // todo: signals - {x]changed

          #ifdef BK_LIB_QT_AVAILABLE

      Impl(bk::qt_gl_functions* gl)
          : vbo_area(gl),
            ibo_area(gl),
            vao_area(gl),
            shader_area(gl),
            vbo_line0(gl),
            vao_line0(gl),
            vbo_line0_stdev(gl),
            vao_line0_stdev(gl),
            vbo_line1(gl),
            vao_line1(gl),
            vbo_line1_stdev(gl),
            vao_line1_stdev(gl),
            shader_line(gl),
            shader_stdev(gl),
            ubo(gl),
            vbo_axis_x(gl),
            vbo_axis_y(gl),
            vao_axis_x(gl),
            vao_axis_y(gl),
            shader_axis(gl),
            vbo_ticks_x(gl),
            vbo_ticks_y(gl),
            ibo_ticks_x(gl),
            ibo_ticks_y(gl),
            vao_ticks_x(gl),
            vao_ticks_y(gl),
            shader_ticks(gl),
          #else

      Impl()
          :
          #endif
          sizeInd_area(0),
          xlabel("x"),
          ylabel("y"),
          xmin(0),
          xmax(1),
          ymin(0),
          ymax(1),
          numxticks(5),
          numyticks(5),
          fillcolor_above(ColorRGBA::Green()),
          fillcolor_below(ColorRGBA::Red()),
          line0color(ColorRGBA::Black()),
          line0stdevcolor(ColorRGBA::Orange()),
          line1color(line0color),
          line1stdevcolor(line0stdevcolor),
          border_width_horizontal_in_percent(0.075),
          border_width_vertical_in_percent(0.075),
          line_width(2)
      {
          fillcolor_above[3] = 0.5;
          fillcolor_below[3] = 0.5;
      }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  AreaPlotView::AreaPlotView()
      : base_type()
  #else

  AreaPlotView::AreaPlotView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo_area.set_usage_STATIC_DRAW();
      _pdata->ibo_area.set_usage_STATIC_DRAW();
      _pdata->vao_area.add_default_attribute_position_2xfloat();
      _pdata->vao_area.add_default_attribute_scalar_1xfloat("diff");

      _pdata->vbo_line0.set_usage_STATIC_DRAW();
      _pdata->vao_line0.add_default_attribute_position_2xfloat();

      _pdata->vbo_line0_stdev.set_usage_STATIC_DRAW();
      _pdata->vao_line0_stdev.add_default_attribute_position_2xfloat();

      _pdata->vbo_line1.set_usage_STATIC_DRAW();
      _pdata->vao_line1.add_default_attribute_position_2xfloat();

      _pdata->vbo_line1_stdev.set_usage_STATIC_DRAW();
      _pdata->vao_line1_stdev.add_default_attribute_position_2xfloat();

      _pdata->vbo_axis_x.set_usage_STATIC_DRAW();
      _pdata->vao_axis_x.add_default_attribute_position_2xfloat();
      _pdata->vbo_axis_y.set_usage_STATIC_DRAW();
      _pdata->vao_axis_y.add_default_attribute_position_2xfloat();

      _pdata->vbo_ticks_x.set_usage_STATIC_DRAW();
      _pdata->ibo_ticks_x.set_usage_STATIC_DRAW();
      _pdata->vao_ticks_x.add_default_attribute_position_2xfloat();
      _pdata->vbo_ticks_y.set_usage_STATIC_DRAW();
      _pdata->ibo_ticks_y.set_usage_STATIC_DRAW();
      _pdata->vao_ticks_y.add_default_attribute_position_2xfloat();
  }

  AreaPlotView::AreaPlotView(AreaPlotView&&) noexcept = default;

  AreaPlotView::~AreaPlotView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  const std::string& AreaPlotView::xlabel() const
  { return _pdata->xlabel; }

  const std::string& AreaPlotView::ylabel() const
  { return _pdata->ylabel; }

  const std::vector<GLfloat>& AreaPlotView::xvalues() const
  { return _pdata->xvalues; }

  const std::vector<GLfloat>& AreaPlotView::yvalues0() const
  { return _pdata->yvalues0; }

  const std::vector<GLfloat>& AreaPlotView::ystdev0() const
  { return _pdata->ystdev0; }

  const std::vector<GLfloat>& AreaPlotView::yvalues1() const
  { return _pdata->yvalues1; }

  const std::vector<GLfloat>& AreaPlotView::ystdev1() const
  { return _pdata->ystdev1; }

  GLfloat AreaPlotView::xmin() const
  { return _pdata->xmin; }

  GLfloat AreaPlotView::xmax() const
  { return _pdata->xmax; }

  GLfloat AreaPlotView::ymin() const
  { return _pdata->ymin; }

  GLfloat AreaPlotView::ymax() const
  { return _pdata->ymax; }

  GLuint AreaPlotView::numxticks() const
  { return _pdata->numxticks; }

  GLuint AreaPlotView::numyticks() const
  { return _pdata->numyticks; }

  const ColorRGBA& AreaPlotView::fillcolor_above() const
  { return _pdata->fillcolor_above; }

  const ColorRGBA& AreaPlotView::fillcolor_below() const
  { return _pdata->fillcolor_below; }

  GLfloat AreaPlotView::border_width_horizontal_in_percent() const
  { return _pdata->border_width_horizontal_in_percent; }

  GLfloat AreaPlotView::border_width_vertical_in_percent() const
  { return _pdata->border_width_vertical_in_percent; }

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool AreaPlotView::is_initialized() const
  { return _pdata->vao_area.is_initialized() && _pdata->shader_line.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void AreaPlotView::set_xlabel(const std::string& xlab)
  { _pdata->xlabel = xlab; }

  void AreaPlotView::set_ylabel(const std::string& ylab)
  { _pdata->ylabel = ylab; }

  void AreaPlotView::set_xvalues(const std::vector<GLfloat>& xv)
  { _pdata->xvalues = xv; }

  void AreaPlotView::set_yvalues0(const std::vector<GLfloat>& yv)
  { _pdata->yvalues0 = yv; }

  void AreaPlotView::set_ystdev0(const std::vector<GLfloat>& ystd)
  { _pdata->ystdev0 = ystd; }

  void AreaPlotView::set_yvalues1(const std::vector<GLfloat>& yv)
  { _pdata->yvalues1 = yv; }

  void AreaPlotView::set_ystdev1(const std::vector<GLfloat>& ystd)
  { _pdata->ystdev1 = ystd; }

  void AreaPlotView::set_xmin(GLfloat x)
  {
      _pdata->xmin = x;

      if (this->is_initialized())
      {
          _pdata->ubo.set_xmin(_pdata->xmin);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_xmax(GLfloat x)
  {
      _pdata->xmax = x;

      if (this->is_initialized())
      {
          _pdata->ubo.set_xmax(_pdata->xmax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_xmin_xmax(GLfloat x0, GLfloat x1)
  {
      _pdata->xmin = std::min(x0, x1);
      _pdata->xmax = std::max(x0, x1);

      if (this->is_initialized())
      {
          _pdata->ubo.set_xmin(_pdata->xmin);
          _pdata->ubo.set_xmax(_pdata->xmax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_ymin(GLfloat y)
  {
      _pdata->ymin = y;

      if (this->is_initialized())
      {
          _pdata->ubo.set_ymin(_pdata->ymin);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_ymax(GLfloat y)
  {
      _pdata->ymax = y;

      if (this->is_initialized())
      {
          _pdata->ubo.set_ymax(_pdata->ymax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_minmax_from_data_values()
  {
      const auto xminmax = std::minmax_element(_pdata->xvalues.begin(), _pdata->xvalues.end());
      set_xmin_xmax(*xminmax.first, *xminmax.second);

      double ymin = std::numeric_limits<double>::max();
      double ymax = -ymin;
      auto compare_ymin_ymax = [&](double vmin, double vmax)
      {
          ymin = std::min(ymin, vmin);
          ymax = std::max(ymax, vmax);
      };
      const bool hasystdev0 = _pdata->ystdev0.size() == _pdata->yvalues0.size();
      const bool hasystdev1 = _pdata->ystdev1.size() == _pdata->yvalues1.size();

      if (!_pdata->yvalues0.empty())
      {
          if (hasystdev0)
          {
              for (unsigned int i = 0; i < _pdata->yvalues0.size(); ++i)
              {
                  const double v = _pdata->yvalues0[i];
                  const double stdev = _pdata->ystdev0[i];

                  compare_ymin_ymax(v - stdev, v + stdev);
              }
          }
          else
          {
              const auto yminmax = std::minmax_element(_pdata->yvalues0.begin(), _pdata->yvalues0.end());
              compare_ymin_ymax(*yminmax.first, *yminmax.second);
          }
      }

      if (!_pdata->yvalues1.empty())
      {
          if (hasystdev1)
          {
              for (unsigned int i = 0; i < _pdata->yvalues1.size(); ++i)
              {
                  const double v = _pdata->yvalues1[i];
                  const double stdev = _pdata->ystdev1[i];

                  compare_ymin_ymax(v - stdev, v + stdev);
              }
          }
          else
          {
              const auto yminmax = std::minmax_element(_pdata->yvalues1.begin(), _pdata->yvalues1.end());
              compare_ymin_ymax(*yminmax.first, *yminmax.second);
          }
      }

      set_ymin_ymax(ymin, ymax);
  }

  void AreaPlotView::set_ymin_ymax(GLfloat y0, GLfloat y1)
  {
      _pdata->ymin = std::min(y0, y1);
      _pdata->ymax = std::max(y0, y1);

      if (this->is_initialized())
      {
          _pdata->ubo.set_ymin(_pdata->ymin);
          _pdata->ubo.set_ymax(_pdata->ymax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_numxticks(GLuint n)
  { _pdata->numxticks = n; }

  void AreaPlotView::set_numyticks(GLuint n)
  { _pdata->numyticks = n; }

  void AreaPlotView::set_fillcolor_above(const ColorRGBA& c)
  {
      _pdata->fillcolor_above = c;

      if (this->is_initialized())
      {
          _pdata->ubo.set_areaColorAbove_r(_pdata->fillcolor_above[0]);
          _pdata->ubo.set_areaColorAbove_g(_pdata->fillcolor_above[1]);
          _pdata->ubo.set_areaColorAbove_b(_pdata->fillcolor_above[2]);
          _pdata->ubo.set_areaColorAbove_a(_pdata->fillcolor_above[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_fillcolor_below(const ColorRGBA& c)
  {
      _pdata->fillcolor_below = c;

      if (this->is_initialized())
      {
          _pdata->ubo.set_areaColorBelow_r(_pdata->fillcolor_below[0]);
          _pdata->ubo.set_areaColorBelow_g(_pdata->fillcolor_below[1]);
          _pdata->ubo.set_areaColorBelow_b(_pdata->fillcolor_below[2]);
          _pdata->ubo.set_areaColorBelow_a(_pdata->fillcolor_below[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_border_width_horizontal_in_percent(GLfloat b)
  {
      _pdata->border_width_horizontal_in_percent = std::max(static_cast<GLfloat>(0), std::min(static_cast<GLfloat>(1), b));

      if (this->is_initialized())
      {
          _pdata->ubo.set_border_width_horizontal_in_percent(_pdata->border_width_horizontal_in_percent);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_border_width_vertical_in_percent(GLfloat b)
  {
      _pdata->border_width_vertical_in_percent = std::max(static_cast<GLfloat>(0), std::min(static_cast<GLfloat>(1), b));

      if (this->is_initialized())
      {
          _pdata->ubo.set_border_width_vertical_in_percent(_pdata->border_width_vertical_in_percent);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::set_area_alpha(GLfloat a)
  {
      _pdata->fillcolor_above[3] = std::max(std::min(a, static_cast<GLfloat>(1)), static_cast<GLfloat>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.set_areaColorAbove_a(_pdata->fillcolor_above[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  AreaPlotView& AreaPlotView::operator=(AreaPlotView&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void AreaPlotView::clear_shader()
  {
      _pdata->shader_area.clear();
      _pdata->shader_line.clear();
      _pdata->shader_stdev.clear();
      _pdata->shader_axis.clear();
      _pdata->shader_ticks.clear();
  }

  void AreaPlotView::clear_ubo()
  { _pdata->ubo.clear(); }

  void AreaPlotView::clear_vbo_vao()
  {
      _pdata->vbo_area.clear();
      _pdata->ibo_area.clear();
      _pdata->vao_area.clear();

      _pdata->vbo_line0.clear();
      _pdata->vao_line0.clear();

      _pdata->vbo_line0_stdev.clear();
      _pdata->vao_line0_stdev.clear();

      _pdata->vbo_line1.clear();
      _pdata->vao_line1.clear();

      _pdata->vbo_line1_stdev.clear();
      _pdata->vao_line1_stdev.clear();

      _pdata->vbo_axis_x.clear();
      _pdata->vao_axis_x.clear();
      _pdata->vbo_axis_y.clear();
      _pdata->vao_axis_y.clear();

      _pdata->vbo_ticks_x.clear();
      _pdata->ibo_ticks_x.clear();
      _pdata->vao_ticks_x.clear();
      _pdata->vbo_ticks_y.clear();
      _pdata->ibo_ticks_y.clear();
      _pdata->vao_ticks_y.clear();
  }

  void AreaPlotView::clear()
  {
      clear_shader();
      clear_vbo_vao();
      clear_ubo();
  }

  void AreaPlotView::init_shader()
  {
      using SL = details::ShaderLibrary::plot;

      // todo
      //_pdata->shader_area.init_from_sources(SL::area::vert(),SL::area::frag());
      //_pdata->shader_line.init_from_sources(SL::line::vert(),SL::line::frag(),SL::line::geom());
      //_pdata->shader_stdev.init_from_sources(SL::stdev::vert(),SL::stdev::frag(),SL::stdev::geom());
      //_pdata->shader_axis.init_from_sources("shader/plot/axis.vert", "shader/plot/axis.frag", "shader/plot/axis.geom");
      //_pdata->shader_ticks.init_from_sources("shader/plot/ticks.vert", "shader/plot/ticks.frag", "shader/plot/ticks.geom");
      
      _pdata->shader_area.init("shader/plot/area_plot.vert", "shader/plot/area_plot.frag");
      _pdata->shader_line.init("shader/plot/plotline.vert", "shader/plot/plotline.frag", "shader/plot/plotline.geom");
      _pdata->shader_stdev.init("shader/plot/stdev.vert", "shader/plot/stdev.frag", "shader/plot/stdev.geom");
      _pdata->shader_axis.init("shader/plot/axis.vert", "shader/plot/axis.frag", "shader/plot/axis.geom");
      _pdata->shader_ticks.init("shader/plot/ticks.vert", "shader/plot/ticks.frag", "shader/plot/ticks.geom");
  }

  void AreaPlotView::init_vbo_vao()
  {
      clear_vbo_vao();

      const unsigned int N = _pdata->xvalues.size();

      if (_pdata->yvalues0.size() != N || _pdata->yvalues1.size() != N)
      { throw; }

      /*
       * lines
       */
      std::vector<GLfloat> vert_line0;
      vert_line0.reserve((N + 2) * 2);

      std::vector<GLfloat> vert_line1;
      vert_line1.reserve((N + 2) * 2);

      // insert first and last point twice for line adjacency
      vert_line0.push_back(_pdata->xvalues[0]);
      vert_line0.push_back(_pdata->yvalues0[0]);
      vert_line1.push_back(_pdata->xvalues[0]);
      vert_line1.push_back(_pdata->yvalues1[0]);

      for (unsigned int i = 0; i < N; ++i)
      {
          vert_line0.push_back(_pdata->xvalues[i]);
          vert_line0.push_back(_pdata->yvalues0[i]);
          vert_line1.push_back(_pdata->xvalues[i]);
          vert_line1.push_back(_pdata->yvalues1[i]);
      }

      // insert first and last point twice for line adjacency
      vert_line0.push_back(_pdata->xvalues[N - 1]);
      vert_line0.push_back(_pdata->yvalues0[N - 1]);
      vert_line1.push_back(_pdata->xvalues[N - 1]);
      vert_line1.push_back(_pdata->yvalues1[N - 1]);

      _pdata->vbo_line0.init(vert_line0);
      _pdata->vao_line0.init(_pdata->vbo_line0);

      _pdata->vbo_line1.init(vert_line1);
      _pdata->vao_line1.init(_pdata->vbo_line1);

      /*
       * stdev bars
       */
      // line 0
      if (_pdata->ystdev0.size() == _pdata->yvalues0.size())
      {
          std::vector<GLfloat> vert_line0_stdev;
          vert_line0.reserve(N * 4);

          for (unsigned int i = 0; i < N; ++i)
          {
              vert_line0_stdev.push_back(_pdata->xvalues[i]);
              vert_line0_stdev.push_back(_pdata->yvalues0[i] - _pdata->ystdev0[i]);

              vert_line0_stdev.push_back(_pdata->xvalues[i]);
              vert_line0_stdev.push_back(_pdata->yvalues0[i] + _pdata->ystdev0[i]);
          }

          _pdata->vbo_line0_stdev.init(vert_line0_stdev);
          _pdata->vao_line0_stdev.init(_pdata->vbo_line0_stdev);
      }

      // line 1
      if (_pdata->ystdev1.size() == _pdata->yvalues1.size())
      {
          std::vector<GLfloat> vert_line1_stdev;
          vert_line1.reserve(N * 4);

          for (unsigned int i = 0; i < N; ++i)
          {
              vert_line1_stdev.push_back(_pdata->xvalues[i]);
              vert_line1_stdev.push_back(_pdata->yvalues1[i] - _pdata->ystdev1[i]);

              vert_line1_stdev.push_back(_pdata->xvalues[i]);
              vert_line1_stdev.push_back(_pdata->yvalues1[i] + _pdata->ystdev1[i]);
          }

          _pdata->vbo_line1_stdev.init(vert_line1_stdev);
          _pdata->vao_line1_stdev.init(_pdata->vbo_line1_stdev);
      }

      /*
       * filled graph area
       */
      std::vector<GLfloat> vert_area;
      vert_area.reserve(N * 2 * 2);

      std::vector<GLuint> indices_area;
      indices_area.reserve(N * 4 * 3);

      GLfloat xintersection = 0;
      GLfloat yintersection = 0;

      // first point
      vert_area.push_back(_pdata->xvalues[0]);
      vert_area.push_back(_pdata->yvalues0[0]);
      vert_area.push_back(_pdata->yvalues1[0] - _pdata->yvalues0[0]);
      vert_area.push_back(_pdata->xvalues[0]);
      vert_area.push_back(_pdata->yvalues1[0]);
      vert_area.push_back(_pdata->yvalues1[0] - _pdata->yvalues0[0]);

      GLuint cnt = 2;

      for (unsigned int i = 1; i < N; ++i)
      {
          const bool consistent_order = (_pdata->yvalues0[i - 1] < _pdata->yvalues1[i - 1] && _pdata->yvalues0[i] < _pdata->yvalues1[i]) || (_pdata->yvalues0[i - 1] >= _pdata->yvalues1[i - 1] && _pdata->yvalues0[i] >= _pdata->yvalues1[i]);

          if (consistent_order)
          {
              vert_area.push_back(_pdata->xvalues[i]);
              vert_area.push_back(_pdata->yvalues0[i]);
              vert_area.push_back(_pdata->yvalues1[i] - _pdata->yvalues0[i]);

              indices_area.push_back(cnt - 2);
              indices_area.push_back(cnt - 1);
              indices_area.push_back(cnt++);

              vert_area.push_back(_pdata->xvalues[i]);
              vert_area.push_back(_pdata->yvalues1[i]);
              vert_area.push_back(_pdata->yvalues1[i] - _pdata->yvalues0[i]);

              indices_area.push_back(cnt - 2);
              indices_area.push_back(cnt - 1);
              indices_area.push_back(cnt++);
          }
          else
          {
              const GLfloat dx = _pdata->xvalues[i] - _pdata->xvalues[i - 1];
              const GLfloat m0 = (_pdata->yvalues0[i] - _pdata->yvalues0[i - 1]) / dx;
              const GLfloat m1 = (_pdata->yvalues1[i] - _pdata->yvalues1[i - 1]) / dx;
              xintersection = (_pdata->yvalues1[i - 1] - _pdata->yvalues0[i - 1]) / (m0 - m1);
              yintersection = m0 * xintersection + _pdata->yvalues0[i - 1];

              vert_area.push_back(_pdata->xvalues[i - 1] + xintersection);
              vert_area.push_back(yintersection);
              vert_area.push_back(yintersection - _pdata->yvalues1[i]);

              indices_area.push_back(cnt - 2);
              indices_area.push_back(cnt - 1);
              indices_area.push_back(cnt++);

              vert_area.push_back(_pdata->xvalues[i]);
              vert_area.push_back(_pdata->yvalues0[i]);
              vert_area.push_back(_pdata->yvalues1[i] - _pdata->yvalues0[i]);
              vert_area.push_back(_pdata->xvalues[i]);
              vert_area.push_back(_pdata->yvalues1[i]);
              vert_area.push_back(_pdata->yvalues1[i] - _pdata->yvalues0[i]);

              indices_area.push_back(cnt - 1);
              indices_area.push_back(cnt++);
              indices_area.push_back(cnt++);
          }
      }

      _pdata->sizeInd_area = indices_area.size();

      _pdata->vbo_area.init(vert_area);
      _pdata->ibo_area.init(indices_area);
      _pdata->vao_area.init(_pdata->vbo_area, _pdata->ibo_area);

      /*
       * x axis
       */
      std::vector<GLfloat> vert_axis_x;
      vert_axis_x.reserve(4);

      vert_axis_x.push_back(_pdata->xmin);
      vert_axis_x.push_back(_pdata->ymin);
      vert_axis_x.push_back(_pdata->xmax);
      vert_axis_x.push_back(_pdata->ymin);

      _pdata->vbo_axis_x.init(vert_axis_x);
      _pdata->vao_axis_x.init(_pdata->vbo_axis_x);

      /*
       * y axis
       */
      std::vector<GLfloat> vert_axis_y;
      vert_axis_y.reserve(4);

      vert_axis_y.push_back(_pdata->xmin);
      vert_axis_y.push_back(_pdata->ymin);
      vert_axis_y.push_back(_pdata->xmin);
      vert_axis_y.push_back(_pdata->ymax);

      _pdata->vbo_axis_y.init(vert_axis_y);
      _pdata->vao_axis_y.init(_pdata->vbo_axis_y);

      /*
       * x ticks
       */
      const auto xTickValueDelta = (_pdata->xmax - _pdata->xmin) / (_pdata->numxticks + 1);
      auto xval = _pdata->xmin;
      std::vector<GLfloat> vert_ticks_x;
      vert_ticks_x.reserve(2 * 2 * _pdata->numxticks);

      std::vector<GLuint> indices_ticks_x;
      indices_ticks_x.reserve(2 * _pdata->numxticks);
      cnt = 0;

      for (unsigned int i = 0; i < _pdata->numxticks + 1; ++i)
      {
          xval += xTickValueDelta;

          vert_ticks_x.push_back(xval);
          vert_ticks_x.push_back(_pdata->ymin);
          indices_ticks_x.push_back(cnt++);

          vert_ticks_x.push_back(xval);
          vert_ticks_x.push_back(_pdata->ymax);
          indices_ticks_x.push_back(cnt++);

          indices_ticks_x.push_back(std::numeric_limits<GLuint>::max());
      }

      _pdata->vbo_ticks_x.init(vert_ticks_x);
      _pdata->ibo_ticks_x.init(indices_ticks_x);
      _pdata->vao_ticks_x.init(_pdata->vbo_ticks_x, _pdata->ibo_ticks_x);

      /*
       * y ticks
       */
      const auto yTickValueDelta = (_pdata->ymax - _pdata->ymin) / (_pdata->numyticks + 1);
      auto yval = _pdata->ymin;
      std::vector<GLfloat> vert_ticks_y;
      vert_ticks_y.reserve(2 * 2 * (_pdata->numyticks + 1));

      std::vector<GLuint> indices_ticks_y;
      indices_ticks_y.reserve(2 * (_pdata->numyticks + 1));
      cnt = 0;

      for (unsigned int i = 0; i < _pdata->numyticks + 1; ++i)
      {
          yval += yTickValueDelta;

          vert_ticks_y.push_back(_pdata->xmin);
          vert_ticks_y.push_back(yval);
          indices_ticks_y.push_back(cnt++);

          vert_ticks_y.push_back(_pdata->xmax);
          vert_ticks_y.push_back(yval);
          indices_ticks_y.push_back(cnt++);

          indices_ticks_y.push_back(std::numeric_limits<GLuint>::max());
      }

      _pdata->vbo_ticks_y.init(vert_ticks_y);
      _pdata->ibo_ticks_y.init(indices_ticks_y);
      _pdata->vao_ticks_y.init(_pdata->vbo_ticks_y, _pdata->ibo_ticks_y);
  }

  void AreaPlotView::init_ubo()
  {
      _pdata->ubo.init_from_registered_values_size();
      _pdata->ubo.set_areaColorAbove_r(_pdata->fillcolor_above[0]);
      _pdata->ubo.set_areaColorAbove_g(_pdata->fillcolor_above[1]);
      _pdata->ubo.set_areaColorAbove_b(_pdata->fillcolor_above[2]);
      _pdata->ubo.set_areaColorAbove_a(_pdata->fillcolor_above[3]);
      _pdata->ubo.set_areaColorBelow_r(_pdata->fillcolor_below[0]);
      _pdata->ubo.set_areaColorBelow_g(_pdata->fillcolor_below[1]);
      _pdata->ubo.set_areaColorBelow_b(_pdata->fillcolor_below[2]);
      _pdata->ubo.set_areaColorBelow_a(_pdata->fillcolor_below[3]);
      _pdata->ubo.set_line_width(_pdata->line_width);
      _pdata->ubo.set_linecolorr(_pdata->line0color[0]);
      _pdata->ubo.set_linecolorg(_pdata->line0color[1]);
      _pdata->ubo.set_linecolorb(_pdata->line0color[2]);
      _pdata->ubo.set_linecolora(_pdata->line0color[3]);
      _pdata->ubo.set_stdevcolorr(_pdata->line0stdevcolor[0]);
      _pdata->ubo.set_stdevcolorg(_pdata->line0stdevcolor[1]);
      _pdata->ubo.set_stdevcolorb(_pdata->line0stdevcolor[2]);
      _pdata->ubo.set_stdevcolora(_pdata->line0stdevcolor[3]);
      _pdata->ubo.set_xmin(_pdata->xmin);
      _pdata->ubo.set_xmax(_pdata->xmax);
      _pdata->ubo.set_ymin(_pdata->ymin);
      _pdata->ubo.set_ymax(_pdata->ymax);
      _pdata->ubo.set_border_width_horizontal_in_percent(_pdata->border_width_horizontal_in_percent);
      _pdata->ubo.set_border_width_vertical_in_percent(_pdata->border_width_vertical_in_percent);
      _pdata->ubo.release();
  }

  void AreaPlotView::init()
  {
      clear();

      init_vbo_vao();
      init_ubo();
      init_shader();

      this->emit_signal_update_required();
  }

  void AreaPlotView::on_resize(GLint /*w*/, GLint /*h*/)
  {
      if (this->is_initialized())
      {
          init_vbo_vao();
          this->emit_signal_update_required();
      }
  }

  void AreaPlotView::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void AreaPlotView::on_animation_enabled(bool /*b*/)
  {
      // todo line indicates current time
  }

  void AreaPlotView::on_modelview_changed(bool)
  { /* do nothing */ }

  void AreaPlotView::on_visible_changed(bool)
  { /* do nothing */ }

  void AreaPlotView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void AreaPlotView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void AreaPlotView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void AreaPlotView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void AreaPlotView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void AreaPlotView::on_mouse_wheel_up()
  { /* do nothing */ }

  void AreaPlotView::on_mouse_wheel_down()
  { /* do nothing */ }

  void AreaPlotView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  void AreaPlotView::draw_impl()
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
       * draw ticks
       */
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->shader_ticks.bind();

      _pdata->vao_ticks_x.bind();
      BK_QT_GL glDrawElements(GL_LINE_STRIP, 3 * (_pdata->numxticks + 1), GL_UNSIGNED_INT, nullptr);
      _pdata->vao_ticks_x.release();

      _pdata->vao_ticks_y.bind();
      BK_QT_GL glDrawElements(GL_LINE_STRIP, 3 * (_pdata->numyticks + 1), GL_UNSIGNED_INT, nullptr);
      _pdata->vao_ticks_y.release();

      _pdata->shader_ticks.release();

      BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);

      /*
       * draw area
       */
      _pdata->vao_area.bind();
      _pdata->shader_area.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_area, GL_UNSIGNED_INT, nullptr);
      _pdata->shader_area.release();
      _pdata->vao_area.release();

      /*
       * draw stdev
       */
      if (_pdata->ystdev0.size() == _pdata->yvalues0.size())
      {
          _pdata->vao_line0_stdev.bind();
          _pdata->shader_stdev.bind();
          BK_QT_GL glDrawArrays(GL_LINES, 0, 2 * _pdata->xvalues.size());
          _pdata->shader_stdev.release();
          _pdata->vao_line0_stdev.release();
      }

      if (_pdata->ystdev1.size() == _pdata->yvalues1.size())
      {
          _pdata->vao_line1_stdev.bind();
          _pdata->shader_stdev.bind();
          BK_QT_GL glDrawArrays(GL_LINES, 0, 2 * _pdata->xvalues.size());
          _pdata->shader_stdev.release();
          _pdata->vao_line1_stdev.release();
      }

      /*
       * draw line 0
       */
      _pdata->ubo.set_linecolorr(_pdata->line0color[0]);
      _pdata->ubo.set_linecolorg(_pdata->line0color[1]);
      _pdata->ubo.set_linecolorb(_pdata->line0color[2]);
      _pdata->ubo.set_linecolora(_pdata->line0color[3]);
      _pdata->ubo.release();

      _pdata->vao_line0.bind();
      _pdata->shader_line.bind();
      BK_QT_GL glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 2 + _pdata->xvalues.size());
      _pdata->shader_line.release();
      _pdata->vao_line0.release();

      /*
       * draw line 1
       */
      _pdata->ubo.set_linecolorr(_pdata->line1color[0]);
      _pdata->ubo.set_linecolorg(_pdata->line1color[1]);
      _pdata->ubo.set_linecolorb(_pdata->line1color[2]);
      _pdata->ubo.set_linecolora(_pdata->line1color[3]);
      _pdata->ubo.release();

      _pdata->vao_line1.bind();
      _pdata->shader_line.bind();
      BK_QT_GL glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, 2 + _pdata->xvalues.size());
      _pdata->shader_line.release();
      _pdata->vao_line1.release();

      /*
       * draw axes
       */
      _pdata->shader_axis.bind();

      _pdata->vao_axis_x.bind();
      BK_QT_GL glDrawArrays(GL_LINE_STRIP, 0, 2);
      _pdata->vao_axis_x.release();

      _pdata->vao_axis_y.bind();
      BK_QT_GL glDrawArrays(GL_LINE_STRIP, 0, 2);
      _pdata->vao_axis_y.release();

      _pdata->shader_axis.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();
      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();
  }
} // namespace bk



