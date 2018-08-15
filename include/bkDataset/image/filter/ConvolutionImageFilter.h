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

#ifndef BK_CONVOLUTIONIMAGEFILTER_H
#define BK_CONVOLUTIONIMAGEFILTER_H

#include <algorithm>

#ifdef BK_EMIT_PROGRESS

    #include "../../../bkTools/Progress"
    #include <bkTools/Localization>

#endif

#include "../../../bkMath/functions/list_grid_id_conversion.h"

namespace bk
{
  class ConvolutionImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ConvolutionImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      unsigned int _num_iterations;
      bool _kernel_is_isotropic_and_separable;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr ConvolutionImageFilter()
          : ConvolutionImageFilter(1, false)
      { /* do nothing */ }

      constexpr ConvolutionImageFilter(const self_type&) = default;
      constexpr ConvolutionImageFilter(self_type&&) = default;

      constexpr ConvolutionImageFilter(unsigned int numIterations, bool kernelIsIsotropicAndSeparable = false)
          : _num_iterations(numIterations),
            _kernel_is_isotropic_and_separable(kernelIsIsotropicAndSeparable)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ConvolutionImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM ITERATIONS
      [[nodiscard]] constexpr unsigned int num_iterations() const
      { return _num_iterations; }
      /// @}

      /// @{ -------------------------------------------------- KERNEL IS SEPARABLE
      [[nodiscard]] constexpr bool kernel_is_isotropic_and_separable() const
      { return _kernel_is_isotropic_and_separable; }
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

      /// @{ -------------------------------------------------- SET KERNEL IS SEPARABLE
      constexpr void set_kernel_is_isotropic_and_separable(bool b)
      { _kernel_is_isotropic_and_separable = b; }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS
      template<typename TImage, typename TKernel>
      [[nodiscard]] static TImage apply(const TImage& img, const TKernel& kernel, unsigned int numIterations)
      {
          #ifdef BK_EMIT_PROGRESS
          Progress& prog = bk_progress.emplace_task(numIterations * img.num_values(), ___("image convolution filtering"));
          #endif

          const unsigned int numValues = img.num_values();

          TImage res;
          res.set_size(img.size());

          if (numIterations == 1)
          {
              #pragma omp parallel for
              for (unsigned int i = 0; i < numValues; ++i)
              { res[i] = img.apply_convolution_kernel(kernel, bk::list_to_grid_id(img.size(), i)); }

              #ifdef BK_EMIT_PROGRESS
              prog.increment(numValues);
              #endif

              return res;
          }
          else
          {
              TImage res2 = img;

              bool lastReadWasImg1 = true;

              for (unsigned int iterId = 0; iterId < numIterations; ++iterId)
              {
                  const TImage* imgRead = lastReadWasImg1 ? &res2 : &res;
                  TImage* imgWrite = lastReadWasImg1 ? &res : &res2;

                  #pragma omp parallel for
                  for (unsigned int i = 0; i < numValues; ++i)
                  { (*imgWrite)[i] = imgRead->apply_convolution_kernel(kernel, bk::list_to_grid_id(img.size(), i)); }

                  lastReadWasImg1 = !lastReadWasImg1;

                  #ifdef BK_EMIT_PROGRESS
                  prog.increment(numValues);
                  #endif
              } // for iterId

              #ifdef BK_EMIT_PROGRESS
              prog.set_finished();
              #endif

              return lastReadWasImg1 ? res2 : res;
          }
      }

      template<typename TImage, typename TKernel>
      [[nodiscard]] static TImage apply_separable(const TImage& img, const TKernel& kernel, unsigned int numIterations)
      {
          #ifdef BK_EMIT_PROGRESS
          Progress& prog = bk_progress.emplace_task(numIterations * img.num_values() * img.num_dimensions(), ___("image convolution filtering"));
          #endif

          TImage res;
          res.set_size(img.size());

          TImage res2 = img;

          auto gid0 = bk::list_to_grid_id(kernel.size(), 0);
          std::vector<double> kernelRow(kernel.size(0));

          double norm = 0;

          for (unsigned int i = 0; i < kernel.size(0); ++i)
          {
              gid0[0] = i;
              kernelRow[i] = kernel(gid0);
              norm += kernelRow[i];
          } // for i

          for (unsigned int i = 0; i < kernel.size(0); ++i)
          { kernelRow[i] /= norm; }

          const auto imgsize = img.size();
          const unsigned int kernelSize = kernelRow.size();
          const int halfKernelSize = static_cast<int>(kernelSize >> 1);
          const int numValues = static_cast<int>(img.num_values());

          bool lastReadWasImg1 = true;

          for (unsigned int iterId = 0; iterId < numIterations; ++iterId)
          {
              for (unsigned int dimId = 0; dimId < img.num_dimensions(); ++dimId)
              {
                  const TImage* imgRead = lastReadWasImg1 ? &res2 : &res;
                  TImage* imgWrite = lastReadWasImg1 ? &res : &res2;

                  const int stride = static_cast<int>(bk::stride_of_dim(imgsize, dimId, img.num_dimensions()));

                  #pragma omp parallel for
                  for (int listId = 0; listId < numValues; ++listId)
                  {
                      int offListId0 = listId - halfKernelSize * stride;
                      const int offListId1 = listId + halfKernelSize * stride;

                      if (offListId0 < 0 || offListId1 >= numValues)
                      {
                          (*imgWrite)[listId] = (*imgRead)[listId];
                          continue;
                      }

                      auto accum = img.template allocate_value<double>();

                      for (unsigned int off = 0; off < kernelSize; ++off, offListId0 += stride)
                      { accum += kernelRow[off] * (*imgRead)[offListId0]; }

                      (*imgWrite)[listId] = accum;
                  } // for listId

                  lastReadWasImg1 = !lastReadWasImg1;

                  #ifdef BK_EMIT_PROGRESS
                  prog.increment(numValues);
                  #endif
              } // for dimId
          } // for iterId

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return lastReadWasImg1 ? res2 : res;
      }
      /// @}

      /// @{ -------------------------------------------------- APPLY
      /*!
       * A kernel is separable if it can be obtained as firstCol * firstRow, e.g.
       * the binomial filter kernel
       *     1 2 1
       *     2 4 2
       *     1 2 1
       * is separable since it is the same as 1
       *                                      2   *   1 2 1
       *                                      1
       */
      template<typename TImage, typename TKernel>
      [[nodiscard]] TImage apply(const TImage& img, const TKernel& kernel) const
      {
          if (num_iterations() == 0)
          { return img; }

          if (!kernel_is_isotropic_and_separable())
          { // run default convolution
              return apply(img, kernel, num_iterations());
          }
          else
          { // run 1D convolution per dimension
              return apply_separable(img, kernel, num_iterations());
          }
      }
      /// @}
  }; // class ConvolutionImageFilter
} // namespace bk

#endif //BK_CONVOLUTIONIMAGEFILTER_H
