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

#ifndef BKTYPETRAITS_COMPLEX_TRAITS_H
#define BKTYPETRAITS_COMPLEX_TRAITS_H

#include <complex>
#include <type_traits>

#include "type_wrapper.h"

namespace bk
{
  /// @{ -------------------------------------------------- IS COMPLEX
  //! true for std::complex
  template<typename T> class is_complex
  {
      using yes = char[1];
      using no = char[2];
      template<typename C>
      static yes& test(char (*)(typename C::complex)); // std::complex
      template<typename C>
      static no& test(...);
    public:
      static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
  }; // class is_complex

  // shortcut
  template<typename T> constexpr bool is_complex_v = is_complex<T>::value;
  /// @}

  /// @{ -------------------------------------------------- MAKE COMPLEX
  //! complex version of type
  /*!
   *  - no changes are made if type is already complex
   */
  // base
  template<typename T, typename E = void> struct make_complex : bk::type_wrapper<std::complex<T>>
  { /* empty */ };

  // shortcut
  template<typename T> using make_complex_t = typename make_complex<T>::type;

  // std::complex
  template<typename T> struct make_complex<T, std::enable_if_t<bk::is_complex_v<T>>> : bk::type_wrapper<T>
  { /* empty */ };
  /// @}
} // namespace bk

#endif //BKTYPETRAITS_COMPLEX_TRAITS_H


