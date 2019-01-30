/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BK_ALGORITHMS_HISTOGRAM_EQUALIZATION_H
#define BK_ALGORITHMS_HISTOGRAM_EQUALIZATION_H

#include <algorithm>
#if defined(__GNUC__) && defined(_OPENMP)
    #include <parallel/algorithm>
#endif

#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

#include <bkAlgorithm/histogram.h>

namespace bk
{
  /// @{ -------------------------------------------------- HISTOGRAM EQUALIZATION
  //! performs histogram equalization on the given range
  template<typename TIter, typename T, typename TCompare>
  void histogram_equalization(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, unsigned int num_buckets, TCompare compare)
  {
      using value_type = std::decay_t<decltype(*first)>;

      const unsigned int N = std::distance(first, last);

      // histogram
      const auto h = bk::histogram_n(first, last, fixedHistogramMin, fixedHistogramMax, num_buckets, compare);
      const value_type range = fixedHistogramMax - fixedHistogramMin;

      // cumulative histogram
      std::vector<double> hc(num_buckets);
      auto hit = std::begin(h);
      hc[0] = hit->second;
      ++hit;
      for (unsigned int i = 1; i < num_buckets; ++i, ++hit)
      {
          hc[i] = hc[i - 1] + hit->second;
          hc[i - 1] /= N;
      }

      hc[num_buckets - 1] /= N;

      const double hcmin = hc.front();
      const double hcmax = hc.back();

      const double hrange = hcmax - hcmin;

      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      {
          const unsigned int bucketId = static_cast<unsigned int>(std::round(static_cast<double>(num_buckets - 1) * (x - fixedHistogramMin) / range));
          x = fixedHistogramMin + range * (hc[bucketId] - hcmin) / hrange;
      });
  }

  template<typename TIter, typename T>
  void histogram_equalization(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, unsigned int num_buckets = 1000U)
  { histogram_equalization(std::move(first), std::move(last), fixedHistogramMin, fixedHistogramMax, num_buckets, std::less < std::decay_t < decltype(*first) >> ()); }

  template<typename TIter>
  void histogram_equalization(TIter first, TIter last, unsigned int num_buckets = 1000U)
  {
      auto compare = std::less < std::decay_t < decltype(*first) >> ();
      auto[itMinElement, itMaxElement] = std::minmax_element(first, last, compare);
      histogram_equalization(std::move(first), std::move(last), *itMinElement, *itMaxElement, num_buckets, compare);
  }

  //template<typename TIter, typename TCompare>
  //void histogram_equalization(TIter first, TIter last, unsigned int num_buckets, TCompare compare)
  //{
  //    using value_type = std::decay_t<decltype(*first)>;
  //
  //    const unsigned int N = std::distance(first, last);
  //
  //    // histogram
  //    const auto h = bk::histogram_n(first, last, num_buckets, compare);
  //
  //    auto[itMinElement, itMaxElement] = std::minmax_element(first, last, compare);
  //    const value_type minValue = *itMinElement;
  //    const value_type maxValue = *itMaxElement;
  //    const value_type range = maxValue - minValue;
  //
  //    // cumulative histogram
  //    std::vector<double> hc(num_buckets);
  //    auto hit = std::begin(h);
  //    hc[0] = hit->second;
  //    ++hit;
  //    for (unsigned int i = 1; i < num_buckets; ++i, ++hit)
  //    {
  //        hc[i] = hc[i - 1] + hit->second;
  //        hc[i - 1] /= N;
  //    }
  //
  //    hc[num_buckets - 1] /= N;
  //
  //    const double hcmin = hc.front();
  //    const double hcmax = hc.back();
  //
  //    const double hrange = hcmax - hcmin;
  //
  //    #if defined(__GNUC__) && defined(_OPENMP)
  //    __gnu_parallel
  //    #else
  //    std
  //    #endif
  //    ::for_each(first, last, [&](auto& x)
  //    {
  //        const unsigned int bucketId = static_cast<unsigned int>(std::round(static_cast<double>(num_buckets - 1) * (x - minValue) / range));
  //        x = minValue + range * (hc[bucketId] - hcmin) / hrange;
  //    });
  //}

  //template<typename TIter>
  //void histogram_equalization(TIter first, TIter last, unsigned int num_buckets = 1000U)
  //{ histogram_equalization(std::move(first), std::move(last), num_buckets, std::less<std::decay_t<decltype(*first)>>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_HISTOGRAM_EQUALIZATION_H


