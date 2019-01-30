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

#include <bkDataset/image/filter/FFTImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  FFTImageFilter::FFTImageFilter()
      : _off(),
        _orig_size_uneven(),
        _normalization(true)
  { /* do nothing */ }

  FFTImageFilter::FFTImageFilter(const self_type&) = default;
  FFTImageFilter::FFTImageFilter(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  FFTImageFilter::~FFTImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET PADDING SIZE
  const std::vector<unsigned int>& FFTImageFilter::padding_size() const
  { return _off; }

  const std::vector<bool>& FFTImageFilter::size_uneven() const
  { return _orig_size_uneven; }
  /// @}

  /// @{ -------------------------------------------------- SET NORMALIZATION
  bool FFTImageFilter::normalization_is_enabled() const
  { return _normalization; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto FFTImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto FFTImageFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NORMALIZATION
  void FFTImageFilter::set_normalization_enabled(bool b)
  { _normalization = b; }
  /// @}
} // namespace bk

