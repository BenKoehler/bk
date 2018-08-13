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

#ifndef BK_THRESHOLDIMAGEFILTER_H
#define BK_THRESHOLDIMAGEFILTER_H

#include <algorithm>

#include "../../../bkAlgorithm/threshold.h"
#include "../../../bkAlgorithm/otsu.h"

#include "ThresholdMode.h"
#include "../../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT ThresholdImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ThresholdImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      ThresholdMode _mode;
      double _threshold;
      double _value_below_threshold;
      double _value_above_threshold;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ThresholdImageFilter();
      ThresholdImageFilter(const self_type&);
      ThresholdImageFilter(self_type&&);
      ThresholdImageFilter(double threshold, ThresholdMode mode, double below_threshold = 0, double above_threshold = 1);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ThresholdImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET THRESHOLD
      [[nodiscard]] double threshold() const;
      /// @}

      /// @{ -------------------------------------------------- GET VALUE BELOW/ABOVE THRESHOLD
      [[nodiscard]] double value_below_threshold() const;
      [[nodiscard]] double value_above_threshold() const;
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
      void set_threshold(double threshold);
      void set_relative_threshold(double threshold);
      void set_absolute_threshold(double threshold);
      /// @}

      /// @{ -------------------------------------------------- SET THRESHOLD VIA OTSU METHOD
      template<typename TImage>
      void set_otsu_threshold(const TImage& img)
      { set_absolute_threshold(bk::otsu(img.begin(), img.end())); }
      /// @}

      /// @{ -------------------------------------------------- SET MODE
      void set_mode(ThresholdMode mode);
      /// @}

      /// @{ -------------------------------------------------- SET VALUE BELOW/ABOVE THRESHOLD
      void set_value_below_threshold(double below_threshold);
      void set_value_above_threshold(double above_threshold);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] TImage apply(const TImage& img) const
      {
          TImage res(img);

          double thresh = _threshold;

          if (mode_is_relative())
          {
              const auto[itMinVal, itMaxVal] = std::minmax_element(img.begin(), img.end());
              const double range = static_cast<double>(*itMaxVal - *itMinVal);
              thresh = static_cast<double>(*itMinVal) + _threshold * range;
          }

          bk::threshold(res.begin(), res.end(), thresh, _value_below_threshold, _value_above_threshold);

          return res;
      }
      /// @}
  }; // class ThresholdImageFilter
} // namespace bk

#endif //BK_THRESHOLDIMAGEFILTER_H
