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

#ifndef BK_NOTOPOLOGY_H
#define BK_NOTOPOLOGY_H

#include <array>

#include "Cell.h"
#include "../lib/bkDataset_export.h"

namespace bk
{
  class BKDATASET_EXPORT NoTopology
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = NoTopology;

    public:
      /// @{ -------------------------------------------------- IS EXPLICIT TOPOLOGY
      [[nodiscard]] static  bool IsExplicit() noexcept
      { return false; }
      /// @}

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      NoTopology() noexcept = default;
      NoTopology(const self_type&) noexcept = default;
      NoTopology(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET CELL
      static  Cell<1> cell(unsigned int id) noexcept
      { return Cell<1>(id); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type&) noexcept = default;
      self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== CONNECTIVITY
      //====================================================================================================
      /// @{ -------------------------------------------------- CELLS OF POINT
      static  std::array<unsigned int, 0> cells_of_point(unsigned int) noexcept
      {
          // no connectivity -> empty
          return std::array<unsigned int, 0>();
      }
      /// @}

      /// @{ -------------------------------------------------- NEIGHBORS OF POINT
      static  std::array<unsigned int, 0> neighbors_of_point(unsigned int) noexcept
      {
          // no connectivity -> empty
          return std::array<unsigned int, 0>();
      }
      /// @}
  }; // class NoTopology
} // namespace bk

#endif //BK_NOTOPOLOGY_H


