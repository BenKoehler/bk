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

#ifndef BK_MORPHOLOGICALEROSIONIMAGEFILTER_H
#define BK_MORPHOLOGICALEROSIONIMAGEFILTER_H

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include <bkDataset/image/filter/KernelFactory.h>
#include <bkDataset/image/filter/MorphologicalOperationImageFilter.h>
#include <bkDataset/image/filter/DistanceMapImageFilter.h>
#include <bkDataset/lib/bkDataset_export.h>

#ifdef BK_EMIT_PROGRESS
    #include <bk/Progress>
    #include <bk/Localization>
#endif

namespace bk
{
  class BKDATASET_EXPORT MorphologicalErosionImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = MorphologicalErosionImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _kernel_size;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      MorphologicalErosionImageFilter();
      MorphologicalErosionImageFilter(const self_type& other);
      MorphologicalErosionImageFilter(self_type&& other) noexcept;
      MorphologicalErosionImageFilter(unsigned int nDims, unsigned int size);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~MorphologicalErosionImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET KERNEL SIZE
      [[nodiscard]] const std::vector<unsigned int>& kernel_size() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET KERNEL SIZE
      template<typename T>
      void set_kernel_size(std::initializer_list<T> ilist)
      { _kernel_size.assign(ilist); }

      template<typename Iter>
      void set_kernel_size(Iter first, Iter last)
      { _kernel_size.assign(first, last); }

      void set_kernel_size(unsigned int nDims, unsigned int size);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] TImage apply(const TImage& img) const
      {
          assert(!_kernel_size.empty() && "call set_kernel_size() first");

          const bool kernel_has_isotropic_size = std::all_of(_kernel_size.begin(), _kernel_size.end(), [&](unsigned int x)
          { return x == _kernel_size.front(); });

          if (kernel_has_isotropic_size)
          {
              #ifdef BK_EMIT_PROGRESS
              bk::Progress& prog = bk_progress.emplace_task(3, ___("Morphological erosion filtering"));
              #endif

              TImage res;
              res.set_size(img.size());

              const auto minVal = img.min_value();

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif

              DistanceMapImageFilter f;
              f.set_value(minVal);
              auto distance_map = img.filter(f);

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif

              const unsigned int halfKernelSize = _kernel_size.front() >> 1;

              #pragma omp parallel for
              for (unsigned int i = 0; i < img.num_values(); ++i)
              { res[i] = distance_map[i] <= halfKernelSize ? minVal : img[i]; }

              #ifdef BK_EMIT_PROGRESS
              prog.set_finished();
              #endif

              return res;
          }
          else
          { return MorphologicalOperationImageFilter::apply(img, KernelFactory::make_erosion_morphological_of_sizes(_kernel_size)); }
      }
      /// @}
  }; // class MorphologicalErosionImageFilter
} // namespace bk

#endif //BK_MORPHOLOGICALEROSIONIMAGEFILTER_H
