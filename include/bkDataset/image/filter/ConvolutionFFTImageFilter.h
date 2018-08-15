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

#ifndef BK_CONVOLUTIONFFTIMAGEFILTER_H
#define BK_CONVOLUTIONFFTIMAGEFILTER_H

#include <algorithm>

#ifdef BK_EMIT_PROGRESS

    #include "../../../bkTools/Progress"
    #include <bkTools/Localization>

#endif

#include "FFTImageFilter.h"
#include "IFFTImageFilter.h"
#include "FFTShiftImageFilter.h"

#include "../../lib/bkDataset_export.h"

namespace bk
{
  class BKDATASET_EXPORT ConvolutionFFTImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ConvolutionFFTImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      unsigned int _num_iterations;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr ConvolutionFFTImageFilter()
          : ConvolutionFFTImageFilter(1)
      { /* do nothing */ }

      constexpr ConvolutionFFTImageFilter(const self_type&) = default;
      constexpr ConvolutionFFTImageFilter(self_type&&) = default;

      constexpr ConvolutionFFTImageFilter(unsigned int numIterations)
          : _num_iterations(numIterations)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ConvolutionFFTImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM ITERATIONS
      [[nodiscard]] constexpr unsigned int num_iterations() const
      { return _num_iterations; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&& other) -> self_type& = default;
      /// @}

      /// @{ -------------------------------------------------- SET NUM ITERATIONS
      constexpr void set_num_iterations(unsigned int numIterations)
      { _num_iterations = numIterations; }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage, typename TKernel>
      [[nodiscard]] static TImage apply(const TImage& img, const TKernel& kernel, unsigned int numIterations)
      {
          if (numIterations == 0)
          { return img; }

          #ifdef BK_EMIT_PROGRESS
          Progress& prog = bk_progress.emplace_task(numIterations + 2 * 3, ___("image convolution filtering"));
          #endif

          FFTImageFilter filter_fft;
          auto img_fft = filter_fft.apply(img);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(2);
          #endif

          auto size_fft = img_fft.size();

          typename TImage::template self_template_type<double> kernel_img;
          kernel_img.set_size(size_fft);

          const unsigned int nDims = img.num_dimensions();
          auto offset = size_fft;

          for (unsigned int dimId = 0; dimId < nDims; ++dimId)
          {
              offset[dimId] /= 2;
              offset[dimId] -= kernel.size()[dimId] / 2;
          }

          for (unsigned int i = 0; i < kernel.num_values(); ++i)
          {
              auto gid_kernel = bk::list_to_grid_id(kernel.size(), i);
              auto gid_kernel_img = gid_kernel;

              for (unsigned int dimId = 0; dimId < nDims; ++dimId)
              { gid_kernel_img[dimId] += offset[dimId]; }

              kernel_img(gid_kernel_img) = kernel[i];
          }

          kernel_img = FFTShiftImageFilter::apply(kernel_img);

          FFTImageFilter filter_fft_kernel;
          filter_fft_kernel.set_normalization_enabled(false);
          auto kernel_fft = filter_fft_kernel.apply(kernel_img);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(2);
          #endif

          const unsigned int numValues = img_fft.num_values();

          for (unsigned int iterId = 0; iterId < numIterations; ++iterId)
          {
              #pragma omp parallel for
              for (unsigned int i = 0; i < numValues; ++i)
              { img_fft[i] *= kernel_fft[i]; }

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif
          } // for iterId

          IFFTImageFilter filter_ifft(filter_fft);
          auto res = filter_ifft.apply(img_fft);

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return res;
      }

      template<typename TImage, typename TKernel>
      [[nodiscard]] TImage apply(const TImage& img, const TKernel& kernel) const
      { return apply(img, kernel, num_iterations()); }
      /// @}
  }; // class ConvolutionFFTImageFilter
} // namespace bk

#endif //BK_CONVOLUTIONFFTIMAGEFILTER_H
