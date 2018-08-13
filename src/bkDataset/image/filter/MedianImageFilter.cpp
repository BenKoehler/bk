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

#include "../../../../include/bkDataset/image/filter/MedianImageFilter.h"

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  MedianImageFilter::MedianImageFilter()
      : MedianImageFilter(2, 3)
  { /* do nothing */ }

  MedianImageFilter::MedianImageFilter(const self_type& other) = default;
  MedianImageFilter::MedianImageFilter(self_type&& other) = default;

  MedianImageFilter::MedianImageFilter(unsigned int nDims, unsigned int size)
      : _kernel_size(nDims, size)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  MedianImageFilter::~MedianImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET KERNEL SIZE
  const std::vector<unsigned int>& MedianImageFilter::kernel_size() const
  { return _kernel_size; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto MedianImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto MedianImageFilter::operator=(self_type&& other) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET KERNEL SIZE
  void MedianImageFilter::set_kernel_size(unsigned int nDims, unsigned int size)
  { _kernel_size.assign(nDims, size); }
  /// @}
} // namespace bk
