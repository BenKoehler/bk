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

#ifndef BKTOOLS_BITVECTORDYNAMIC_H
#define BKTOOLS_BITVECTORDYNAMIC_H

#include <fstream>
#include <string_view>
#include <vector>

#include <bkTools/bitvector/BitVectorBase.h>
#include <bkTools/bitvector/BitRef.h>
#include <bkTools/lib/bkTools_export.h>

namespace bk::details
{
  class BKTOOLS_EXPORT BitVectorDynamic : public BitVectorBase<-1, BitVectorDynamic>
  {
      //====================================================================================================
      //===== DYNAMIC CONSTANTS, DEFINITIONS, FRIENDS
      //====================================================================================================
      using base_type = BitVectorBase<-1, BitVectorDynamic>;
    public:
      using self_type = BitVectorDynamic;
      using value_type = typename base_type::value_type;
      using container_type=std::vector<value_type>;

      //====================================================================================================
      //===== GETTER (COMPILE-TIME)
      //====================================================================================================
      /// @{ -------------------------------------------------- NUMBER OF BITS
      [[nodiscard]] static constexpr int Size() noexcept
      { return -1; }
      /// @}

      /// @{ -------------------------------------------------- NUMBER OF BYTES
      [[nodiscard]] static constexpr unsigned int NumBytesAtCompileTime() noexcept
      { return 0U; }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      unsigned int _size; ///< number of represented bits
      container_type _data; ///< byte storage vector

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      BitVectorDynamic() = default;
      BitVectorDynamic(const self_type&) = default;
      BitVectorDynamic(self_type&&) = default;
      BitVectorDynamic(unsigned int size);

      template<typename... Bits_>
      BitVectorDynamic(Bits_... b)
          : _size(sizeof...(b)),
            _data((_size / 8) + (_size % 8 == 0 ? 0 : 1), 0)
      { this->template _set_bits<0>(b...); }
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~BitVectorDynamic() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SIZE / NUMBER OF BITS
      [[nodiscard]] unsigned int size() const;
      /// @}

      /// @{ -------------------------------------------------- ACCESS
      //! access bit at position i
      /// \return BitRef object that acts like a reference
      [[nodiscard]] auto operator[](unsigned int i) -> BitRef<value_type>;
      [[nodiscard]] auto operator[](unsigned int i) const -> BitRef<const value_type>;
      /// @}

      /// @{ -------------------------------------------------- ACCESS RAW BYTES
      [[nodiscard]] auto data() -> typename container_type::value_type*;
      [[nodiscard]] auto data() const -> const container_type::value_type*;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SIZE
      //! resize internal data vector
      void set_size(unsigned int size);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filepath);
      void load(std::ifstream& file);
      /// @}
  }; // class BitVectorDynamic
} // namespace bk::details

#endif //BKTOOLS_BITVECTORDYNAMIC_H


