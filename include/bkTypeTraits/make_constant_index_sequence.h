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

#ifndef BKTYPETRAITS_MAKE_CONSTANT_INDEX_SEQUENCE_H
#define BKTYPETRAITS_MAKE_CONSTANT_INDEX_SEQUENCE_H

#include <utility>

namespace bk
{
  /// @{ -------------------------------------------------- MAKE CONSTANT INDEX SEQUENCE
  //! repeat value I N times
  /*!
   * example: make_constant_index_sequence<0,5> produces std::integer_sequence<int,0,0,0,0,0>
   */
  /// @}
  template<auto I, auto N, auto... IL>
  constexpr auto make_constant_index_sequence()
  {
      if constexpr (sizeof...(IL) < N)
      { return make_constant_index_sequence<I, N, I, IL...>(); }
      else
      { return std::integer_sequence<decltype(I), IL...>(); }
  }
} // namespace bk

#endif //BKTYPETRAITS_MAKE_CONSTANT_INDEX_SEQUENCE_H
