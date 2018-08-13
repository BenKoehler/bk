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

#ifndef BK_COMMON_TYPE_H
#define BK_COMMON_TYPE_H

#include <complex>
#include <type_traits>

#include "../../../bkTypeTraits/complex_traits.h"
#include "../../../bkTypeTraits/has_value_type.h"
#include "../../../bkTypeTraits/type_wrapper.h"

#include "signed_type.h"
#include "matrix_traits.h"

namespace bk
{
  /// @{ -------------------------------------------------- IS COMMON TYPE
  //! Determines whether A is the common type of A and B
  /*!
   *  - plain types are compared directly
   *  - if both are classes and have "value_type" defined, those are compared
   *  - if A is a complex number and B is a plain type, A::value_type and B are compared
   */
  // base
  template<typename A, typename B, typename E = void> struct is_common_type : std::false_type
  { /* empty */ };

  // shortcut
  template<typename A, typename B> constexpr bool is_common_type_v = is_common_type<A, std::decay_t<B>>::value;

  // arithmetic type / arithmetic type
  template<typename A, typename B> struct is_common_type<A, B, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B> >>
      : std::integral_constant<bool, std::is_same<A, std::common_type_t<A, B>>::value>
  { /* empty */ };

  // class with value_type / class with value_type
  template<typename A, typename B>
  struct is_common_type<A, B, std::enable_if_t<std::is_class<std::decay_t<A>>::value && bk::has_value_type_v<std::decay_t<A>> && std::is_class<std::decay_t<B>>::value && bk::has_value_type_v<std::decay_t<B>> >>
      : std::integral_constant<bool, is_common_type_v<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>
  { /* empty */ };

  // complex / arithmetic type
  template<typename A, typename B>
  struct is_common_type<A, B, std::enable_if_t<bk::is_complex_v<std::decay_t<A>> && std::is_arithmetic<std::decay_t<B>>::value && !std::is_class<std::decay_t<B>>::value >>
      : std::integral_constant<bool, is_common_type_v<typename std::decay_t<A>::value_type, B>>
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- SIGNED COMMON TYPE
  //! Returns make_signed of the common_type of A and B
  /*!
   *  - If A and B are unsigned, the result type will be unsigned.
   *    If a signed return type shall be enforced, the third template parameter forceSign (false by default)
   *     can be set to true
   *  - signed_common_type is only defined for arithmetic types, i.e., integral or floating point values,
   *    or std::complex with arithmetic value_types
   */
  // base
  template<typename A, typename B, bool forceSign = false, typename E = void> struct signed_common_type : bk::type_wrapper<void>
  { /* empty */ };

  // shortcut
  template<typename A, typename B, bool forceSign = false> using signed_common_type_t = typename signed_common_type<std::decay_t<A>, std::decay_t<B>, forceSign>::type;

  // arithmetic type / arithmetic type
  template<typename A, typename B, bool forceSign>
  struct signed_common_type<A, B, forceSign, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<A> && std::is_unsigned_v<B> && !forceSign, std::common_type_t<A, B>, bk::make_signed_t<std::common_type_t<A, B>>>>
  { /* empty */ };

  // std::complex / arithmetic type
  template<typename A, typename B, bool forceSign> struct signed_common_type<A, B, forceSign, std::enable_if_t<bk::is_complex_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<typename std::decay_t<A>::value_type> && std::is_unsigned_v<B> && !forceSign, typename std::decay_t<A>::template self_template_type<std::common_type_t<typename std::decay_t<A>::value_type, B>>, typename std::decay_t<A>::template self_template_type<bk::make_signed_t<std::common_type_t<typename std::decay_t<A>::value_type, B>>>>>
  { /* empty */ };

  // arithmetic type / std::complex
  template<typename A, typename B, bool forceSign> struct signed_common_type<A, B, forceSign, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && bk::is_complex_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<A> && std::is_unsigned_v<typename std::decay_t<B>::value_type> && !forceSign, typename std::decay_t<B>::template self_template_type<std::common_type_t<A, typename std::decay_t<B>::value_type>>, typename std::decay_t<B>::template self_template_type<bk::make_signed_t<std::common_type_t<A, typename std::decay_t<B>::value_type>>>>>
  { /* empty */ };

  template<typename A, typename B, bool forceSign> struct signed_common_type<A, B, forceSign, std::enable_if_t<bk::is_complex_v<A> && bk::is_complex_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<typename std::decay_t<A>::value_type> && std::is_unsigned_v<typename std::decay_t<B>::value_type> && !forceSign, std::complex<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>, std::complex<bk::make_signed_t<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>>>>
  { /* empty */ };

  // Matrix (not reference) / arithmetic type
  template<typename A, typename B, bool forceSign>
  struct signed_common_type<A, B, forceSign, std::enable_if_t<bk::is_matrix_v<A> && !bk::is_ref_matrix_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<typename std::decay_t<A>::value_type> && std::is_unsigned_v<B> && !forceSign, typename std::decay_t<A>::template self_template_type<std::common_type_t<typename std::decay_t<A>::value_type, B>, std::decay_t<A>::RowsAtCompileTime(), std::decay_t<A>::ColsAtCompileTime(), std::decay_t<A>::AlignmentAtCompileTime()>, typename std::decay_t<A>::template self_template_type<bk::make_signed_t<std::common_type_t<typename std::decay_t<A>::value_type, B>>, std::decay_t<A>::RowsAtCompileTime(), std::decay_t<A>::ColsAtCompileTime(), std::decay_t<A>::AlignmentAtCompileTime()>>>
  { /* empty */ };

  // arithmetic type / Matrix (not reference)
  template<typename A, typename B, bool forceSign>
  struct signed_common_type<A, B, forceSign, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && bk::is_matrix_v<B> && !bk::is_ref_matrix_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<A> && std::is_unsigned_v<typename std::decay_t<B>::value_type> && !forceSign, typename std::decay_t<B>::template self_template_type<std::common_type_t<A, typename std::decay_t<B>::value_type>, std::decay_t<B>::RowsAtCompileTime(), std::decay_t<B>::ColsAtCompileTime(), std::decay_t<B>::AlignmentAtCompileTime()>, typename std::decay_t<B>::template self_template_type<bk::make_signed_t<std::common_type_t<A, typename std::decay_t<B>::value_type>>, std::decay_t<B>::RowsAtCompileTime(), std::decay_t<B>::ColsAtCompileTime(), std::decay_t<B>::AlignmentAtCompileTime()>>>
  { /* empty */ };

  // Matrix (not reference) / Matrix (not reference)
  template<typename A, typename B, bool forceSign>
  struct signed_common_type<A, B, forceSign, std::enable_if_t<bk::is_matrix_v<A> && !bk::is_ref_matrix_v<A> && bk::is_matrix_v<B> && !bk::is_ref_matrix_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_unsigned_v<typename std::decay_t<A>::value_type> && std::is_unsigned_v<typename std::decay_t<B>::value_type> && !forceSign, typename std::decay_t<A>::template self_template_type<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>, std::decay_t<A>::RowsAtCompileTime(), std::decay_t<A>::ColsAtCompileTime(), std::decay_t<A>::AlignmentAtCompileTime()>, typename std::decay_t<A>::template self_template_type<bk::make_signed_t<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>, std::decay_t<A>::RowsAtCompileTime(), std::decay_t<A>::ColsAtCompileTime(), std::decay_t<A>::AlignmentAtCompileTime()>>>
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- IS SIGNED COMMON TYPE
  //! Checks if A equals the signed_common_type of A and B
  /*!
   *  - A and B can each either be an arithmetic type (integral or float; no pointers) or a
   *    class with "value_type" defined
   *  - for unknown types, false is returned by default
   *  - analogous to signed_common_type_t, signs can be enforced if the third template parameter forceSign
   *    (false by default) is set to true
   */
  // base
  template<typename A, typename B, bool forceSign = false, typename E = void> struct is_signed_common_type : std::false_type
  { /* empty */ };

  // shortcut
  template<typename A, typename B, bool forceSign = false> constexpr bool is_signed_common_type_v = is_signed_common_type<A, std::decay_t<B>, forceSign>::value;

  // (arithmetic type or class with value_type) / (arithmetic type or class with value_type)
  template<typename A, typename B, bool forceSign>
  struct is_signed_common_type<A, B, forceSign, std::enable_if_t<((std::is_arithmetic_v<A> && !std::is_class_v<A>) || (std::is_class<std::decay_t<A>>::value && bk::has_value_type_v<std::decay_t<A>>)) && ((std::is_arithmetic_v<B> && !std::is_class_v<B>) || (std::is_class<std::decay_t<B>>::value && bk::has_value_type_v<std::decay_t<B>>))>>
      : std::integral_constant<bool, std::is_same<A, signed_common_type_t<A, B, forceSign>>::value>
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- SIGNED COMMON FLOAT
  //! Returns a suitable floating point type for the common_type of A and B
  /*!
   *  - If A and B are integral, the result type will be double
   *  - signed_common_float is only defined for arithmetic types, i.e. integral or floating point values (no pointers),
   *    or std::complex with arithmetic value_types
   */
  // base
  template<typename A, typename B, typename E = void> struct signed_common_float : bk::type_wrapper<void>
  {
      static_assert(!std::is_same<A, A>::value, "COULD NOT DERIVE SIGNED COMMON FLOAT OF GIVEN TYPES");
  };

  // shortcut
  template<typename A, typename B> using signed_common_float_t = typename signed_common_float<std::decay_t<A>, std::decay_t<B>>::type;

  // arithmetic type / arithmetic type
  template<typename A, typename B> struct signed_common_float<A, B, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_floating_point<std::common_type_t<A, B>>::value, std::common_type_t<A, B>, double>>
  { /* empty */ };

  // std::complex / arithmetic type
  template<typename A, typename B> struct signed_common_float<A, B, std::enable_if_t<bk::is_complex_v<A> && std::is_arithmetic_v<B> && !std::is_class_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_floating_point<std::common_type_t<typename std::decay_t<A>::value_type, B>>::value, typename std::decay_t<A>::template self_template_type<std::common_type_t<typename std::decay_t<A>::value_type, B>>, typename std::decay_t<A>::template self_template_type<double>>>
  { /* empty */ };

  // arithmetic type / std::complex
  template<typename A, typename B> struct signed_common_float<A, B, std::enable_if_t<std::is_arithmetic_v<A> && !std::is_class_v<A> && bk::is_complex_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_floating_point<std::common_type_t<A, typename std::decay_t<B>::value_type>>::value, typename std::decay_t<B>::template self_template_type<std::common_type_t<A, typename std::decay_t<B>::value_type>>, typename std::decay_t<B>::template self_template_type<double>>>
  { /* empty */ };

  // std::complex / std::complex
  template<typename A, typename B> struct signed_common_float<A, B, std::enable_if_t<bk::is_complex_v<A> && bk::is_complex_v<B>>>
      : bk::type_wrapper<std::conditional_t<std::is_floating_point<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>::value, typename std::decay_t<A>::template self_template_type<std::common_type_t<typename std::decay_t<A>::value_type, typename std::decay_t<B>::value_type>>, typename std::decay_t<A>::template self_template_type<double>>>
  { /* empty */ };
  /// @}

  /// @{ -------------------------------------------------- IS SIGNED COMMON FLOAT
  //! Checks if A equals the signed_common_float_type of A and B
  /*!
   *  - A and B can each either be an arithmetic type (integral or float; no pointers) or a
   *    class with "value_type" defined
   *  - for unknown types, false is returned by default
   */
  // base
  template<typename A, typename B, typename E = void> struct is_signed_common_float : std::false_type
  { /* empty */ };

  // shortcut
  template<typename A, typename B> constexpr bool is_signed_common_float_v = is_signed_common_float<A, std::decay_t<B>>::value;

  // (arithmetic type or class with value_type) / (arithmetic type or class with value_type)
  template<typename A, typename B>
  struct is_signed_common_float<A, B, std::enable_if_t<((std::is_arithmetic_v<A> && !std::is_class_v<A>) || (std::is_class<std::decay_t<A>>::value && bk::has_value_type_v<std::decay_t<A>>)) && ((std::is_arithmetic_v<B> && !std::is_class_v<B>) || (std::is_class<std::decay_t<B>>::value && bk::has_value_type_v<std::decay_t<B>>))>>
      : std::integral_constant<bool, std::is_same<A, signed_common_float_t<A, B>>::value>
  { /* empty */ };
  /// @}

} // namespace bk

#endif //BK_COMMON_TYPE_H


