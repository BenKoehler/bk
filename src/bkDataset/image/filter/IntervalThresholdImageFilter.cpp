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

#include "../../../../include/bkDataset/image/filter/IntervalThresholdImageFilter.h"

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  IntervalThresholdImageFilter::IntervalThresholdImageFilter()
      : IntervalThresholdImageFilter(0.25, 0.75, ThresholdMode::Relative, 0, 1)
  { /* do nothing */ }

  IntervalThresholdImageFilter::IntervalThresholdImageFilter(const self_type&) = default;
  IntervalThresholdImageFilter::IntervalThresholdImageFilter(self_type&&) = default;

  IntervalThresholdImageFilter::IntervalThresholdImageFilter(double threshold_lower, double threshold_upper, ThresholdMode mode, double outside_interval, double inside_interval)
      : _mode(mode),
        _threshold_lower(threshold_lower),
        _threshold_upper(threshold_upper),
        _value_inside_interval(inside_interval),
        _value_outside_interval(outside_interval)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  IntervalThresholdImageFilter::~IntervalThresholdImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET THRESHOLD
  std::pair<double, double> IntervalThresholdImageFilter::threshold() const
  { return std::pair<double, double>(_threshold_lower, _threshold_upper); }

  double IntervalThresholdImageFilter::lower_threshold() const
  { return _threshold_lower; }

  double IntervalThresholdImageFilter::upper_threshold() const
  { return _threshold_upper; }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE BELOW/ABOVE THRESHOLD
  double IntervalThresholdImageFilter::value_inside_interval() const
  { return _value_inside_interval; }

  double IntervalThresholdImageFilter::value_outside_interval() const
  { return _value_outside_interval; }
  /// @}

  /// @{ -------------------------------------------------- GET THRESHOLD MODE
  ThresholdMode IntervalThresholdImageFilter::mode() const
  { return _mode; }

  bool IntervalThresholdImageFilter::mode_is_relative() const
  { return _mode == ThresholdMode::Relative; }

  bool IntervalThresholdImageFilter::mode_is_absolute() const
  { return _mode == ThresholdMode::Absolute; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto IntervalThresholdImageFilter::operator=(const self_type&) -> self_type& = default;
  auto IntervalThresholdImageFilter::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET THRESHOLD
  void IntervalThresholdImageFilter::set_threshold(double lower_threshold, double upper_threshold)
  {
      _threshold_lower = lower_threshold;
      _threshold_upper = upper_threshold;
  }

  void IntervalThresholdImageFilter::set_relative_threshold(double lower_threshold, double upper_threshold)
  {
      _threshold_lower = lower_threshold;
      _threshold_upper = upper_threshold;
      _mode = ThresholdMode::Relative;
  }

  void IntervalThresholdImageFilter::set_absolute_threshold(double lower_threshold, double upper_threshold)
  {
      _threshold_lower = lower_threshold;
      _threshold_upper = upper_threshold;
      _mode = ThresholdMode::Absolute;
  }
  /// @}

  /// @{ -------------------------------------------------- SET MODE
  void IntervalThresholdImageFilter::set_mode(ThresholdMode mode)
  { _mode = mode; }
  /// @}

  /// @{ -------------------------------------------------- SET VALUE BELOW/ABOVE THRESHOLD
  void IntervalThresholdImageFilter::set_value_inside_interval(double inside_interval)
  { _value_inside_interval = inside_interval; }

  void IntervalThresholdImageFilter::set_value_outside_interval(double outside_interval)
  { _value_outside_interval = outside_interval; }
  /// @}
} // namespace bk
