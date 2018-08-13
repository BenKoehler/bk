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

#ifndef BK_GRIDTOPOLOGYITERATOR_H
#define BK_GRIDTOPOLOGYITERATOR_H

#include <iterator>
#include <memory>

#include "Cell.h"

namespace bk
{
  // -------------------- forward declaration
  template<int> class GridTopology;
  // -------------------- forward declaration END

  template<int TDims> class GridTopologyIterator
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GridTopologyIterator<TDims>;
      using grid_topology_type = GridTopology<TDims>;
    public:
      using difference_type = int;
      using value_type = Cell<>;
      using reference = value_type;
      using pointer = std::unique_ptr<value_type>;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = unsigned int;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      const grid_topology_type& _grid_topology;
      size_type _current_cell_id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GridTopologyIterator() = delete;

      explicit GridTopologyIterator(const grid_topology_type& grid_topology)
          : GridTopologyIterator<TDims>(grid_topology, 0)
      { /* do nothing */ }

      explicit GridTopologyIterator(const grid_topology_type& grid_topology, size_type current_cell_id)
          : _grid_topology(grid_topology),
            _current_cell_id(current_cell_id)
      { /* do nothing */ }

      GridTopologyIterator(const self_type&) = default;
      GridTopologyIterator(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GridTopologyIterator() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] value_type operator*()
      { return _grid_topology.cell(_current_cell_id); }

      [[nodiscard]] value_type operator*() const
      { return _grid_topology.cell(_current_cell_id); }

      [[nodiscard]] pointer operator->() const
      { return std::make_unique<value_type>(operator*()); }

      //====================================================================================================
      //===== ARITHMETIC OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- PREFIX IN-/DECREMENT
      [[maybe_unused]] self_type& operator++()
      {
          ++_current_cell_id;
          return *this;
      }

      [[maybe_unused]] self_type& operator--()
      {
          if (_current_cell_id > 0)
          { --_current_cell_id; }

          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- POSTFIX IN-/DECREMENT
      [[maybe_unused]] self_type operator++(int)
      {
          GridTopologyIterator res(*this);
          ++(*this);
          return res;
      }

      [[maybe_unused]] self_type operator--(int)
      {
          GridTopologyIterator res(*this);
          --(*this);
          return res;
      }
      /// @}

      [[nodiscard]] self_type operator+(size_type n) const
      { return GridTopologyIterator(_grid_topology, _current_cell_id + n); }

      [[maybe_unused]] self_type& operator+=(size_type n)
      {
          _current_cell_id += n;
          return *this;
      }

      [[nodiscard]] self_type operator-(size_type n) const
      { return GridTopologyIterator(_grid_topology, _current_cell_id - n); }

      [[nodiscard]] difference_type operator-(const self_type& other) const
      { return static_cast<difference_type>(_current_cell_id) - static_cast<difference_type>(other._current_cell_id); }

      [[maybe_unused]] self_type& operator-=(size_type n)
      {
          _current_cell_id -= n;
          return *this;
      }

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      [[nodiscard]] bool operator==(const self_type& other) const
      {
          if (_current_cell_id >= _grid_topology.num_cells() && other._current_cell_id >= other._grid_topology.num_cells())
          {
              // both are "end"
              return true;
          }

          return _current_cell_id == other._current_cell_id;
      }

      [[nodiscard]] bool operator!=(const self_type& other) const
      { return !operator==(other); }

      [[nodiscard]] bool operator<(const self_type& other) const
      {
          if (_current_cell_id >= _grid_topology.num_cells() && other._current_cell_id >= other._grid_topology.num_cells())
          {
              // both are "end"
              return false;
          }

          return _current_cell_id < other._current_cell_id;
      }

      [[nodiscard]] bool operator<=(const self_type& other) const
      { return _current_cell_id < other._current_cell_id; }

      [[nodiscard]] bool operator>(const self_type& other) const
      { return !operator<=(other); }

      [[nodiscard]] bool operator>=(const self_type& other) const
      { return !operator<(other); }

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      void swap(self_type& other)
      { _current_cell_id = other._current_cell_id; }

      void swap(self_type&& other)
      { _current_cell_id = std::move(other._current_cell_id); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}
  }; // class GridTopologyIterator

  //------------------------------------------------------------------------------------------------------
  // external functions
  //------------------------------------------------------------------------------------------------------
  /// @{ -------------------------------------------------- OPERATOR +
  template<int TDims>
  auto operator+(unsigned int n, const bk::GridTopologyIterator<TDims>& it)
  { return it + n; }
  /// @}

  /// @{ -------------------------------------------------- SWAP
  template<int TDims>
  void swap(bk::GridTopologyIterator<TDims>& gcit0, bk::GridTopologyIterator<TDims>& gcit1)
  { gcit0.swap(gcit1); }

  template<int TDims>
  void swap(bk::GridTopologyIterator<TDims>&& gcit0, bk::GridTopologyIterator<TDims>& gcit1)
  { gcit1.swap(std::move(gcit0)); }

  template<int TDims>
  void swap(bk::GridTopologyIterator<TDims>& gcit0, bk::GridTopologyIterator<TDims>&& gcit1)
  { gcit0.swap(std::move(gcit1)); }
  /// @}
} // namespace bk

//------------------------------------------------------------------------------------------------------
// iterator_traits
//------------------------------------------------------------------------------------------------------
namespace std
{
  template<int TDims> struct iterator_traits<bk::GridTopologyIterator<TDims>>
  {
      using difference_type = int;
      using value_type = typename bk::GridTopologyIterator<TDims>::value_type;
      using reference = typename bk::GridTopologyIterator<TDims>::reference;
      using pointer = typename bk::GridTopologyIterator<TDims>::pointer;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = typename bk::GridTopologyIterator<TDims>::size_type;
  };
} // namespace std

#endif //BK_GRIDTOPOLOGYITERATOR_H
