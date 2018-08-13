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

#ifndef BKMATH_BERNOULLI_NUMBER_H
#define BKMATH_BERNOULLI_NUMBER_H

#include <cassert>

#include "binomial_coefficient.h"

namespace bk
{
  // -------------------- forward declaration
  [[nodiscard]] constexpr double bernoulli_number(unsigned int n) noexcept;
  // -------------------- forward declaration END

  namespace details
  {
    [[nodiscard]] constexpr double _bernoulli_number(unsigned int k, unsigned int n) noexcept
    { return binomial_coefficient(n + 1, k) * bernoulli_number(k) + (k != 0 ? _bernoulli_number(k - 1, n) : 0); }
  } // namespace details

  //! Bernoulli number
  /*!
   * https://en.wikipedia.org/wiki/Bernoulli_number#Recursive_definition
   *
   * floating point arguments will be truncated
   */
  [[nodiscard]] constexpr double bernoulli_number(unsigned int n) noexcept
  {
      switch (n)
      {
          case 0: return 1;
          case 1: return 0.5; // +- 0.5, depends on implementation
      }

      if (n % 2) // odd
      { return 0; }

      switch (n)
      {
          case 2: return 1.6666666666666666666666666666666666666666666666666666666666666666666666e-1; // 1/6
          case 4: return -3.3333333333333333333333333333333333333333333333333333333333333333333333e-2; // −1/30
          case 6: return 2.3809523809523809523809523809523809523809523809523809523809523809523809e-2; // 1/42
          case 8: return -3.3333333333333333333333333333333333333333333333333333333333333333333333e-2; // −1/30
          case 10: return 7.5757575757575757575757575757575757575757575757575757575757575757575757e-2; // 5/66
          case 12: return -2.5311355311355311355311355311355311355311355311355311355311355311355311e-1; // −691/2730
          case 14: return 1.1666666666666666666666666666666666666666666666666666666666666666666666; // 7/6
          case 16: return -7.0921568627450980392156862745098039215686274509803921568627450980392156; // −3617/510
          case 18: return 54.971177944862155388471177944862155388471177944862155388471177944862155; // 43867/798
          case 20: return -529.12424242424242424242424242424242424242424242424242424242424242424242; // −174611/330
          case 22: return 6192.12318840579710144927536231884057971014492753623188405797101449275362; // 854513/138
          case 24: return -86580.2531135531135531135531135531135531135531135531135531135531135531135; // −236364091/2730
          case 26: return 1.42551716666666666666666666666666666666666666666666666666666666666666666e6; // 8553103/6
          case 28: return -2.72982310678160919540229885057471264367816091954022988505747126436781609e7; // −23749461029/870
          case 30: return 6.015808739006423683843038681748359167714006423683843038681748359167714006e8; // 8615841276005/14322
      }

      return -1.0 / (n + 1) * details::_bernoulli_number(n - 1, n);
  }
} // namespace bk

#endif // BKMATH_BERNOULLI_NUMBER_H