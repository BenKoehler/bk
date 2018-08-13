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

#ifndef __BK_MATRIX_OPERATORS_H__
#define __BK_MATRIX_OPERATORS_H__

#include <algorithm>
#include <ostream>
#include <type_traits>
#include <utility>

#include "Matrix.h"
#include "type_traits/matrix_traits.h"
#include "RefMatrix.h"
#include "MatrixAlignment.h"

//====================================================================================================
//===== STREAM OPERATOR
//====================================================================================================
namespace bk::details
{
  template<typename TMatrix>
  [[maybe_unused]] std::ostream& _cout_matrix(std::ostream& strm, const TMatrix& M)
  {
      for (unsigned int r = 0; r < M.num_rows() - 1; ++r)
      {
          for (unsigned int c = 0; c < M.num_cols() - 1; ++c)
          { strm << M(r, c) << " "; }

          // no " " after last element in col
          strm << M(r, M.num_cols() - 1) << std::endl;
      }

      // no std::endl after last row
      for (unsigned int c = 0; c < M.num_cols() - 1; ++c)
      { strm << M(M.num_rows() - 1, c) << " "; }

      strm << M(M.num_rows() - 1, M.num_cols() - 1);

      return strm;
  }

  //====================================================================================================
  //===== OPERATOR <<
  //====================================================================================================
  template<typename TValue, int TRows, int TCols, MatrixAlignment TAlignment>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& strm, const Matrix<TValue, TRows, TCols, TAlignment>& M)
  { return bk::details::_cout_matrix(strm, M); }

  template<typename TMatrix, int TRowId0, int TRowId1, int TColId0, int TColId1>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& strm, const RefMatrix<TMatrix, TRowId0, TRowId1, TColId0, TColId1>& M)
  { return bk::details::_cout_matrix(strm, M); }

  /*----- m/m
    1: m + m
    2: m&& + m
    3: m + m&&
    4: m&& + m&&
    5: m&& + m&&

    ----- m/r
    6: m + r
    7: m&& + r
    8: r + m
    9: r + m&&

    ----- m/s
    10: m + s
    11: m&& + s
    12: s + m
    13: s + m&&

    ----- r/r
    14: r + r

    ----- r/s
    15: r + s
    16: s + r*/

  //====================================================================================================
  //===== OPERATOR +
  //====================================================================================================
  /*
   * 12: value + Matrix
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator+(const T& x, const Matrix<TValue, TRow, TCol, TAlignment>& m)
  { return m + x; }

  /*
   * 13: value + Matrix&&
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr Matrix<TValue, TRow, TCol, TAlignment> operator+(const T& x, Matrix<TValue, TRow, TCol, TAlignment>&& m)
  { return std::move(m) + x; }

  /*
   * 16: value + RefMatrix
   */
  template<typename M0, int TRowId0, int TRowId1, int TColId0, int TColId1, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator+(const T& x, const RefMatrix<M0, TRowId0, TRowId1, TColId0, TColId1>& m)
  { return m + x; }

  //====================================================================================================
  //===== OPERATOR -
  //====================================================================================================
  /*
   * 12: value - Matrix
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator-(const T& x, const Matrix<TValue, TRow, TCol, TAlignment>& m)
  { return -m + x; }

  /*
   * 13: value - Matrix&&
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr Matrix<TValue, TRow, TCol, TAlignment> operator-(const T& x, Matrix<TValue, TRow, TCol, TAlignment>&& m)
  { return -std::move(m) + x; }

  /*
   * 16: value - RefMatrix
   */
  template<typename M0, int TRowId0, int TRowId1, int TColId0, int TColId1, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator-(const T& x, const RefMatrix<M0, TRowId0, TRowId1, TColId0, TColId1>& m)
  { return -m + x; }

  //====================================================================================================
  //===== OPERATOR *
  //====================================================================================================
  /*
   * 12: value * Matrix
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator*(const T& x, const Matrix<TValue, TRow, TCol, TAlignment>& m)
  { return m * x; }

  /*
   * 13: value * Matrix&&
   */
  template<typename TValue, int TRow, int TCol, MatrixAlignment TAlignment, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr Matrix<TValue, TRow, TCol, TAlignment> operator*(const T& x, Matrix<TValue, TRow, TCol, TAlignment>&& m)
  { return std::move(m) * x; }

  /*
   * 16: value * RefMatrix
   */
  template<typename M0, int TRowId0, int TRowId1, int TColId0, int TColId1, typename T, std::enable_if_t<!bk::is_matrix_v<T>>* = nullptr>
  [[nodiscard]] constexpr auto operator*(const T& x, const RefMatrix<M0, TRowId0, TRowId1, TColId0, TColId1>& m)
  { return m * x; }
} // namespace bk::details

#endif //__BK_MATRIX_OPERATORS_H__


