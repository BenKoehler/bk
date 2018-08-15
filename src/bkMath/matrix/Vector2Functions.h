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

#ifndef BK_VECTOR2FUNCTIONS_H
#define BK_VECTOR2FUNCTIONS_H

#include <cassert>
#include <cmath>
#include <utility>

#include <bkMath/functions/radians_degree_conversion.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>
#include <bkTypeTraits/floating_point.h>

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isVec2> class Vector2Functions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class Vector2Functions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Vector2Functions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr Vector2Functions() noexcept = default;
      constexpr Vector2Functions(const self_type& other) noexcept = default;
      constexpr Vector2Functions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Vector2Functions() noexcept = default;
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

      [[nodiscard]] constexpr const self_type* const_this() noexcept
      { return static_cast<const self_type*>(this); }

    public:
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other) noexcept = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&& other) noexcept = default;
      /// @}
    public:

      /// @{ -------------------------------------------------- SET TO AXIS
      constexpr void set_x_axis()
      { deriv()->set(1, 0); }

      constexpr void set_y_axis()
      { deriv()->set(0, 1); }
      /// @}

      //====================================================================================================
      //===== MATH
      //====================================================================================================

      /// @{ -------------------------------------------------- CROSS PRODUCT
      //! Calculate the cross product of two 2D vectors (equals their determinant)
      template<typename TVec2>
      [[nodiscard]] constexpr auto cross(const TVec2& rhs) const
      {
          static_assert(bk::is_static_vector_of_size_v<TVec2, 2> || bk::is_dynamic_matrix_v<TVec2>);
          assert(bk::matrix_traits(rhs).is_vector_of_size(2));

          return (*deriv())[0] * rhs[1] - (*deriv())[1] * rhs[0];
      }

      // alias for cross

      template<typename TVec2>
      [[nodiscard]] constexpr auto cross2(const TVec2& rhs) const
      { return cross(rhs); }
      /// @}

      /// @{ -------------------------------------------------- PERPENDICULAR
      //! create a non-normalized vector (-y,x) perpendicular to this (x,y)
      constexpr void perpendicular_internal()
      {
          static_assert(bk::is_signed_v<value_type>, "you are calling the perpendicular function from an unsigned type vector. performed negation will cause errors!");
          assert(bk::matrix_traits(*deriv()).is_vector_of_size(2));

          value_type temp = (*deriv())[0];
          (*deriv())[0] = -(*deriv())[1];
          (*deriv())[1] = std::move(temp);
      }

      [[nodiscard]] constexpr auto perpendicular()&&
      {
          if constexpr(bk::is_signed_v<value_type>)
          {
              perpendicular_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->perpendicular(); }
      }

      [[nodiscard]] constexpr auto perpendicular() const&
      {
          using Signed_ = bk::make_signed_t<value_type>;
          auto res = deriv()->template copy<Signed_>();
          res.perpendicular_internal();
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- ROTATE (COUNTER)CLOCKWISE
      //! Rotation of 2D vector
      /*!
       *  Multiply rotation matrix to the vector.
       *  The counterclockwise rotation matrix is:
       *                  cos(a) -sin(a)
       *                  sin(a)  cos(a)
       *  The clockwise rotation matrix is:
       *                  cos(a)  sin(a)
       *                  -sin(a) cos(a)
       */
      // rad ccw

      constexpr void rotate_rad_counterclockwise_internal(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR2 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          assert(bk::matrix_traits(*deriv()).is_vector_of_size(2));

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);
          const double x0 = (*deriv())[0] * cosa - (*deriv())[1] * sina;
          const double x1 = (*deriv())[0] * sina + (*deriv())[1] * cosa;
          deriv()->set(x0, x1);
      }

      [[nodiscard]] constexpr auto rotate_rad_counterclockwise(double angle_rad)&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          {
              rotate_rad_counterclockwise_internal(angle_rad);
              return *std::move(deriv());
          }
          else
          { return const_this()->rotate_rad_counterclockwise(angle_rad); }
      }

      [[nodiscard]] constexpr auto rotate_rad_counterclockwise(double angle_rad) const&
      {
          using Float_ = make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_rad_counterclockwise_internal(angle_rad);
          return res;
      }

      // deg ccw

      constexpr void rotate_deg_counterclockwise_internal(double angle_deg)
      { rotate_rad_counterclockwise_internal(bk::degrees_to_radians(angle_deg)); }

      [[nodiscard]] constexpr auto rotate_deg_counterclockwise(double angle_deg)&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          { return std::forward<self_type>(*this).rotate_rad_counterclockwise(bk::degrees_to_radians(angle_deg)); }
          else
          { return const_this()->rotate_deg_counterclockwise(angle_deg); }
      }

      [[nodiscard]] constexpr auto rotate_deg_counterclockwise(double angle_deg) const&
      { return rotate_rad_counterclockwise(bk::degrees_to_radians(angle_deg)); }

      // rad cw

      constexpr void rotate_rad_clockwise_internal(double angle_rad)
      { rotate_rad_counterclockwise_internal(-angle_rad); }

      [[nodiscard]] constexpr auto rotate_rad_clockwise(double angle_rad)&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          { return std::forward<self_type>(*this).rotate_rad_counterclockwise(-angle_rad); }
          else
          { return const_this()->rotate_rad_clockwise(angle_rad); }
      }

      [[nodiscard]] constexpr auto rotate_rad_clockwise(double angle_rad) const&
      { return rotate_rad_counterclockwise(-angle_rad); }

      // deg cw

      constexpr void rotate_deg_clockwise_internal(double angle_deg)
      { rotate_rad_clockwise_internal(bk::degrees_to_radians(angle_deg)); }

      [[nodiscard]] constexpr auto rotate_deg_clockwise(double angle_deg)&&
      {
          if constexpr(bk::is_floating_point_v<value_type>)
          { return std::forward<self_type>(*this).rotate_rad_clockwise(bk::degrees_to_radians(angle_deg)); }
          else
          { return const_this()->rotate_deg_clockwise(angle_deg); }
      }

      [[nodiscard]] constexpr auto rotate_deg_clockwise(double angle_deg) const&
      { return rotate_rad_clockwise(bk::degrees_to_radians(angle_deg)); }
      /// @}
  }; // class Vector2Functions
} // namespace bk::details

#endif //BK_VECTOR2FUNCTIONS_H


