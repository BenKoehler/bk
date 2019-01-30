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

#include <bkDataset/image/filter/MaximumImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  MaximumImageFilter::MaximumImageFilter()
      : MaximumImageFilter(2, 3)
  { /* do nothing */ }

  MaximumImageFilter::MaximumImageFilter(const self_type& other) = default;
  MaximumImageFilter::MaximumImageFilter(self_type&& other) noexcept = default;

  MaximumImageFilter::MaximumImageFilter(unsigned int nDims, unsigned int size)
      : _kernel_size(nDims, size)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  MaximumImageFilter::~MaximumImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET KERNEL SIZE
  const std::vector<unsigned int>& MaximumImageFilter::kernel_size() const
  { return _kernel_size; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto MaximumImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto MaximumImageFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET KERNEL SIZE
  void MaximumImageFilter::set_kernel_size(unsigned int nDims, unsigned int size)
  { _kernel_size.assign(nDims, size); }
  /// @}
} // namespace bk
