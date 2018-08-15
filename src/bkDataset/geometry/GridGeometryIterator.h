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

#ifndef BK_GRIDGEOMETRYITERATOR_H
#define BK_GRIDGEOMETRYITERATOR_H

#include <iterator>
#include <memory>

#include <bk/Matrix>

namespace bk
{
  // -------------------- forward declaration
  template<int, typename> class GridGeometry;
  // -------------------- forward declaration END

  template<int TDims, typename TValue> class GridGeometryIterator
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GridGeometryIterator<TDims, TValue>;
      using grid_geometry_type = GridGeometry<TDims, TValue>;
    public:
      using difference_type = int;
      using value_type = Vec<TDims, TValue>;
      using reference = value_type;
      using pointer = std::unique_ptr<value_type>;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = unsigned int;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      const grid_geometry_type& _grid_geometry;
      size_type _current_point_id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GridGeometryIterator() = delete;

      explicit GridGeometryIterator(const grid_geometry_type& grid_geometry)
          : GridGeometryIterator<TDims, TValue>(grid_geometry, 0)
      { /* do nothing */ }

      explicit GridGeometryIterator(const grid_geometry_type& grid_geometry, size_type current_point_id)
          : _grid_geometry(grid_geometry),
            _current_point_id(current_point_id)
      { /* do nothing */ }

      GridGeometryIterator(const self_type&) = default;
      GridGeometryIterator(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GridGeometryIterator() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] value_type operator*()
      { return _grid_geometry.point(_current_point_id); }

      [[nodiscard]] value_type operator*() const
      { return _grid_geometry.point(_current_point_id); }

      [[nodiscard]] pointer operator->() const
      { return std::make_unique<value_type>(operator*()); }

      //====================================================================================================
      //===== ARITHMETIC OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- PREFIX IN-/DECREMENT
      [[maybe_unused]] self_type& operator++()
      {
          ++_current_point_id;
          return *this;
      }

      [[maybe_unused]] self_type& operator--()
      {
          if (_current_point_id > 0)
          { --_current_point_id; }

          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- POSTFIX IN-/DECREMENT
      [[maybe_unused]] self_type operator++(int)
      {
          GridGeometryIterator res(*this);
          ++(*this);
          return res;
      }

      [[maybe_unused]] self_type operator--(int)
      {
          GridGeometryIterator res(*this);
          --(*this);
          return res;
      }
      /// @}

      [[nodiscard]] self_type operator+(size_type n) const
      { return GridGeometryIterator(_grid_geometry, _current_point_id + n); }

      [[maybe_unused]] self_type& operator+=(size_type n)
      {
          _current_point_id += n;
          return *this;
      }

      [[nodiscard]] self_type operator-(size_type n) const
      { return GridGeometryIterator(_grid_geometry, _current_point_id - n); }

      [[nodiscard]] difference_type operator-(const self_type& other) const
      { return static_cast<difference_type>(_current_point_id) - static_cast<difference_type>(other._current_point_id); }

      [[maybe_unused]] self_type& operator-=(size_type n)
      {
          _current_point_id -= n;
          return *this;
      }

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      [[nodiscard]] bool operator==(const self_type& other) const
      {
          if (_current_point_id >= _grid_geometry.num_points() && other._current_point_id >= other._grid_geometry.num_points())
          {
              // both are "end"
              return true;
          }

          return _current_point_id == other._current_point_id;
      }

      [[nodiscard]] bool operator!=(const self_type& other) const
      { return !operator==(other); }

      [[nodiscard]] bool operator<(const self_type& other) const
      {
          if (_current_point_id >= _grid_geometry.num_points() && other._current_point_id >= other._grid_geometry.num_points())
          {
              // both are "end"
              return false;
          }

          return _current_point_id < other._current_point_id;
      }

      [[nodiscard]] bool operator<=(const self_type& other) const
      { return _current_point_id < other._current_point_id; }

      [[nodiscard]] bool operator>(const self_type& other) const
      { return !operator<=(other); }

      [[nodiscard]] bool operator>=(const self_type& other) const
      { return !operator<(other); }

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      void swap(self_type& other)
      { _current_point_id = other._current_point_id; }

      void swap(self_type&& other)
      { _current_point_id = std::move(other._current_point_id); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}
  }; // class GridGeometryIterator

  //------------------------------------------------------------------------------------------------------
  // external functions
  //------------------------------------------------------------------------------------------------------
  /// @{ -------------------------------------------------- OPERATOR +
  template<int TDims, typename TValue>
  auto operator+(unsigned int n, const bk::GridGeometryIterator<TDims, TValue>& it)
  { return it + n; }
  /// @}

  /// @{ -------------------------------------------------- SWAP
  template<int TDims, typename TValue>
  void swap(bk::GridGeometryIterator<TDims, TValue>& gcit0, bk::GridGeometryIterator<TDims, TValue>& gcit1)
  { gcit0.swap(gcit1); }

  template<int TDims, typename TValue>
  void swap(bk::GridGeometryIterator<TDims, TValue>&& gcit0, bk::GridGeometryIterator<TDims, TValue>& gcit1)
  { gcit1.swap(std::move(gcit0)); }

  template<int TDims, typename TValue>
  void swap(bk::GridGeometryIterator<TDims, TValue>& gcit0, bk::GridGeometryIterator<TDims, TValue>&& gcit1)
  { gcit0.swap(std::move(gcit1)); }
  /// @}
} // namespace bk

//------------------------------------------------------------------------------------------------------
// iterator_traits
//------------------------------------------------------------------------------------------------------
namespace std
{
  template<int TDims, typename TValue> struct iterator_traits<bk::GridGeometryIterator<TDims, TValue>>
  {
      using difference_type = int;
      using value_type = typename bk::GridGeometryIterator<TDims, TValue>::value_type;
      using reference = typename bk::GridGeometryIterator<TDims, TValue>::reference;
      using pointer = typename bk::GridGeometryIterator<TDims, TValue>::pointer;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = typename bk::GridGeometryIterator<TDims, TValue>::size_type;
  };
} // namespace std

#endif //BK_GRIDGEOMETRYITERATOR_H
