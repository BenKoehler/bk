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

#ifndef BK_CELLBASE_H
#define BK_CELLBASE_H

#include <algorithm>
#include <type_traits>

namespace bk::details
{
  template<int N = -1> class CellBase
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(N > 0 || N == -1, "invalid cell size (N > 0 or N == -1 required)");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr unsigned int NumValuesAtCompileTime() noexcept
      { return static_cast<unsigned int>(std::max(N, 0)); }
      /// @}
    private:

      using self_type = CellBase<N>;
      using container_type = std::conditional_t<(N > 0), std::array<unsigned int, N>, std::vector<unsigned int>>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      container_type _pointIds;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      CellBase() = default;
      CellBase(const self_type&) = default;
      CellBase(self_type&&) = default;

      template<typename... TIds>
      CellBase(TIds ... ids)
          : _pointIds({static_cast<unsigned int>(ids)...})
      {
          static_assert(N == -1 || N == sizeof...(TIds), "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_integral<TIds>...>, "all ids must be integral type");
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~CellBase() = default;
      /// @}
    public:

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] unsigned int size() const
      { return _pointIds.size(); }
      /// @}

      /// @{ -------------------------------------------------- IS CELL TYPE
      [[nodiscard]] bool is_line() const
      { return size() == 2; }

      [[nodiscard]] bool is_triangle() const
      { return size() == 3; }

      [[nodiscard]] bool is_quad() const
      { return size() == 4; }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      [[nodiscard]] unsigned int& operator[](unsigned int i)
      { return _pointIds[i]; }

      [[nodiscard]] unsigned int operator[](unsigned int i) const
      { return _pointIds[i]; }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] typename container_type::iterator begin()
      { return _pointIds.begin(); }

      [[nodiscard]] typename container_type::const_iterator begin() const
      { return _pointIds.begin(); }

      [[nodiscard]] typename container_type::iterator end()
      { return _pointIds.end(); }

      [[nodiscard]] typename container_type::const_iterator end() const
      { return _pointIds.end(); }

      [[nodiscard]] typename container_type::reverse_iterator rbegin()
      { return _pointIds.rbegin(); }

      [[nodiscard]] typename container_type::const_reverse_iterator rbegin() const
      { return _pointIds.rbegin(); }

      [[nodiscard]] typename container_type::reverse_iterator rend()
      { return _pointIds.rend(); }

      [[nodiscard]] typename container_type::const_reverse_iterator rend() const
      { return _pointIds.rend(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}
    public:

      /// @{ -------------------------------------------------- SET
      template<typename... TIds>
      void set(TIds ... ids)
      {
          static_assert(N == -1 || N == sizeof...(TIds), "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_integral<TIds>...>, "all ids must be integral type");

          _pointIds = {static_cast<unsigned int>(ids)...};
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INVERT CELL ORDERING
      void invert_order()
      { std::reverse(_pointIds.begin(), _pointIds.end()); }
      /// @}

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR==
      [[nodiscard]] bool operator==(const self_type& other) const
      { return _pointIds == other._pointIds; }

      template<int _N>
      [[nodiscard]] bool operator==(const CellBase<_N>& other) const
      {
          if (size() != other.size())
          {return false;}

          for (unsigned int i = 0; i < size(); ++i)
          {
              if (operator[](i) != other[i])
              {return false;}
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR!=
      [[nodiscard]] bool operator!=(const self_type& other) const
      { return !operator==(other); }
      /// @}
  }; // class CellBase
} // namespace bk::details

#endif //BK_CELLBASE_H
