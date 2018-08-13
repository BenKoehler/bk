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

#include <cassert>
#include <cmath>

#include "../../../include/bkGL/trackball/Quaternion.h"

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Quaternion::Quaternion(double yaw, double pitch, double roll)
  { from_euler_angle(yaw, pitch, roll); }

  Quaternion::Quaternion(double angle, const bk::Vec3d& axis)
  {
      const double d = axis.norm();
      assert(d != 0.0 && "quaternion axis is Zero");

      angle *= bk::pi<double> / 180;
      const double s = std::sin(angle / 2) / d;

      w() = std::cos(angle / 2);
      x() = axis[0] * s;
      y() = axis[1] * s;
      z() = axis[2] * s;
  }
  /// @}

  //====================================================================================================
  //===== EXTERNAL MATH OPERATORS
  //====================================================================================================
  Quaternion operator*(const bk::Vec3d& v, const Quaternion& a)
  {
      Quaternion res(0, v);
      res *= a;
      return res;
  }

  Quaternion operator*(const Quaternion& a, const bk::Vec3d& v)
  { return a * Quaternion(0, v); }
} // namespace bk


