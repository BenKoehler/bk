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

#ifndef BK_ALGORITHMS_SUM_H
#define BK_ALGORITHMS_SUM_H

#if defined(__GNUC__) && defined(_OPENMP)
    #include <parallel/numeric>
#else
    #include <numeric>
#endif

#include <type_traits>

namespace bk
{
  /// @{ -------------------------------------------------- SUM
  template<typename TIter, typename T>
  [[nodiscard]] T sum(TIter first, TIter last, T zero)
  {
      return
          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::accumulate(std::move(first), std::move(last), zero);
  }

  template<typename TIter>
  [[nodiscard]] auto sum(TIter first, TIter last) -> std::decay_t<decltype(*first)>
  { return sum(std::move(first), std::move(last), std::decay_t<decltype(*first)>()); }
  /// @}

  /// @{ -------------------------------------------------- SUM OF SQUARED ELEMENTS
  template<typename TIter, typename T>
  [[nodiscard]] T sum_of_squared_elements(TIter first, TIter last, T zero)
  {
      return
          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::accumulate(std::move(first), std::move(last), zero, [](const auto& x, const auto& y)
          { return x + y * y; });
  }

  template<typename TIter>
  [[nodiscard]] auto sum_of_squared_elements(TIter first, TIter last) -> std::decay_t<decltype(*first)>
  { return sum_of_squared_elements(std::move(first), std::move(last), std::decay_t<decltype(*first)>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_SUM_H


