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

#ifndef BK_ALGORITHMS_SMOOTH_H
#define BK_ALGORITHMS_SMOOTH_H

#include <algorithm>
#include <iterator>
#include <vector>

#include "../bkMath/functions/binomial_coefficient.h"

namespace bk
{
  /// @{ -------------------------------------------------- BINOMIAL SMOOTHING
  template<typename Iter, typename T = typename std::iterator_traits<Iter>::value_type>
  void smooth_binomial(Iter first, Iter last, unsigned int iterations, unsigned int kernel_size, T zero_val = T())
  {
      if (iterations == 0 || kernel_size < 2)
      { return; }

      const unsigned int N = std::distance(first, last);
      const unsigned int ks = kernel_size + (kernel_size % 2 == 0 ? 1 : 0);

      /*
       * binomial coefficients
       */
      std::vector<double> w(ks, 0);
      double wsum = 0;
      for (unsigned int i = 0; i < ks; ++i)
      {
          w[i] = binomial_coefficient(ks - 1, i);
          wsum += w[i];
      }

      for (unsigned int i = 0; i < ks; ++i)
      { w[i] /= wsum; }

      /*
       * temp data
       */
      std::vector<T> temp0(first, last);
      std::vector<T> temp1(temp0);
      std::vector<T>* write = nullptr;
      std::vector<T>* read = nullptr;

      /*
       * smoothing
       */
      for (unsigned int it = 0; it < iterations; ++it)
      {
          write = it % 2 == 0 ? &temp1 : &temp0;
          read = it % 2 == 0 ? &temp0 : &temp1;

          #pragma omp parallel for
          for (unsigned int i = ks / 2; i < N - ks / 2; ++i)
          {
              (*write)[i] = zero_val;

              for (unsigned int k = 0; k < ks; ++k)
              {
                  const int off = -static_cast<int>(ks / 2) + static_cast<int>(k);
                  (*write)[i] += (*read)[i + off] * w[k];
              }
          } // for i: N

          for (unsigned int i = 0; i < ks / 2; ++i)
          { (*write)[i] = (*read)[i]; }

          for (unsigned int i = N - ks / 2; i < N; ++i)
          { (*write)[i] = (*read)[i]; }
      } // for it: iterations

      /*
       * copy back
       */
      if (iterations % 2 != 0)
      { std::copy(write->begin(), write->end(), first); }
      else
      { std::copy(read->begin(), read->end(), first); }
  }
  /// @}

  /// @{ -------------------------------------------------- SMOOTH LAMBDA/MU
  template<typename Iter, typename T = typename std::iterator_traits<Iter>::value_type>
  void smooth_lambda_mu(Iter first, Iter last, unsigned int iterations, unsigned int kernel_size, double lambda, double mu, T zero_val = T())
  {
      if (iterations == 0 || kernel_size < 1 || (lambda == 0 && mu == 0))
      { return; }

      const unsigned int N = std::distance(first, last);
      const unsigned int ks = kernel_size + (kernel_size % 2 == 0 ? 1 : 0);

      /*
       * temp data
       */
      std::vector<T> temp0(first, last);
      std::vector<T> temp1(temp0);
      std::vector<T>* write = nullptr;
      std::vector<T>* read = nullptr;

      /*
       * smoothing
       */
      for (unsigned int it = 0; it < iterations; ++it)
      {
          write = it % 2 == 0 ? &temp1 : &temp0;
          read = it % 2 == 0 ? &temp0 : &temp1;

          const double w = (it % 2 == 0 ? lambda : mu);

          #pragma omp parallel for
          for (unsigned int i = ks / 2; i < N - ks / 2; ++i)
          {
              T mean = zero_val;
              for (unsigned int k = 0; k < ks / 2; ++k)
              {
                  const int off = -static_cast<int>(ks / 2) + static_cast<int>(k);
                  mean += (*read)[i + off];
              }

              for (unsigned int k = ks / 2 + 1; k < ks; ++k)
              {
                  const int off = -static_cast<int>(ks / 2) + static_cast<int>(k);
                  mean += (*read)[i + off];
              }

              mean /= (ks - 1);

              T diff = mean - (*read)[i];
              diff *= w;

              (*write)[i] = (*read)[i] + diff;
          } // for i: N

          for (unsigned int i = 0; i < ks / 2; ++i)
          { (*write)[i] = (*read)[i]; }

          for (unsigned int i = N - ks / 2; i < N; ++i)
          { (*write)[i] = (*read)[i]; }
      } // for it: iterations

      /*
       * copy back
       */
      if (iterations % 2 != 0)
      { std::copy(write->begin(), write->end(), first); }
      else
      { std::copy(read->begin(), read->end(), first); }
  }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_SMOOTH_H


