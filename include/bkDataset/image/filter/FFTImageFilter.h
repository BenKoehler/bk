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

#ifndef BK_FFTIMAGEFILTER_H
#define BK_FFTIMAGEFILTER_H

#include <cassert>
#include <complex>
#include <vector>

#include "../../../bkMath/FFT"

#include "../../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT FFTImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = FFTImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _off;
      std::vector<bool> _orig_size_uneven;
      bool _normalization;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      FFTImageFilter();
      FFTImageFilter(const self_type&);
      FFTImageFilter(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~FFTImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET PADDING SIZE
      /*!
       * @brief padding per dimension
       * 
       * This FFT implementation requires that the size of each dimension is a power of 2.
       * The original image will be copied into the middle of a new zero-padded image.
       * This function provides the size of the boundary (padding) per dimension.
       */
      [[nodiscard]] const std::vector<unsigned int>& padding_size() const;

      [[nodiscard]] const std::vector<bool>& size_uneven() const;
      /// @}

      /// @{ -------------------------------------------------- SET NORMALIZATION
      [[nodiscard]] bool normalization_is_enabled() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET NORMALIZATION
      void set_normalization_enabled(bool b);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] typename TImage::template self_template_type<std::complex<double>> apply(const TImage& img)
      {
          const unsigned int nDims = img.num_dimensions();
          assert(nDims >= 1 && nDims <= 4 && "fft is only implemented for 1/2/3/4D images");

          const auto size = img.size();

          /*
           * make each image dimension size a power of 2
           */
          std::vector<unsigned int> sizePow2(nDims, 1);
          unsigned int newNumValues = 1;

          _off.resize(nDims);
          _orig_size_uneven.resize(nDims);

          for (unsigned int dimId = 0; dimId < nDims; ++dimId)
          {
              unsigned int& p = sizePow2[dimId];

              while (p < size[dimId])
              { p <<= 1; }

              newNumValues *= p;

              _off[dimId] = (sizePow2[dimId] - size[dimId]) / 2;
              _orig_size_uneven[dimId] = (sizePow2[dimId] - size[dimId]) % 2;
          } // for dimId

          /*
           * create result image
           */
          typename TImage::template self_template_type<std::complex<double>> fftimg;
          fftimg.set_size(sizePow2);

          #pragma omp parallel for
          for (unsigned int i = 0; i < newNumValues; ++i)
          {
              auto gid = bk::list_to_grid_id(sizePow2, i);

              bool insideOriginalImage = true;
              for (unsigned int dimId = 0; dimId < nDims; ++dimId)
              {
                  if (gid[dimId] < _off[dimId] || gid[dimId] >= size[dimId] + _off[dimId])
                  {
                      insideOriginalImage = false;
                      break;
                  }
              }

              if (!insideOriginalImage)
              {
                  fftimg[i].real(0);
                  continue;
              }

              auto gidoff = gid;

              for (unsigned int dimId = 0; dimId < nDims; ++dimId)
              { gidoff[dimId] -= _off[dimId]; }

              fftimg[i].real(img(gidoff));
          } // for i

          std::complex<double>* rawDataPtr = fftimg.data().data().data();

          switch (nDims)
          {
              case 1:
              {
                  FFT1D(rawDataPtr, sizePow2[0], _normalization);
                  break;
              }
              case 2:
              {
                  FFT2D(rawDataPtr, sizePow2[0], sizePow2[1], _normalization);
                  break;
              }
              case 3:
              {
                  FFT3D(rawDataPtr, sizePow2[0], sizePow2[1], sizePow2[2], _normalization);
                  break;
              }
              case 4:
              {
                  FFT4D(rawDataPtr, sizePow2[0], sizePow2[1], sizePow2[2], sizePow2[3], _normalization);
                  break;
              }
          }

          return fftimg;
      }
      /// @}
  }; // class FFTImageFilter
} // namespace bk

#endif //BK_FFTIMAGEFILTER_H
