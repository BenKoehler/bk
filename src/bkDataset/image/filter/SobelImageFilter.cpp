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

#include <bkDataset/image/filter/SobelImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  SobelImageFilter::SobelImageFilter()
      : SobelImageFilter(2, 3, 0)
  { /* do nothing */ }

  SobelImageFilter::SobelImageFilter(const self_type& other) = default;
  SobelImageFilter::SobelImageFilter(self_type&& other) = default;

  SobelImageFilter::SobelImageFilter(unsigned int nDims, unsigned int size, unsigned int targetDimId)
      : _kernel_size(nDims, size),
        _target_dimension(targetDimId)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  SobelImageFilter::~SobelImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET KERNEL SIZE
  const std::vector<unsigned int>& SobelImageFilter::kernel_size() const
  { return _kernel_size; }
  /// @}

  /// @{ -------------------------------------------------- GET TARGET DIMENSION ID
  unsigned int SobelImageFilter::target_dimension_id() const
  { return _target_dimension; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto SobelImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto SobelImageFilter::operator=(self_type&& other) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET KERNEL SIZE
  void SobelImageFilter::set_kernel_size(unsigned int nDims, unsigned int size)
  { _kernel_size.assign(nDims, size); }
  /// @}

  /// @{ -------------------------------------------------- SET TARGET DIMENSION ID
  void SobelImageFilter::set_target_dimension_id(unsigned int id)
  { _target_dimension = id; }
  /// @}

} // namespace bk
