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

#ifndef BKMATH_EQUALS_APPROX_H
#define BKMATH_EQUALS_APPROX_H

#include <cmath>
#include <type_traits>

namespace bk
{
  template<typename T>
  constexpr T _precision_of()
  {
      if constexpr (std::is_same_v<T, float>)
      { return 1e-6; }
      else if constexpr (std::is_same_v<T, double>)
      { return 1e-12; }
      else if constexpr (std::is_same_v<T, long double>)
      { return 1e-13; }
      else // int types
      { return 0; }
  }

  template<typename TReal0, typename TReal1, typename TPrecision = std::common_type_t<TReal0, TReal1>>
  constexpr bool equals_approx(TReal0 x, TReal1 y, TPrecision precision = _precision_of<std::common_type_t<TReal0, TReal1>>()) noexcept
  {
      using common_value_type = std::common_type_t<TReal0, TReal1>;
      return std::abs(static_cast<common_value_type>(x) - static_cast<common_value_type>(y)) <= precision;
  }
} // namespace bk

#endif // BKMATH_EQUALS_APPROX_H