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

#include <bkDataset/image/filter/HistogramEqualizationImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  HistogramEqualizationImageFilter::HistogramEqualizationImageFilter()
      : _num_buckets(5000)
  { /* do nothing */ }

  HistogramEqualizationImageFilter::HistogramEqualizationImageFilter(const self_type&) = default;
  HistogramEqualizationImageFilter::HistogramEqualizationImageFilter(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  HistogramEqualizationImageFilter::~HistogramEqualizationImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM BUCKETS
  unsigned int HistogramEqualizationImageFilter::num_buckets() const
  { return _num_buckets; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto HistogramEqualizationImageFilter::operator=(const self_type& other) -> self_type& = default;
  [[maybe_unused]] auto HistogramEqualizationImageFilter::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NUM BUCKETS
  void HistogramEqualizationImageFilter::set_num_buckets(unsigned int numBuckets)
  { _num_buckets = numBuckets; }
  /// @}
} // namespace bk

