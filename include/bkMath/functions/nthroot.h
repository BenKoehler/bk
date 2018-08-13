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

#ifndef BKMATH_NTHROOT_H
#define BKMATH_NTHROOT_H

#include <algorithm>
#include <limits>

#include "equals_approx.h"

namespace bk
{
  namespace details
  {
    constexpr double _nthroot(double x, unsigned int n, double guess, double answer, double precision, int iter = 0) noexcept
    {
        if (bk::equals_approx(answer, guess, precision))
        {
            if (iter > 1) // safety iteration; otherwise sqrt(2) does not work
            { return answer; }
            else
            { return _nthroot(x, n, answer, 1.0 / n * ((n - 1) * guess + x / std::pow(guess, n - 1)), precision, ++iter); }
        }
        else
        { return _nthroot(x, n, answer, 1.0 / n * ((n - 1) * guess + x / std::pow(guess, n - 1)), precision); }
    }
  } // namespace details

  constexpr double nthroot(double x, unsigned int n) noexcept
  {
      const unsigned int nui = std::max(n, 1U);
      constexpr const double precision = std::pow(10.0, -std::numeric_limits<double>::digits10);

      return details::_nthroot(x, nui, x, x / n, precision);
  }
} // namespace bk

#endif // BKMATH_NTHROOT_H


