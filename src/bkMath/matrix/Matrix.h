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

#ifndef __BK_MATRIX_H__
#define __BK_MATRIX_H__

#include <type_traits>

#include <bkMath/matrix/MatrixAlignment.h>
#include <bkMath/matrix/RefMatrix.h>
#include <bkMath/matrix/MatrixMemoryStatic.h>
#include <bkMath/matrix/MatrixMemoryDynamic.h>
#include <bkMath/matrix/MatrixFunctions.h>
#include <bkMath/matrix/SquareMatrixFunctions.h>
#include <bkMath/matrix/SquareMatrix2Functions.h>
#include <bkMath/matrix/SquareMatrix3Functions.h>
#include <bkMath/matrix/VectorFunctions.h>
#include <bkMath/matrix/Vector2Functions.h>
#include <bkMath/matrix/Vector3Functions.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>

namespace bk
{
  template<typename TValue, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()> class Matrix
      : public details::MatrixMemoryDynamic<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols>>,
        public details::MatrixMemoryStatic<TValue, TRows, TCols, TAlignment, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_static_matrix_row_col_v<TRows, TCols>>,
        public details::MatrixFunctions<TValue, Matrix<TValue, TRows, TCols, TAlignment>>,
        public details::SquareMatrixFunctions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_square_matrix_row_col_v<TRows, TCols>>,
        public details::SquareMatrix2Functions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_square_matrix_row_col_of_size_v<TRows, TCols, 2>>,
        public details::SquareMatrix3Functions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_square_matrix_row_col_of_size_v<TRows, TCols, 3>>,
        public details::VectorFunctions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_vector_row_col_v<TRows, TCols>>,
        public details::Vector2Functions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_vector_row_col_of_size_v<TRows, TCols, 2>>,
        public details::Vector3Functions<TValue, Matrix<TValue, TRows, TCols, TAlignment>, bk::is_dynamic_matrix_row_col_v<TRows, TCols> || bk::is_static_vector_row_col_of_size_v<TRows, TCols, 3>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
    private:
      using self_type = Matrix<TValue, TRows, TCols, TAlignment>;
      using base_memory_type = std::conditional_t<bk::is_dynamic_matrix_row_col_v<TRows, TCols>, details::MatrixMemoryDynamic<TValue, self_type, true>, details::MatrixMemoryStatic<TValue, TRows, TCols, TAlignment, self_type, true>>;
    public:
      template<typename TValue_, int TRows_ = base_memory_type::RowsAtCompileTime(), int TCols_ = base_memory_type::ColsAtCompileTime(), MatrixAlignment TAlignment_ = bk::matrix_row_col_traits<TRows_, TCols_>::default_alignment()> using self_template_type = Matrix<TValue_, TRows_, TCols_, TAlignment_>;

      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr Matrix() = default;
      constexpr Matrix(const self_type&) = default;
      constexpr Matrix(self_type&&) noexcept = default;

      template<typename... Args>
      constexpr Matrix(Args&& ... args)
          : base_memory_type(std::forward<Args>(args)...)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Matrix() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- INDEX OPERATORS
      [[nodiscard]] constexpr decltype(auto) operator()(unsigned int rowID, unsigned int colID)
      { return (*this)[this->listId_from_rowId_colId(rowID, colID)]; }

      [[nodiscard]] constexpr decltype(auto) operator()(unsigned int rowID, unsigned int colID) const
      { return (*this)[this->listId_from_rowId_colId(rowID, colID)]; }
      /// @}

      /// @{ -------------------------------------------------- SUB MATRIX REF
      template<unsigned int fromRowID, unsigned int toRowID, unsigned int fromColID, unsigned int toColID>
      [[nodiscard]] constexpr auto sub_matrix_ref()
      { return RefMatrix<self_type, fromRowID, toRowID, fromColID, toColID>(*this); }

      [[nodiscard]] auto sub_matrix_ref(unsigned int fromRowID, unsigned int toRowID, unsigned int fromColID, unsigned int toColID)
      { return RefMatrix<self_type>(*this, fromRowID, toRowID, fromColID, toColID); }
      /// @}

      /// @{ -------------------------------------------------- ROW/COL REF
      template<unsigned int TId>
      [[nodiscard]] constexpr auto row_ref()
      {
          if constexpr (bk::is_dynamic_matrix_row_col_v<TRows, TCols>)
          { return row_ref(TId); }
          else
          { return RefMatrix<self_type, TId, TId, 0, self_type::ColsAtCompileTime() - 1>(*this); }
      }

      [[nodiscard]] auto row_ref(unsigned int Id)
      {
          RefMatrix<self_type> res(*this);
          res.set_size(Id, Id, 0, this->num_cols() - 1);
          return res;
      }

      template<unsigned int TId>
      [[nodiscard]] constexpr auto col_ref()
      {
          if constexpr (bk::is_dynamic_matrix_row_col_v<TRows, TCols>)
          { return col_ref(TId); }
          else
          { return RefMatrix<self_type, 0, self_type::RowsAtCompileTime() - 1, TId, TId>(*this); }
      }

      [[nodiscard]] auto col_ref(unsigned int Id)
      { return RefMatrix<self_type>(*this, 0, this->num_rows() - 1, Id, Id); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) noexcept = default;

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
  }; // class Matrix
} // namespace bk

#include "matrix_operators.h"
#include "matrix_types.h"
#include "MatrixFactory.h"

#endif //__BK_MATRIX_H__