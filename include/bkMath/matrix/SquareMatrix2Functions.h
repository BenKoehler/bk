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

#ifndef BK_SQUAREMATRIX2FUNCTIONS_H
#define BK_SQUAREMATRIX2FUNCTIONS_H

#include <cmath>

#include "../../bkTypeTraits/floating_point.h"

#include "type_traits/matrix_traits.h"

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isSquare2> class SquareMatrix2Functions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class SquareMatrix2Functions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SquareMatrix2Functions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr SquareMatrix2Functions() noexcept = default;
      constexpr SquareMatrix2Functions(const self_type& other) noexcept = default;
      constexpr SquareMatrix2Functions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SquareMatrix2Functions() noexcept = default;
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

    public:
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other) noexcept = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&& other) noexcept = default;
    public:
      /// @}

      /// @{ -------------------------------------------------- RESIZE DYNAMIC IF NECESSARY
    private:
      constexpr void _resize_dynamic()
      {
          if constexpr (bk::is_dynamic_matrix_v<derived_type>)
          { deriv()->set_size(2, 2); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SET TO (COUNTER)CLOCKWISE 2D ROTATION MATRIX
      constexpr void set_rotation_matrix_2d_counterclockwise(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "do not call this function from a matrix with non-floating point value type; this will cut off values and cause errors");

          _resize_dynamic();

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);

          // col 0
          deriv->operator()(0, 0) = cosa;
          deriv->operator()(1, 0) = sina;

          // col 1
          deriv->operator()(0, 1) = -sina;
          deriv->operator()(1, 1) = cosa;
      }

      constexpr void set_rotation_matrix_2d_clockwise(double angle_rad)
      { set_rotation_matrix_2d_counterclockwise(-angle_rad); }
      /// @}
  }; // class SquareMatrix2Functions
} // namespace bk::details

#endif //BK_SQUAREMATRIX2FUNCTIONS_H


