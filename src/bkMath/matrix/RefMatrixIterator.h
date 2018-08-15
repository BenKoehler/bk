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

#ifndef BK_REFMATRIXITERATOR_H
#define BK_REFMATRIXITERATOR_H

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace bk
{
  template<typename TRefMatrix> class RefMatrixIterator
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RefMatrixIterator<TRefMatrix>;
    public:
      using ref_matrix_type = TRefMatrix;

      using difference_type = int;
      using value_type = typename ref_matrix_type::value_type;
      using reference = std::add_lvalue_reference_t<value_type>;
      using pointer = std::add_pointer_t<value_type>;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = unsigned int;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      ref_matrix_type& _m;
      size_type _i;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      constexpr RefMatrixIterator() = delete;
      constexpr RefMatrixIterator(const self_type&) = default;
      constexpr RefMatrixIterator(self_type&&) noexcept = default;

      constexpr RefMatrixIterator(ref_matrix_type& m, size_type i)
          : _m(m),
            _i(std::clamp(i, static_cast<size_type>(0), static_cast<size_type>(m.num_elements())))
      { /* do nothing */ }

      constexpr RefMatrixIterator(ref_matrix_type& m)
          : RefMatrixIterator(m, static_cast<size_type>(0))
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~RefMatrixIterator() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] constexpr decltype(auto) operator*()
      { return _m[_i]; }

      [[nodiscard]] constexpr decltype(auto) operator*() const
      { return _m[_i]; }

      [[nodiscard]] constexpr reference operator[](size_type n) const
      { return operator+(n); }

      [[nodiscard]] constexpr pointer operator->() const
      { return std::addressof(_m[_i]); }

      //====================================================================================================
      //===== ARITHMETIC OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- PREFIX IN-/DECREMENT
      [[maybe_unused]] constexpr self_type& operator++()
      {
          _i = std::min(_i + 1, static_cast<size_type>(_m.num_elements()));
          return *this;
      }

      [[maybe_unused]] constexpr self_type& operator--()
      {
          _i = std::max(_i - 1, static_cast<size_type>(0));
          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- POSTFIX IN-/DECREMENT
      [[maybe_unused]] constexpr self_type operator++(int)
      {
          const int old_i = _i;
          _i = std::min(_i + 1, static_cast<size_type>(_m.num_elements()));
          return self_type(_m, old_i);
      }

      [[maybe_unused]] constexpr self_type operator--(int)
      {
          const int old_i = _i;
          _i = std::max(_i - 1, static_cast<size_type>(0));
          return self_type(_m, old_i);
      }
      /// @}

      [[nodiscard]] constexpr self_type operator+(size_type n) const
      { return self_type(_m, _i + n); }

      [[maybe_unused]] constexpr self_type& operator+=(size_type n)
      {
          _i = std::min(_i + 1, _m.num_elements());
          return *this;
      }

      [[nodiscard]] constexpr self_type operator-(size_type n) const
      { return self_type(_m, _i - n); }

      [[nodiscard]] constexpr size_type operator-(const self_type& other) const
      { return _i - other._i; }

      [[maybe_unused]] constexpr self_type& operator-=(size_type n)
      {
          _i = std::max(_i + 1, static_cast<size_type>(0));
          return *this;
      }

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      [[nodiscard]] constexpr bool operator==(const self_type& other) const
      { return _i == other._i; }

      [[nodiscard]] constexpr bool operator!=(const self_type& other) const
      { return !operator==(other); }

      [[nodiscard]] constexpr bool operator<(const self_type& other) const
      { return _i < other._i; }

      [[nodiscard]] constexpr bool operator<=(const self_type& other) const
      { return _i < other._i; }

      [[nodiscard]] constexpr bool operator>(const self_type& other) const
      { return !operator<=(other); }

      [[nodiscard]] constexpr bool operator>=(const self_type& other) const
      { return !operator<(other); }

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      void swap(self_type& other)
      { std::swap(_i, other._i); }

      void swap(self_type&& other)
      { _i = std::move(other._i); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class RefMatrixIterator

  /// @{ -------------------------------------------------- OPERATOR +
  template<typename TRefMatrix>
  auto operator+(unsigned int n, const bk::RefMatrixIterator<TRefMatrix>& it)
  { return it + n; }
  /// @}

  /// @{ -------------------------------------------------- SWAP
  // & / &
  template<typename TRefMatrix0, typename TRefMatrix1>
  void swap(bk::RefMatrixIterator<TRefMatrix0>& m0, bk::RefMatrixIterator<TRefMatrix1>& m1)
  { m0.swap(m1); }

  // && / &
  template<typename TRefMatrix0, typename TRefMatrix1>
  void swap(bk::RefMatrixIterator<TRefMatrix0>&& m0, bk::RefMatrixIterator<TRefMatrix1>& m1)
  { m1.swap(std::move(m0)); }

  // & / &&
  template<typename TRefMatrix0, typename TRefMatrix1>
  void swap(bk::RefMatrixIterator<TRefMatrix0>& m0, bk::RefMatrixIterator<TRefMatrix1>&& m1)
  { m0.swap(std::move(m1)); }
  /// @}
} // namespace bk

namespace std
{
  template<typename TRefMatrix> struct iterator_traits<bk::RefMatrixIterator<TRefMatrix>>
  {
      using difference_type = int;
      using value_type = typename TRefMatrix::value_type;
      using reference = std::add_lvalue_reference_t<value_type>;
      using pointer = std::add_pointer_t<value_type>;
      using iterator_category = std::random_access_iterator_tag;
      using size_type = unsigned int;
  };
} // namespace std

#endif //BK_REFMATRIXITERATOR_H


