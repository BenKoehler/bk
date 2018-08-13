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

#ifndef BK_ALGORITHMS_VARIANCE_H
#define BK_ALGORITHMS_VARIANCE_H

#include <type_traits>

#include "mean.h"

namespace bk
{
  template<typename TIter, typename T>
  [[nodiscard]] T variance(TIter first, TIter last, T zero)
  {
      const auto m = mean(first, last);
      double N = 0;

      for (; first != last; ++first, ++N)
      {
          const auto temp = *first - m;
          zero += (temp * temp);
      }

      return zero / N;
  }

  template<typename TIter>
  [[nodiscard]] auto variance(TIter first, TIter last)
  {
      using value_type = std::decay_t<decltype(*first)>;
      using variance_type = std::conditional_t<std::is_arithmetic_v<value_type>, std::conditional_t<std::is_floating_point_v<value_type>, value_type, double>, value_type>;

      return variance(std::move(first), std::move(last), variance_type());
  }
} // namespace bk

#endif //BK_ALGORITHMS_VARIANCE_H


