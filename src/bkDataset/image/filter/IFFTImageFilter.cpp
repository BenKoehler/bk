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

#include <bkDataset/image/filter/IFFTImageFilter.h>
#include <bkDataset/image/filter/FFTImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  IFFTImageFilter::IFFTImageFilter() = default;
  IFFTImageFilter::IFFTImageFilter(const self_type&) = default;
  IFFTImageFilter::IFFTImageFilter(self_type&&) noexcept = default;

  IFFTImageFilter::IFFTImageFilter(const FFTImageFilter& filter_fft)
  {
      set_padding_size(filter_fft);
      set_size_uneven(filter_fft);
  }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  IFFTImageFilter::~IFFTImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto IFFTImageFilter::operator=(const self_type& other) -> self_type& = default;
  auto IFFTImageFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  void IFFTImageFilter::set_padding_size(const FFTImageFilter& filter_fft)
  { set_padding_size(filter_fft.padding_size().begin(), filter_fft.padding_size().end()); }

  void IFFTImageFilter::set_size_uneven(const FFTImageFilter& filter_fft)
  { set_size_uneven(filter_fft.size_uneven().begin(), filter_fft.size_uneven().end()); }
} // namespace bk

