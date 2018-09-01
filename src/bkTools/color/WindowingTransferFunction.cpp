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

#include <bkTools/color/WindowingTransferFunction.h>

#include <algorithm>
#include <cassert>
#include <limits>

#include <bkTools/color/ColorRGBA.h>
#include <bkTools/color/ColorBarRGBA.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct WindowingTransferFunction::Impl
  {
      colorbar_type cols;
      double center;
      double width;
      double intensity_min;
      double intensity_min_eps;
      double intensity_max;
      double intensity_max_eps;
      double intensity_range;
      double base_data_values[4];

      Impl()
          : Impl(0, 1, 0.5, 0.4)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) noexcept = default;

      Impl(double intensity_min_, double intensity_max_, double center_, double width_)
          : cols(color_type(0, 0, 0, 0), color_type(0, 0, 0, 0), color_type(1, 1, 1, 1), color_type(1, 1, 1, 1)),
            center(center_),
            width(width_),
            intensity_min(std::min(intensity_min_, intensity_max_)),
            intensity_min_eps(intensity_min + std::numeric_limits<double>::epsilon()),
            intensity_max(std::max(intensity_min_, intensity_max_)),
            intensity_max_eps(intensity_max - std::numeric_limits<double>::epsilon()),
            intensity_range(intensity_max_eps - intensity_min_eps),
            base_data_values{0, 1, 2, 3}
      { /* do nothing */ }

      Impl& operator=(const Impl&) = default;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  WindowingTransferFunction::WindowingTransferFunction()
      : WindowingTransferFunction(0, 1, 0.5, 0.4)
  { /* do nothing */ }

  WindowingTransferFunction::WindowingTransferFunction(const self_type&) = default;

  WindowingTransferFunction::WindowingTransferFunction(self_type&&) noexcept = default;

  WindowingTransferFunction::WindowingTransferFunction(double intensity_min, double intensity_max, double center, double width)
      : _pdata(intensity_min, intensity_max, center, width)
  {
      _set_color_boundaries();
      _set_inner_colors();
  }

  WindowingTransferFunction::~WindowingTransferFunction() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  double WindowingTransferFunction::center() const
  { return _pdata->center; }

  double WindowingTransferFunction::width() const
  { return _pdata->width; }

  auto WindowingTransferFunction::colorbar() const -> const colorbar_type&
  { return _pdata->cols; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void WindowingTransferFunction::_set_color_boundaries()
  {
      _pdata->base_data_values[0] = _pdata->intensity_min;
      _pdata->base_data_values[3] = _pdata->intensity_max;
  }

  void WindowingTransferFunction::_set_inner_colors()
  {
      _pdata->base_data_values[1] = _pdata->center - _pdata->width;
      _pdata->base_data_values[2] = _pdata->center + _pdata->width;
  }

  void WindowingTransferFunction::set_intensity_min_max(double imin, double imax)
  {
      _pdata->intensity_min = std::min(imin, imax);
      _pdata->intensity_min_eps = _pdata->intensity_min + std::numeric_limits<double>::epsilon();
      _pdata->intensity_max = std::max(imin, imax);
      _pdata->intensity_max_eps = _pdata->intensity_max - std::numeric_limits<double>::epsilon();
      _pdata->intensity_range = _pdata->intensity_max_eps - _pdata->intensity_min_eps;

      _set_color_boundaries();

      if (_pdata->base_data_values[1] < _pdata->intensity_min_eps)
      { _pdata->base_data_values[1] = _pdata->intensity_min_eps; }

      if (_pdata->base_data_values[2] > _pdata->intensity_max_eps)
      { _pdata->base_data_values[2] = _pdata->intensity_max_eps; }
  }

  void WindowingTransferFunction::_set_center(double c)
  { _pdata->center = std::clamp(c, _pdata->intensity_min_eps, _pdata->intensity_max_eps); }

  void WindowingTransferFunction::set_center(double c)
  {
      _set_center(c);
      _set_inner_colors();
  }

  void WindowingTransferFunction::shift_center_left(double percent)
  { set_center(_pdata->center - 0.01 * _pdata->intensity_range * std::clamp(percent, 0.0, 100.0)); }

  void WindowingTransferFunction::shift_center_right(double percent)
  { set_center(_pdata->center + 0.01 * _pdata->intensity_range * std::clamp(percent, 0.0, 100.0)); }

  void WindowingTransferFunction::_set_width(double w)
  { _pdata->width = std::max(0.01 * _pdata->intensity_range, std::min(w, 3 * _pdata->intensity_range)); }

  void WindowingTransferFunction::set_width(double w)
  {
      _set_width(w);
      _set_inner_colors();
  }

  double WindowingTransferFunction::intensity_min() const
  { return _pdata->intensity_min; }

  double WindowingTransferFunction::intensity_max() const
  { return _pdata->intensity_max; }

  double WindowingTransferFunction::intensity_range() const
  { return _pdata->intensity_range; }

  void WindowingTransferFunction::increase_width(double percent)
  { set_width(_pdata->width + 0.01 * _pdata->intensity_range * std::clamp(percent, 0.0, 100.0)); }

  void WindowingTransferFunction::decrease_width(double percent)
  { set_width(_pdata->width - 0.01 * _pdata->intensity_range * std::clamp(percent, 0.0, 100.0)); }

  void WindowingTransferFunction::reset(bool tolerant)
  {
      if (!tolerant)
      {
          _pdata->center = 0.5 * (_pdata->intensity_min + _pdata->intensity_max);
          _pdata->width = 0.45 * _pdata->intensity_range;
      }
      else
      {
          _pdata->center = 0.2 * (_pdata->intensity_min + _pdata->intensity_max);
          _pdata->width = 0.2 * _pdata->intensity_range;
      }

      _set_color_boundaries();
      _set_inner_colors();
  }

  auto WindowingTransferFunction::operator=(const self_type& other) -> self_type& = default;

  auto WindowingTransferFunction::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  auto WindowingTransferFunction::apply(double data_value) const -> color_type
  {
      const unsigned int N = colorbar().num_colors();

      if (N == 0)
      { return color_type(); }

      if (data_value <= _pdata->base_data_values[0])
      { return _pdata->cols[0]; }

      for (unsigned int i = 0; i < N - 1; ++i)
      {
          if (_pdata->base_data_values[i] <= data_value && data_value <= _pdata->base_data_values[i + 1])
          {
              assert(_pdata->base_data_values[i] != _pdata->base_data_values[i + 1] && "duplicate values in colorbar");

              const double w = (data_value - _pdata->base_data_values[i]) / (_pdata->base_data_values[i + 1] - _pdata->base_data_values[i]);
              return (_pdata->cols[i] * (1 - w)) + (_pdata->cols[i + 1] * w);
          }
      }

      return _pdata->cols[N - 1];
  }
} // namespace bk


