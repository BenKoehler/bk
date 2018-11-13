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

#ifndef BK_VECTOR3FUNCTIONS_H
#define BK_VECTOR3FUNCTIONS_H

#include <cassert>
#include <cmath>
#include <type_traits>
#include <utility>

#include <bkMath/functions/equals_approx.h>
#include <bkMath/functions/radians_degree_conversion.h>
#include <bkMath/matrix/MatrixAlignment.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>
#include <bkTypeTraits/floating_point.h>

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isVec3> class Vector3Functions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class Vector3Functions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Vector3Functions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr Vector3Functions() noexcept = default;
      constexpr Vector3Functions(const self_type& other) noexcept = default;
      constexpr Vector3Functions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Vector3Functions() noexcept = default;
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
      /// @}
    public:

      /// @{ -------------------------------------------------- SET TO AXIS
      constexpr void set_x_axis()
      { deriv()->set(1, 0, 0); }

      constexpr void set_y_axis()
      { deriv()->set(0, 1, 0); }

      constexpr void set_z_axis()
      { deriv()->set(0, 0, 1); }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER FUNCTIONS FOR CROSS PRODUCT
      //! assertion that rhs is a 3D vector
    private:
      template<typename V>
      constexpr void _cross_assert_rhs_is_vec3(const V& rhs) const
      {
          static_assert(bk::matrix_traits<V>::template is_static_vector_of_size<3> || bk::matrix_traits<V>::is_dynamic);
          assert(bk::matrix_traits(rhs).is_vector_of_size(3));
      }
    public:
      /// @}

      /// @{ -------------------------------------------------- CROSS PRODUCT
      //! Calculate the cross product of two 3D vectors, which yields a vector that is perpendicular to both
      template<typename TVec3>
      [[nodiscard]] constexpr auto cross(const TVec3& rhs) const&
      {
          static_assert(bk::is_static_vector_of_size_v<TVec3, 3> || bk::is_dynamic_matrix_v<TVec3>);
          assert(bk::matrix_traits(rhs).is_vector_of_size(3));

          using signed_common_value_type = bk::signed_common_type_t<value_type, typename TVec3::value_type, true /*enforceSign*/>;

          auto res = deriv()->template copy<signed_common_value_type>();
          res.cross_internal(rhs);
          return res;
      }

      template<typename TVec3>
      [[nodiscard]] constexpr auto cross(TVec3&& rhs)&&
      {
          using T = std::decay_t<TVec3>;
          static_assert(bk::is_static_vector_of_size_v<T, 3> || bk::is_dynamic_matrix_v<T>);
          assert(bk::matrix_traits(rhs).is_vector_of_size(3));

          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_signed_common_type_v<value_type, typename T::value_type, true /*enforceSign*/>)
          {
              cross_internal(rhs);
              return std::move(*deriv());
          }
          else
          { return static_cast<const self_type*>(this)->cross(rhs); }
      }

      template<typename TVec3>
      constexpr void cross_internal(const TVec3& rhs)
      {
          _cross_assert_rhs_is_vec3(rhs);
          auto x0 = deriv()->operator[](1) * rhs[2] - deriv()->operator[](2) * rhs[1];
          auto x1 = deriv()->operator[](2) * rhs[0] - deriv()->operator[](0) * rhs[2];
          auto x2 = deriv()->operator[](0) * rhs[1] - deriv()->operator[](1) * rhs[0];
          deriv()->set(std::move(x0), std::move(x1), std::move(x2));
      }

      // alias

      template<typename TVec3>
      [[nodiscard]] constexpr auto cross3(TVec3&& rhs)&&
      { return std::forward<self_type>(*this).cross(std::forward<TVec3>(rhs)); }

      template<typename TVec3>
      [[nodiscard]] constexpr auto cross3(TVec3&& rhs) const&
      { return cross(std::forward<TVec3>(rhs)); }

      template<typename TVec3>
      constexpr void cross3_internal(TVec3&& rhs)
      { cross_internal(std::forward<TVec3>(rhs)); }
      /// @}

      /// @{ -------------------------------------------------- ROTATE AROUND ARBITRARY AXIS
      //! Rotation around a general unit vector (3D only).
      /*!
       *  Uses Rodrigues' Rotation Formula to rotate a 3D vector
       *  around a given axis. A normalized version of the passed
       *  axis is used in the function.
       *
       *  \param angle The rotation angle in radians.
       *  \param axis  The axis vector to rotate around.
       */
      template<typename Vec_>
      constexpr void rotate_rad_internal(const Vec_& axis, double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR3 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          static_assert((bk::is_static_vector_v<std::decay_t<Vec_>> && ((std::decay_t<Vec_>::RowsAtCompileTime() == 3 && std::decay_t<Vec_>::ColsAtCompileTime() == 1) || (std::decay_t<Vec_>::RowsAtCompileTime() == 1 && std::decay_t<Vec_>::ColsAtCompileTime() == 3))) || bk::is_dynamic_matrix_v<std::decay_t<Vec_>>);
          assert(matrix_traits(axis).is_vector_of_size(3));
          assert(deriv()->num_elements() == 3);

          const auto cosa = std::cos(static_cast<value_type>(angle_rad));
          const auto sina = std::sin(static_cast<value_type>(angle_rad));
          auto x0 = deriv()->operator[](0) * (cosa + axis[0]*axis[0] * (static_cast<value_type>(1) - cosa)) + deriv()->operator[](1) * (axis[0] * axis[1] * (static_cast<value_type>(1) - cosa) - axis[2] * sina) + deriv()->operator[](2) * (axis[0] * axis[2] * (static_cast<value_type>(1) - cosa) + axis[1] * sina);
          auto x1 = deriv()->operator[](0) * (axis[1] * axis[0] * (static_cast<value_type>(1) - cosa) + axis[2] * sina) + deriv()->operator[](1) * (cosa + axis[1] * axis[1] * (static_cast<value_type>(1) - cosa)) + deriv()->operator[](2) * (axis[1] * axis[2] * (static_cast<value_type>(1) - cosa) - axis[0] * sina);
          auto x2 = deriv()->operator[](0) * (axis[2] * axis[0] * (static_cast<value_type>(1) - cosa) - axis[1] * sina) + deriv()->operator[](1) * (axis[2] * axis[1] * (static_cast<value_type>(1) - cosa) + axis[0] * sina) + deriv()->operator[](2) * (cosa + axis[2] * axis[2] * (static_cast<value_type>(1) - cosa));
          deriv()->set(std::move(x0), std::move(x1), std::move(x2));
      }

      template<typename Vec_, typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_rad(const Vec_& axis, double angle_rad)&&
      {
          rotate_rad_internal(axis, angle_rad);
          return *std::move(deriv());
      }

      template<typename Vec_>
      [[nodiscard]] constexpr auto rotate_rad(const Vec_& axis, double angle_rad) const&
      {
          using Float_ = bk::make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_rad_internal(axis, angle_rad);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- ROTATE AROUND X-AXIS
      //! Rotation of 3D vector around x-axis
      /*!
       *  Multiply x-axis rotation matrix to the vector.
       *  The matrix is:  1    0       0
       *                  0  cos(a) -sin(a)
       *                  0  sin(a)  cos(a)
       *
       *  \param angle The rotation angle in radians
       */
      constexpr void rotate_rad_around_x_axis_internal(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR3 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          assert(deriv()->num_elements() == 3);

          const auto cosa = std::cos(static_cast<value_type>(angle_rad));
          const auto sina = std::sin(static_cast<value_type>(angle_rad));
          auto x0 = deriv()->operator[](0);
          auto x1 = cosa * deriv()->operator[](1) - sina * deriv()->operator[](2);
          auto x2 = sina * deriv()->operator[](1) + cosa * deriv()->operator[](2);
          deriv()->set(std::move(x0), std::move(x1), std::move(x2));
      }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_rad_around_x_axis(double angle_rad)&&
      {
          rotate_rad_around_x_axis_internal(angle_rad);
          return *std::move(deriv());
      }

      [[nodiscard]] constexpr auto rotate_rad_around_x_axis(double angle_rad) const&
      {
          using Float_ = bk::make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_rad_around_x_axis_internal(angle_rad);
          return res;
      }

      //

      constexpr void rotate_deg_around_x_axis_internal(double angle_deg)
      { rotate_rad_around_x_axis_internal(bk::degrees_to_radians(angle_deg)); }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_deg_around_x_axis(double angle_deg)&&
      { return std::forward<self_type>(*this).rotate_rad_around_x_axis(bk::degrees_to_radians(angle_deg)); }

      [[nodiscard]] constexpr auto rotate_deg_around_x_axis(double angle_deg) const&
      { return rotate_rad_around_x_axis(bk::degrees_to_radians(angle_deg)); }
      /// @}

      /// @{ -------------------------------------------------- ROTATE AROUND Y-AXIS
      //! Rotation of 3D vector around y-axis
      /*!
       *  Multiply y-axis rotation matrix to the vector.
       *  The matrix is:  cos(a)  0  sin(a)
       *                    0     1    0
       *                 -sin(a)  0  cos(a)
       *
       *  \param angle The rotation angle in radians
       */
      constexpr void rotate_rad_around_y_axis_internal(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR3 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          assert(deriv()->num_elements() == 3);

          const auto cosa = std::cos(static_cast<value_type>(angle_rad));
          const auto sina = std::sin(static_cast<value_type>(angle_rad));
          auto x0 = cosa * deriv()->operator[](0) + sina * deriv()->operator[](2);
          auto x1 = deriv()->operator[](1);
          auto x2 = -sina * deriv()->operator[](0) + cosa * deriv()->operator[](2);
          deriv()->set(std::move(x0), std::move(x1), std::move(x2));
      }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_rad_around_y_axis(double angle_rad)&&
      {
          rotate_rad_around_y_axis_internal(angle_rad);
          return *std::move(deriv());
      }

      [[nodiscard]] constexpr auto rotate_rad_around_y_axis(double angle_rad) const&
      {
          using Float_ = bk::make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_rad_around_y_axis_internal(angle_rad);
          return res;
      }

      //

      constexpr void rotate_deg_around_y_axis_internal(double angle_deg)
      { rotate_rad_around_y_axis_internal(bk::degrees_to_radians(angle_deg)); }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_deg_around_y_axis(double angle_deg)&&
      { return std::forward<self_type>(*this).rotate_rad_around_y_axis(bk::degrees_to_radians(angle_deg)); }

      [[nodiscard]] constexpr auto rotate_deg_around_y_axis(double angle_deg) const&
      { return rotate_rad_around_y_axis(bk::degrees_to_radians(angle_deg)); }
      /// @}

      /// @{ -------------------------------------------------- ROTATE AROUND Z-AXIS
      //! Rotation of 3D vector around y-axis
      /*!
       *  Multiply y-axis rotation matrix to the vector.
       *  The matrix is:  cos(a)  -sin(a)  0
       *                  sin(a)   cos(a)  0
       *                    0       0      1
       *
       *  \param angle The rotation angle in radians
       */
      constexpr void rotate_rad_around_z_axis_internal(double angle_rad)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR3 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          assert(deriv()->num_elements() == 3);

          const auto cosa = std::cos(static_cast<value_type>(angle_rad));
          const auto sina = std::sin(static_cast<value_type>(angle_rad));
          auto x0 = cosa * deriv()->operator[](0) - sina * deriv()->operator[](1);
          auto x1 = sina * deriv()->operator[](0) + cosa * deriv()->operator[](1);
          auto x2 = deriv()->operator[](2);
          deriv()->set(std::move(x0), std::move(x1), std::move(x2));
      }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_rad_around_z_axis(double angle_rad)&&
      {
          rotate_rad_around_z_axis_internal(angle_rad);
          return *std::move(deriv());
      }

      [[nodiscard]] constexpr auto rotate_rad_around_z_axis(double angle_rad) const&
      {
          using Float_ = bk::make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_rad_around_z_axis_internal(angle_rad);
          return res;
      }

      //

      constexpr void rotate_deg_around_z_axis_internal(double angle_deg)
      { rotate_rad_around_z_axis_internal(bk::degrees_to_radians(angle_deg)); }

      template<typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[nodiscard]] constexpr auto rotate_deg_around_z_axis(double angle_deg)&&
      { return std::forward<self_type>(*this).rotate_rad_around_z_axis(bk::degrees_to_radians(angle_deg)); }

      [[nodiscard]] constexpr auto rotate_deg_around_z_axis(double angle_deg) const&
      { return rotate_rad_around_z_axis(bk::degrees_to_radians(angle_deg)); }
      /// @}

      /// @{ -------------------------------------------------- ROTATE ONTO DIRECTION
      //! Rotation onto direction internal (3D).
      /*!
       *  Rotates the vector so that it is aligned with a
       *  given direction. The normalized direction and normalized
       *  vector are the same after the operation.
       *
       *  \param direction The direction to align the vector with.
       *  \param angle_out Optional return parameter for the used
       *                   angle in radians around the rotation_axis.
       *                   the parameter is set to nullptr by default.
       *  \param axis_out  Optional return parameter for the rotation_axis.
       *                   The parameter is set to nullptr by default.
       *
       *  \return false if one vector has length 0 or if vectors are already parallel
       */
      template<typename TVec0, typename TVec1 = TVec0, typename TReal = double>
      [[maybe_unused]] constexpr bool rotate_onto_direction_internal(TVec0&& direction, TReal* rotAngle_out = nullptr, TVec1* rotAxis_out = nullptr)
      {
          static_assert(bk::is_floating_point_v<value_type>, "VECTOR3 :: ROTATION_INTERNAL - DO NOT USE THIS ON VECTORS WITH INTEGRAL VALUE TYPES; OTHERWISE VALUES WILL BE CUT OFF WHICH CAUSES ERRORS");
          static_assert((bk::is_static_vector_v<std::decay_t<TVec0>> && ((std::decay_t<TVec0>::RowsAtCompileTime() == 3 && std::decay_t<TVec0>::ColsAtCompileTime() == 1) || (std::decay_t<TVec0>::RowsAtCompileTime() == 1 && std::decay_t<TVec0>::ColsAtCompileTime() == 3))) || bk::is_dynamic_matrix_v<std::decay_t<TVec0>>);
          assert(matrix_traits(direction).is_vector_of_size(3));
          assert(deriv()->num_elements() == 3);

          const auto dirnorm = direction.norm();

          if (bk::equals_approx(dirnorm, 0))
          { // target direction has length 0
              return false;
          }

          auto rotAxis = std::forward<TVec0>(direction) / dirnorm;

          if (bk::equals_approx(std::cos(deriv()->angle(rotAxis)), 1))
          { // this and target direction are already parallel
              return false;
          }

          const auto rotAngle = -deriv()->angle(rotAxis);
          rotAxis.cross_internal(*deriv()); // cross order is important
          rotAxis.normalize_internal();
          this->rotate_rad_internal(rotAxis, rotAngle);

          if (rotAngle_out != nullptr)
          {
              static_assert(std::is_arithmetic_v<TReal>);
              *rotAngle_out = static_cast<TReal>(rotAngle);
          }

          if (rotAxis_out != nullptr)
          {
              static_assert((bk::is_static_vector_v<std::decay_t<TVec1>> && ((std::decay_t<TVec1>::RowsAtCompileTime() == 3 && std::decay_t<TVec1>::ColsAtCompileTime() == 1) || (std::decay_t<TVec1>::RowsAtCompileTime() == 1 && std::decay_t<TVec1>::ColsAtCompileTime() == 3))) || bk::is_dynamic_matrix_v<std::decay_t<TVec1>>);
              assert(matrix_traits(*rotAxis_out).is_vector_of_size(3));
              assert(rotAxis_out->num_elements() == 3);

              *rotAxis_out = std::move(rotAxis);
          }
          return true;
      }

      template<typename TVec0, typename TVec1 = TVec0, typename TReal = double, typename _V = value_type, std::enable_if_t<bk::is_floating_point_v<_V>>* = nullptr>
      [[maybe_unused]] constexpr auto rotate_onto_direction(TVec0&& direction, TReal* rotAngle_out = nullptr, TVec1* rotAxis_out = nullptr)&&
      {
          rotate_onto_direction_internal(std::forward<TVec0>(direction), rotAngle_out, rotAxis_out);
          return std::move(*deriv());
      }

      template<typename TVec0, typename TVec1 = TVec0, typename TReal = double>
      [[maybe_unused]] constexpr auto rotate_onto_direction(TVec0&& direction, TReal* rotAngle_out = nullptr, TVec1* rotAxis_out = nullptr) const&
      {
          using Float_ = make_floating_point_t<value_type>;
          auto res = deriv()->template copy<Float_>();
          res.rotate_onto_direction_internal(std::forward<TVec0>(direction), rotAngle_out, rotAxis_out);
          return res;
      }
      /// @}

  }; // class Vector3Functions
} // namespace bk::details

#endif //BK_VECTOR3FUNCTIONS_H


