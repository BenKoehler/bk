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

#ifndef BKTOOLS_BITVECTORSTATIC_H
#define BKTOOLS_BITVECTORSTATIC_H

#include <cassert>
#include <cstdint>
#include <utility>

#include "BitVectorBase.h"
#include "BitRef.h"

namespace bk::details
{
  template<unsigned int N> class BitVectorStatic : public BitVectorBase<N, BitVectorStatic<N>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = BitVectorBase<N, BitVectorStatic<N>>;
    public:
      using self_type = BitVectorStatic<N>;
      using value_type = typename base_type::value_type;

      //====================================================================================================
      //===== GETTER (COMPILE-TIME)
      //====================================================================================================
      /// @{ -------------------------------------------------- NUMBER OF BITS
      [[nodiscard]] static constexpr int Size() noexcept
      { return static_cast<int>(N); }
      /// @}

      /// @{ -------------------------------------------------- NUMBER OF BYTES
      [[nodiscard]] static constexpr unsigned int NumBytesAtCompileTime() noexcept
      { return (Size() / 8) + (Size() % 8 == 0 ? 0 : 1); }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      value_type _data[NumBytesAtCompileTime()]; ///< byte storage array

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: CTOR
      template<typename Array_, std::size_t... Is>
      constexpr BitVectorStatic(const Array_& a, std::index_sequence<Is...>)
          : base_type(),
            _data{a[Is]...}
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- CTOR
    public:
      constexpr BitVectorStatic()
          : _data{static_cast<value_type>(0)}
      { /* do nothing */ }

      constexpr BitVectorStatic(const self_type& other)
          : BitVectorStatic(other._data, std::make_index_sequence<NumBytesAtCompileTime()>())
      { /* do nothing */ }

      constexpr BitVectorStatic(self_type&& other)
          : BitVectorStatic(other._data, std::make_index_sequence<NumBytesAtCompileTime()>())
      { /* do nothing */ }

      template<typename... Bits_>
      BitVectorStatic(Bits_... b)
      {
          static_assert(sizeof...(b) == Size(), "INVALID NUMBER OF BITS AS PARAMETERS");
          this->template _set_bits<0>(b...);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~BitVectorStatic() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SIZE / NUMBER OF BITS
      [[nodiscard]] static constexpr auto size() noexcept
      { return Size(); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      //! access bit at position i
      /// \return BitRef object that acts like a reference
      [[nodiscard]] constexpr BitRef<value_type> operator[](unsigned int i)
      {
          assert(static_cast<decltype(Size())>(i) < Size() && "invalid id");
          return BitRef<value_type>(_data[i / 8], static_cast<value_type>(i % 8));
      }

      [[nodiscard]] constexpr BitRef<const value_type> operator[](unsigned int i) const
      {
          assert(static_cast<decltype(Size())>(i) < Size() && "invalid id");
          return BitRef<const value_type>(_data[i / 8], static_cast<value_type>(i % 8));
      }
      /// @}

      /// @{ -------------------------------------------------- ACCESS RAW BYTES
      [[nodiscard]] constexpr value_type* data()
      { return _data; }

      [[nodiscard]] constexpr const value_type* data() const
      { return _data; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other)
      {
          for (unsigned int i = 0; i < Size(); ++i)
          { _data[i] = other._data[i]; }

          return *this;
      }

      [[maybe_unused]] constexpr self_type& operator=(self_type&& other)
      {
          for (unsigned int i = 0; i < Size(); ++i)
          { _data[i] = std::move(other._data[i]); }

          return *this;
      }
      /// @}
  }; // class BitVectorStatic
} // namespace bk::details

#endif //BKTOOLS_BITVECTORSTATIC_H


