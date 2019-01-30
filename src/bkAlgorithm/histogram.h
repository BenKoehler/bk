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

#ifndef BK_ALGORITHMS_HISTOGRAM_H
#define BK_ALGORITHMS_HISTOGRAM_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>
#else
    #include <algorithm>
#endif

#include <cmath>
#include <exception>
#include <functional>
#include <map>
#include <type_traits>
#include <utility>

namespace bk
{
  /// @{ -------------------------------------------------- HISTOGRAM
  //! counts the number of occurrences of each value
  /*!
   * - default comparison is operator<
   *
   * Example: {0,0,1,2,2,6,9} -> map contains: ((0,2) , (1,1) , (2,2) , (6,1) , (9,1))
   *  -> subsequent keys in the map do not necessarily have the same difference!
   *
   * \return a map<value_type, unsigned int> where the second value is the number of occurrences and the first is the corresponding value
   */
  template<typename TIter, typename TCompare>
  [[nodiscard]] auto histogram(TIter first, TIter last, TCompare) -> std::map<std::decay_t<decltype(*first)>, unsigned int, TCompare>
  {
      std::map<std::decay_t<decltype(*first)>, unsigned int, TCompare> h;

      std::for_each(first, last, [&](const auto& x)
      { ++h.try_emplace(x, 0).first->second; });

      return h;
  }

  template<typename TIter>
  [[nodiscard]] auto histogram(TIter first, TIter last)
  { return histogram(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first) >>()); }
  /// @}

  /// @{ -------------------------------------------------- HISTOGRAM WITH FIXED MIN/MAX AND FIXED NUM BUCKETS
  //! counts the number of occurrences of each value with a fixed min/max value and a fixed number of buckets
  /*!
   * For example, you have a grayscale image. The theoretical min/max is [0,255], but the real min/max is [7,243].
   * To make sure that the histogram goes from 0 to 255, these are passed as fixed min/max values.
   * The resulting map contains each possible value within the range, even it the number of occurences is 0.
   *
   * Example: {0,0,1,2,2,6,9} with fixedHistogramMin = 0 and fixedHistogramMax = 10 creates the following map:
   * ((0,2) , (1,1) , (2,2) , (3,0) , (4,0) , (5,0) , (6,1) , (7,0) , (8,0) , (9,1) , (10,0))
   *
   * \return a map<value_type, unsigned int> where the second value is the number of occurrences and the first is the corresponding value
   */
  template<typename TIter, typename T, typename TCompare>
  [[nodiscard]] auto histogram_n(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, unsigned int num_buckets, TCompare compare) -> std::map<std::conditional_t<std::is_floating_point_v<T>, T, double>, unsigned int>
  {
      if (num_buckets < 2)
      { throw std::invalid_argument("at least 2 buckets are required! (num_buckets >= 2)"); }

      if (!compare(fixedHistogramMin, fixedHistogramMax))
      { throw std::invalid_argument("compare(min,max) must evaluate to true. By default, min < max is required."); }

      using common_type = std::common_type_t<T, std::decay_t<decltype(*first)>>;
      using res_value_type = std::conditional_t<std::is_floating_point_v<common_type>, common_type, double>;

      std::map<res_value_type, unsigned int, TCompare> temp;

      const res_value_type range = fixedHistogramMax - fixedHistogramMin;

      std::for_each(first, last, [&](const auto& x)
      {
          const res_value_type x_clamped = std::clamp(static_cast<res_value_type>(x), static_cast<res_value_type>(fixedHistogramMin), static_cast<res_value_type>(fixedHistogramMax));

const auto y = static_cast<unsigned int>(std::round(static_cast<res_value_type>(num_buckets - 1) * static_cast<res_value_type>(x_clamped - fixedHistogramMin) / range));
          ++temp.try_emplace(y, 0U).first->second;
      });

      std::map<res_value_type, unsigned int> h;

      for (const auto&[x, n] : temp)
      { h.emplace(fixedHistogramMin + range * x / (num_buckets - 1), n); }

      for (unsigned int i = 0; i < num_buckets; ++i)
      { h.try_emplace(fixedHistogramMin + i * range / (num_buckets - 1), 0U); }

      return h;
  }

  template<typename TIter, typename T>
  [[nodiscard]] auto histogram_n(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, unsigned int num_buckets)
  { return histogram_n(std::move(first), std::move(last), fixedHistogramMin, fixedHistogramMax, num_buckets, std::less<std::decay_t<decltype(*first) >>()); }

  template<typename TIter, typename TCompare>
  [[nodiscard]] auto histogram_n(TIter first, TIter last, unsigned int num_buckets, TCompare compare)
  {
      auto[itMinElement, itMaxElement] = std::minmax_element(first, last, compare);
      return histogram_n(std::move(first), std::move(last), *itMinElement, *itMaxElement, num_buckets, compare);
  }

  template<typename TIter>
  [[nodiscard]] auto histogram_n(TIter first, TIter last, unsigned int num_buckets)
  { return histogram_n(std::move(first), std::move(last), num_buckets, std::less<std::decay_t<decltype(*first) >>()); }
  /// @}

  /// @{ -------------------------------------------------- HISTOGRAM WITH FIXED MIN/MAX AND FIXED PRECISION
  //! counts the number of occurrences of each value with a fixed min/max value and a fixed precision
  /*!
   * Precision is the difference between 2 subsequent elements in the histogram.
   *
   * For example, you have a normalized double image with floating point values between 0 and 1.
   * The theoretical min/max is [0,1], but the true min/max is [0.001234, 0.956].
   * Set the fixed min/max to [0,1] and the precision, e.g., to 0.01 to create a histogram with 100 buckets ((1-0)/0.01).
   * The resulting map contains each possible value within the range, even it the number of occurences is 0.
   *
   * Example: {0.0 , 0.0 , 1.0 , 2.5 , 2.0 , 3.0} with min/max = [0,4] and precision = 0.5 creates the following map:
   * ((0.0,2) , (0.5,0) , (1.0,1) , (1.5,0) , (2.0,1) , (2.5,1) , (3.0,1) , (3.5,0) , (4.0,0))
   *
   * \return a map<value_type, unsigned int> where the second value is the number of occurrences and the first is the corresponding value
   */
  template<typename TIter, typename T, typename TCompare>
  [[nodiscard]] auto histogram_n_precision(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, T precision, TCompare compare)
  {
      const unsigned int numBuckets = 1 + (fixedHistogramMax - fixedHistogramMin) / precision;
      return histogram_n(first, last, fixedHistogramMin, fixedHistogramMax, numBuckets, compare);
  }

  template<typename TIter, typename T>
  [[nodiscard]] auto histogram_n_precision(TIter first, TIter last, T fixedHistogramMin, T fixedHistogramMax, T precision)
  { return histogram_n_precision(first, last, fixedHistogramMin, fixedHistogramMax, precision, std::less<std::decay_t<decltype(*first) >>()); }
  /// @}

  /// @{ -------------------------------------------------- HISTOGRAM WITH FIXED PRECISION
  //! counts the number of occurrences of each value with a fixed precision between the occurring min/max value
  /*!
   * Precision is the difference between 2 subsequent elements in the histogram.
   *
   * For example, you have a double image with floating point values between 0 and 1.
   * Set the precision, e.g., to 0.01 to create a histogram with 100 buckets ((1-0)/0.01).
   * The resulting map contains each possible value within the range, even it the number of occurences is 0.
   *
   * Example: {0.0 , 0.0 , 1.0 , 2.5 , 2.0 , 3.0} with precision = 0.5 creates the following map:
   * ((0.0,2) , (0.5,0) , (1.0,1) , (1.5,0) , (2.0,1) , (2.5,1) , (3.0,1))
   *
   * \return a map<value_type, unsigned int> where the second value is the number of occurrences and the first is the corresponding value
   */
  template<typename TIter, typename T, typename TCompare>
  [[nodiscard]] auto histogram_precision(TIter first, TIter last, T precision, TCompare compare)
  {
      auto[itMinElement, itMaxElement] = std::minmax_element(first, last);
      const unsigned int numBuckets = 1 + (*itMaxElement - *itMinElement) / precision;
      return histogram_n(first, last, *itMinElement, *itMaxElement, numBuckets, compare);
  }

  template<typename TIter, typename T>
  [[nodiscard]] auto histogram_precision(TIter first, TIter last, T precision)
  { return histogram_precision(first, last, precision, std::less<std::decay_t<decltype(*first) >>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_HISTOGRAM_H


