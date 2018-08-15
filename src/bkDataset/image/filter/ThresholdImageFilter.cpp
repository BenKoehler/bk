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

#include <bkDataset/image/filter/ThresholdImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ThresholdImageFilter::ThresholdImageFilter()
      : ThresholdImageFilter(0.5, ThresholdMode::Relative, 0, 1)
  { /* do nothing */ }

  ThresholdImageFilter::ThresholdImageFilter(const self_type&) = default;
  ThresholdImageFilter::ThresholdImageFilter(self_type&&) noexcept = default;

  ThresholdImageFilter::ThresholdImageFilter(double threshold, ThresholdMode mode, double below_threshold, double above_threshold)
      : _mode(mode),
        _threshold(threshold),
        _value_below_threshold(below_threshold),
        _value_above_threshold(above_threshold)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ThresholdImageFilter::~ThresholdImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET THRESHOLD
  double ThresholdImageFilter::threshold() const
  { return _threshold; }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE BELOW/ABOVE THRESHOLD
  double ThresholdImageFilter::value_below_threshold() const
  { return _value_below_threshold; }

  double ThresholdImageFilter::value_above_threshold() const
  { return _value_above_threshold; }
  /// @}

  /// @{ -------------------------------------------------- GET THRESHOLD MODE
  ThresholdMode ThresholdImageFilter::mode() const
  { return _mode; }

  bool ThresholdImageFilter::mode_is_relative() const
  { return _mode == ThresholdMode::Relative; }

  bool ThresholdImageFilter::mode_is_absolute() const
  { return _mode == ThresholdMode::Absolute; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto ThresholdImageFilter::operator=(const self_type&) -> self_type& = default;
  auto ThresholdImageFilter::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET THRESHOLD
  void ThresholdImageFilter::set_threshold(double threshold)
  { _threshold = threshold; }

  void ThresholdImageFilter::set_relative_threshold(double threshold)
  {
      _threshold = threshold;
      _mode = ThresholdMode::Relative;
  }

  void ThresholdImageFilter::set_absolute_threshold(double threshold)
  {
      _threshold = threshold;
      _mode = ThresholdMode::Absolute;
  }
  /// @}

  /// @{ -------------------------------------------------- SET MODE
  void ThresholdImageFilter::set_mode(ThresholdMode mode)
  { _mode = mode; }
  /// @}

  /// @{ -------------------------------------------------- SET VALUE BELOW/ABOVE THRESHOLD
  void ThresholdImageFilter::set_value_below_threshold(double below_threshold)
  { _value_below_threshold = below_threshold; }

  void ThresholdImageFilter::set_value_above_threshold(double above_threshold)
  { _value_above_threshold = above_threshold; }
  /// @}
} // namespace bk
