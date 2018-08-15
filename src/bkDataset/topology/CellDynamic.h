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

#ifndef BK_CELLDYNAMIC_H
#define BK_CELLDYNAMIC_H

#include <cassert>
#include <ostream>

#include <bkDataset/topology/CellBase.h>

namespace bk
{
  template<> class Cell<-1> : public details::CellBase<-1>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Cell<-1>;
      using base_type = details::CellBase<-1>;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      Cell() = default;
      Cell(const self_type&) = default;
      Cell(self_type&&) = default;

      template<int _N>
      Cell(const Cell<_N>& other)
      {
          // base constructor is not called on purpose

          set_size(other.size());
          std::copy(other.begin(), other.end(), this->begin());
      }

      template<typename... TIds>
      Cell(TIds ... ids)
          : base_type(ids...)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Cell() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR=
      self_type& operator=(const self_type&) = default;
      self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      void set_size(unsigned int n)
      { this->_pointIds.resize(n); };
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear()
      { this->_pointIds.clear(); }
      /// @}

      /// @{ -------------------------------------------------- RESERVE
      void reserve(unsigned int n)
      { this->_pointIds.reserve(n); }
      /// @}

      /// @{ -------------------------------------------------- PUSH BACK
      void push_back(unsigned int pointId)
      { this->_pointIds.push_back(pointId); }
      /// @}
  }; // class Cell
} // namespace bk

#endif //BK_CELLDYNAMIC_H
