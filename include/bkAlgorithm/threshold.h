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

#ifndef BK_ALGORITHMS_THRESHOLD_H
#define BK_ALGORITHMS_THRESHOLD_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>
#else
    #include <algorithm>
#endif

#include <functional>
#include <type_traits>

#include "otsu.h"

namespace bk
{
  /// @{ -------------------------------------------------- THRESHOLD
  template<typename TIter, typename T, typename TCompare>
  void threshold(TIter first, TIter last, T thresh, T value_below, T value_above, TCompare compare)
  {
      using value_type = std::decay_t<decltype(*first)>;
      using common_type = std::common_type_t<value_type, std::decay_t<T>>;

      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      { x = compare(static_cast<common_type>(x), static_cast<common_type>(thresh)) ? value_below : value_above; });
      //{ x = compare(x, thresh) ? value_below : value_above; });
  }

  template<typename TIter, typename T>
  void threshold(TIter first, TIter last, T thresh, T value_below, T value_above)
  { threshold(std::move(first), std::move(last), thresh, value_below, value_above, std::less<std::decay_t<decltype(*first)>>()); }

  template<typename TIter, typename T>
  void threshold(TIter first, TIter last, T thresh)
  { threshold(std::move(first), std::move(last), thresh, static_cast<T>(0), static_cast<T>(1)); }

  template<typename TIter>
  void threshold(TIter first, TIter last)
  { threshold(first, last, bk::otsu(first, last)); }
  /// @}

  /// @{ -------------------------------------------------- INTERVAL THRESHOLD
  template<typename TIter, typename T, typename TCompare>
  void interval_threshold(TIter first, TIter last, T thresh_lower, T thresh_upper, T value_inside, T value_outside, TCompare compare)
  {
      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      { x = compare(thresh_lower, x) && !compare(thresh_upper, x) ? value_inside : value_outside; });
  }

  template<typename TIter, typename T>
  void interval_threshold(TIter first, TIter last, T thresh_lower, T thresh_upper, T value_inside, T value_outside)
  { interval_threshold(std::move(first), std::move(last), thresh_lower, thresh_upper, value_inside, value_outside, std::less<std::decay_t<decltype(*first)>>()); }

  template<typename TIter, typename T>
  void interval_threshold(TIter first, TIter last, T thresh_lower, T thresh_upper)
  { interval_threshold(std::move(first), std::move(last), thresh_lower, thresh_upper, static_cast<T>(1), static_cast<T>(0)); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_THRESHOLD_H


