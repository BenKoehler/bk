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

#ifndef BK_MORPHOLOGICALOPERATIONIMAGEFILTER_H
#define BK_MORPHOLOGICALOPERATIONIMAGEFILTER_H

#include <algorithm>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Progress>
    #include <bk/Localization>

#endif

#include <bkMath/matrix/MatrixFactory.h>
#include <bkMath/functions/list_grid_id_conversion.h>
#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT MorphologicalOperationImageFilter
  {
      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS
    private:
      template<typename TKernel, typename TIndexAccessible0, typename TIndexAccessible1, typename TIndexAccessible2, typename TImage, typename T>
      static void _apply_morphological_operation_kernel(unsigned int dimId, const TKernel& kernel, const TIndexAccessible0& gid, TIndexAccessible1& off, TIndexAccessible2& kernel_gid, TImage& res, const T& fillValue)
      {
          const int halfsize = kernel.size(dimId) >> 1; // integer division

          if (dimId < res.num_dimensions() - 1)
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;
                  kernel_gid[dimId] = i + halfsize;
                  _apply_morphological_operation_kernel(dimId + 1, kernel, gid, off, kernel_gid, res, fillValue);
              } // for i
          }
          else // if (d == res.num_dimensions()-1)
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;
                  kernel_gid[dimId] = i + halfsize;

                  auto gidoff = MatrixFactory::create<int, TImage::NumDimensionsAtCompileTime(), 1>(res.num_dimensions(), 1);

                  for (unsigned int d = 0; d < res.num_dimensions(); ++d)
                  { gidoff[d] = gid[d] + off[d]; }

                  gidoff = res.clamp_to_size(std::move(gidoff));

                  #pragma omp critical
                  { res(gidoff) = fillValue * kernel(kernel_gid); }
              } // for i
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- APPLY
      template<typename TImage, typename TKernel>
      [[nodiscard]] static TImage apply(const TImage& img, const TKernel& kernel)
      {
          using value_type = typename TImage::value_type;

          TImage res = img;

          /*const*/ auto[itMinVal, itMaxVal] = std::minmax_element(img.begin(), img.end());
          const value_type structel = *itMinVal + (*itMaxVal - *itMinVal) * kernel[kernel.num_values() / 2];

          #ifdef BK_EMIT_PROGRESS
          Progress& prog = bk_progress.emplace_task(img.num_values(), ___("Morphological image filtering"));
          #endif

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          {
              if (img[i] == structel)
              {
                  const auto gid = bk::list_to_grid_id(img.size(), i);
                  auto gidoff = MatrixFactory::create<int, TImage::NumDimensionsAtCompileTime(), 1>(img.num_dimensions(), 1);
                  auto kernel_gid = MatrixFactory::create<int, TImage::NumDimensionsAtCompileTime(), 1>(img.num_dimensions(), 1);

                  _apply_morphological_operation_kernel(0, kernel, gid, gidoff, kernel_gid, res, structel);
              }

              #ifdef BK_EMIT_PROGRESS
                  #pragma omp critical
              { prog.increment(1); }
              #endif
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return res;
      }
      /// @}
  }; // class MorphologicalOperationImageFilter
} // namespace bk

#endif //BK_MORPHOLOGICALOPERATIONIMAGEFILTER_H
