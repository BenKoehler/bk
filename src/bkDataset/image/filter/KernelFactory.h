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

#ifndef BK_KERNELFACTORY_H
#define BK_KERNELFACTORY_H

#include <type_traits>
#include <utility>
#include <vector>

#include <bkTypeTraits/has_index_operator.h>
#include <bkTypeTraits/make_constant_index_sequence.h>
#include <bkMath/functions/binomial_coefficient.h>
#include <bkMath/functions/list_grid_id_conversion.h>
#include <bk/NDContainer>

#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  struct BKDATASET_EXPORT KernelFactory
  {
      //====================================================================================================
      //===== CONVOLUTION KERNELS
      //====================================================================================================
      /// @{ -------------------------------------------------- AVERAGE
      /*!
       *  default average kernel, e.g.:
       *        1  1  1
       *        1  1  1
       *        1  1  1
       */
    private:
      template<typename TKernel>
      static constexpr void set_average(TKernel& k)
      { k.fill(1.0 / k.num_values()); }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_average_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_average(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_average_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_average(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_average_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_average(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_average_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_average_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_average_of_dim_and_isotropic_size() noexcept
      { return _make_average_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_average_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_average_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      /// @{ -------------------------------------------------- BINOMIAL
      /*!
       *  default binomial kernel, e.g.:
       *        1  2  1
       *        2  4  2
       *        1  2  1
       */
    private:
      template<typename TKernel>
      static constexpr void set_binomial_non_normalized(TKernel& k)
      {
          k.fill(1);

          for (unsigned int d = 0; d < k.num_dimensions(); ++d)
          {
              for (unsigned int i = 0; i < k.num_values(); ++i)
              {
                  const auto gid = bk::list_to_grid_id(k.size(), i);
                  k(gid) *= bk::binomial_coefficient(k.size(d) - 1, gid[d]);
              } // for i
          } // for d
      }

      template<typename TKernel>
      static constexpr void set_binomial(TKernel& k)
      {
          set_binomial_non_normalized(k);

          // normalize
          double sum = 0;

          for (unsigned int i = 0; i < k.num_values(); ++i)
          { sum += k[i]; }

          for (unsigned int i = 0; i < k.num_values(); ++i)
          { k[i] /= sum; }
      }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_binomial_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_binomial(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_binomial_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_binomial(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_binomial_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_binomial(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_binomial_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_binomial_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_binomial_of_dim_and_isotropic_size() noexcept
      { return _make_binomial_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_binomial_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_binomial_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      /// @{ -------------------------------------------------- LAPLACE
      /*!
       *  default laplace kernel, e.g.:
       *        -1  -1  -1
       *        -1   8  -1
       *        -1  -1  -1
       */
    private:
      template<typename TKernel>
      static constexpr void set_laplace(TKernel& k)
      {
          k.fill(-1);
          k[k.num_values() / 2] = k.num_values() - 1;
      }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_laplace_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_laplace(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_laplace_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_laplace(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_laplace_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_laplace(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_laplace_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_laplace_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_laplace_of_dim_and_isotropic_size() noexcept
      { return _make_laplace_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_laplace_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_laplace_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      /// @{ -------------------------------------------------- LAPLACIAN OF BINOMIAL
      /*!
       *  laplace kernel in binomial manner, e.g.:
       *        -1  -2  -1
       *        -2  12  -2
       *        -1  -2  -1
       */
    private:
      template<typename TKernel>
      static constexpr void set_laplace_binomial(TKernel& k)
      {
          set_binomial_non_normalized(k);
          k[k.num_values() / 2] = 0;

          double sum = 0;

          for (double& x: k)
          {
              sum += x;
              x = -x;
          }

          k[k.num_values() / 2] = sum;
      }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_laplace_binomial_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_laplace_binomial(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_laplace_binomial_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_laplace_binomial(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_laplace_binomial_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_laplace_binomial(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_laplace_binomial_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_laplace_binomial_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_laplace_binomial_of_dim_and_isotropic_size() noexcept
      { return _make_laplace_binomial_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_laplace_binomial_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_laplace_binomial_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      /// @{ -------------------------------------------------- SOBEL
      /*!
       *  default sobel kernel, e.g.:
       *        -1  0  1
       *        -2  0  2
       *        -1  0  1
       *
       *  @param targetDim sobel produces derivatives for a given dimension (dim0 == x in the example)
       */
    private:
      template<typename TKernel>
      static constexpr void set_sobel(TKernel& k, unsigned int targetDim)
      {
          const auto s = k.size();
          const unsigned int targetDim_half = s[targetDim] >> 1;

          set_binomial_non_normalized(k);

          for (unsigned int i = 0; i < k.num_values(); ++i)
          {
              const auto gid = bk::list_to_grid_id(s, i);
              k(gid) *= gid[targetDim] < targetDim_half ? -1 /*negate*/ : gid[targetDim] > targetDim_half ? +1 /*do not alter*/ : 0 /*set zero*/;
          }
      }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_sobel_of_sizes(unsigned int targetDim) noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_sobel(kernel, targetDim);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_sobel_of_sizes(unsigned int targetDim, TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_sobel(kernel, targetDim);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_sobel_of_sizes(unsigned int targetDim, const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_sobel(kernel, targetDim);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_sobel_of_dim_and_isotropic_size(unsigned int targetDim, std::integer_sequence<int, Sizes...>) noexcept
      { return make_sobel_of_sizes<Sizes...>(targetDim); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_sobel_of_dim_and_isotropic_size(unsigned int targetDim) noexcept
      { return _make_sobel_of_dim_and_isotropic_size(targetDim, bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_sobel_of_dim_and_isotropic_size(unsigned int targetDim, unsigned int num_dimensions, unsigned int size)
      { return make_sobel_of_sizes(targetDim, std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      //====================================================================================================
      //===== MORPHOLOGICAL STRUCTURE ELEMENTS
      //====================================================================================================
      /// @{ -------------------------------------------------- DILATION
      /*!
       * morphological dilation structure element, e.g.:
       *        1  1  1
       *        1  1  1
       *        1  1  1
       */
    private:
      template<typename TKernel>
      static constexpr void set_dilation_morphological(TKernel& k)
      { k.fill(1.0); }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_dilation_morphological_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_dilation_morphological(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_dilation_morphological_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_dilation_morphological(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_dilation_morphological_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_dilation_morphological(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_dilation_morphological_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_dilation_morphological_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_dilation_morphological_of_dim_and_isotropic_size() noexcept
      { return _make_dilation_morphological_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_dilation_morphological_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_dilation_morphological_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}

      /// @{ -------------------------------------------------- EROSION
      /*!
       * morphological erosion structure element, e.g.:
       *        0  0  0
       *        0  0  0
       *        0  0  0
       */
    private:
      template<typename TKernel>
      static constexpr void set_erosion_morphological(TKernel& k)
      { k.fill(0.0); }

    public:

      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> make_erosion_morphological_of_sizes() noexcept
      {
          NDArray<double, Sizes...> kernel;
          set_erosion_morphological(kernel);
          return kernel;
      }

      template<typename... TSizes>
      [[nodiscard]] static NDVector<double> make_erosion_morphological_of_sizes(TSizes... sizes)
      {
          NDVector<double> kernel(sizes...);
          set_erosion_morphological(kernel);
          return kernel;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] static NDVector<double> make_erosion_morphological_of_sizes(const TIndexAccessible& size)
      {
          NDVector<double> kernel(size.begin(), size.end());
          set_erosion_morphological(kernel);
          return kernel;
      }

    private:
      template<auto... Sizes>
      [[nodiscard]] static constexpr NDArray<double, Sizes...> _make_erosion_morphological_of_dim_and_isotropic_size(std::integer_sequence<int, Sizes...>) noexcept
      { return make_erosion_morphological_of_sizes<Sizes...>(); }

    public:

      template<auto Dims, auto Size>
      [[nodiscard]] static constexpr auto make_erosion_morphological_of_dim_and_isotropic_size() noexcept
      { return _make_erosion_morphological_of_dim_and_isotropic_size(bk::make_constant_index_sequence<Size, Dims>()); }

      [[nodiscard]] static NDVector<double> make_erosion_morphological_of_dim_and_isotropic_size(unsigned int num_dimensions, unsigned int size)
      { return make_erosion_morphological_of_sizes(std::vector<unsigned int>(num_dimensions, size)); }
      /// @}
  };
} // namespace bk

#endif //BK_KERNELFACTORY_H
