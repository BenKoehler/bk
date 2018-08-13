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

#ifndef BKMATH_NUM_DIGITS_H
#define BKMATH_NUM_DIGITS_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

#include "../../bkTypeTraits/floating_point.h"

namespace bk
{
  namespace details
  {
    template<typename T>
    constexpr unsigned int _num_digits_int(T i) noexcept
    { return (i < 10 ? 1 : 1 + _num_digits_int(i / 10)); }

    template<typename T>
    constexpr unsigned int _num_digits_after_comma(T x) noexcept
    {
        if constexpr (bk::is_floating_point_v<T>)
        {
            const T frac = x - std::floor(x);
            return frac != 0 ? 1 + _num_digits_after_comma(frac * 10) : 0;
        }
        else // if constexpr (std::is_integral_v<T>)
        { return 0; }
    }
  } // namespace details

  template<typename T>
  constexpr unsigned int num_digits_int(T i) noexcept
  {
      if constexpr (std::is_signed_v<T>)
      { return (std::abs(i) < 10 ? 1 : 1 + details::_num_digits_int(std::abs(i) / 10)); }
      else
      { return (i < 10 ? 1 : 1 + details::_num_digits_int(i / 10)); }
  }

  template<typename T>
  constexpr unsigned int num_digits_before_comma(T i) noexcept
  { return num_digits_int(i); }

  template<typename T>
  constexpr unsigned int num_digits_after_comma(T x) noexcept
  { return std::min(static_cast<unsigned int>(std::numeric_limits<T>::digits10), details::_num_digits_after_comma(x)); }
} // namespace bk

#endif // BKMATH_NUM_DIGITS_H


