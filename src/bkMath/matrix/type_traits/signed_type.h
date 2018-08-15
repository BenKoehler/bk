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

#ifndef BK_SIGNED_TYPE_TRAITS_H
#define BK_SIGNED_TYPE_TRAITS_H

#include <type_traits>

#include <bkMath/matrix/type_traits/matrix_traits.h>
#include <bkTypeTraits/complex_traits.h>
#include <bkTypeTraits/has_value_type.h>
#include <bkTypeTraits/type_wrapper.h>

namespace bk
{
  /// @{ -------------------------------------------------- IS SIGNED
  //! Determines whether the specified type has a sign.
  /*!
   *  - Signed integral and floating point types have signs,
   *    other types (e.g. bool/classes) do not
   * 
   *  - If a class has "value_type" defined, is_signed is
   *    applied to this
   */
  // base
  template<typename T, typename E_ = void> struct is_signed : std::false_type
  { /* empty */ };

  // shortcut
  template<typename T> constexpr bool is_signed_v = is_signed<T>::value;

  // integral type (except bool)
  template<typename T> struct is_signed<T, std::enable_if_t<std::is_integral_v<T> && !std::is_class_v<T> && !std::is_same_v<bool, T>>> : std::integral_constant<bool, std::is_signed_v<T>>
  { /* empty */ };

  // bool
  template<typename T> struct is_signed<T, std::enable_if_t<std::is_same_v<T, bool>>> : std::false_type
  { /* empty */ };

  // floating point type
  template<typename T> struct is_signed<T, std::enable_if_t<!std::is_class_v<T> && std::is_floating_point_v<T>>> : std::true_type
  { /* empty */ };

  // class with value_type
  template<typename T> struct is_signed<T, std::enable_if_t<std::is_class_v<T> && bk::has_value_type_v<T>>> : std::integral_constant<bool, is_signed_v<typename T::value_type>>
  { /* empty */ };

  // class without value_type
  template<typename T> struct is_signed<T, std::enable_if_t<std::is_class_v<T> && !bk::has_value_type_v<T>>> : std::false_type
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- MAKE SIGNED
  //! std::make_signed that also accepts floating point and other types
  /*!
   *  Since floating point types already have a sign, they are
   *  simply returned as signed type by make_signed
   * 
   *  - T = integral type except bool -> make_signed<T>
   *  - T = other type -> T
   * 
   *  - std::complex<T = integral type except bool> -> std::complex<make_signed<T>>
   *  - std::complex<T = other type> -> std::complex<T>
   * 
   *  - Matrix<T = integral type except bool> -> Matrix<make_signed<T>>
   *  - Matrix<T = other type> -> Matrix<T>
   */
  // base + shortcut
  template<typename T, typename E_ = void> struct make_signed
  {
      static_assert(!std::is_same_v<T, T>, "USE OF MAKE_SIGNED WITH INVALID TYPE");
  };

  template<typename T> using make_signed_t = typename make_signed<T>::type;

  // integral type (except bool)
  template<typename T> struct make_signed<T, std::enable_if_t<std::is_integral_v<T> && !std::is_class_v<T> && !std::is_same_v<bool, T>>> : bk::type_wrapper<std::make_signed_t<T>>
  { /* empty */ };

  // floating point type or bool
  template<typename T> struct make_signed<T, std::enable_if_t<!std::is_class_v<T> && (std::is_floating_point_v<T> || std::is_same_v<T, bool>)>> : bk::type_wrapper<T>
  { /* empty */ };

  // std::complex
  template<typename T> struct make_signed<T, std::enable_if_t<bk::is_complex_v<T> && bk::has_value_type_v<T>>> : bk::type_wrapper<std::complex<make_signed_t<typename T::value_type>>>
  { /* empty */ };

  // Matrix
  template<typename T> struct make_signed<T, std::enable_if_t<bk::is_matrix_v<T> && bk::has_value_type_v<T>>>
      : bk::type_wrapper<typename T::template self_template_type<make_signed_t<typename T::value_type>, T::Cols, T::Rows, T::Alignment>>
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- IS UNSIGNED
  ///
  //! Determines whether the specified type has no sign.
  /*!
   *  - Signed integral and floating point types have signs,
   *    other types (e.g. bool/classes) do not
   * 
   *  - If a class has "value_type" defined, is_unsigned is
   *    applied to this
   */
  // base
  template<typename T, typename E_ = void> struct is_unsigned : std::true_type
  { /* empty */ };

  // shortcut
  template<typename T> constexpr bool is_unsigned_v = is_unsigned<T>::value;

  // integral type (except bool)
  template<typename T> struct is_unsigned<T, std::enable_if_t<std::is_integral_v<T> && !std::is_class_v<T> && !std::is_same_v<bool, T>>> : std::integral_constant<bool, std::is_unsigned_v<T>>
  { /* empty */ };

  // bool
  template<typename T> struct is_unsigned<T, std::enable_if_t<std::is_same_v<T, bool>>> : std::true_type
  { /* empty */ };

  // floating point type
  template<typename T> struct is_unsigned<T, std::enable_if_t<std::is_floating_point_v<T> && !std::is_class_v<T>>> : std::false_type
  { /* empty */ };

  // class with value_type
  template<typename T> struct is_unsigned<T, std::enable_if_t<std::is_class_v<T> && bk::has_value_type_v<T>>> : std::integral_constant<bool, is_unsigned_v<typename T::value_type>>
  { /* empty */ };

  // class without value_type
  template<typename T> struct is_unsigned<T, std::enable_if_t<std::is_class_v<T> && !bk::has_value_type_v<T>>> : std::true_type
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- MAKE UNSIGNED
  //! std::make_unsigned that also accepts floating point and other types
  /*!
   *  Since floating point types already have a sign, they are
   *  simply returned as signed type by make_signed
   * 
   *  - T = integral type except bool -> make_signed<T>
   *  - T = other type -> T
   * 
   *  - std::complex<T = integral type except bool> -> std::complex<make_signed<T>>
   *  - std::complex<T = other type> -> std::complex<T>
   * 
   *  - Matrix<T = integral type except bool> -> Matrix<make_signed<T>>
   *  - Matrix<T = other type> -> Matrix<T>
   */
  // base + shortcut
  template<typename T, typename E_ = void> struct make_unsigned
  {
      static_assert(!std::is_same_v<T, T>, "USE OF MAKE_SIGNED WITH INVALID TYPE");
  };

  template<typename T> using make_unsigned_t = typename make_unsigned<T>::type;

  // integral type (except bool)
  template<typename T> struct make_unsigned<T, std::enable_if_t<std::is_integral_v<T> && !std::is_class_v<T> && !std::is_same_v<bool, T>>> : bk::type_wrapper<std::make_unsigned_t<T>>
  { /* empty */ };

  // bool
  template<typename T> struct make_unsigned<T, std::enable_if_t<std::is_same_v<T, bool>>> : bk::type_wrapper<T>
  { /* empty */ };

  // std::complex
  template<typename T>
  struct make_unsigned<T, std::enable_if_t<bk::is_complex_v<T> && bk::has_value_type_v<T>>> : bk::type_wrapper<typename T::template self_template_type<make_unsigned_t<typename T::value_type>>>
  { /* empty */ };

  // Matrix
  template<typename T> struct make_unsigned<T, std::enable_if_t<bk::is_matrix_v<T> && bk::has_value_type_v<T>>>
      : bk::type_wrapper<typename T::template self_template_type<make_unsigned_t<typename T::value_type>, T::Cols, T::Rows, T::Alignment>>
  { /* empty */ };
  /// @}
} // namespace bk

#endif //BK_SIGNED_TYPE_TRAITS_H


