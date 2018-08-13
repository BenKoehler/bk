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

#ifndef BK_SOBELIMAGEFILTER_H
#define BK_SOBELIMAGEFILTER_H

#include <cassert>
#include <initializer_list>
#include <type_traits>

#include "ConvolutionImageFilter.h"
#include "KernelFactory.h"
#include "../../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT SobelImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SobelImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _kernel_size;
      unsigned int _target_dimension;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      SobelImageFilter();
      SobelImageFilter(const self_type& other);
      SobelImageFilter(self_type&& other);
      SobelImageFilter(unsigned int nDims, unsigned int size, unsigned int targetDimId);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SobelImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET KERNEL SIZE
      [[nodiscard]] const std::vector<unsigned int>& kernel_size() const;
      /// @}

      /// @{ -------------------------------------------------- GET TARGET DIMENSION ID
      [[nodiscard]] unsigned int target_dimension_id() const;
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

      /// @{ -------------------------------------------------- SET TARGET DIMENSION ID
      void set_target_dimension_id(unsigned int id);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] TImage apply(const TImage& img) const
      {
          assert(!_kernel_size.empty() && "call set_kernel_size() first");
          assert(_target_dimension < img.num_dimensions() && "invalid target dimension id");

          ConvolutionImageFilter f;
          f.set_num_iterations(1);
          f.set_kernel_is_isotropic_and_separable(false);

          const auto kernel = KernelFactory::make_sobel_of_sizes(_target_dimension, _kernel_size);

          return f.apply(img, kernel);
      }
      /// @}
  }; // class SobelImageFilter
} // namespace bk

#endif //BK_SOBELIMAGEFILTER_H
