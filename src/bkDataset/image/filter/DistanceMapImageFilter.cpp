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

#include <bkDataset/image/filter/DistanceMapImageFilter.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DistanceMapImageFilter::DistanceMapImageFilter()
      : _val(1),
        _value_was_set(false)
  { /* do nothing */ }

  DistanceMapImageFilter::DistanceMapImageFilter(const self_type&) = default;
  DistanceMapImageFilter::DistanceMapImageFilter(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DistanceMapImageFilter::~DistanceMapImageFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TARGET VALUE
  [[nodiscard]] double DistanceMapImageFilter::value() const
  { return _val; }

  [[nodiscard]] bool DistanceMapImageFilter::custom_value_was_set() const
  { return _value_was_set; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto DistanceMapImageFilter::operator=(const self_type& other) -> self_type& = default;
  [[maybe_unused]] auto DistanceMapImageFilter::operator=(self_type&& other) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET VALUE
  //! distance to pixels with this value will be determined
  void DistanceMapImageFilter::set_value(double val)
  {
      _val = val;
      _value_was_set = true;
  }
  /// @}
} // namespace bk