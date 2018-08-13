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

#ifndef BK_HISTOGRAMEQUALIZATIONIMAGEFILTER_H
#define BK_HISTOGRAMEQUALIZATIONIMAGEFILTER_H

#include <type_traits>

#include "../../../bkAlgorithm/histogram_equalization.h"

#include "../../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT HistogramEqualizationImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = HistogramEqualizationImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      unsigned int _num_buckets;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      HistogramEqualizationImageFilter();
      HistogramEqualizationImageFilter(const self_type& other);
      HistogramEqualizationImageFilter(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~HistogramEqualizationImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM BUCKETS
      [[nodiscard]] unsigned int num_buckets() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET NUM BUCKETS
      void set_num_buckets(unsigned int numBuckets);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] TImage apply(const TImage& img) const
      {
          static_assert(std::is_arithmetic_v<typename TImage::value_type>, "only for images with arithmetic value type");

          if (_num_buckets < 1)
          { return img; }

          TImage res(img);

          bk::histogram_equalization(res.begin(), res.end(), _num_buckets);

          return res;
      }
      /// @}
  }; // class HistogramEqualizationImageFilter
} // namespace bk

#endif //BK_HISTOGRAMEQUALIZATIONIMAGEFILTER_