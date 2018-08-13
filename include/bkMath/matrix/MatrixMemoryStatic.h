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

#ifndef BK_MATRIXMEMORYSTATIC_H
#define BK_MATRIXMEMORYSTATIC_H

#include <iterator>
#include <type_traits>
#include <utility>

#include "../../bkTypeTraits/make_constant_index_sequence.h"
#include "../../bkTypeTraits/floating_point.h"

#include "MatrixAlignment.h"

namespace bk::details
{
  template<typename TValue, int TRows, int TCols, MatrixAlignment TAlignment, typename TDerived, bool isStatic> class MatrixMemoryStatic
  { /*empty class*/ };

  template<typename TValue, int TRows, int TCols, MatrixAlignment TAlignment, typename TDerived> class MatrixMemoryStatic<TValue, TRows, TCols, TAlignment, TDerived, true>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TRows >= 1);
      static_assert(TCols >= 1);
      static_assert(TAlignment == MatrixAlignment::ColMajor || TAlignment == MatrixAlignment::RowMajor);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = MatrixMemoryStatic<TValue, TRows, TCols, TAlignment, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      value_type _val[TRows * TCols];

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR HELPER
    private:
      template<typename TIndexAccessible, std::size_t... TIndices>
      constexpr MatrixMemoryStatic(const TIndexAccessible& a, std::index_sequence<TIndices...>) noexcept
          : _val{static_cast<value_type>(a[TIndices])...}
      { /* do nothing */ }

      template<int... TIndices>
      constexpr MatrixMemoryStatic(std::integer_sequence<int, TIndices...>) noexcept
          : _val{static_cast<value_type>(TIndices)...}
      { /* do nothing */ }

    public:
      /// @}

      /// @{ -------------------------------------------------- CTOR
      constexpr MatrixMemoryStatic() noexcept
          : MatrixMemoryStatic(bk::make_constant_index_sequence<0, TRows * TCols>())
      { /* do nothing */ }

      constexpr MatrixMemoryStatic(const self_type& other) noexcept
          : MatrixMemoryStatic(other, std::make_index_sequence<TRows * TCols>())
      { /* do nothing */ }

      constexpr MatrixMemoryStatic(self_type&& other) noexcept
          : MatrixMemoryStatic(other, std::make_index_sequence<TRows * TCols>())
      { /* do nothing */ }

      template<typename TContainer, std::enable_if_t<std::is_class_v<TContainer>>* = nullptr>
      constexpr MatrixMemoryStatic(const TContainer& other) noexcept
          : MatrixMemoryStatic(other, std::make_index_sequence<TRows * TCols>())
      {
          static_assert(!bk::is_matrix_v<TContainer> || (bk::is_static_matrix_of_size_v<TContainer, TRows, TCols> || bk::is_dynamic_matrix_v<TContainer>), "size mismatch");
      }

      template<typename... TValues>
      constexpr MatrixMemoryStatic(TValues ... values) noexcept
          : _val{static_cast<value_type>(values)...}
      { static_assert(sizeof...(TValues) == TRows * TCols); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~MatrixMemoryStatic() noexcept = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr int RowsAtCompileTime() noexcept
      { return TRows; }

      [[nodiscard]] static constexpr int ColsAtCompileTime() noexcept
      { return TCols; }

      [[nodiscard]] static constexpr int NumElementsAtCompileTime() noexcept
      { return TRows * TCols; }

      [[nodiscard]] static constexpr MatrixAlignment AlignmentAtCompileTime() noexcept
      { return TAlignment; }
      /// @}


      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] static constexpr unsigned int num_rows() noexcept
      { return static_cast<unsigned int>(TRows); }

      [[nodiscard]] static constexpr unsigned int num_cols() noexcept
      { return static_cast<unsigned int>(TCols); }

      [[nodiscard]] static constexpr unsigned int num_elements() noexcept
      { return static_cast<unsigned int>(TRows * TCols); }
      /// @}

      /// @{ -------------------------------------------------- GET ALIGNMENT
      [[nodiscard]] static constexpr MatrixAlignment alignment() noexcept
      { return TAlignment; }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR []
      [[nodiscard]] constexpr value_type& operator[](unsigned int listId) noexcept
      { return _val[listId]; }

      [[nodiscard]] constexpr const value_type& operator[](unsigned int listId) const noexcept
      { return _val[listId]; }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] constexpr value_type* begin() noexcept
      { return _val; }

      [[nodiscard]] constexpr const value_type* begin() const noexcept
      { return _val; }

      [[nodiscard]] constexpr value_type* end() noexcept
      { return _val + num_elements(); }

      [[nodiscard]] constexpr const value_type* end() const noexcept
      { return _val + num_elements(); }

      [[nodiscard]] constexpr value_type* rbegin() noexcept
      { return std::reverse_iterator<value_type*>(end()); }

      [[nodiscard]] constexpr const value_type* rbegin() const noexcept
      { return std::reverse_iterator<const value_type*>(end()); }

      [[nodiscard]] constexpr value_type* rend() noexcept
      { return std::reverse_iterator<value_type*>(begin()); }

      [[nodiscard]] constexpr const value_type* rend() const noexcept
      { return std::reverse_iterator<const value_type*>(begin()); }
      /// @}

      /// @{ -------------------------------------------------- GET SPECIAL MATRICES
      [[nodiscard]] static constexpr derived_type Constant(value_type x) noexcept
      {
          derived_type res;
          res.set_constant(x);
          return res;
      }

      [[nodiscard]] static constexpr derived_type Zero() noexcept
      { return Constant(0); }

      [[nodiscard]] static constexpr derived_type One() noexcept
      { return Constant(1); }

      [[nodiscard]] static constexpr derived_type Identity() noexcept
      {
          derived_type res;
          res.set_identity();
          return res;
      }

      [[nodiscard]] static constexpr derived_type Random_int(value_type rmin, value_type rmax) noexcept
      {
          derived_type res;
          res.set_random_int(rmin, rmax);
          return res;
      }

      [[nodiscard]] static constexpr derived_type Random_float(value_type rmin = static_cast<value_type>(0), value_type rmax = static_cast<value_type>(1)) noexcept
      {
          derived_type res;
          res.set_random_float(rmin, rmax);
          return res;
      }

      //!  generates a compile-time random uint vector with values between rmin and rmax.
      /*!
       *          Usage: Random_ct<__LINE__*__COUNTER__>(min, max)
       *
       *           - __COUNTER__ is a compile-time macro that is increased on each occurrence in the code
       *           - __LINE__ is a compile-time macro that returns the code line
       *           - write __LINE__*__COUNTER__ explicitly; do not use it in a for loop!
       *           - This will create three identical vectors:
       *               for(int = 0; i < 3; ++i) Mat3i::Random_ct<0,10,__LINE__*__COUNTER__>
       *
       *          - Instead, manually unroll the loop
       */
      template<unsigned long long seedAdd>
      [[nodiscard]] static constexpr derived_type Random_ct(value_type rmin = static_cast<value_type>(0), value_type rmax = static_cast<value_type>(1)) noexcept
      {
          derived_type res;
          res.template set_random_ct<seedAdd>();

          const value_type range = rmax - rmin;

          for (unsigned int i = 0; i < TRows * TCols; ++i)
          {
              if constexpr (std::is_integral_v<value_type>)
              { res[i] = (res[i] % range) + rmin; }
              else if constexpr (bk::is_floating_point_v<value_type>)
              { res[i] = rmin + static_cast<value_type>(static_cast<long double>(res[i]) / static_cast<long double>((1UL << 31) - 1 /*uint32max*/)) * range; }
          }

          return res;
      }
      /// @}
      
      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
    protected:
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other) noexcept = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&& other) noexcept = default;
    public:
      /// @}

  }; // class MatrixMemoryStatic
} // namespace bk::details

#endif //BK_MATRIXMEMORY_H


