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

#ifndef __BK_REFMATRIX_H__
#define __BK_REFMATRIX_H__

#include <initializer_list>
#include <iterator>
#include <type_traits>

#include "MatrixAlignment.h"
#include "RefMatrixMemoryStatic.h"
#include "RefMatrixMemoryDynamic.h"
#include "RefMatrixIterator.h"
#include "MatrixFunctions.h"
#include "SquareMatrixFunctions.h"
#include "SquareMatrix2Functions.h"
#include "SquareMatrix3Functions.h"
#include "VectorFunctions.h"
#include "Vector2Functions.h"
#include "Vector3Functions.h"
#include "type_traits/matrix_traits.h"

namespace bk
{
  namespace
  {
    // helper for brevity
    template<int R0, int R1, int C0, int C1> constexpr bool is_dynamic_ref_matrix_v = R0 < 0 || R1 < 0 || C0 < 0 || C1 < 0 || R1 - R0 + 1 <= 0 || C1 - C0 + 1 <= 0;
  } // anonymous namespace

  template<typename TMatrix, int TRowId0 = -1, int TRowId1 = -1, int TColId0 = -1, int TColId1 = -1> class RefMatrix
      : public details::RefMatrixMemoryDynamic<TMatrix, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1>>,
        public details::RefMatrixMemoryStatic<TMatrix, TRowId0, TRowId1, TColId0, TColId1, !is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1>>,
        public details::MatrixFunctions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>>,
        public details::SquareMatrixFunctions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_square_matrix_row_col_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1>>,
        public details::SquareMatrix2Functions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_square_matrix_row_col_of_size_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1, 2>>,
        public details::SquareMatrix3Functions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_square_matrix_row_col_of_size_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1, 3>>,
        public details::VectorFunctions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_vector_row_col_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1>>,
        public details::Vector2Functions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_vector_row_col_of_size_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1, 2>>,
        public details::Vector3Functions<typename TMatrix::value_type, RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>, is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1> || bk::is_static_vector_row_col_of_size_v<TRowId1 - TRowId0 + 1, TColId1 - TColId0 + 1, 3>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert((TRowId0 < 0 && TRowId1 < 0) || TRowId0 <= TRowId1, "invalid row ids");
      static_assert(TRowId1 < 0 || TRowId1 < TMatrix::RowsAtCompileTime(), "invalid row ids");
      static_assert((TColId0 < 0 && TColId1 < 0) || TColId0 <= TColId1, "invalid col ids");
      static_assert(TColId1 < 0 || TColId1 < TMatrix::ColsAtCompileTime(), "invalid col ids");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>;
      using base_memory_type = std::conditional_t<is_dynamic_ref_matrix_v<TRowId0, TRowId1, TColId0, TColId1>, details::RefMatrixMemoryDynamic<TMatrix, true>, details::RefMatrixMemoryStatic<TMatrix, TRowId0, TRowId1, TColId0, TColId1, true>>;
    public:
      using matrix_type = TMatrix;
      template<typename TValue_, int TRows_ = TRowId1 - TRowId0 + 1, int TCols_ = TColId1 - TColId0 + 1, MatrixAlignment TAlignment_ = bk::matrix_row_col_traits<TRows_, TCols_>::default_alignment()> using self_template_type = typename matrix_type::template self_template_type<TValue_, TRows_, TCols_, TAlignment_>;
      using value_type = typename matrix_type::value_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr RefMatrix() = delete;
      constexpr RefMatrix(const self_type&) = default;
      constexpr RefMatrix(self_type&&) = default;

      constexpr RefMatrix(TMatrix& m)
          : base_memory_type(m)
      { /* do nothing */ }

      constexpr RefMatrix(TMatrix& m, unsigned int rowIDBegin, unsigned int rowIDEnd, unsigned int colIDBegin, unsigned int colIDEnd)
          : base_memory_type(m, rowIDBegin, rowIDEnd, colIDBegin, colIDEnd)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~RefMatrix() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR()
      [[nodiscard]] constexpr decltype(auto) operator()(unsigned int rowID, unsigned int colID)
      { return this->operator[](this->listId_from_rowId_colId(rowID, colID)); }

      [[nodiscard]] constexpr decltype(auto) operator()(unsigned int rowID, unsigned int colID) const
      { return this->operator[](this->listId_from_rowId_colId(rowID, colID)); }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] constexpr auto begin()
      { return RefMatrixIterator<self_type>(*this, 0); }

      [[nodiscard]] constexpr auto begin() const
      { return RefMatrixIterator<const self_type>(*this, 0); }

      [[nodiscard]] constexpr auto end()
      { return RefMatrixIterator<self_type>(*this, this->num_elements()); }

      [[nodiscard]] constexpr auto end() const
      { return RefMatrixIterator<const self_type>(*this, this->num_elements()); }

      [[nodiscard]] constexpr auto rbegin()
      { return std::reverse_iterator<decltype(end())>(end()); }

      [[nodiscard]] constexpr auto rbegin() const
      { return std::reverse_iterator<decltype(end())>(end()); }

      [[nodiscard]] constexpr auto rend()
      { return std::reverse_iterator<decltype(begin())>(begin()); }

      [[nodiscard]] constexpr auto rend() const
      { return std::reverse_iterator<decltype(begin())>(begin()); }
      /// @}

      /// @{ -------------------------------------------------- COPY
      template<typename Value__ = value_type>
      [[nodiscard]] constexpr auto copy() const
      {
          using result_type = typename matrix_type::template self_template_type<Value__, self_type::RowsAtCompileTime(), self_type::ColsAtCompileTime(), matrix_type::AlignmentAtCompileTime()>;

          result_type res;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              res.set_size(this->num_rows(), this->num_cols());
              res.set_alignment(this->alignment());
          }

          for (unsigned int r = 0; r < this->num_rows(); ++r)
          {
              for (unsigned int c = 0; c < this->num_cols(); ++c)
              { res(r, c) = static_cast<Value__>(this->_m(r + this->rowID_begin(), c + this->colID_begin())); }
          }

          return res;
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& rhs)
      {
          for (unsigned int i = 0; i < this->num_elements(); ++i)
          { this->operator[](i) = rhs[i]; }

          return *this;
      }

      [[maybe_unused]] constexpr self_type& operator=(self_type&& rhs)
      {
          for (unsigned int i = 0; i < this->num_elements(); ++i)
          { this->operator[](i) = std::move(rhs[i]); }

          return *this;
      }

      template<typename T_>
      [[maybe_unused]] constexpr self_type& operator=(T_&& rhs)
      {
          this->set(std::forward<T_>(rhs));
          return *this;
      }

      template<typename T_>
      [[maybe_unused]] constexpr self_type& operator=(std::initializer_list<T_> rhs)
      {   // if this overloading is not defined, a = {1,2,3,4} invokes a constructor call
          this->set(std::move(rhs));
          return *this;
      }
      /// @}
  }; // class RefMatrix

} // namespace bk

#endif //__BK_REFMATRIX_H__


