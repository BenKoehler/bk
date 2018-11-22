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

#ifndef BK_ATTRIBUTE_INFO_H
#define BK_ATTRIBUTE_INFO_H

#include <bk/Matrix>
#include <bk/StringUtils>

#define BK_DEFINE_ATTRIBUTE_HASH(x, T)\
 constexpr unsigned long long x() { return bk::string_utils::hash(#x); }\
 constexpr const char* x##_str() { return #x; }\
 using x##_value_type = T;\
 template<> struct type_of< x() > {  static constexpr unsigned long long hash = x(); using type = x##_value_type; static constexpr const char* name = x##_str(); };

namespace bk::attribute_info
{
  //------------------------------------------------------------------------------------------------------
  // helper class: type from hash at compile-time
  //------------------------------------------------------------------------------------------------------
  template<unsigned long long TAttributeHash> struct type_of
  {
      static constexpr unsigned long long hash = TAttributeHash;
      using type = void;
      static constexpr const char* name = "";
  };

  template<unsigned long long TAttributeHash> using type_of_t = typename type_of<TAttributeHash>::type;

  //------------------------------------------------------------------------------------------------------
  // attributes
  //------------------------------------------------------------------------------------------------------
  BK_DEFINE_ATTRIBUTE_HASH(center_direction2d, Vec2d)
  BK_DEFINE_ATTRIBUTE_HASH(center_direction3d, Vec3d)

  BK_DEFINE_ATTRIBUTE_HASH(local_coord_sys2d, Mat2d)
  BK_DEFINE_ATTRIBUTE_HASH(local_coord_sys3d, Mat3d)

  BK_DEFINE_ATTRIBUTE_HASH(normal2d, Vec2d)
  BK_DEFINE_ATTRIBUTE_HASH(normal3d, Vec3d)

  BK_DEFINE_ATTRIBUTE_HASH(tangent2d, Vec2d)
  BK_DEFINE_ATTRIBUTE_HASH(tangent3d, Vec3d)

  BK_DEFINE_ATTRIBUTE_HASH(time, double)

  BK_DEFINE_ATTRIBUTE_HASH(velocity, double)
  BK_DEFINE_ATTRIBUTE_HASH(axial_velocity, double)

  BK_DEFINE_ATTRIBUTE_HASH(pressure, double)

  BK_DEFINE_ATTRIBUTE_HASH(cosangle_to_centerline, double)

  BK_DEFINE_ATTRIBUTE_HASH(rotation_direction, double)

  BK_DEFINE_ATTRIBUTE_HASH(length, double)

  BK_DEFINE_ATTRIBUTE_HASH(angle, double)

  BK_DEFINE_ATTRIBUTE_HASH(radius, double)

  BK_DEFINE_ATTRIBUTE_HASH(temp0_double, double)
  BK_DEFINE_ATTRIBUTE_HASH(temp1_double, double)
  BK_DEFINE_ATTRIBUTE_HASH(temp2_double, double)
  BK_DEFINE_ATTRIBUTE_HASH(temp3_double, double)
  BK_DEFINE_ATTRIBUTE_HASH(temp4_double, double)

  BK_DEFINE_ATTRIBUTE_HASH(id, int)

  BK_DEFINE_ATTRIBUTE_HASH(wall_shear_stress, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(wall_shear_stress_vector, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(wall_shear_stress_mean, double)
  BK_DEFINE_ATTRIBUTE_HASH(wall_shear_stress_mean_vector, Vec3d)
  BK_DEFINE_ATTRIBUTE_HASH(oscillatory_shear_index, double)

  BK_DEFINE_ATTRIBUTE_HASH(axial_wall_shear_stress, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(axial_wall_shear_stress_vector, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(axial_wall_shear_stress_mean, double)
  BK_DEFINE_ATTRIBUTE_HASH(axial_wall_shear_stress_mean_vector, Vec3d)
  BK_DEFINE_ATTRIBUTE_HASH(axial_oscillatory_shear_index, double)

  BK_DEFINE_ATTRIBUTE_HASH(circumferential_wall_shear_stress, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(circumferential_wall_shear_stress_vector, MatXd)
  BK_DEFINE_ATTRIBUTE_HASH(circumferential_wall_shear_stress_mean, double)
  BK_DEFINE_ATTRIBUTE_HASH(circumferential_wall_shear_stress_mean_vector, Vec3d)
  BK_DEFINE_ATTRIBUTE_HASH(circumferential_oscillatory_shear_index, double)
} // namespace bk::attribute_info

//#undef BK_DEFINE_ATTRIBUTE_HASH

#endif //BK_ATTRIBUTE_INFO_H


