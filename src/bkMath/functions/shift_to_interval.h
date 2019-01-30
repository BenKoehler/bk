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

#ifndef BKMATH_SHIFT_TO_INTERVAL_H
#define BKMATH_SHIFT_TO_INTERVAL_H

#include <cassert>
#include <cmath>

namespace bk
{
  //! shift value into interval; e.g. useful for periodic functions such as sin/cos
  template<typename T>
  constexpr auto shift_to_interval(T x, T lower_bound, T upper_bound) noexcept
  {
      assert(lower_bound <= upper_bound && "shift_to_interval requires lower_bound <= upper_bound");
      return std::fmod(x - lower_bound, upper_bound - lower_bound) + lower_bound;
  }
} // namespace bk

#endif // BKMATH_SHIFT_TO_INTERVAL_H


