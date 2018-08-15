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

#pragma once

#ifndef BK_INTERVALTHRESHOLDIMAGEFILTER_H
#define BK_INTERVALTHRESHOLDIMAGEFILTER_H

#include <algorithm>
#include <utility>

#include <bkAlgorithm/threshold.h>

#include <bkDataset/image/filter/ThresholdMode.h>
#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT IntervalThresholdImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = IntervalThresholdImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      ThresholdMode _mode;
      double _threshold_lower;
      double _threshold_upper;
      double _value_inside_interval;
      double _value_outside_interval;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      IntervalThresholdImageFilter();
      IntervalThresholdImageFilter(const self_type&);
      IntervalThresholdImageFilter(self_type&&);
      IntervalThresholdImageFilter(double threshold_lower, double threshold_upper, ThresholdMode mode, double outside_interval = 0, double inside_interval = 1);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~IntervalThresholdImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET UPPER/LOWER THRESHOLD
      [[nodiscard]] std::pair<double, double> threshold() const;
      [[nodiscard]] double lower_threshold() const;
      [[nodiscard]] double upper_threshold() const;
      /// @}

      /// @{ -------------------------------------------------- GET VALUE BELOW/ABOVE THRESHOLD
      [[nodiscard]] double value_inside_interval() const;
      [[nodiscard]] double value_outside_interval() const;
      /// @}

      /// @{ -------------------------------------------------- GET THRESHOLD MODE
      [[nodiscard]] ThresholdMode mode() const;
      [[nodiscard]] bool mode_is_relative() const;
      [[nodiscard]] bool mode_is_absolute() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET THRESHOLD
      void set_threshold(double lower_threshold, double upper_threshold);
      void set_relative_threshold(double lower_threshold, double upper_threshold);
      void set_absolute_threshold(double lower_threshold, double upper_threshold);
      /// @}

      /// @{ -------------------------------------------------- SET MODE
      void set_mode(ThresholdMode mode);
      /// @}

      /// @{ -------------------------------------------------- SET VALUE BELOW/ABOVE THRESHOLD
      void set_value_inside_interval(double inside_interval);
      void set_value_outside_interval(double outside_interval);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] TImage apply(const TImage& img) const
      {
          TImage res(img);

          double thresh_lower = _threshold_lower;
          double thresh_upper = _threshold_upper;

          if (mode_is_relative())
          {
              const auto[itMinVal, itMaxVal] = std::minmax_element(img.begin(), img.end());
              const double range = static_cast<double>(*itMaxVal - *itMinVal);

              thresh_lower = static_cast<double>(*itMinVal) + _threshold_lower * range;
              thresh_upper = static_cast<double>(*itMinVal) + _threshold_upper * range;
          }

          bk::interval_threshold(res.begin(), res.end(), thresh_lower, thresh_upper, _value_inside_interval, _value_outside_interval);

          return res;
      }
      /// @}
  }; // class IntervalThresholdImageFilter
} // namespace bk

#endif //BK_INTERVALTHRESHOLDIMAGEFILTER_H
