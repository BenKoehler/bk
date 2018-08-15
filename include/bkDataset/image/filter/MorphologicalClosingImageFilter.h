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

#ifndef BK_MORPHOLOGICALCLOSINGIMAGEFILTER_H
#define BK_MORPHOLOGICALCLOSINGIMAGEFILTER_H

#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include "MorphologicalDilationImageFilter.h"
#include "MorphologicalErosionImageFilter.h"
#include "../../lib/bkDataset_export.h"

namespace bk
{
  class BKDATASET_EXPORT MorphologicalClosingImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = MorphologicalClosingImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _kernel_size;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      MorphologicalClosingImageFilter();
      MorphologicalClosingImageFilter(const self_type& other);
      MorphologicalClosingImageFilter(self_type&& other);
      MorphologicalClosingImageFilter(unsigned int nDims, unsigned int size);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~MorphologicalClosingImageFilter();
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
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
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
          MorphologicalDilationImageFilter fdilation;
          fdilation.set_kernel_size(_kernel_size.begin(), _kernel_size.end());

          MorphologicalErosionImageFilter ferosion;
          ferosion.set_kernel_size(_kernel_size.begin(), _kernel_size.end());

          return ferosion.apply(fdilation.apply(img));
      }
      /// @}
  }; // class MorphologicalClosingImageFilter
} // namespace bk

#endif //BK_MORPHOLOGICALCLOSINGIMAGEFILTER_H
