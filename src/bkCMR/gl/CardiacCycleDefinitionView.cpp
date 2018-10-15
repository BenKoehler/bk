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
#include <cmath>
#include <iostream>
#include <limits>

#include <bkCMR/gl/CardiacCycleDefinitionView.h>
#include <bkGL/renderable/plot/PlotMarker.h>
#include <bkGL/renderable/plot/PlotAxis.h>
#include <bkGL/renderable/text/TextView.h>
#include <bkGL/EMouseButton.h>

#include <bk/Color>
#include <bk/Localization>
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct CardiacCycleDefinitionView::Impl
  {
      std::shared_ptr<PlotMarker> marker_systole_begin;
      std::shared_ptr<PlotMarker> marker_systole_end;
      std::shared_ptr<PlotMarker> marker_current;
      std::shared_ptr<PlotMarker> last_marker;
      GLfloat temporal_resolution;
      unsigned int num_time_steps;

      GLfloat bwleft;
      GLfloat bwright;
      GLfloat current_x_percent;
      GLfloat current_data_value;

      bool mouse_left_is_pressed;
      bool mouse_right_is_pressed;

      unsigned int time_point_id_systole_begin;
      unsigned int time_point_id_systole_end;

      Signal<unsigned int, unsigned int> s_times_changed;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          #else
      explicit Impl(qt_gl_functions* gl)
          #endif
          : marker_systole_begin(nullptr),
            marker_systole_end(nullptr),
            marker_current(nullptr),
            last_marker(marker_current),
            temporal_resolution(1),
            num_time_steps(0),
            bwleft(0),
            bwright(0),
            current_x_percent(0),
            current_data_value(0),
            mouse_left_is_pressed(false),
            mouse_right_is_pressed(false),
            time_point_id_systole_begin(0),
            time_point_id_systole_end(0)
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

  CardiacCycleDefinitionView::CardiacCycleDefinitionView()
      : base_type()
  #else
  CardiacCycleDefinitionView::CardiacCycleDefinitionView(qt_gl_functions* gl)
  : base_type(gl),
    _pdata(gl)
  #endif
  {
      /*
       * marker order is important
       * - temp marker is drawn first
       * - if begin/end markers are set by clicking the mouse,
       *   the temp marker is behind them
       */
      _pdata->marker_current = this->add_marker_vertical();
      _pdata->marker_current->set_line_width(3);
      _pdata->marker_current->set_color(details::ColorFactory<ColorRGBA>::Orange());

      _pdata->marker_systole_begin = this->add_marker_vertical();
      _pdata->marker_systole_begin->set_line_width(6);
      _pdata->marker_systole_begin->set_color(details::ColorFactory<ColorRGBA>::Light_Green());

      _pdata->marker_systole_end = this->add_marker_vertical();
      _pdata->marker_systole_end->set_line_width(6);
      _pdata->marker_systole_end->set_color(details::ColorFactory<ColorRGBA>::Light_Red());

      this->x_axis().set_label(___("Time [ms]"));
      this->y_axis().set_label(___("Forward velocity [m/s]"));

      this->x_axis().set_line_width(2);
      this->y_axis().set_line_width(2);
      this->x_axis().set_num_ticks(3);
      this->y_axis().set_num_ticks(3);
      this->x_axis().set_tick_precision(0);
      this->y_axis().set_tick_precision(1);
      this->x_axis().text_view_label().set_scale(0.4);
      this->y_axis().text_view_label().set_scale(0.4);
      this->set_border_width_x_in_percent(0.1);
      this->set_border_width_y_in_percent(0.11);
  }

  CardiacCycleDefinitionView::CardiacCycleDefinitionView(CardiacCycleDefinitionView&&) noexcept = default;
  CardiacCycleDefinitionView::~CardiacCycleDefinitionView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_systole_begin()
  { return _pdata->marker_systole_begin; }

  const std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_systole_begin() const
  { return _pdata->marker_systole_begin; }

  std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_systole_end()
  { return _pdata->marker_systole_end; }

  const std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_systole_end() const
  { return _pdata->marker_systole_end; }

  std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_current()
  { return _pdata->marker_current; }

  const std::shared_ptr<PlotMarker>& CardiacCycleDefinitionView::marker_current() const
  { return _pdata->marker_current; }

  unsigned int CardiacCycleDefinitionView::time_point_id_systole_begin() const
  { return _pdata->time_point_id_systole_begin; }

  unsigned int CardiacCycleDefinitionView::time_point_id_systole_end() const
  { return _pdata->time_point_id_systole_end; }

  double CardiacCycleDefinitionView::time_systole_begin() const
  { return std::round(_pdata->time_point_id_systole_begin * _pdata->temporal_resolution); }

  double CardiacCycleDefinitionView::time_systole_end() const
  { return std::round(_pdata->time_point_id_systole_end * _pdata->temporal_resolution); }

  Signal<unsigned int, unsigned int>& CardiacCycleDefinitionView::signal_times_changed()
  { return _pdata->s_times_changed; }

  unsigned int CardiacCycleDefinitionView::current_time_id() const
  { return static_cast<unsigned int>(std::round(_pdata->current_data_value / _pdata->temporal_resolution)) % _pdata->num_time_steps; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  CardiacCycleDefinitionView& CardiacCycleDefinitionView::operator=(CardiacCycleDefinitionView&&) noexcept = default;

  void CardiacCycleDefinitionView::set_temporal_resolution(double temporal_resolution)
  {
      _pdata->temporal_resolution = temporal_resolution;
      _pdata->num_time_steps = x_max() / _pdata->temporal_resolution;
  }

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void CardiacCycleDefinitionView::set_current_marker_pos()
  {
      _pdata->last_marker->set_data_value(_pdata->current_data_value);

      if (_pdata->mouse_left_is_pressed)
      { _pdata->time_point_id_systole_begin = current_time_id(); }
      else if (_pdata->mouse_right_is_pressed)
      { _pdata->time_point_id_systole_end = current_time_id(); }

      _pdata->s_times_changed.emit_signal(_pdata->time_point_id_systole_begin, _pdata->time_point_id_systole_end);
  }

  void CardiacCycleDefinitionView::scroll_current_marker(bool up)
  {
      GLfloat t = _pdata->last_marker->data_value();
      t += (up ? 1 : -1) * _pdata->temporal_resolution;
      t = std::fmod(t, x_max());

      _pdata->last_marker->set_data_value(t);
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  void CardiacCycleDefinitionView::on_mouse_pos_changed(GLint x, GLint y)
  {
      base_type::on_mouse_pos_changed(x, y);

      _pdata->bwleft = this->border_width_x_in_percent() * this->window_width();
      _pdata->bwright = 0.125f * this->border_width_x_in_percent() * this->window_width();

      x = std::max(0.0f, x - _pdata->bwleft);
      _pdata->current_x_percent = std::clamp(x / (this->window_width() - _pdata->bwleft - _pdata->bwright), 0.0f, 1.0f);
      _pdata->current_data_value = x_min() + _pdata->current_x_percent * (x_max() - x_min());

      // round current data value to next time step
      _pdata->current_data_value = std::round(_pdata->current_data_value / _pdata->temporal_resolution) * _pdata->temporal_resolution;

      if (_pdata->mouse_left_is_pressed || _pdata->mouse_right_is_pressed)
      { set_current_marker_pos(); }

      _pdata->marker_current->set_data_value(_pdata->current_data_value);
  }

  void CardiacCycleDefinitionView::on_mouse_button_pressed(MouseButton_ btn)
  {
      base_type::on_mouse_button_pressed(btn);

      if (btn == MouseButton_Left)
      {
          _pdata->mouse_left_is_pressed = true;
          _pdata->last_marker = _pdata->marker_systole_begin;
          set_current_marker_pos();
      }
      else if (btn == MouseButton_Right)
      {
          _pdata->mouse_right_is_pressed = true;
          _pdata->last_marker = _pdata->marker_systole_end;
          set_current_marker_pos();
      }
  }

  void CardiacCycleDefinitionView::on_mouse_button_released(MouseButton_ btn)
  {
      base_type::on_mouse_button_released(btn);

      if (btn == MouseButton_Left)
      {
          _pdata->mouse_left_is_pressed = false;
          _pdata->time_point_id_systole_begin = current_time_id();
          _pdata->s_times_changed.emit_signal(_pdata->time_point_id_systole_begin, _pdata->time_point_id_systole_end);
      }
      else if (btn == MouseButton_Right)
      {
          _pdata->mouse_right_is_pressed = false;
          _pdata->time_point_id_systole_end = current_time_id();
          _pdata->s_times_changed.emit_signal(_pdata->time_point_id_systole_begin, _pdata->time_point_id_systole_end);
      }
  }

  void CardiacCycleDefinitionView::on_mouse_wheel_up()
  {
      base_type::on_mouse_wheel_up();
      scroll_current_marker(true);
  }

  void CardiacCycleDefinitionView::on_mouse_wheel_down()
  {
      base_type::on_mouse_wheel_down();
      scroll_current_marker(false);
  }
} // namespace bk