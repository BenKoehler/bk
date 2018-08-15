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

#ifndef BK_GRADIENTSTRENGTHIMAGEFILTER_H
#define BK_GRADIENTSTRENGTHIMAGEFILTER_H

#include <bkMath/functions/list_grid_id_conversion.h>

namespace bk
{
  class GradientStrengthImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GradientStrengthImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr GradientStrengthImageFilter() = default;
      constexpr GradientStrengthImageFilter(const self_type&) = default;
      constexpr GradientStrengthImageFilter(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GradientStrengthImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&& other) noexcept -> self_type& = default;
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

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          { res[i] = img.gradient_strength(bk::list_to_grid_id(img.size(), i)); }

          return res;
      }
      /// @}
  }; // class GradientStrengthImageFilter
} // namespace bk

#endif //BK_GRADIENTSTRENGTHIMAGEFILTER_H
