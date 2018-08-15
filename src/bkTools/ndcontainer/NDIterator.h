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

#ifndef BKTOOLS_NDITERATOR_H
#define BKTOOLS_NDITERATOR_H

#include <iterator>
#include <type_traits>

#include <bkMath/functions/list_grid_id_conversion.h>

namespace bk
{
  template<typename TNDGrid> class NDIterator
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = NDIterator<TNDGrid>;
      using ndgrid_type = TNDGrid;
    public:
      using difference_type = int;
      using value_type = std::decay_t<typename ndgrid_type::value_type>;
      using reference = std::conditional_t<std::is_const_v<TNDGrid>, const value_type&, value_type&>;
      using const_reference = std::conditional_t<std::is_const_v<TNDGrid>, value_type&, const value_type&>;
      using pointer = value_type*;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = unsigned int;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      ndgrid_type* _ndgrid;
      size_type _current_lid;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      NDIterator()
          : _ndgrid(nullptr),
            _current_lid(0)
      { /* do nothing */ }

      NDIterator(ndgrid_type& ndgrid)
          : NDIterator<TNDGrid>(ndgrid, 0)
      { /* do nothing */ }

      NDIterator(ndgrid_type& ndgrid, size_type current_lid)
          : _ndgrid(&ndgrid),
            _current_lid(current_lid)
      { /* do nothing */ }

      NDIterator(const self_type&) = default;
      NDIterator(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~NDIterator() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] reference operator*()
      { return (*_ndgrid)[_current_lid]; }

      [[nodiscard]] const_reference operator*() const
      { return (*_ndgrid)[_current_lid]; }

      [[nodiscard]] pointer operator->() const
      { return &(*_ndgrid)[_current_lid]; }

      /// @{ -------------------------------------------------- GET LIST ID
      [[nodiscard]] size_type list_id() const
      { return _current_lid; }
      /// @}

      /// @{ -------------------------------------------------- GET GRID IDS
      [[nodiscard]] auto grid_id() const
      { return bk::list_to_grid_id(_ndgrid->size(), _current_lid); }
      /// @}

      //====================================================================================================
      //===== ARITHMETIC OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- PREFIX IN-/DECREMENT
      [[maybe_unused]] self_type& operator++()
      {
          ++_current_lid;
          return *this;
      }

      [[maybe_unused]] self_type& operator--()
      {
          if (_current_lid > 0)
          { --_current_lid; }

          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- POSTFIX IN-/DECREMENT
      [[maybe_unused]] self_type operator++(int)
      {
          NDIterator res(*this);
          ++(*this);
          return res;
      }

      [[maybe_unused]] self_type operator--(int)
      {
          NDIterator res(*this);
          --(*this);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR +(=)
      [[nodiscard]] self_type operator+(size_type n) const
      { return NDIterator(*_ndgrid, _current_lid + n); }

      [[maybe_unused]] self_type& operator+=(size_type n)
      {
          _current_lid += n;
          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR -(=)
      [[nodiscard]] self_type operator-(size_type n) const
      { return NDIterator(*_ndgrid, _current_lid - n); }

      [[nodiscard]] difference_type operator-(const self_type& other) const
      { return static_cast<difference_type>(_current_lid) - static_cast<difference_type>(other._current_lid); }

      [[maybe_unused]] self_type& operator-=(size_type n)
      {
          _current_lid -= n;
          return *this;
      }
      /// @}

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      [[nodiscard]] bool operator==(const self_type& other) const
      {
          if (_current_lid >= _ndgrid->num_values() && other._current_lid >= other._ndgrid->num_values())
          {
              // both are "end"
              return true;
          }

          return _current_lid == other._current_lid;
      }

      [[nodiscard]] bool operator!=(const self_type& other) const
      { return !operator==(other); }

      [[nodiscard]] bool operator<(const self_type& other) const
      {
          if (_current_lid >= _ndgrid->num_values() && other._current_lid >= other._ndgrid->num_values())
          {
              // both are "end"
              return false;
          }

          return _current_lid < other._current_lid;
      }

      [[nodiscard]] bool operator<=(const self_type& other) const
      { return _current_lid < other._current_lid; }

      [[nodiscard]] bool operator>(const self_type& other) const
      { return !operator<=(other); }

      [[nodiscard]] bool operator>=(const self_type& other) const
      { return !operator<(other); }

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- SWAP
      void swap(self_type& other)
      { _current_lid = other._current_lid; }

      void swap(self_type&& other)
      { _current_lid = std::move(other._current_lid); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class NDIterator

  //====================================================================================================
  //===== EXTERNAL FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR +
  template<typename TNDGrid>
  auto operator+(unsigned int n, const bk::NDIterator<TNDGrid>& it)
  { return it + n; }
  /// @}

  /// @{ -------------------------------------------------- SWAP
  template<typename TNDGrid>
  void swap(bk::NDIterator<TNDGrid>& gcit0, bk::NDIterator<TNDGrid>& gcit1)
  { gcit0.swap(gcit1); }

  template<typename TNDGrid>
  void swap(bk::NDIterator<TNDGrid>&& gcit0, bk::NDIterator<TNDGrid>& gcit1)
  { gcit1.swap(std::move(gcit0)); }

  template<typename TNDGrid>
  void swap(bk::NDIterator<TNDGrid>& gcit0, bk::NDIterator<TNDGrid>&& gcit1)
  { gcit0.swap(std::move(gcit1)); }
  /// @}
} // namespace bk

//------------------------------------------------------------------------------------------------------
// iterator_traits
//------------------------------------------------------------------------------------------------------
namespace std
{
  template<typename TNDGrid> struct iterator_traits<bk::NDIterator<TNDGrid>>
  {
      using difference_type = int;
      using value_type = typename bk::NDIterator<TNDGrid>::value_type;
      using reference = typename bk::NDIterator<TNDGrid>::reference;
      using const_reference = typename bk::NDIterator<TNDGrid>::const_reference;
      using pointer = typename bk::NDIterator<TNDGrid>::pointer;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = typename bk::NDIterator<TNDGrid>::size_type;
  };
} // namespace std

#endif //BKTOOLS_NDITERATOR_H