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

#ifndef BK_AVERAGESMOOTHINGIMAGEFILTER_H
#define BK_AVERAGESMOOTHINGIMAGEFILTER_H

#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include "ConvolutionImageFilter.h"
#include "KernelFactory.h"

#include "../../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT AverageSmoothingImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AverageSmoothingImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      unsigned int _num_iterations;
      std::vector<unsigned int> _kernel_size;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      AverageSmoothingImageFilter();
      AverageSmoothingImageFilter(const self_type& other);
      AverageSmoothingImageFilter(self_type&& other);
      AverageSmoothingImageFilter(unsigned int numIterations, unsigned int nDims, unsigned int size);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~AverageSmoothingImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM ITERATIONS
      [[nodiscard]] unsigned int num_iterations() const;
      /// @}

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

      /// @{ -------------------------------------------------- SET NUM ITERATIONS
      void set_num_iterations(unsigned int numIterations);
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

          ConvolutionImageFilter f;
          f.set_num_iterations(_num_iterations);
          f.set_kernel_is_isotropic_and_separable(kernel_has_isotropic_size);

          const auto kernel = KernelFactory::make_average_of_sizes(_kernel_size);

          return f.apply(img, kernel);
      }
      /// @}
  }; // class AverageSmoothingImageFilter
} // namespace bk

#endif //BK_AVERAGESMOOTHINGIMAGEFILTER_H
