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

#pragma once

#ifndef BK_IFFTIMAGEFILTER_H
#define BK_IFFTIMAGEFILTER_H

#include <cassert>
#include <complex>
#include <initializer_list>
#include <vector>

#include <bk/FFT>
#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  // -------------------- forward declaration
  class FFTImageFilter;
  // -------------------- forward declaration END

  class BKDATASET_EXPORT IFFTImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = IFFTImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _off;
      std::vector<bool> _orig_size_uneven;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      IFFTImageFilter();
      IFFTImageFilter(const self_type&);
      IFFTImageFilter(self_type&&) noexcept;
      IFFTImageFilter(const FFTImageFilter& filter_fft);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~IFFTImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================


      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET PADDING SIZE
      /*!
       * @brief padding per dimension
       *
       * This IFFT implementation requires that the size of each dimension is a power of 2.
       * The original image was copied into the middle of a new zero-padded image.
       * This function sets the size of the boundary (padding) per dimension.
       */
      template<typename T>
      void set_padding_size(std::initializer_list<T> ilist)
      { _off.assign(ilist); }

      template<typename Iter>
      void set_padding_size(Iter first, Iter last)
      { _off.assign(first, last); }

      void set_padding_size(const FFTImageFilter& filter_fft);

      //

      template<typename T>
      void set_size_uneven(std::initializer_list<T> ilist)
      { _orig_size_uneven.assign(ilist); }

      template<typename Iter>
      void set_size_uneven(Iter first, Iter last)
      { _orig_size_uneven.assign(first, last); }

      void set_size_uneven(const FFTImageFilter& filter_fft);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] typename TImage::template self_template_type<double> apply(const TImage& img)
      {
          const unsigned int nDims = img.num_dimensions();
          assert(nDims >= 1 && nDims <= 4 && "ifft is only implemented for 1/2/3/4D images");

          const auto size = img.size();

          // if padding was not set
          _off.resize(nDims, 0);
          _orig_size_uneven.resize(nDims, false);

          TImage ifftimg = img;
          std::complex<double>* rawDataPtr = ifftimg.data().data().data();

          switch (nDims)
          {
              case 1:
              {
                  IFFT1D(rawDataPtr, size[0]);
                  break;
              }
              case 2:
              {
                  IFFT2D(rawDataPtr, size[0], size[1]);
                  break;
              }
              case 3:
              {
                  IFFT3D(rawDataPtr, size[0], size[1], size[2]);
                  break;
              }
              case 4:
              {
                  IFFT4D(rawDataPtr, size[0], size[1], size[2], size[3]);
                  break;
              }
          }

          /*
           * create result image
           */
          std::vector<unsigned int> sizeWithoutPadding(size.begin(), size.end());

          for (unsigned int dimId = 0; dimId < nDims; ++dimId)
          {
              sizeWithoutPadding[dimId] -= 2 * _off[dimId];

              if (_orig_size_uneven[dimId])
              { sizeWithoutPadding[dimId] -= 1; }
          }

          typename TImage::template self_template_type<double> res;
          res.set_size(sizeWithoutPadding);

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          {
              auto gid = bk::list_to_grid_id(size, i);

              bool insideOriginalImage = true;
              for (unsigned int dimId = 0; dimId < nDims; ++dimId)
              {
                  if (gid[dimId] < _off[dimId] || gid[dimId] >= sizeWithoutPadding[dimId] + _off[dimId])
                  {
                      insideOriginalImage = false;
                      break;
                  }
              }

              if (!insideOriginalImage)
              { continue; }

              auto gidoff = gid;

              for (unsigned int dimId = 0; dimId < nDims; ++dimId)
              {
                  gidoff[dimId] -= _off[dimId];

                  //if (_orig_size_uneven[dimId])
                  //{ gidoff[dimId] -= 1; }
              }

              res(gidoff) = ifftimg[i].real();
          } // for i

          return res;
      }
      /// @}
  }; // class IFFTImageFilter
} // namespace bk

#endif //BK_IFFTIMAGEFILTER_H
