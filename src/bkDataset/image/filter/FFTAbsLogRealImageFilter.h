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

#ifndef BK_FFTABSLOGREALIMAGEFILTER_H
#define BK_FFTABSLOGREALIMAGEFILTER_H

#include <cmath>
#include <complex>

#include <bkTypeTraits/complex_traits.h>

namespace bk
{
  class FFTAbsLogRealImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = FFTAbsLogRealImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      FFTAbsLogRealImageFilter() = default;
      FFTAbsLogRealImageFilter(const self_type&) = default;
      FFTAbsLogRealImageFilter(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~FFTAbsLogRealImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] static typename TImage::template self_template_type<double> apply(const TImage& img)
      {
          using value_type = typename TImage::value_type;

          typename TImage::template self_template_type<double> res;
          res.set_size(img.size());

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          {
              if (bk::is_complex_v<value_type>)
              { res[i] = std::log(std::abs(img[i].real())); }
              else
              { res[i] = std::log(std::abs(img[i])); }
          }

          return res;
      }
      /// @}
  }; // class FFTAbsLogRealImageFilter
} // namespace bk

#endif //BK_FFTABSLOGREALIMAGEFILTER_H
