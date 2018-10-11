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

#ifndef BK_SQUAREMATRIX3FUNCTIONS_H
#define BK_SQUAREMATRIX3FUNCTIONS_H

#include <cmath>

#include <bkMath/matrix/type_traits/matrix_traits.h>
#include <bkTypeTraits/floating_point.h>

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isSquare3> class SquareMatrix3Functions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class SquareMatrix3Functions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SquareMatrix3Functions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr SquareMatrix3Functions() = default;
      constexpr SquareMatrix3Functions(const self_type&) = default;
      constexpr SquareMatrix3Functions(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SquareMatrix3Functions() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv()
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const
      { return static_cast<const derived_type*>(this); }

    public:
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}
    public:

      /// @{ -------------------------------------------------- RESIZE DYNAMIC IF NECESSARY
    private:
      constexpr void _resize_dynamic()
      {
          if constexpr (bk::is_dynamic_matrix_v<derived_type>)
          { deriv()->set_size(3, 3); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SET TO GENERAL 3D ROTATION MATRIX
      /*!
       * See https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle for further details
       */
      template<typename TVec>
      constexpr void set_rotation_matrix_3d_general(TVec&& axis, double angle_rad, bool normalize_axis = true)
      {
          static_assert(bk::is_floating_point_v<value_type>, "do not call this function from a matrix with non-floating point value type; this will cut off values and cause errors");
          static_assert(bk::is_static_vector_of_size_v<std::decay_t<TVec>, 3> || bk::is_dynamic_matrix_v<std::decay_t<TVec>>, "argument must be matrix/vector type");
          assert(bk::matrix_traits(axis).is_vector_of_size(3));

          _resize_dynamic();

          auto axis_normalized = std::forward<TVec>(axis);

          if (normalize_axis)
          { axis_normalized.normalize_internal(); }

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);

          // col 0
          deriv()->operator()(0, 0) = cosa + axis_normalized[0] * axis_normalized[0] * (1.0 - cosa);
          deriv()->operator()(1, 0) = axis_normalized[1] * axis_normalized[0] * (1.0 - cosa) + axis_normalized[2] * sina;
          deriv()->operator()(2, 0) = axis_normalized[2] * axis_normalized[0] * (1.0 - cosa) - axis_normalized[1] * sina;

          // col 1
          deriv()->operator()(0, 1) = axis_normalized[0] * axis_normalized[1] * (1.0 - cosa) - axis_normalized[2] * sina;
          deriv()->operator()(1, 1) = cosa + axis_normalized[1] * axis_normalized[1] * (1.0 - cosa);
          deriv()->operator()(2, 1) = axis_normalized[2] * axis_normalized[1] * (1.0 - cosa) + axis_normalized[0] * sina;

          // col 2
          deriv()->operator()(0, 2) = axis_normalized[0] * axis_normalized[2] * (1.0 - cosa) + axis_normalized[1] * sina;
          deriv()->operator()(1, 2) = axis_normalized[1] * axis_normalized[2] * (1.0 - cosa) - axis_normalized[0] * sina;
          deriv()->operator()(2, 2) = cosa + axis_normalized[2] * axis_normalized[2] * (1.0 - cosa);
      }
      /// @}

      /// @{ -------------------------------------------------- SET TO X/Y/Z-AXIS 3D ROTATION MATRIX
      constexpr void set_rotation_matrix_3d_x(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "do not call this function from a matrix with non-floating point value type; this will cut off values and cause errors");

          _resize_dynamic();
          deriv()->set_identity();

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);

          deriv()->operator()(1, 1) = cosa;
          deriv()->operator()(2, 1) = sina;
          deriv()->operator()(1, 2) = -sina;
          deriv()->operator()(2, 2) = cosa;
      }

      constexpr void set_rotation_matrix_3d_y(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "do not call this function from a matrix with non-floating point value type; this will cut off values and cause errors");

          _resize_dynamic();
          deriv()->set_identity();

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);

          deriv()->operator()(0, 0) = cosa;
          deriv()->operator()(2, 0) = -sina;
          deriv()->operator()(0, 2) = sina;
          deriv()->operator()(2, 2) = cosa;
      }

      constexpr void set_rotation_matrix_3d_z(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "do not call this function from a matrix with non-floating point value type; this will cut off values and cause errors");

          _resize_dynamic();
          deriv()->set_identity();

          const double cosa = std::cos(angle_rad);
          const double sina = std::sin(angle_rad);

          deriv()->operator()(0, 0) = cosa;
          deriv()->operator()(1, 0) = sina;
          deriv()->operator()(0, 1) = -sina;
          deriv()->operator()(1, 1) = cosa;
      }
      /// @}
  }; // class SquareMatrix3Functions
} // namespace bk::details

#endif //BK_SQUAREMATRIX3FUNCTIONS_H


