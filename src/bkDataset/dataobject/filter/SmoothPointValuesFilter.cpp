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

#include <bkDataset/dataobject/filter/SmoothPointValuesFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  SmoothPointValuesFilter::SmoothPointValuesFilter()
      : _num_iterations(50),
        _lambda(0.5),
        _mu(-0.5)
  { /* do nothing */ }

  SmoothPointValuesFilter::SmoothPointValuesFilter(const self_type& other) = default;
  SmoothPointValuesFilter::SmoothPointValuesFilter(self_type&& other) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  SmoothPointValuesFilter::~SmoothPointValuesFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM ITERATIONS
  unsigned int SmoothPointValuesFilter::num_iterations() const
  { return _num_iterations; }
  /// @}

  /// @{ -------------------------------------------------- GET LAMBDA
  double SmoothPointValuesFilter::lambda() const
  { return _lambda; }
  /// @}

  /// @{ -------------------------------------------------- GET MU
  double SmoothPointValuesFilter::mu() const
  { return _mu; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto SmoothPointValuesFilter::operator=(const self_type& other) -> self_type& = default;
  auto SmoothPointValuesFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NUM ITERATIONS
  void SmoothPointValuesFilter::set_num_iterations(unsigned int numIterations)
  { _num_iterations = numIterations; }
  /// @}

  /// @{ -------------------------------------------------- SET LAMBDA
  void SmoothPointValuesFilter::set_lambda(double lambda)
  { _lambda = lambda; }
  /// @}

  /// @{ -------------------------------------------------- SET MU
  void SmoothPointValuesFilter::set_mu(double mu)
  { _mu = mu; }
  /// @}
} // namespace bk
