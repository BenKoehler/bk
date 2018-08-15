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

#ifndef BK_IFFTSHIFTIMAGEFILTER_H
#define BK_IFFTSHIFTIMAGEFILTER_H

#include <cassert>
#include <complex>
#include <vector>

#include <bk/FFT>

namespace bk
{
  class IFFTShiftImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = IFFTShiftImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr IFFTShiftImageFilter() = default;
      constexpr IFFTShiftImageFilter(const self_type&) = default;
      constexpr IFFTShiftImageFilter(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~IFFTShiftImageFilter() = default;
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
      [[nodiscard]] static constexpr TImage apply(const TImage& img)
      {
          const unsigned int nDims = img.num_dimensions();
          assert(nDims >= 1 && nDims <= 4 && "fftshift is only implemented for 1/2/3/4D images");

          const auto size = img.size();

          TImage res = img;

          switch (nDims)
          {
              case 1:
              {
                  IFFTShift1D(res.data(), size[0]);
                  break;
              }
              case 2:
              {
                  IFFTShift2D(res.data(), size[0], size[1]);
                  break;
              }
              case 3:
              {
                  IFFTShift3D(res.data(), size[0], size[1], size[2]);
                  break;
              }
              case 4:
              {
                  IFFTShift4D(res.data(), size[0], size[1], size[2], size[3]);
                  break;
              }
          }

          return res;
      }
      /// @}
  }; // class IFFTShiftImageFilter
} // namespace bk

#endif //BK_IFFTSHIFTIMAGEFILTER_H
