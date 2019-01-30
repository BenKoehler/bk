/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BKTYPETRAITS_HAS_VALUE_TYPE_H
#define BKTYPETRAITS_HAS_VALUE_TYPE_H

namespace bk
{
  /// @{ -------------------------------------------------- HAS VALUE_TYPE
  template<typename T> class has_value_type
  {
      using yes = char[1];
      using no = char[2];
      template<typename C>
      static yes& test(char (*)(typename C::value_type));
      template<typename C>
      static no& test(...);

    public:
      static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
  }; // class has_value_type

  // shortcut
  template<typename T> constexpr bool has_value_type_v = has_value_type<T>::value;
  /// @}
} // namespace bk

#endif //BKTYPETRAITS_HAS_VALUE_TYPE_H


