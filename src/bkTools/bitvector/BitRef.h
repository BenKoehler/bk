/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BKTOOLS_BITREF_H
#define BKTOOLS_BITREF_H

#include <cmath>
#include <cassert>
#include <iostream>
#include <type_traits>

#include <bkTools/bitvector/type_traits/is_bitref.h>

namespace bk::details
{
  template<typename TValue> class BitRef
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = BitRef<TValue>;
    public:
      using value_type = TValue;

      template<typename T> friend class BitRef;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      value_type& _b; ///< reference to storage byte in the bitvector
      std::decay_t<value_type> _mask; ///< bitmask to access the relative position [0-7] (left to right) within _b

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr BitRef() = delete;
      constexpr BitRef(const self_type&) = default;
      constexpr BitRef(self_type&&) noexcept = default;

      explicit constexpr BitRef(value_type& bit, std::decay_t<value_type> pos)
          : _b(bit),
            _mask(std::pow(2, 7 - pos))
      { assert(pos < 8 && "invalid bitref position"); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~BitRef() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- EVALUATE
      //! allow usage as bool
    public:
      [[nodiscard]] constexpr operator bool() const
      { return eval(); }

      [[nodiscard]] constexpr bool operator!() const
      { return !eval(); }

      [[nodiscard]] constexpr bool eval() const
      { return (_b & _mask) != 0; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: SET
      template<typename T_>
      constexpr void _set(T_&& b)
      {
          if (b)
          {
              //   xxxx xxxx
              //   0001 0000 |
              // -------------
              // = xxx1 xxxx
              _b |= _mask;
          }
          else
          {
              //   xxxx xxxx
              //   1110 1111 &
              // -------------
              // = xxx0 xxxx
              _b &= ~_mask;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other)
      {
          _set(other);
          return *this;
      }

      [[maybe_unused]] constexpr self_type& operator=(self_type&& other) noexcept
      {
          _set(std::move(other));
          return *this;
      }

      template<typename T_>
      [[maybe_unused]] constexpr self_type& operator=(T_&& b)
      {
          _set(std::forward<T_>(b));
          return *this;
      }
      /// @}

      constexpr void flip()
      { operator=(!eval()); }

      //====================================================================================================
      //===== BIT OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR &(=)
      template<typename T_>
      [[nodiscard]] constexpr auto operator&(T_&& bit) const
      {
          auto res = *this;
          res &= std::forward<T_>(bit);
          return res;
      }

      template<typename T_>
      constexpr void operator&=(T_&& bit)
      {
          static_assert(is_bitref_v<std::decay_t<T_>> || std::is_integral_v<std::decay_t<T_>>, "BITREF &=: PARAMETER MUST BE INTEGRAL OR BITREF TYPE");

          if (!bit || !eval())
          { *this = false; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR |(=)
      template<typename T_>
      [[nodiscard]] constexpr auto operator|(T_&& bit) const
      {
          auto res = *this;
          res |= std::forward<T_>(bit);
          return res;
      }

      template<typename T_>
      constexpr void operator|=(T_&& bit)
      {
          static_assert(is_bitref_v<std::decay_t<T_>> || std::is_integral_v<std::decay_t<T_>>, "BITREF |=: PARAMETER MUST BE INTEGRAL OR BITREF TYPE");

          if (bit || eval())
          { *this = true; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR ^(=)
      template<typename T_>
      [[nodiscard]] constexpr auto operator^(T_&& bit) const
      {
          auto res = *this;
          res ^= std::forward<T_>(bit);
          return res;
      }

      template<typename T_>
      constexpr void operator^=(T_&& bit)
      {
          static_assert(is_bitref_v<std::decay_t<T_>> || std::is_integral_v<std::decay_t<T_>>, "BITREF ^=: PARAMETER MUST BE INTEGRAL OR BITREF TYPE");

          *this = (!bit && eval()) || (bit && !eval());
      }
      /// @}

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      void swap(self_type& other)
      {
          const bool b = this->eval();
          this->set(other);
          other.set(b);
      }

      void swap(self_type&& other)
      {
          const bool b = this->eval();
          this->set(other);
          other.set(b);
      }
  }; // class BitRef

  /// @{ -------------------------------------------------- SWAP
  template<typename TValue0, typename TValue1>
  void swap(details::BitRef<TValue0>& b0, details::BitRef<TValue1>& b1)
  { b0.swap(b1); }

  template<typename TValue0, typename TValue1>
  void swap(details::BitRef<TValue0>&& b0, details::BitRef<TValue1>& b1)
  { b1.swap(std::move(b0)); }

  template<typename TValue0, typename TValue1>
  void swap(details::BitRef<TValue0>& b0, details::BitRef<TValue1>&& b1)
  { b0.swap(std::move(b1)); }

  template<typename TValue0, typename TValue1>
  void swap(details::BitRef<TValue0>&& b0, details::BitRef<TValue1>&& b1)
  { /* do nothing */ }
  /// @}
} // namespace bk::details

namespace bk
{
  /// @{ -------------------------------------------------- OPERATOR<<
  template<typename T>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& s, const details::BitRef<T>& b)
  {
      s << (b ? "1" : "0");
      return s;
  }
  /// @}
} // namespace bk

#endif //BKTOOLS_BITREF_H


