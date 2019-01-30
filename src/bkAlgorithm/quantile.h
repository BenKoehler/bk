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

#ifndef BK_ALGORITHMS_QUANTILE_H
#define BK_ALGORITHMS_QUANTILE_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>
#else
    #include <algorithm>
#endif

#include <cmath>
#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

namespace bk
{
  /// @{ -------------------------------------------------- QUANTILE
  //! get value at p% of the sorted range
  /*!
   * If the range is not already sorted, a local copy will be created.
   * The original range is not altered.
   */
  template<typename TIter, typename TCompare>
  [[nodiscard]] auto quantile(TIter first, TIter last, double p, unsigned int N, TCompare compare) -> std::decay_t<decltype(*first)>
  {
      const unsigned int id = std::min(static_cast<unsigned int>(std::floor(p * N)), N - 1);

      if (!std::is_sorted(first, last))
      {
          using value_type = std::decay_t<decltype(*first)>;
          std::vector<value_type> temp(std::move(first), std::move(last));

          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::nth_element(temp.begin(), temp.begin() + id, temp.end(), compare);

          return temp[id];
      }
      else
      { return *(first + id); }
  }

  template<typename TIter>
  [[nodiscard]] auto quantile(TIter first, TIter last, double p, unsigned int N) -> std::decay_t<decltype(*first)>
  { return quantile(std::move(first), std::move(last), p, N, std::less<std::decay_t<decltype(*first) >>()); }

  template<typename TIter, typename TCompare, std::enable_if_t<std::is_class_v<TCompare>>* = nullptr>
  [[nodiscard]] auto quantile(TIter first, TIter last, double p, TCompare compare) -> std::decay_t<decltype(*first)>
  { return quantile(first, last, p, std::distance(first, last), compare); }

  template<typename TIter>
  [[nodiscard]] auto quantile(TIter first, TIter last, double p) -> std::decay_t<decltype(*first)>
  { return quantile(first, last, p, std::distance(first, last), std::less<std::decay_t<decltype(*first) >>()); }
  /// @}

  /// @{ -------------------------------------------------- INTERQUARTILE RANGE
  //! get quantiles at 25% and 75%
  template<typename TIter, typename TCompare>
  [[nodiscard]] auto interquartile_range(TIter first, TIter last, TCompare compare) -> std::pair<std::decay_t<decltype(*first)>, std::decay_t<decltype(*first)>>
  { return std::make_pair(quantile(first, last, 0.25, compare), quantile(first, last, 0.75, compare)); }

  template<typename TIter>
  [[nodiscard]] auto interquartile_range(TIter first, TIter last) -> std::pair<std::decay_t<decltype(*first)>, std::decay_t<decltype(*first)>>
  { return interquartile_range(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first) >>()); }
  /// @}

  /// @{ -------------------------------------------------- MEDIAN
  //! get 50% quantile
  template<typename TIter, typename TCompare>
  [[nodiscard]] auto median(TIter first, TIter last, TCompare compare) -> std::decay_t<decltype(*first)>
  { return quantile(std::move(first), std::move(last), 0.5, std::move(compare)); }

  template<typename TIter>
  [[nodiscard]] auto median(TIter first, TIter last) -> std::decay_t<decltype(*first)>
  { return median(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first) >>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_QUANTILE_H


