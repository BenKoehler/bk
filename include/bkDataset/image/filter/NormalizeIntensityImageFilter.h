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

#pragma once

#ifndef BK_NORMALIZEINTENSITYIMAGEFILTER_H
#define BK_NORMALIZEINTENSITYIMAGEFILTER_H

#include <algorithm>

namespace bk
{
  class NormalizeIntensityImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = NormalizeIntensityImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr NormalizeIntensityImageFilter() = default;
      constexpr NormalizeIntensityImageFilter(const self_type&) = default;
      constexpr NormalizeIntensityImageFilter(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~NormalizeIntensityImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&& other) -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] static typename TImage::template self_template_type<double> apply(const TImage& img)
      {
          typename TImage::template self_template_type<double> res;
          res.set_size(img.size());

          auto[itMinVal, itMaxVal] = std::minmax_element(img.begin(), img.end());
          const auto range = *itMaxVal - *itMinVal;

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          { res[i] = (img[i] - *itMinVal) / range; }

          return res;
      }
      /// @}
  }; // class NormalizeIntensityImageFilter
} // namespace bk

#endif //BK_NORMALIZEINTENSITYIMAGEFILTER_H
