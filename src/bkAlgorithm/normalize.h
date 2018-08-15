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

#ifndef BK_ALGORITHMS_NORMALIZE_H
#define BK_ALGORITHMS_NORMALIZE_H

#include <algorithm>
#include <cmath>
#if defined(__GNUC__) && defined(_OPENMP)
    #include <parallel/algorithm>
#endif

#include <utility>
#include <type_traits>

#include <bkAlgorithm/sum.h>

namespace bk
{
  /// @{ -------------------------------------------------- NORM
  //! get norm/length of range (sqrt of sum of squared elements)
  template<typename TIter, typename T>
  [[nodiscard]] auto norm(TIter first, TIter last, T zero)
  { return std::sqrt(bk::sum_of_squared_elements(first, last, zero)); }

  template<typename TIter>
  [[nodiscard]] auto norm(TIter first, TIter last)
  { return norm(std::move(first), std::move(last), std::decay_t<decltype(*first)>()); }
  /// @}

  /// @{ -------------------------------------------------- NORMALIZE TO LENGTH
  template<typename TIter, typename T, typename TCompare>
  void normalize_to_length(TIter first, TIter last, T length, TCompare compare)
  {
      const auto n = bk::norm(first, last);

      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      { x *= length / n; });
  }

  template<typename TIter, typename T>
  void normalize_to_length(TIter first, TIter last, T length)
  { normalize_to_length(std::move(first), std::move(last), length, std::less<std::decay_t<decltype(*first)>>()); }
  /// @}

  /// @{ -------------------------------------------------- NORMALIZE
  template<typename TIter, typename TCompare>
  void normalize(TIter first, TIter last, TCompare compare)
  { normalize_to_length(std::move(first), std::move(last), 1, compare); }

  template<typename TIter>
  void normalize(TIter first, TIter last)
  { normalize(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first)>>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_NORMALIZE_H


