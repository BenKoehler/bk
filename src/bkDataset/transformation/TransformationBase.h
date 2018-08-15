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

#ifndef BK_TRANSFORMATIONBASE_H
#define BK_TRANSFORMATIONBASE_H

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

#include <bkTypeTraits/has_index_operator.h>

namespace bk::details
{
  template<int TDims, typename TDerived> class TransformationBase
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TransformationBase<TDims, TDerived>;
      using derived_type = TDerived;
    public:
      using value_type = double;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
    protected:
      constexpr TransformationBase() = default;
      constexpr TransformationBase(const self_type&) = default;
      constexpr TransformationBase(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TransformationBase() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
    protected:
      constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) noexcept = default;
    public:
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- TO WORLD COORDINATES
      template<typename T>
      [[nodiscard]] constexpr std::decay_t<T> to_world_coordinates(T&& vec) const
      {
          static_assert(bk::has_index_operator_v<std::decay_t<T>>, "parameter must provide index operator[]");

          std::decay_t<T> res(std::forward<T>(vec));
          deriv()->to_world_coordinates_impl(res);
          return res;
      }

      template<typename... TL, std::enable_if_t<(sizeof...(TL) > 1)>* = nullptr>
      [[nodiscard]] constexpr std::array<value_type, sizeof...(TL)> to_world_coordinates(TL... scalar_values) const
      {
          static_assert(sizeof...(TL) > 0, "arguments are missing");
          static_assert(TDims == -1 || sizeof...(TL) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TL>...>, "all arguments must have arithmetic types");

          return to_world_coordinates(std::array<value_type, sizeof...(TL)>{static_cast<value_type>(scalar_values)...});
      }

      template<typename T>
      [[nodiscard]] std::vector<T> to_world_coordinates(std::initializer_list<T> ilist) const
      { return to_world_coordinates(std::vector<T>(ilist.begin(), ilist.end())); }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
      template<typename T>
      [[nodiscard]] constexpr std::decay_t<T> to_object_coordinates(T&& vec) const
      {
          static_assert(bk::has_index_operator_v<std::decay_t<T>>, "parameter must provide index operator[]");

          std::decay_t<T> res(std::forward<T>(vec));
          deriv()->to_object_coordinates_impl(res);
          return res;
      }

      template<typename... TL, std::enable_if_t<(sizeof...(TL) > 1)>* = nullptr>
      [[nodiscard]] constexpr std::array<value_type, sizeof...(TL)> to_object_coordinates(TL... scalar_values) const
      {
          static_assert(sizeof...(TL) > 0, "arguments are missing");
          static_assert(TDims == -1 || sizeof...(TL) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TL>...>, "all arguments must have arithmetic types");

          return to_object_coordinates(std::array<value_type, sizeof...(TL)>{static_cast<value_type>(scalar_values)...});
      }

      template<typename T>
      [[nodiscard]] std::vector<T> to_object_coordinates(std::initializer_list<T> ilist) const
      { return to_object_coordinates(std::vector<T>(ilist.begin(), ilist.end())); }
      /// @}
  }; // class TransformationBase
} // namespace bk::details

#endif //BK_TRANSFORMATIONBASE_H