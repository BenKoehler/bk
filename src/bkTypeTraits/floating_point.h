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

#ifndef BKTYPETRAITS_FLOATING_POINT_H
#define BKTYPETRAITS_FLOATING_POINT_H

#include <complex>
#include <type_traits>

#include "type_wrapper.h"
#include "complex_traits.h"
#include "has_value_type.h"

namespace bk
{

  /// @{ -------------------------------------------------- IS FLOATING POINT
  //! Checks whether the class is a floating point type
  /*!
   * If a class has "value_type" defined, is_floating_point is applied to this internal type
   */
  // base
  template<typename T, typename E = void> struct is_floating_point : std::integral_constant<bool, std::is_floating_point_v<T>>
  { /* empty */ };

  // shortcut
  template<typename T> constexpr bool is_floating_point_v = is_floating_point<T>::value;

  // class with value_type
  template<typename T>
  struct is_floating_point<T, std::enable_if_t<std::is_class_v<T> && bk::has_value_type_v<T>>> : std::integral_constant<bool, std::is_floating_point_v<typename std::decay_t<T>::value_type>>
  { /* empty */ };
  /// @}


  /// @{ -------------------------------------------------- MAKE FLOAT
  // base
  template<typename T, typename E = void> struct make_floating_point : bk::type_wrapper<void>
  {
      static_assert(!std::is_same_v<T, T>, "MAKE_FLOAT: INVALID TEMPLATE PARAMETER; MUST BE ARITHMETIC OR COMPLEX<ARITHMETIC>");
  };

  // shortcut
  template<typename T> using make_floating_point_t = typename make_floating_point<T>::type;

  // arithmetic
  template<typename T>
  struct make_floating_point<T, std::enable_if_t<std::is_arithmetic_v<T>>> : bk::type_wrapper<std::conditional_t<bk::is_floating_point_v<std::decay_t<T>>, std::decay_t<T>, double>>
  { /* empty */ };

  // std::complex
  template<typename T> struct make_floating_point<T, std::enable_if_t<bk::is_complex_v<T>>>
      : bk::type_wrapper<std::conditional_t<bk::is_floating_point_v<typename std::decay_t<T>::value_type>, std::complex<typename std::decay_t<T>::value_type>, std::complex<double>>>
  { /* empty */ };
  /// @}

} // namespace bk

#endif //BKTYPETRAITS_FLOATING_POINT_H


