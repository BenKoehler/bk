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

#include <bkDataset/image/filter/ConnectedComponentsAnalysisImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ConnectedComponentAnalysisImageFilter::ConnectedComponentAnalysisImageFilter() = default;
  ConnectedComponentAnalysisImageFilter::ConnectedComponentAnalysisImageFilter(const self_type&) = default;
  ConnectedComponentAnalysisImageFilter::ConnectedComponentAnalysisImageFilter(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ConnectedComponentAnalysisImageFilter::~ConnectedComponentAnalysisImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM LABELS
  unsigned int ConnectedComponentAnalysisImageFilter::num_labels() const
  { return _labels.size(); }
  /// @}

  /// @{ -------------------------------------------------- GET LABELS
  const std::map<unsigned int, unsigned int>& ConnectedComponentAnalysisImageFilter::labels() const
  { return _labels; }
  /// @}

  /// @{ -------------------------------------------------- GET REGION SIZES
  unsigned int ConnectedComponentAnalysisImageFilter::num_pixels_with_label(unsigned int labelId) const
  {
      if (auto it = _labels.find(labelId); it != _labels.end())
      { return it->second; }

      return 0;
  }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto ConnectedComponentAnalysisImageFilter::operator=(const self_type&) -> self_type& = default;
  auto ConnectedComponentAnalysisImageFilter::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}
} // namespace bk