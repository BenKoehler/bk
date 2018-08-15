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

#ifndef BK_SCALETRANSFORMATION_H
#define BK_SCALETRANSFORMATION_H

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>
#include <type_traits>

#include <bkTypeTraits/has_index_operator.h>
#include <bkTypeTraits/make_constant_index_sequence.h>
#include <bk/NDContainer>

#include <bkDataset/transformation/TransformationBase.h>

namespace bk
{
  template<int TDims = -1> class ScaleTransformation : public details::TransformationBase<TDims, ScaleTransformation<TDims>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || TDims > 0);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ScaleTransformation<TDims>;
      using base_type = details::TransformationBase<TDims, ScaleTransformation<TDims>>;
      using value_type = typename base_type::value_type;
      using scale_vector_type = std::conditional_t<(TDims > 0), std::array<value_type, TDims>, std::vector<value_type>>;

      friend base_type;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      scale_vector_type _scales;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- CTOR HELPERS
      template<int... TScalar>
      constexpr ScaleTransformation(std::integer_sequence<int, TScalar...>) noexcept
          : _scales{static_cast<value_type>(TScalar)...}
      { /* do nothing */ }
      /// @}

    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr ScaleTransformation()
          : ScaleTransformation(bk::make_constant_index_sequence<0, std::max(TDims, 0)>())
      { /* do nothing */ }

      constexpr ScaleTransformation(const self_type&) = default;
      constexpr ScaleTransformation(self_type&&) noexcept = default;

      template<typename... TScales>
      constexpr ScaleTransformation(TScales... scales)
          : _scales{static_cast<value_type>(scales)...}
      {
          static_assert(sizeof...(TScales) != 0, "scale arguments are missing");
          static_assert(TDims == -1 || sizeof...(TScales) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TScales>...>, "all scale arguments must be arithmetic types");
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ScaleTransformation() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] constexpr unsigned int num_dimensions() const
      { return _scales.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET SCALE
      [[nodiscard]] constexpr scale_vector_type& scale()
      { return _scales; }

      [[nodiscard]] constexpr const scale_vector_type& scale() const
      { return _scales; }

      [[nodiscard]] constexpr value_type scale(unsigned int dimId) const
      {
          assert(dimId < num_dimensions() && "dimId out of bounds");
          return _scales[dimId];
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET SCALE
      template<typename TScaleVec, std::enable_if_t<bk::has_index_operator_v<TScaleVec>>* = nullptr>
      void set_scale(const TScaleVec& scale)
      {
          if constexpr (TDims == -1)
          { _scales.assign(scale.begin(), scale.end()); }
          else
          {
              const unsigned int N = std::distance(scale.begin(), scale.end());
              assert(N == TDims && "scaleVec has invalid number of arguments");
              std::copy(scale.begin(), scale.end(), _scales.begin());
          }
      }

      template<typename... TScales>
      void set_scale(TScales ... scales)
      {
          static_assert(sizeof...(TScales) != 0, "scale arguments are missing");
          static_assert(TDims == -1 || sizeof...(TScales) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TScales>...>, "all scale arguments must have arithmetic types");

          _scales = {static_cast<value_type>(scales)...};
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- TO WORLD COORDINATES
      template<typename T>
      constexpr void to_world_coordinates_impl(T& vec) const
      {
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { vec[i] *= _scales[i]; }
      }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
      template<typename T>
      constexpr void to_object_coordinates_impl(T& vec) const
      {
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { vec[i] /= _scales[i]; }
      }
      /// @}
  }; // class ScaleTransformation
} // namespace bk

#endif //BK_SCALETRANSFORMATION_H
