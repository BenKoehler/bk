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

#ifndef BKGL_QUATERNION_H
#define BKGL_QUATERNION_H

#include <array>
#include <cassert>

#include "../../bkMath/Matrix"
#include "../../bkMath/constants/pi.h"

#include "../lib/bkGL_export.h"

namespace bk
{
  class BKGL_EXPORT Quaternion
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Quaternion;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::array<double, 4> _wxyz;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr Quaternion()
          : _wxyz{{0, 0, 0, 0}}
      { /* do nothing */ }

      constexpr Quaternion(const self_type&) = default;
      constexpr Quaternion(self_type&&) = default;

      constexpr Quaternion(double w, double x, double y, double z)
          : _wxyz{{w, x, y, z}}
      { /* do nothing */ }

      Quaternion(double yaw, double pitch, double roll);

      constexpr Quaternion(const bk::Vec3d& xyz)
          : _wxyz{{0, xyz[0], xyz[1], xyz[2]}}
      { /* do nothing */ }

      Quaternion(double angle, const bk::Vec3d& axis);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Quaternion() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR []
      [[nodiscard]] constexpr double operator[](unsigned int i) const
      {
          assert(i <= 3 && "invalid id");
          return _wxyz[i];
      }

      [[nodiscard]] constexpr double& operator[](unsigned int i)
      {
          assert(i <= 3 && "invalid id");
          return _wxyz[i];
      }
      /// @}

      /// @{ -------------------------------------------------- GET COMPONENT
      [[nodiscard]] constexpr double w() const
      { return operator[](0); }

      [[nodiscard]] constexpr double& w()
      { return operator[](0); }

      [[nodiscard]] constexpr double x() const
      { return operator[](1); }

      [[nodiscard]] constexpr double& x()
      { return operator[](1); }

      [[nodiscard]] constexpr double y() const
      { return operator[](2); }

      [[nodiscard]] constexpr double& y()
      { return operator[](2); }

      [[nodiscard]] constexpr double z() const
      { return operator[](3); }

      [[nodiscard]] constexpr double& z()
      { return operator[](3); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type&) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&&) -> self_type& = default;
      /// @}

      /// @{ -------------------------------------------------- SET
      constexpr void set(double qw, double qx, double qy, double qz)
      {
          w() = qw;
          x() = qx;
          y() = qy;
          z() = qz;
      }
      /// @}

      //====================================================================================================
      //===== COMPARE
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR ==
      [[nodiscard]] constexpr bool operator==(const self_type& other)
      {
          for (unsigned int i = 0; i < 4; ++i)
          {
              if (_wxyz[i] != other._wxyz[i])
              { return false; }
          }

          return true;
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- EULER
      constexpr void from_euler_angle(double yaw, double pitch, double roll)
      {
          yaw *= bk::pi<double> / 360;
          double c_yaw = std::cos(yaw);
          double s_yaw = std::sin(yaw);

          pitch *= bk::pi<double> / 360;
          double c_pitch = std::cos(pitch);
          double s_pitch = std::sin(pitch);

          roll *= bk::pi<double> / 360;
          double c_roll = std::cos(roll);
          double s_roll = std::sin(roll);

          w() = c_roll * c_pitch * c_yaw + s_roll * s_pitch * s_yaw;
          x() = c_roll * s_pitch * c_yaw + s_roll * c_pitch * s_yaw;
          y() = c_roll * c_pitch * s_yaw - s_roll * s_pitch * c_yaw;
          z() = s_roll * c_pitch * c_yaw - c_roll * s_pitch * s_yaw;
      }
      /// @}

      /// @{ -------------------------------------------------- DOT
      [[nodiscard]] constexpr double dot(const self_type& other)
      {
          double dot = 0;

          for (unsigned int i = 0; i < 4; ++i)
          { dot += _wxyz[i] * other._wxyz[i]; }

          return dot;
      }
      /// @}

      /// @{ -------------------------------------------------- NORMALIZE
      constexpr void normalize(double len = static_cast<double>(1))
      {
          const double l = norm();

          if (l != 0)
          { *this *= (len / l); };
      }
      /// @}

      /// @{ -------------------------------------------------- LENGTH
      [[nodiscard]] constexpr double norm() const
      { return std::sqrt(norm_squared()); }
      /// @}

      /// @{ -------------------------------------------------- LENGTH2
      [[nodiscard]] constexpr double norm_squared() const
      {
          double sqLen = 0;

          for (unsigned int i = 0; i < 4; ++i)
          { sqLen += _wxyz[i] * _wxyz[i]; }

          return sqLen;
      }
      /// @}

      /// @{ -------------------------------------------------- TO VEC 3
      [[nodiscard]] constexpr bk::Vec3d to_vec3() const
      { return bk::Vec3d(x(), y(), z()); }
      /// @}

      /// @{ -------------------------------------------------- ROTATE
      [[nodiscard]] constexpr bk::Vec3d& rotate(bk::Vec3d& v)
      {
          self_type temp = *this;
          temp *= self_type(v);
          temp *= self_type(w(), -x(), -y(), -z());

          v = temp.to_vec3();
          v *= static_cast<double>(1) / norm_squared();

          return v;
      }
      /// @}

      //====================================================================================================
      //===== MATH
      //====================================================================================================
      /// @{ -------------------------------------------------- MATH OPERATORS
      constexpr void operator*=(double x)
      {
          for (unsigned int i = 0; i < 4; ++i)
          { _wxyz[i] *= x; }
      }

      constexpr void operator*=(const self_type& other)
      {
          const double mw = w() * other.w() - x() * other.x() - y() * other.y() - z() * other.z();
          const double mx = w() * other.x() + x() * other.w() + y() * other.z() - z() * other.y();
          const double my = w() * other.y() - x() * other.z() + y() * other.w() + z() * other.x();
          const double mz = w() * other.z() + x() * other.y() - y() * other.x() + z() * other.w();

          w() = mw;
          x() = mx;
          y() = my;
          z() = mz;
      }

      constexpr void operator+=(const self_type& other)
      {
          for (unsigned int i = 0; i < 4; ++i)
          { _wxyz[i] += other._wxyz[i]; }
      }

      constexpr void operator-=(const self_type& other)
      {
          for (unsigned int i = 0; i < 4; ++i)
          { _wxyz[i] -= other._wxyz[i]; }
      }
      /// @}
  }; // class Quaternion

  //====================================================================================================
  //===== EXTERNAL MATH OPERATORS
  //====================================================================================================
  [[nodiscard]] constexpr Quaternion operator+(const Quaternion& a, const Quaternion& b)
  {
      Quaternion res(a);
      res += b;
      return res;
  }

  [[nodiscard]] constexpr Quaternion operator-(const Quaternion& a, const Quaternion& b)
  {
      Quaternion res(a);
      res -= b;
      return res;
  }

  [[nodiscard]] constexpr Quaternion operator*(const Quaternion& a, const Quaternion& b)
  {
      Quaternion res(a);
      res *= b;
      return res;
  }

  [[nodiscard]] constexpr Quaternion operator*(const Quaternion& a, double x)
  {
      Quaternion res(a);
      res *= x;
      return res;
  }

  [[nodiscard]] constexpr Quaternion operator*(double x, const Quaternion& a)
  { return a * x; }

  [[nodiscard]] Quaternion operator*(const bk::Vec3d& v, const Quaternion& a);
  [[nodiscard]] Quaternion operator*(const Quaternion& a, const bk::Vec3d& v);
} // namespace bk

#endif //BKGL_QUATERNION_H


