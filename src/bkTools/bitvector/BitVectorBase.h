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

#ifndef BKTOOLS_BITVECTORBASE_H
#define BKTOOLS_BITVECTORBASE_H

#include <cmath>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include <bkTools/bitvector/type_traits/is_bitvector.h>
#include <bkTools/bitvector/BitVectorIterator.h>

namespace bk::details
{
  template<int N, typename TDerived> class BitVectorBase
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using derived_type = TDerived;
    public:
      using self_type = BitVectorBase<N, TDerived>;
      using value_type = std::uint8_t;
      using iterator = BitVectorIterator<self_type>;
      using const_iterator = BitVectorIterator<const self_type>;

      //====================================================================================================
      //===== GETTER (COMPILE-TIME)
      //====================================================================================================
      /// @{ -------------------------------------------------- NUMBER OF BITS
      [[nodiscard]] static constexpr int Size() noexcept
      {return static_cast<int>(N);}
      /// @}

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr BitVectorBase() = default;
      constexpr BitVectorBase(const BitVectorBase& other) = default;
      constexpr BitVectorBase(BitVectorBase&& other) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~BitVectorBase() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TO DERIVED
    private:
      [[nodiscard]] derived_type* deriv()
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] const derived_type* deriv() const
      { return static_cast<const derived_type*>(this); }

    public:
      /// @}

      /// @{ -------------------------------------------------- SIZE / NUMBER OF BITS
      [[nodiscard]] constexpr auto size() const
      { return deriv()->size(); }
      /// @}

      /// @{ -------------------------------------------------- ACCESS
      //! access bit at position i
      /// \return BitRef object that acts like a reference
      [[nodiscard]] constexpr auto operator[](unsigned int i)
      { return deriv()->operator[](i); }

      [[nodiscard]] constexpr auto operator[](unsigned int i) const
      { return deriv()->operator[](i); }
      /// @}

      /// @{ -------------------------------------------------- ACCESS RAW BYTES
      [[nodiscard]] constexpr decltype(auto) data()
      { return deriv()->data(); }

      [[nodiscard]] constexpr decltype(auto) data() const
      { return deriv()->data(); }
      /// @}

      /// @{ -------------------------------------------------- ITERATORS
      [[nodiscard]] auto begin()
      { return BitVectorIterator<self_type>(*this, 0); }

      [[nodiscard]] auto begin() const
      { return BitVectorIterator<const self_type>(*this, 0); }

      [[nodiscard]] auto end()
      { return BitVectorIterator<self_type>(*this, size()); }

      [[nodiscard]] auto end() const
      { return BitVectorIterator<const self_type>(*this, size()); }

      [[nodiscard]] constexpr auto rbegin()
      { return std::reverse_iterator<decltype(end())>(end()); }

      [[nodiscard]] constexpr auto rbegin() const
      { return std::reverse_iterator<decltype(end())>(end()); }

      [[nodiscard]] constexpr auto rend()
      { return std::reverse_iterator<decltype(begin())>(begin()); }

      [[nodiscard]] constexpr auto rend() const
      { return std::reverse_iterator<decltype(begin())>(begin()); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS TO PROCESS VARIADIC TEMPLATE CTOR
    protected:
      template<int i, typename Bit_>
      void _set_bits(Bit_ b)
      { operator[](i) = static_cast<value_type>(b); }

      template<int i, typename Bit_, typename... Bits_>
      void _set_bits(Bit_ b, Bits_... bn)
      {
          _set_bits<i>(b);
          _set_bits<i + 1>(bn...);
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SET ALL TO TRUE(1) OR FALSE(0)
      constexpr void set_zero_internal()
      {
          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) = false; }
      }

      constexpr void set_one_internal()
      {
          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) = true; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== BIT OPERATIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- FLIP
      //! inverts each value
      constexpr void flip_internal()
      {
          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) = !operator[](i); }
      }

      [[nodiscard]] constexpr auto flip() const
      {
          auto res = *deriv();
          res.flip_internal();
          return res;
      }

      [[nodiscard]] constexpr auto operator~() const
      { return flip(); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR &(=)
      //! bitwise AND
      template<typename T_>
      [[nodiscard]] constexpr auto operator&(T_&& rhs) const
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR &: RHS IS NO BITVECTOR");

          auto res = *deriv();
          res &= std::forward<T_>(rhs);
          return res;
      }

      template<typename T_>
      constexpr void operator&=(T_&& rhs)
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR &=: RHS IS NO BITVECTOR");

          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) &= rhs[i]; } // for i = 0; i < size()
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR |(=)
      //! bitwise OR
      template<typename T_>
      [[nodiscard]] constexpr auto operator|(T_&& rhs) const
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR |: RHS IS NO BITVECTOR");

          auto res = *deriv();
          res |= std::forward<T_>(rhs);
          return res;
      }

      template<typename T_>
      constexpr void operator|=(T_&& rhs)
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR |=: RHS IS NO BITVECTOR");

          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) |= rhs[i]; } // for i = 0; i < size()
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR ^(=)
      ///
      //! bitwise XOR
      template<typename T_>
      [[nodiscard]] constexpr auto operator^(T_&& rhs) const
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR ^: RHS IS NO BITVECTOR");

          auto res = *deriv();
          res ^= std::forward<T_>(rhs);
          return res;
      }

      template<typename T_>
      constexpr void operator^=(T_&& rhs)
      {
          static_assert(bk::is_bitvector_v<std::decay_t<T_>>, "BITVECTOR ^=: RHS IS NO BITVECTOR");

          for (unsigned int i = 0; i < size(); ++i)
          { operator[](i) ^= rhs[i]; } // for i = 0; i < size()
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR <<(=)
      ///
      //! bitshift left
      [[nodiscard]] constexpr auto operator<<(unsigned int off) const
      {
          auto res = *deriv();
          res <<= off;
          return res;
      }

      constexpr void operator<<=(unsigned int off)
      {
          if (static_cast<unsigned int>(off) >= size())
          { set_zero_internal(); }
          else
          {
              for (unsigned int i = 0; i < size() - off; ++i)
              { operator[](i) = operator[](i + off); } // for i = 0; i < size()-off

              for (unsigned int i = size() - off; i < size(); ++i)
              { operator[](i) = 0; } // for i = 0; i < size()
          }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR >>(=)
      //! bitshift right
      [[nodiscard]] constexpr auto operator>>(unsigned int off) const
      {
          auto res = *deriv();
          res >>= off;
          return res;
      }

      constexpr void operator>>=(unsigned int off)
      {
          if (static_cast<unsigned int>(off) >= size())
          { set_zero_internal(); }
          else
          {
              for (unsigned int i = size() - 1; i >= static_cast<unsigned int>(off); --i)
              { operator[](i) = operator[](i - off); } // for i = off; i < size()

              for (unsigned int i = 0; i < static_cast<unsigned int>(off); ++i)
              { operator[](i) = 0; } // for i = 0; i < off
          }
      }
      /// @}

      //====================================================================================================
      //===== MISC FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- TO NUMBER
      //! converts the bit vector to an integer
      /// \param littleEndian set true if little endian, false if big endian
      [[nodiscard]] constexpr unsigned int to_number(bool littleEndian = true) const
      {
          const unsigned int s = size();
          unsigned int res = 0;

          if (littleEndian)
          {
              unsigned int temp = std::pow(2, s - 1);
              for (unsigned int i = 0; i < s; ++i)
              {
                  res += operator[](i) * temp;
                  temp >>= 1;
              } // for i = 0; i < s
          }
          else
          {
              unsigned int temp = 1;
              for (unsigned int i = 0; i < s; ++i)
              {
                  res += operator[](i) * temp;
                  temp <<= 1;
              } // for i = 0; i < s
          }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- TO STRING
      [[nodiscard]] std::string to_string() const
      {
          const unsigned int n = size();
          std::stringstream ss;
          for (unsigned int i = 0; i < n; ++i)
          {
              ss << operator[](i);
              if (i != n - 1 && (i + 1) % 8 == 0)
              { ss << " "; }
          } // for i = 0; i < size()

          return ss.str();
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] constexpr bool save(const std::string& filepath) const
      {
          if (filepath.empty())
          { return false; }

          std::ofstream file(filepath, std::ios_base::out | std::ios_base::binary);

          if (file.good())
          {
              save(file);
              file.close();
              return true;
          }

          return false;
      }

      constexpr void save(std::ofstream& file) const
      {
          using file_size_type = std::uint64_t;
          const file_size_type N_ = (this->size() / 8) + (this->size() % 8 == 0 ? 0 : 1);
          file.write(reinterpret_cast<const char*>(&N_), sizeof(file_size_type));
          file.write(reinterpret_cast<const char*>(data()), N_ * sizeof(value_type));
      }
      /// @}
  }; // class BitVectorBase
} // namespace bk::details

namespace bk
{
  template<int N, typename TDeriv>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& s, const details::BitVectorBase <N, TDeriv>& bitvec)
  {
      s << bitvec.to_string();
      return s;
  }
} // namespace bk

#endif //BKTOOLS_BITVECTORBASE_H


