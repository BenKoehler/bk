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

#ifndef BK_LAPLACEBINOMIALIMAGEFILTER_H
#define BK_LAPLACEBINOMIALIMAGEFILTER_H

#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

#include <bkDataset/image/filter/ConvolutionImageFilter.h>
#include <bkDataset/image/filter/KernelFactory.h>
#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT LaplaceBinomialImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = LaplaceBinomialImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::vector<unsigned int> _kernel_size;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      LaplaceBinomialImageFilter();
      LaplaceBinomialImageFilter(const self_type& other);
      LaplaceBinomialImageFilter(self_type&& other) noexcept;
      LaplaceBinomialImageFilter(unsigned int nDims, unsigned int size);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~LaplaceBinomialImageFilter();
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

          ConvolutionImageFilter f;
          f.set_num_iterations(1);
          f.set_kernel_is_isotropic_and_separable(false);

          const auto kernel = KernelFactory::make_laplace_binomial_of_sizes(_kernel_size);

          return f.apply(img, kernel);
      }
      /// @}
  }; // class LaplaceBinomialImageFilter
} // namespace bk

#endif //BK_LAPLACEBINOMIALIMAGEFILTER_H
