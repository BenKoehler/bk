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

#ifndef BK_ALGORITHMS_OTSU_H
#define BK_ALGORITHMS_OTSU_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>
#else
    #include <algorithm>
#endif

#include <iterator>

#include <bkAlgorithm/histogram.h>

namespace bk
{
  template<typename TIter>
  [[nodiscard]] auto otsu(TIter first, TIter last, unsigned int num_buckets = 256U)
  {
      const unsigned int N = std::distance(first, last);

      double threshold = 0.0;
      double var_max = 0.0;
      double sum = 0.0;
      double sumB = 0.0;
      double q1 = 0.0;

      const auto hist = bk::histogram_n(first, last, num_buckets);

      auto histit = std::cbegin(hist);
      for (unsigned int i = 0; i < num_buckets; ++i, ++histit)
      { sum += i * histit->second; }

      histit = std::cbegin(hist);
      for (unsigned int t = 0; t < num_buckets; ++t, ++histit)
      {
          q1 += (*histit).second;

          if (q1 == 0)
          { continue; }

          sumB += t * (*histit).second;
          const double q2 = N - q1;
          const double temp = (sumB / q1) - (sum - sumB) / q2;
          const double sigma2 = q1 * q2 * temp * temp;

          if (sigma2 > var_max)
          {
              threshold = t;
              var_max = sigma2;
          }
      }

      const auto[itMinElement, itMaxElement] = std::minmax_element(first, last);

      return *itMinElement + (threshold / num_buckets) * (*itMaxElement - *itMinElement);
  }
} // namespace bk

#endif //BK_ALGORITHMS_OTSU_H


