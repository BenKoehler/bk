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

#include <bkDataset/image/filter/UnsharpMaskingImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  UnsharpMaskingImageFilter::UnsharpMaskingImageFilter()
      : UnsharpMaskingImageFilter(1, 2, 3)
  { /* do nothing */ }

  UnsharpMaskingImageFilter::UnsharpMaskingImageFilter(const self_type& other) = default;
  UnsharpMaskingImageFilter::UnsharpMaskingImageFilter(self_type&& other) noexcept = default;

  UnsharpMaskingImageFilter::UnsharpMaskingImageFilter(unsigned int numIterations, unsigned int nDims, unsigned int size)
      : _num_iterations(numIterations),
        _kernel_size(nDims, size)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UnsharpMaskingImageFilter::~UnsharpMaskingImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM ITERATIONS
  unsigned int UnsharpMaskingImageFilter::num_iterations() const
  { return _num_iterations; }
  /// @}

  /// @{ -------------------------------------------------- GET KERNEL SIZE
  const std::vector<unsigned int>& UnsharpMaskingImageFilter::kernel_size() const
  { return _kernel_size; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UnsharpMaskingImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto UnsharpMaskingImageFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NUM ITERATIONS
  void UnsharpMaskingImageFilter::set_num_iterations(unsigned int numIterations)
  { _num_iterations = numIterations; }
  /// @}

  /// @{ -------------------------------------------------- SET KERNEL SIZE
  void UnsharpMaskingImageFilter::set_kernel_size(unsigned int nDims, unsigned int size)
  { _kernel_size.assign(nDims, size); }
  /// @}
} // namespace bk
