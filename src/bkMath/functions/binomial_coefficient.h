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

#ifndef BKMATH_BINOMIAL_COEFFICIENT_H
#define BKMATH_BINOMIAL_COEFFICIENT_H

#include <stdexcept>
#include <utility>

namespace bk
{
  [[nodiscard]] constexpr unsigned int binomial_coefficient(unsigned int n, unsigned int k)
  {
      if (k > n)
      { throw std::invalid_argument("binomial_coefficient requires k <= n"); }

      if (k == 0 || k == n)
      { return 1U; }

      const std::pair<unsigned int, unsigned int> minmax = k >= n - k ? std::make_pair(k, n - k) : std::make_pair(n - k, k);
      unsigned int nom = 1U;

      for (unsigned int i = n; i > minmax.first; --i)
      { nom *= i; }

      unsigned int denom = 1U;

      for (unsigned int i = minmax.second; i > 1U; --i)
      { denom *= i; }

      return nom == 0U || denom == 0U ? 0U : nom / denom;
  }
} // namespace bk

#endif // BKMATH_BINOMIAL_COEFFICIENT_H


