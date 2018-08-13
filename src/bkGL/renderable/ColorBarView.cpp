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

#include <algorithm>
#include <sstream>

#include "../../../include/bkTools/Color"
#include "../../../include/bkMath/Matrix"
#include "../../../include/bkTools/Signal"

#include "../../../include/bkGL/renderable/ColorBarView.h"
#include "../../../include/bkGL/renderable/text/TextView.h"
#include "../../../include/bkGL/buffer/VBO.h"
#include "../../../include/bkGL/shader/Shader.h"
#include "../../../include/bkGL/shader/ShaderLibrary.h"
#include "../../../include/bkGL/vao/VAO.h"

namespace bk
{
  struct ColorBarView::Impl
  {
      VBO vbo;
      VAO vao;
      Shader shader;
      TextView textview_title;
      TextView textview_min;
      TextView textview_max;
      std::vector<TextView> textview_ticks;
      bool position_horizontal_right;
      unsigned int position_vertical;
      GLfloat min_value;
      GLfloat max_value;
      GLfloat min_clamp_value;
      GLfloat max_clamp_value;
      unsigned int num_colors;
      unsigned int num_ticks;
      unsigned int label_precision;
      inline static GLfloat width_in_percent = 0.2;
      inline static GLfloat height_in_percent = 0.066;
      inline static bk::ColorRGBA color_text = bk::ColorRGBA(1, 1, 1, 1);
      inline static bk::ColorRGBA color_text_background = bk::ColorRGBA(0, 0, 0, 0.5);
      inline static GLfloat scale_text = 0.66;
      inline static double boundary_width_in_screenCoords = 0.025;
      std::string title;
      bool linear_interpolation_enabled;
      bk::Signal<std::string> s_title_changed;
      bk::Signal<double, double> s_min_max_value_changed;
      bk::Signal<double, double> s_min_max_clamp_value_changed;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else
      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl),
            shader(gl),
            textview_title(gl),
            textview_min(gl),
            textview_max(gl),
          #endif
          position_horizontal_right(true),
          position_vertical(0),
          min_value(0),
          max_value(1),
          min_clamp_value(0),
          max_clamp_value(1),
          num_colors(0),
          num_ticks(2),
          label_precision(1),
          title(""),
          linear_interpolation_enabled(true)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  ColorBarView::ColorBarView()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  ColorBarView::ColorBarView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();

      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->vao.add_default_attribute_colorRGB_3xfloat();

      this->forward_signals(&_pdata->textview_title);
      this->forward_signals(&_pdata->textview_min);
      this->forward_signals(&_pdata->textview_max);

      set_num_ticks(_pdata->num_ticks); // create textviews for ticks
  }

  ColorBarView::ColorBarView(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ColorBarView::~ColorBarView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET POSITION
  bool ColorBarView::is_on_left_side() const
  { return !is_on_right_side(); }

  bool ColorBarView::is_on_right_side() const
  { return _pdata->position_horizontal_right; }

  unsigned int ColorBarView::position_vertical() const
  { return _pdata->position_vertical; }
  /// @}

  /// @{ -------------------------------------------------- GET RANGE
  GLfloat ColorBarView::min_value() const
  { return _pdata->min_value; }

  GLfloat ColorBarView::max_value() const
  { return _pdata->max_value; }
  /// @}

  /// @{ -------------------------------------------------- GET CLAMPING
  GLfloat ColorBarView::min_clamp_value() const
  { return _pdata->min_clamp_value; }

  GLfloat ColorBarView::max_clamp_value() const
  { return _pdata->max_value; }
  /// @}

  /// @{ -------------------------------------------------- GET NUM TICKS
  unsigned int ColorBarView::num_ticks() const
  { return _pdata->num_ticks; }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE PRECISION
  unsigned int ColorBarView::value_precision() const
  { return _pdata->label_precision; }
  /// @}

  /// @{ -------------------------------------------------- GET TITLE
  const std::string& ColorBarView::title() const
  { return _pdata->title; }
  /// @}

  /// @{ -------------------------------------------------- GET SIZE
  GLfloat ColorBarView::width_in_percent() const
  { return _pdata->width_in_percent; }

  GLfloat ColorBarView::height_in_percent() const
  { return _pdata->height_in_percent; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<std::string>& ColorBarView::signal_title_changed()
  { return _pdata->s_title_changed; }

  bk::Signal<double, double>& ColorBarView::signal_min_max_value_changed()
  { return _pdata->s_min_max_value_changed; }

  bk::Signal<double, double>& ColorBarView::signal_min_max_clamp_value_changed()
  { return _pdata->s_min_max_clamp_value_changed; }
  /// @}

  /// @{ -------------------------------------------------- DISCRETE MODE
  bool ColorBarView::linear_interpolation_is_enabled() const
  { return _pdata->linear_interpolation_enabled; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto ColorBarView::operator=(self_type&& other) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET POSITION
  void ColorBarView::set_position_horizontal_left()
  {
      if (_pdata->position_horizontal_right)
      {
          _pdata->position_horizontal_right = false;
          _update_vbo_positions();
      }
  }

  void ColorBarView::set_position_horizontal_right()
  {
      if (!_pdata->position_horizontal_right)
      {
          _pdata->position_horizontal_right = true;
          _update_vbo_positions();
      }
  }

  void ColorBarView::set_position_vertical(unsigned int posv)
  {
      if (_pdata->position_vertical != posv)
      {
          _pdata->position_vertical = posv;
          _update_vbo_positions();
      }
  }

  /// @}

  /// @{ -------------------------------------------------- SET RANGE
  void ColorBarView::set_value_range(GLfloat rmin, GLfloat rmax, bool copytoClampValueRange)
  {
      _pdata->min_value = std::min(rmin, rmax);
      _pdata->max_value = std::max(rmin, rmax);

      std::stringstream s;
      s << std::fixed;
      s.precision(_pdata->label_precision);
      s << rmin;

      _pdata->textview_min.set_text(s.str());

      s.str("");
      s << std::fixed;
      s.precision(_pdata->label_precision);
      s << rmax;
      _pdata->textview_max.set_text(s.str());

      if (copytoClampValueRange)
      { set_clamp_value_range(_pdata->min_value, _pdata->max_value); }
      else
      { _update_vbo_positions(); }

      _pdata->s_min_max_value_changed.emit_signal(_pdata->min_value, _pdata->max_value);
  }

  void ColorBarView::set_min_value(GLfloat rmin, bool copytoClampValueRange)
  { set_value_range(rmin, _pdata->max_value, copytoClampValueRange); }

  void ColorBarView::set_max_value(GLfloat rmax, bool copytoClampValueRange)
  { set_value_range(_pdata->min_value, rmax, copytoClampValueRange); }
  /// @}

  /// @{ -------------------------------------------------- SET CLAMPING
  void ColorBarView::set_clamp_value_range(GLfloat cmin, GLfloat cmax)
  {
      _pdata->min_clamp_value = std::min(cmin, cmax);
      _pdata->max_clamp_value = std::max(cmin, cmax);

      _update_vbo_positions();

      _pdata->s_min_max_clamp_value_changed.emit_signal(_pdata->min_clamp_value, _pdata->max_clamp_value);
  }

  void ColorBarView::set_min_clamp_value(GLfloat cmin)
  { set_clamp_value_range(cmin, _pdata->max_clamp_value); }

  void ColorBarView::set_max_clamp_value(GLfloat cmax)
  { set_clamp_value_range(_pdata->min_clamp_value, cmax); }
  /// @}

  /// @{ -------------------------------------------------- SET NUM TICKS
  void ColorBarView::set_num_ticks(unsigned int n)
  {
      _pdata->num_ticks = n;

      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].clear(); }

      _pdata->textview_ticks.clear();

      for (unsigned int i = 0; i < _pdata->num_ticks; ++i)
      {
          #ifndef BK_LIB_QT_AVAILABLE
          TextView tv;
          #else
          TextView tv(this->_gl);
          #endif

          tv.set_position_mode_relative();
          tv.set_color_text(_pdata->color_text.r(), _pdata->color_text.g(), _pdata->color_text.b());
          tv.set_color_background(_pdata->color_text_background.r(), _pdata->color_text_background.g(), _pdata->color_text_background.b(), _pdata->color_text_background.alpha());
          tv.set_scale(_pdata->scale_text);
          tv.init("0");

          _pdata->textview_ticks.emplace_back(std::move(tv));
      }

      if (this->is_initialized())
      { _update_vbo_positions(); }
  }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE PRECISION
  //! number of digits after comma
  void ColorBarView::set_value_precision(unsigned int nDigitsAfterComma)
  {
      _pdata->label_precision = nDigitsAfterComma;

      if (this->is_initialized())
      { _update_vbo_positions(); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET TITLE
  void ColorBarView::set_title(std::string_view t)
  {
      _pdata->title = t;
      _pdata->textview_title.set_text(_pdata->title);

      if (this->is_initialized())
      { _update_vbo_positions(); }

      _pdata->s_title_changed.emit_signal(_pdata->title);
  }
  /// @}

  /// @{ -------------------------------------------------- SET SIZE
  void ColorBarView::set_width_in_percent(GLfloat s)
  {
      _pdata->width_in_percent = std::max(std::min(s, static_cast<GLfloat>(1)), static_cast<GLfloat>(0));
      _update_vbo_positions();
  }

  void ColorBarView::set_height_in_percent(GLfloat h)
  {
      _pdata->height_in_percent = std::max(std::min(h, static_cast<GLfloat>(1)), static_cast<GLfloat>(0));
      _update_vbo_positions();
  }
  /// @}

  /// @{ -------------------------------------------------- LINEAR INTERPOLATION (DISCRETE MODE)
  void ColorBarView::set_linear_color_interpolation_enabled(bool b)
  { _pdata->linear_interpolation_enabled = b; }

  void ColorBarView::enable_linear_color_interpolation()
  { set_linear_color_interpolation_enabled(true); }

  void ColorBarView::disable_linear_color_interpolation()
  { set_linear_color_interpolation_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool ColorBarView::is_initialized() const
  { return _pdata->vao.is_initialized() && _pdata->shader.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void ColorBarView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }

  void ColorBarView::clear_shader()
  { _pdata->shader.clear(); }

  void ColorBarView::clear_text()
  {
      _pdata->textview_title.clear();
      _pdata->textview_min.clear();
      _pdata->textview_max.clear();

      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].clear(); }

      _pdata->textview_ticks.clear();
  }

  void ColorBarView::clear()
  {
      clear_buffers();
      clear_shader();
      clear_text();

      set_value_range(0, 1);
      set_clamp_value_range(0, 1);
      set_title("");
  }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: MIN MAX SCREEN POS
  std::tuple<double, double, double, double> ColorBarView::_minx_maxx_miny_maxy_screen_pos() const
  {
      const double width_percent_screenCoords = 2.0 * _pdata->width_in_percent;
      const double minx = _pdata->position_horizontal_right ? 1.0 - _pdata->boundary_width_in_screenCoords - width_percent_screenCoords : -1.0 + _pdata->boundary_width_in_screenCoords;
      const double maxx = minx + width_percent_screenCoords;

      constexpr double yOffScale = 2.5;
      const double miny = -1.0 + _pdata->boundary_width_in_screenCoords + _pdata->position_vertical * _pdata->height_in_percent * yOffScale;
      const double maxy = miny + _pdata->height_in_percent;

      return std::make_tuple(minx, maxx, miny, maxy);
  }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: UPDATE
  void ColorBarView::_update_vbo_positions()
  {
      if (!this->is_initialized())
      { return; }

      GLfloat* vbodata = _pdata->vbo.map_write_only<GLfloat>();
      if (vbodata != nullptr)
      {
          const auto[minx, maxx, miny, maxy] = _minx_maxx_miny_maxy_screen_pos();

          const double minx_clamp = minx + (maxx - minx) * (_pdata->min_clamp_value - _pdata->min_value) / (_pdata->max_value - _pdata->min_value);
          const double maxx_clamp = minx + (maxx - minx) * (_pdata->max_clamp_value - _pdata->min_value) / (_pdata->max_value - _pdata->min_value);

          unsigned int nValsVBO = 2 * (_pdata->num_colors + 2) * (2 + 3);
          if (!linear_interpolation_is_enabled())
          { nValsVBO *= 2; }

          constexpr unsigned int nValuesPerVertex = 5;

          if (linear_interpolation_is_enabled())
          {
              // leftmost bottom
              vbodata[0] = minx;
              vbodata[1] = miny;

              // leftmost top
              vbodata[nValuesPerVertex] = minx;
              vbodata[nValuesPerVertex + 1] = maxy;

              // rightmost bottom
              vbodata[nValsVBO - 2 * nValuesPerVertex] = maxx;
              vbodata[nValsVBO - 2 * nValuesPerVertex + 1] = miny;

              // rightmost top
              vbodata[nValsVBO - nValuesPerVertex] = maxx;
              vbodata[nValsVBO - nValuesPerVertex + 1] = maxy;

              const GLfloat delta = (maxx_clamp - minx_clamp) / (_pdata->num_colors - 1);

              for (unsigned int i = 0; i < _pdata->num_colors; ++i)
              {
                  // 2x floats per pos
                  // (i+2) -> two leftest verts offset
                  // nValuesPerVertex = 5 -> 2x pos + 3x col per vertex
                  const unsigned int off = 2 * (i + 1) * nValuesPerVertex;

                  const GLfloat x = minx_clamp + i * delta;

                  vbodata[0 * nValuesPerVertex + off] = x;
                  vbodata[0 * nValuesPerVertex + off + 1] = miny;

                  vbodata[1 * nValuesPerVertex + off] = x;
                  vbodata[1 * nValuesPerVertex + off + 1] = maxy;
              }
          }
          else // if (!linear_interpolation_is_enabled())
          {
              const GLfloat delta = (maxx - minx) / (_pdata->num_colors);

              // 4x floats per pos
              // (i+2) -> two leftest verts offset
              // (2+3) -> 2x pos + 3x col per vertex

              for (unsigned int i = 0; i < _pdata->num_colors; ++i)
              {
                  const unsigned int off = 4 * i * nValuesPerVertex;

                  // left bottom
                  vbodata[off + 0] = minx + i * delta;
                  vbodata[off + 1] = miny;

                  // left top
                  vbodata[off + nValuesPerVertex] = minx + i * delta;
                  vbodata[off + nValuesPerVertex + 1] = maxy;

                  // right top
                  vbodata[off + 2 * nValuesPerVertex] = minx + (i + 1) * delta;
                  vbodata[off + 2 * nValuesPerVertex + 1] = maxy;

                  // right bottom
                  vbodata[off + 3 * nValuesPerVertex] = minx + (i + 1) * delta;
                  vbodata[off + 3 * nValuesPerVertex + 1] = miny;
              }
          }

          _pdata->vbo.unmap_and_release();

          //------------------------------------------------------------------------------------------------------
          // text views
          //------------------------------------------------------------------------------------------------------
          constexpr auto screenTo01 = [](double x)
          { return 0.5 * x + 0.5; };

          const GLfloat texttitle_x = screenTo01(0.5 * (minx + maxx)) - 0.5 * _pdata->textview_title.text_pixel_width_relative();
          const GLfloat textmin_x = screenTo01(minx);
          const GLfloat textmax_x = screenTo01(maxx) - _pdata->textview_max.text_pixel_width();

          _pdata->textview_title.set_position(texttitle_x, screenTo01(maxy));
          _pdata->textview_min.set_position(textmin_x, screenTo01(miny) + 0.5 * _pdata->textview_min.text_pixel_height_relative());
          _pdata->textview_max.set_position(textmax_x, screenTo01(miny) + 0.5 * _pdata->textview_max.text_pixel_height_relative());

          GLfloat currentValue = _pdata->min_value;
          const GLfloat deltaValue = (_pdata->max_value - _pdata->min_value) / (_pdata->num_ticks + 1);
          GLfloat currentPosX = screenTo01(minx);
          const GLfloat deltaPosX = (screenTo01(maxx) - currentPosX) / (_pdata->num_ticks + 1);

          for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
          {
              currentValue += deltaValue;
              currentPosX += deltaPosX;

              std::stringstream s;
              s << std::fixed;
              s.precision(_pdata->label_precision);
              s << currentValue;

              _pdata->textview_ticks[i].set_text(s.str());

              _pdata->textview_ticks[i].set_position(currentPosX - 0.5 * _pdata->textview_ticks[i].text_pixel_width(), screenTo01(miny) + 0.5 * _pdata->textview_ticks[i].text_pixel_height_relative());
          }

          //------------------------------------------------------------------------------------------------------
          // update
          //------------------------------------------------------------------------------------------------------
          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void ColorBarView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::colorbar;

      if (linear_interpolation_is_enabled())
      { _pdata->shader.init_from_sources(SL::vert(), SL::frag()); }
      else
      { _pdata->shader.init_from_sources(SL::vert_discrete(), SL::frag_discrete()); }
  }

  void ColorBarView::_init(const std::vector<GLfloat>& r, const std::vector<GLfloat>& g, const std::vector<GLfloat>& b)
  {
      clear_buffers();

      if (r.size() != g.size() || r.size() != b.size())
      { return; }

      _pdata->num_colors = r.size();

      if (_pdata->num_colors <= 1)
      { return; }

      init_shader();

      std::vector<GLfloat> poscol;

      if (linear_interpolation_is_enabled())
      {
          poscol.reserve(2 * (_pdata->num_colors + 2) * (2 + 3));

          for (unsigned int i = 0; i < _pdata->num_colors; ++i)
          {
              if (i == 0 || i == _pdata->num_colors - 1)
              {
                  // extra entry at start/end for clamping

                  for (unsigned int k = 0; k < 2; ++k)
                  {
                      poscol.push_back(0);
                      poscol.push_back(0); // position dummy

                      poscol.push_back(r[i]);
                      poscol.push_back(g[i]);
                      poscol.push_back(b[i]);
                  }
              }

              for (unsigned int k = 0; k < 2; ++k)
              {
                  poscol.push_back(0);
                  poscol.push_back(0); // position dummy

                  poscol.push_back(r[i]);
                  poscol.push_back(g[i]);
                  poscol.push_back(b[i]);
              }
          }
      }
      else
      {
          poscol.reserve(4 * _pdata->num_colors * (2 + 3));

          for (unsigned int i = 0; i < _pdata->num_colors; ++i)
          {
              for (unsigned int k = 0; k < 4; ++k)
              {
                  poscol.push_back(0);
                  poscol.push_back(0); // position dummy

                  poscol.push_back(r[i]);
                  poscol.push_back(g[i]);
                  poscol.push_back(b[i]);
              }
          }
      }

      _pdata->vbo.init(poscol);
      _pdata->vao.init(_pdata->vbo);

      for (TextView* t: {&_pdata->textview_title, &_pdata->textview_min, &_pdata->textview_max})
      {
          t->set_position_mode_relative();
          t->set_color_text(_pdata->color_text.r(), _pdata->color_text.g(), _pdata->color_text.b());
          t->set_color_background(_pdata->color_text_background.r(), _pdata->color_text_background.g(), _pdata->color_text_background.b(), _pdata->color_text_background.alpha());
          t->set_scale(_pdata->scale_text);
      }

      _pdata->textview_title.init(_pdata->title);

      std::stringstream s;
      s << _pdata->min_value;
      _pdata->textview_min.init(s.str());

      s.str("");
      s << _pdata->max_value;
      _pdata->textview_max.init(s.str());

      _update_vbo_positions();
  }

  void ColorBarView::init(const ColorBarRGBA& cb)
  {
      const unsigned int N = cb.num_colors();
      init_manual_num_colors(cb, N);
  }

  void ColorBarView::init_manual_num_colors(const ColorBarRGBA& cb, unsigned int nColors)
  {
      std::vector<GLfloat> r;
      std::vector<GLfloat> g;
      std::vector<GLfloat> b;
      r.reserve(nColors);
      g.reserve(nColors);
      b.reserve(nColors);

      for (unsigned int i = 0; i < nColors; ++i)
      {
          const ColorRGBA& c = cb[i];

          r.push_back(c[0]);
          g.push_back(c[1]);
          b.push_back(c[2]);
      }

      _init(r, g, b);
  }

  void ColorBarView::init_heat()
  { init(ColorBarRGBA::Heat()); }

  void ColorBarView::init_rainbow()
  { init(ColorBarRGBA::Rainbow()); }

  void ColorBarView::init_blue_to_red()
  { init(ColorBarRGBA::Blue_To_Red()); }

  void ColorBarView::init_magenta()
  { init(ColorBarRGBA::Magenta()); }

  void ColorBarView::init_uniform_yellow()
  { init(ColorBarRGBA::Uniform_Yellow()); }

  void ColorBarView::init_traffic_light()
  { init(ColorBarRGBA::Traffic_Light()); }

  void ColorBarView::init_cluster(int nClusters)
  {
      if (nClusters <= 0)
      { init(ColorBarRGBA::Cluster()); }
      else
      { init_manual_num_colors(ColorBarRGBA::Cluster(), nClusters); }
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void ColorBarView::on_resize(GLint w, GLint h)
  {
      _pdata->textview_title.on_resize(w, h);
      _pdata->textview_min.on_resize(w, h);
      _pdata->textview_max.on_resize(w, h);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_resize(w, h); }

      _update_vbo_positions();
  }

  void ColorBarView::on_oit_enabled(bool b)
  {
      _pdata->textview_title.on_oit_enabled(b);
      _pdata->textview_min.on_oit_enabled(b);
      _pdata->textview_max.on_oit_enabled(b);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_oit_enabled(b); }
  }

  void ColorBarView::on_animation_enabled(bool b)
  {
      _pdata->textview_title.on_animation_enabled(b);
      _pdata->textview_min.on_animation_enabled(b);
      _pdata->textview_max.on_animation_enabled(b);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_animation_enabled(b); }
  }

  void ColorBarView::on_modelview_changed(bool b)
  {
      _pdata->textview_title.on_modelview_changed(b);
      _pdata->textview_min.on_modelview_changed(b);
      _pdata->textview_max.on_modelview_changed(b);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_modelview_changed(b); }
  }

  void ColorBarView::on_visible_changed(bool b)
  {
      _pdata->textview_title.on_visible_changed(b);
      _pdata->textview_min.on_visible_changed(b);
      _pdata->textview_max.on_visible_changed(b);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_visible_changed(b); }
  }

  void ColorBarView::on_mouse_pos_changed(GLint x, GLint y)
  {
      _pdata->textview_title.on_mouse_pos_changed(x, y);
      _pdata->textview_min.on_mouse_pos_changed(x, y);
      _pdata->textview_max.on_mouse_pos_changed(x, y);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_mouse_pos_changed(x, y); }
  }

  void ColorBarView::on_mouse_button_pressed(MouseButton btn)
  {
      _pdata->textview_title.on_mouse_button_pressed(btn);
      _pdata->textview_min.on_mouse_button_pressed(btn);
      _pdata->textview_max.on_mouse_button_pressed(btn);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_mouse_button_pressed(btn); }
  }

  void ColorBarView::on_mouse_button_released(MouseButton btn)
  {
      _pdata->textview_title.on_mouse_button_released(btn);
      _pdata->textview_min.on_mouse_button_released(btn);
      _pdata->textview_max.on_mouse_button_released(btn);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_mouse_button_released(btn); }
  }

  void ColorBarView::on_ssaa_factor_changed(GLint ssaa_factor)
  {
      _pdata->textview_title.on_ssaa_factor_changed(ssaa_factor);
      _pdata->textview_min.on_ssaa_factor_changed(ssaa_factor);
      _pdata->textview_max.on_ssaa_factor_changed(ssaa_factor);
      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].on_ssaa_factor_changed(ssaa_factor); }
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void ColorBarView::draw_impl()
  {
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glDepthFunc(GL_ALWAYS);

      _pdata->shader.bind();
      _pdata->vao.bind();

      if (linear_interpolation_is_enabled())
      { BK_QT_GL glDrawArrays(GL_TRIANGLE_STRIP, 0, (_pdata->num_colors + 2) * 2); }
      else
      { BK_QT_GL glDrawArrays(GL_QUADS, 0, (_pdata->num_colors) * 4); }

      _pdata->vao.release();
      _pdata->shader.release();

      BK_QT_GL glDepthFunc(GL_LESS);

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      _pdata->textview_title.draw();
      _pdata->textview_min.draw();
      _pdata->textview_max.draw();

      for (unsigned int i = 0; i < _pdata->textview_ticks.size(); ++i)
      { _pdata->textview_ticks[i].draw(); }
  }
  /// @}
} // namespace bk


