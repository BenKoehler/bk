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

#ifndef BK_ALGORITHMS_SCALE_H
#define BK_ALGORITHMS_SCALE_H

#include <algorithm>

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>

#endif

#include <utility>
#include <type_traits>

namespace bk
{
  /// @{ -------------------------------------------------- SCALE TO RANGE
  template<typename TIter, typename T, typename TCompare>
  void scale_to_range(TIter first, TIter last, T newMinValue, T newMaxValue, TCompare compare)
  {
      const auto[itMinElement, itMaxElement] = std::minmax_element(first, last, compare);
      const auto minval = *itMinElement;
      const auto range = *itMaxElement - minval;
      const auto newRange = newMaxValue - newMinValue;

      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      { x = newMinValue + newRange * (x - minval) / range; });
  }

  template<typename TIter, typename T>
  void scale_to_range(TIter first, TIter last, T newMinValue, T newMaxValue)
  { scale_to_range(std::move(first), std::move(last), newMinValue, newMaxValue, std::less<std::decay_t<decltype(*first)>>()); }
  /// @}

  /// @{ -------------------------------------------------- SCALE BY FACTOR
  template<typename TIter, typename T>
  void scale_by_factor(TIter first, TIter last, T factor)
  {
      #if defined(__GNUC__) && defined(_OPENMP)
      __gnu_parallel
      #else
      std
      #endif
      ::for_each(first, last, [&](auto& x)
      { x *= factor; });
  }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_SCALE_H