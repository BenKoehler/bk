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

#ifndef BKTOOLS_BITVECTORITERATOR_H
#define BKTOOLS_BITVECTORITERATOR_H

#include <algorithm>
#include <iterator>

#include <bkTools/bitvector/BitRef.h>

namespace bk
{
  namespace details
  {
    template<typename TBitVector> class BitVectorIterator
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = BitVectorIterator;
      public:
        using bitvector_type = TBitVector;

        using difference_type = int;
        using value_type = BitRef<typename bitvector_type::value_type>;
        using reference = value_type;
        using pointer = std::add_pointer_t<value_type>;
        using iterator_category = std::random_access_iterator_tag;
        using size_type = int;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      private:
        bitvector_type& _bv;
        int _i;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CONSTRUCTORS
        BitVectorIterator() = delete;
        BitVectorIterator(const self_type&) = default;
        BitVectorIterator(self_type&&) noexcept = default;

        BitVectorIterator(bitvector_type& bv, size_type i)
            : _bv(bv),
              _i(std::clamp(i, 0, static_cast<size_type>(bv.size())))
        { /*do nothing*/ }

        BitVectorIterator(bitvector_type& bv) : BitVectorIterator(bv, 0)
        { /*do nothing*/ }
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        ~BitVectorIterator() = default;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] self_type& operator=(const self_type& other)
        {
            _i = other._i;
            return *this;
        }

        [[maybe_unused]] self_type& operator=(self_type&& other) noexcept
        {
            _i = std::move(other._i);
            return *this;
        }

        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        // dereference *
        [[nodiscard]] auto operator*()
        //reference operator*()
        { return _bv[_i]; }

        [[nodiscard]] auto operator*() const
        //value_type operator*() const
        { return _bv[_i]; }

        // index operator
        [[nodiscard]] reference operator[](size_type n) const
        { return operator+(n); }

        /*
         * Overloading operator-> would require to return a pointer to the current element.
         * However, the current element is accessed with a new BitRef object which is not
         * stored in BitVectorIterator (-> created on demand).
         * Therefore, operator-> is not provided
         */

        //====================================================================================================
        //===== ARITHMETIC OPERATORS
        //====================================================================================================
        // prefix increment
        [[maybe_unused]] self_type& operator++()
        {
            _i = std::min(_i + 1, static_cast<size_type>(_bv.size()));
            return *this;
        }

        // postfix increment
        [[maybe_unused]] self_type operator++(size_type)
        {
            const int old_i = _i;
            _i = std::min(_i + 1, static_cast<size_type>(_bv.size()));
            return self_type(_bv, old_i);
        }

        // prefix decrement
        [[maybe_unused]] self_type& operator--()
        {
            _i = std::max(_i - 1, 0);
            return *this;
        }

        // postfix decrement
        [[maybe_unused]] self_type operator--(size_type)
        {
            const int old_i = _i;
            _i = std::max(_i - 1, 0);
            return self_type(_bv, old_i);
        }

        [[nodiscard]] self_type operator+(size_type n) const
        { return self_type(_bv, _i + n); }

        [[maybe_unused]] self_type& operator+=(size_type n)
        {
            _i = std::min(_i + 1, static_cast<int>(_bv.size()));
            return *this;
        }

        [[nodiscard]] self_type operator-(size_type n) const
        { return self_type(_bv, _i - n); }

        [[nodiscard]] size_type operator-(const self_type& other) const
        { return _i - other._i; }

        [[maybe_unused]] self_type& operator-=(size_type n)
        {
            _i = std::max(_i + 1, 0);
            return *this;
        }

        //====================================================================================================
        //===== COMPARISON
        //====================================================================================================
        [[nodiscard]] bool operator==(const self_type& other) const
        { return _i == other._i; }

        [[nodiscard]] bool operator!=(const self_type& other) const
        { return !operator==(other); }

        [[nodiscard]] bool operator<(const self_type& other) const
        { return _i < other._i; }

        [[nodiscard]] bool operator<=(const self_type& other) const
        { return _i < other._i; }

        [[nodiscard]] bool operator>(const self_type& other) const
        { return !operator<=(other); }

        [[nodiscard]] bool operator>=(const self_type& other) const
        { return !operator<(other); }

        //====================================================================================================
        //===== MISC FUNCTIONS
        //====================================================================================================
        void swap(self_type& other)
        { std::swap(_i, other._i); }

        void swap(self_type&& other)
        { _i = std::move(other._i); }
    }; // class BitVectorIterator
  } // namespace details

  /// @{ -------------------------------------------------- OPERATOR +
  template<typename TBitVector>
  [[nodiscard]] auto operator+(int n, const details::BitVectorIterator<TBitVector>& bv)
  { return bv + n; }
  /// @}

  /// @{ -------------------------------------------------- SWAP
  // & / &
  template<typename TBitVector0, typename TBitVector1>
  void swap(details::BitVectorIterator<TBitVector0>& bv0, details::BitVectorIterator<TBitVector1>& bv1)
  { bv0.swap(bv1); }

  // && / &
  template<typename TBitVector0, typename TBitVector1>
  void swap(details::BitVectorIterator<TBitVector0>&& bv0, details::BitVectorIterator<TBitVector1>& bv1)
  { bv1.swap(std::move(bv0)); }

  // & / &&
  template<typename TBitVector0, typename TBitVector1>
  void swap(details::BitVectorIterator<TBitVector0>& bv0, details::BitVectorIterator<TBitVector1>&& bv1)
  { bv0.swap(std::move(bv1)); }

  // && / &&
  template<typename TBitVector0, typename TBitVector1>
  void swap(details::BitVectorIterator<TBitVector0>&& bv0, details::BitVectorIterator<TBitVector1>&& bv1)
  {
      /* do nothing */
  }
  /// @}


} // namespace bk


namespace std
{
  template<typename TBitVector> struct iterator_traits<bk::details::BitVectorIterator<TBitVector>>
  {
      using difference_type = int;
      using value_type = bk::details::BitRef<typename TBitVector::value_type>;
      using reference = value_type;
      using pointer = std::add_pointer_t<value_type>;
      using iterator_category = random_access_iterator_tag;
      using size_type = int;
  };
} // namespace std


#endif //BKTOOLS_BITVECTORITERATOR_H


