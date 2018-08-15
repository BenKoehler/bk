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

#ifndef BKTOOLS_GC_EDGE_H
#define BKTOOLS_GC_EDGE_H

#include <array>

namespace bk::gc_details
{
  template<int TDims> class Edge
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Edge<TDims>;
      using pt_type = std::array<int, TDims>;

      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims > 1, "Edge: invalid N");

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    public:
      unsigned int blockID0;
      unsigned int blockID1;
      int score;
      pt_type size_from;
      pt_type size_to;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCTORS
      constexpr Edge() = default;
      constexpr Edge(const self_type&) = default;
      constexpr Edge(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~Edge() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class Edge
} // namespace bk::gc_details

#endif //BKTOOLS_GC_EDGE_H


