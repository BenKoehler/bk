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

#include "PlotBase.h"

#include <limits>
#include <iostream>
#include <vector>

#include <bkGL/EKey.h>
#include <bkGL/EMouseButton.h>
#include <bkGL/UBOPlot.h>
#include <bkGL/renderable/plot/PlotAxis.h>
#include <bkGL/renderable/plot/PlotMarker.h>
#include <bkGL/renderable/plot/PlotAbstractDataVectorView.h>
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotBase::Impl
  {
      details::UBOPlot ubo;
      PlotAxis xaxis;
      PlotAxis yaxis;
      std::vector<std::shared_ptr<details::PlotAbstractDataVectorView>> plotobjects;
      std::vector<std::shared_ptr<PlotMarker>> markers;
      GLfloat xmin;
      GLfloat xmax;
      GLfloat ymin;
      GLfloat ymax;
      GLfloat border_width_x_in_percent;
      GLfloat border_width_y_in_percent;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : ubo(gl),
            xaxis(gl),
            yaxis(gl),
          #endif
          xmin(std::numeric_limits<GLfloat>::max()),
          xmax(-std::numeric_limits<GLfloat>::max()),
          ymin(std::numeric_limits<GLfloat>::max()),
          ymax(-std::numeric_limits<GLfloat>::max()),
          border_width_x_in_percent(0.075),
          border_width_y_in_percent(0.075)
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

  PlotBase::PlotBase()
      : base_type()
  #else

  PlotBase::PlotBase(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->xaxis.set_orientation_horizontal();
      _pdata->yaxis.set_orientation_vertical();

      _pdata->xaxis.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);
      _pdata->xaxis.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);
      _pdata->xaxis.set_draw_ticks_manually(true);

      _pdata->yaxis.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);
      _pdata->yaxis.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);
      _pdata->yaxis.set_draw_ticks_manually(true);

      _pdata->xaxis.signal_update_required().connect([&]()
                                                     { this->emit_signal_update_required(); });
      _pdata->yaxis.signal_update_required().connect([&]()
                                                     { this->emit_signal_update_required(); });
  }

  PlotBase::PlotBase(PlotBase&&) noexcept = default;

  PlotBase::~PlotBase() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  PlotAxis& PlotBase::x_axis()
  { return _pdata->xaxis; }

  const PlotAxis& PlotBase::x_axis() const
  { return _pdata->xaxis; }

  PlotAxis& PlotBase::y_axis()
  { return _pdata->yaxis; }

  const PlotAxis& PlotBase::y_axis() const
  { return _pdata->yaxis; }

  GLfloat PlotBase::_x_min_from_data() const
  {
      GLfloat xmin = std::numeric_limits<GLfloat>::max();

      for (auto& o: _pdata->plotobjects)
      { xmin = std::min(xmin, o->x_min()); }

      return xmin;
  }

  GLfloat PlotBase::_x_max_from_data() const
  {
      GLfloat xmax = -std::numeric_limits<GLfloat>::max();

      for (auto& o: _pdata->plotobjects)
      { xmax = std::max(xmax, o->x_max()); }

      return xmax;
  }

  GLfloat PlotBase::_y_min_from_data() const
  {
      GLfloat ymin = std::numeric_limits<GLfloat>::max();

      for (auto& o: _pdata->plotobjects)
      { ymin = std::min(ymin, o->y_min()); }

      return ymin;
  }

  GLfloat PlotBase::_y_max_from_data() const
  {
      GLfloat ymax = -std::numeric_limits<GLfloat>::max();

      for (auto& o: _pdata->plotobjects)
      { ymax = std::max(ymax, o->y_max()); }

      return ymax;
  }

  GLfloat PlotBase::x_min() const
  { return _pdata->xmin != std::numeric_limits<GLfloat>::max() ? _pdata->xmin : _x_min_from_data(); }

  GLfloat PlotBase::x_max() const
  { return _pdata->xmax != -std::numeric_limits<GLfloat>::max() ? _pdata->xmax : _x_max_from_data(); }

  GLfloat PlotBase::y_min() const
  { return _pdata->ymin != std::numeric_limits<GLfloat>::max() ? _pdata->ymin : _y_min_from_data(); }

  GLfloat PlotBase::y_max() const
  { return _pdata->ymax != -std::numeric_limits<GLfloat>::max() ? _pdata->ymax : _y_max_from_data(); }

  GLfloat PlotBase::border_width_x_in_percent() const
  { return _pdata->border_width_x_in_percent; }

  GLfloat PlotBase::border_width_y_in_percent() const
  { return _pdata->border_width_y_in_percent; }

  unsigned int PlotBase::num_datavectorviews() const
  { return _pdata->plotobjects.size(); }

  std::shared_ptr<details::PlotAbstractDataVectorView>& PlotBase::datavectorview(unsigned int i)
  { return _pdata->plotobjects[i]; }

  const std::shared_ptr<details::PlotAbstractDataVectorView>& PlotBase::datavectorview(unsigned int i) const
  { return _pdata->plotobjects[i]; }

  /// @{ -------------------------------------------------- MARKERS
  unsigned int PlotBase::num_markers() const
  { return _pdata->markers.size(); }

  unsigned int PlotBase::num_markers_horizontal() const
  {
      return std::count_if(_pdata->markers.begin(), _pdata->markers.end(), [](const std::shared_ptr<PlotMarker>& m)
      { return m->orientation_is_horizontal(); });
  }

  unsigned int PlotBase::num_markers_vertical() const
  {
      return std::count_if(_pdata->markers.begin(), _pdata->markers.end(), [](const std::shared_ptr<PlotMarker>& m)
      { return m->orientation_is_vertical(); });
  }

  std::shared_ptr<PlotMarker>& PlotBase::marker_horizontal(unsigned int i)
  {
      unsigned int cnt = 0;
      for (auto& m: _pdata->markers)
      {
          if (m->orientation_is_horizontal())
          {
              if (cnt == i)
              { return m; }

              ++cnt;
          }
      }

      return _pdata->markers[0]; // todo: remove this workaround
  }

  const std::shared_ptr<PlotMarker>& PlotBase::marker_horizontal(unsigned int i) const
  {
      unsigned int cnt = 0;
      for (auto& m: _pdata->markers)
      {
          if (m->orientation_is_horizontal())
          {
              if (cnt == i)
              { return m; }

              ++cnt;
          }
      }

      return _pdata->markers[0]; // todo: remove this workaround
  }

  std::shared_ptr<PlotMarker>& PlotBase::marker_vertical(unsigned int i)
  {
      unsigned int cnt = 0;
      for (auto& m: _pdata->markers)
      {
          if (m->orientation_is_vertical())
          {
              if (cnt == i)
              { return m; }

              ++cnt;
          }
      }

      return _pdata->markers[0]; // todo: remove this workaround
  }

  const std::shared_ptr<PlotMarker>& PlotBase::marker_vertical(unsigned int i) const
  {
      unsigned int cnt = 0;
      for (auto& m: _pdata->markers)
      {
          if (m->orientation_is_vertical())
          {
              if (cnt == i)
              { return m; }

              ++cnt;
          }
      }

      return _pdata->markers[0]; // todo: remove this workaround
  }

  bool PlotBase::is_initialized() const
  { return _pdata->ubo.is_initialized(); }

  void PlotBase::clear_markers()
  { _pdata->markers.clear(); }

  void PlotBase::clear_markers_horizontal()
  {
      _pdata->markers.erase(std::remove_if(_pdata->markers.begin(), _pdata->markers.end(), [](const std::shared_ptr<PlotMarker>& m)
      { return m->orientation_is_horizontal(); }), _pdata->markers.end());
  }

  void PlotBase::clear_markers_vertical()
  {
      _pdata->markers.erase(std::remove_if(_pdata->markers.begin(), _pdata->markers.end(), [](const std::shared_ptr<PlotMarker>& m)
      { return m->orientation_is_vertical(); }), _pdata->markers.end());
  }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto PlotBase::operator=(PlotBase&&) noexcept -> PlotBase& = default;

  std::shared_ptr<details::PlotAbstractDataVectorView>& PlotBase::add_datavectorview(std::shared_ptr<details::PlotAbstractDataVectorView> dvv)
  {
      _pdata->plotobjects.push_back(dvv);

      _pdata->plotobjects.back()->signal_update_required().connect([&]()
                                                                   {
                                                                       this->emit_signal_update_required();
                                                                   });

      return _pdata->plotobjects.back();
  }

  bool PlotBase::remove_datavectorview(std::shared_ptr<details::PlotAbstractDataVectorView> dvv)
  {
      for (auto it = _pdata->plotobjects.begin(); it != _pdata->plotobjects.end(); ++it)
      {
          if (*it == dvv)
          {
              (*it)->clear();
              _pdata->plotobjects.erase(it);
              return true;
          }
      }

      return false;
  }

  /// @{ -------------------------------------------------- ADD MARKER
  std::shared_ptr<PlotMarker>& PlotBase::_add_marker()
  {
      _pdata->markers.emplace_back(std::make_shared<PlotMarker>(
      #ifdef BK_LIB_QT_AVAILABLE
          this->_gl
      #endif
      ));

      _pdata->markers.back()->signal_update_required().connect([&]()
                                                               {
                                                                   this->emit_signal_update_required();
                                                               });

      return _pdata->markers.back();
  }

  std::shared_ptr<PlotMarker>& PlotBase::add_marker_horizontal()
  {
      std::shared_ptr<PlotMarker>& m = _add_marker();
      m->set_orientation_horizontal();
      return m;
  }

  std::shared_ptr<PlotMarker>& PlotBase::add_marker_vertical()
  {
      std::shared_ptr<PlotMarker>& m = _add_marker();
      m->set_orientation_vertical();
      return m;
  }
  /// @}

  void PlotBase::set_x_min(GLfloat xmin)
  {
      _pdata->xmin = xmin;
      xmin = x_min();

      _pdata->xaxis.set_x_min(xmin);
      _pdata->yaxis.set_x_min(xmin);

      if (this->is_initialized())
      {
          _pdata->ubo.set_xmin(xmin);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotBase::set_x_max(GLfloat xmax)
  {
      _pdata->xmax = xmax;
      xmax = x_max();

      _pdata->xaxis.set_x_max(xmax);
      _pdata->yaxis.set_x_max(xmax);

      if (this->is_initialized())
      {
          _pdata->ubo.set_xmax(xmax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotBase::set_y_min(GLfloat ymin)
  {
      _pdata->ymin = ymin;
      ymin = y_min();

      _pdata->xaxis.set_y_min(ymin);
      _pdata->yaxis.set_y_min(ymin);

      if (this->is_initialized())
      {
          _pdata->ubo.set_ymin(ymin);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotBase::set_y_max(GLfloat ymax)
  {
      _pdata->ymax = ymax;
      ymax = y_max();

      _pdata->xaxis.set_y_max(ymax);
      _pdata->yaxis.set_y_max(ymax);

      if (this->is_initialized())
      {
          _pdata->ubo.set_ymax(ymax);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotBase::_reset_xy_minmax()
  {
      set_x_min(std::numeric_limits<GLfloat>::max());
      set_x_max(-std::numeric_limits<GLfloat>::max());
      set_y_min(std::numeric_limits<GLfloat>::max());
      set_y_max(-std::numeric_limits<GLfloat>::max());
  }

  void PlotBase::set_xy_minmax_from_data()
  {
      set_x_min(_x_min_from_data());
      set_x_max(_x_max_from_data());
      set_y_min(_y_min_from_data());
      set_y_max(_y_max_from_data());
  }

  void PlotBase::set_border_width_x_in_percent(GLfloat b)
  {
      _pdata->border_width_x_in_percent = b;
      _pdata->xaxis.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);
      _pdata->yaxis.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotBase::set_border_width_y_in_percent(GLfloat b)
  {
      _pdata->border_width_y_in_percent = b;
      _pdata->xaxis.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);
      _pdata->yaxis.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);

      if (this->is_initialized())
      {
          _pdata->ubo.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotBase::init_ubo()
  {
      clear_ubo();

      if (_pdata->ubo.init_from_registered_values_size())
      {
          _pdata->ubo.set_xmin(x_min());
          _pdata->ubo.set_xmax(x_max());
          _pdata->ubo.set_ymin(y_min());
          _pdata->ubo.set_ymax(y_max());
          _pdata->ubo.set_border_width_x_in_percent(_pdata->border_width_x_in_percent);
          _pdata->ubo.set_border_width_y_in_percent(_pdata->border_width_y_in_percent);
          _pdata->ubo.release();

          return true;
      }
      else
      { return false; }
  }

  bool PlotBase::init()
  {
      bool success = init_ubo();
      if (!success)
      { std::cerr << "PlotBase::init - ubo failed" << std::endl; }

      _pdata->xaxis.set_x_min(x_min());
      _pdata->xaxis.set_x_max(x_max());
      _pdata->xaxis.set_y_min(y_min());
      _pdata->xaxis.set_y_max(y_max());

      success &= _pdata->xaxis.init();
      if (!success)
      { std::cerr << "PlotBase::init - xaxis failed" << std::endl; }

      _pdata->yaxis.set_x_min(x_min());
      _pdata->yaxis.set_x_max(x_max());
      _pdata->yaxis.set_y_min(y_min());
      _pdata->yaxis.set_y_max(y_max());

      success &= _pdata->yaxis.init();
      if (!success)
      { std::cerr << "PlotBase::init - yaxis failed" << std::endl; }

      for (auto& o: _pdata->plotobjects)
      {
          success &= o->init();
          if (!success)
          { std::cerr << "PlotBase::init - plotobjects failed" << std::endl; }
      }

      for (auto& m: _pdata->markers)
      {
          m->set_x_min(x_min());
          m->set_x_max(x_max());
          m->set_y_min(y_min());
          m->set_y_max(y_max());

          success &= m->init();
          if (!success)
          { std::cerr << "PlotBase::init - markers failed" << std::endl; }
      }

      if (!success)
      { clear(); }
      else
      { this->emit_signal_update_required(); }

      return success;
  }

  void PlotBase::clear_ubo()
  { _pdata->ubo.clear(); }

  void PlotBase::clear()
  {
      clear_ubo();
      _pdata->xaxis.clear();
      _pdata->yaxis.clear();

      for (auto& o: _pdata->plotobjects)
      { o->clear(); }

      _pdata->plotobjects.clear();

      for (auto& m: _pdata->markers)
      { m->clear(); }

      _pdata->markers.clear();
  }

  void PlotBase::on_resize(GLint w, GLint h)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_resize(w, h); }

      for (auto& o: _pdata->markers)
      { o->on_resize(w, h); }

      _pdata->xaxis.on_resize(w, h);
      _pdata->yaxis.on_resize(w, h);
  }

  void PlotBase::on_oit_enabled(bool b)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_oit_enabled(b); }

      for (auto& o: _pdata->markers)
      { o->on_oit_enabled(b); }

      _pdata->xaxis.on_oit_enabled(b);
      _pdata->yaxis.on_oit_enabled(b);
  }

  void PlotBase::on_animation_enabled(bool b)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_animation_enabled(b); }

      for (auto& o: _pdata->markers)
      { o->on_animation_enabled(b); }

      _pdata->xaxis.on_animation_enabled(b);
      _pdata->yaxis.on_animation_enabled(b);
  }

  void PlotBase::on_modelview_changed(bool b)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_modelview_changed(b); }

      for (auto& o: _pdata->markers)
      { o->on_modelview_changed(b); }

      _pdata->xaxis.on_modelview_changed(b);
      _pdata->yaxis.on_modelview_changed(b);
  }

  void PlotBase::on_visible_changed(bool b)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_visible_changed(b); }

      for (auto& o: _pdata->markers)
      { o->on_visible_changed(b); }

      _pdata->xaxis.on_visible_changed(b);
      _pdata->yaxis.on_visible_changed(b);
  }

  void PlotBase::on_mouse_pos_changed(GLint x, GLint y)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_mouse_pos_changed(x, y); }

      for (auto& o: _pdata->markers)
      { o->on_mouse_pos_changed(x, y); }

      _pdata->xaxis.on_mouse_pos_changed(x, y);
      _pdata->yaxis.on_mouse_pos_changed(x, y);
  }

  void PlotBase::on_mouse_button_pressed(MouseButton_ btn)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_mouse_button_pressed(btn); }

      for (auto& o: _pdata->markers)
      { o->on_mouse_button_pressed(btn); }

      _pdata->xaxis.on_mouse_button_pressed(btn);
      _pdata->yaxis.on_mouse_button_pressed(btn);
  }

  void PlotBase::on_mouse_button_released(MouseButton_ btn)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_mouse_button_released(btn); }

      for (auto& o: _pdata->markers)
      { o->on_mouse_button_released(btn); }

      _pdata->xaxis.on_mouse_button_released(btn);
      _pdata->yaxis.on_mouse_button_released(btn);
  }

  void PlotBase::on_key_pressed(Key_ k)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_key_pressed(k); }

      for (auto& o: _pdata->markers)
      { o->on_key_pressed(k); }

      _pdata->xaxis.on_key_pressed(k);
      _pdata->yaxis.on_key_pressed(k);
  }

  void PlotBase::on_key_released(Key_ k)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_key_released(k); }

      for (auto& o: _pdata->markers)
      { o->on_key_released(k); }

      _pdata->xaxis.on_key_released(k);
      _pdata->yaxis.on_key_released(k);
  }

  void PlotBase::on_mouse_wheel_up()
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_mouse_wheel_up(); }

      for (auto& o: _pdata->markers)
      { o->on_mouse_wheel_up(); }

      _pdata->xaxis.on_mouse_wheel_up();
      _pdata->yaxis.on_mouse_wheel_up();
  }

  void PlotBase::on_mouse_wheel_down()
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_mouse_wheel_down(); }

      for (auto& o: _pdata->markers)
      { o->on_mouse_wheel_down(); }

      _pdata->xaxis.on_mouse_wheel_down();
      _pdata->yaxis.on_mouse_wheel_down();
  }

  void PlotBase::on_ssaa_factor_changed(GLint ssaa_factor)
  {
      for (auto& o: _pdata->plotobjects)
      { o->on_ssaa_factor_changed(ssaa_factor); }

      for (auto& o: _pdata->markers)
      { o->on_ssaa_factor_changed(ssaa_factor); }

      _pdata->xaxis.on_ssaa_factor_changed(ssaa_factor);
      _pdata->yaxis.on_ssaa_factor_changed(ssaa_factor);
  }

  void PlotBase::draw_impl()
  {
      _pdata->ubo.bind_to_default_base();

      _pdata->xaxis.draw_ticks();
      _pdata->yaxis.draw_ticks();

      for (auto& m: _pdata->markers)
      { m->draw(); }

      for (auto& o: _pdata->plotobjects)
      { o->draw(); }

      _pdata->xaxis.draw();
      _pdata->yaxis.draw();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


