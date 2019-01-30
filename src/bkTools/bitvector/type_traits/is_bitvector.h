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

#ifndef BKTOOLS_IS_BITVECTOR_H
#define BKTOOLS_IS_BITVECTOR_H

namespace bk
{
  /// @{ -------------------------------------------------- IS BITVECTOR
  template<typename T> class is_bitvector
  {
      using yes = char[1];
      using no = char[2];
      template<typename C>
      static yes& test(char (*)(typename C::BitVectorBase));

      template<typename C>
      static no& test(...);
    public:
      static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
  }; // class is_bitvector

  // shortcut for convencience
  template<typename T> constexpr bool is_bitvector_v = is_bitvector<T>::value;
  ///
  /// @}

} // namespace bk

#endif //BKTOOLS_IS_BITVECTOR_H


