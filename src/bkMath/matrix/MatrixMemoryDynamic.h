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

#ifndef BK_MATRIXMEMORYDYNAMIC_H
#define BK_MATRIXMEMORYDYNAMIC_H

#include <algorithm>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include <bkMath/matrix/MatrixAlignment.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isDynamic> class MatrixMemoryDynamic
  { /* empty class */ };

  template<typename TValue, typename TDerived> class MatrixMemoryDynamic<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = MatrixMemoryDynamic<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      unsigned int _rows;
      unsigned int _cols;
      MatrixAlignment _alignment;
    protected:
      std::vector<value_type> _val;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      MatrixMemoryDynamic()
          : _rows(1U),
            _cols(1U),
            _alignment(MatrixAlignment::ColMajor),
            _val(1, 0)
      { /* do nothing */ }

      MatrixMemoryDynamic(const self_type&) = default;
      MatrixMemoryDynamic(self_type&&) noexcept = default;

      template<typename TMatrix, std::enable_if_t<bk::is_matrix_v<TMatrix>>* = nullptr>
      MatrixMemoryDynamic(const TMatrix& other)
          : _rows(other.num_rows()),
            _cols(other.num_cols()),
            _alignment(other.alignment()),
            _val(_rows * _cols)
      { std::copy(other.begin(), other.end(), _val.begin()); }

      template<typename TContainer, std::enable_if_t<std::is_class_v<TContainer> && !bk::is_matrix_v<TContainer>>* = nullptr>
      MatrixMemoryDynamic(const TContainer& other)
          : _rows(other.size()),
            _cols(1),
            _alignment(MatrixAlignment::ColMajor),
            _val(_rows * _cols)
      { std::copy(other.begin(), other.end(), _val.begin()); }

      template<typename... TValues, std::enable_if_t<(sizeof...(TValues) > 0) && !std::conjunction_v<bk::details::is_mat<TValues>...>>* = nullptr>
      MatrixMemoryDynamic(TValues ... values)
          : _rows(sizeof...(TValues)),
            _cols(1),
            _alignment(MatrixAlignment::ColMajor),
            _val({static_cast<value_type>(std::forward<TValues>(values))...})
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~MatrixMemoryDynamic() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr int RowsAtCompileTime() noexcept
      { return 0; }

      [[nodiscard]] static constexpr int ColsAtCompileTime() noexcept
      { return 0; }

      [[nodiscard]] static constexpr int NumElementsAtCompileTime() noexcept
      { return 0; }

      [[nodiscard]] static constexpr MatrixAlignment AlignmentAtCompileTime() noexcept
      { return MatrixAlignment::Dynamic; }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] unsigned int num_rows() const noexcept
      { return _rows; }

      [[nodiscard]] unsigned int num_cols() const noexcept
      { return _cols; }

      [[nodiscard]] unsigned int num_elements() const noexcept
      { return _rows * _cols; }
      /// @}

      /// @{ -------------------------------------------------- GET ALIGNMENT
      [[nodiscard]] MatrixAlignment alignment() const noexcept
      { return _alignment; }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR []
      [[nodiscard]] value_type& operator[](unsigned int listId)
      { return _val[listId]; }

      [[nodiscard]] const value_type& operator[](unsigned int listId) const
      { return _val[listId]; }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] typename std::vector<value_type>::iterator begin()
      { return _val.begin(); }

      [[nodiscard]] typename std::vector<value_type>::const_iterator begin() const
      { return _val.begin(); }

      [[nodiscard]] typename std::vector<value_type>::iterator end()
      { return _val.end(); }

      [[nodiscard]] typename std::vector<value_type>::const_iterator end() const
      { return _val.end(); }

      [[nodiscard]] typename std::vector<value_type>::iterator rbegin()
      { return _val.rbegin(); }

      [[nodiscard]] typename std::vector<value_type>::const_iterator rbegin() const
      { return _val.rbegin(); }

      [[nodiscard]] typename std::vector<value_type>::iterator rend()
      { return _val.rend(); }

      [[nodiscard]] typename std::vector<value_type>::const_iterator rend() const
      { return _val.rend(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

    public:

      /// @{ -------------------------------------------------- SET SIZE
      void set_size(unsigned int rows, unsigned int cols)
      {
          assert(rows > 0 && cols > 0);

          _rows = std::max(1U, rows);
          _cols = std::max(1U, cols);
          _val.resize(num_elements());
      }
      /// @}

      /// @{ -------------------------------------------------- SET SIZE (VECTOR VERSION)
      //! set new vector dimensions (N)
      /*!
       * This must be a vector, i.e. nRows or nCols must be 1.
       * If this is not the case a new col-vector will be created (caution! existing values can get lost this way)
       */
      void set_size(unsigned int N)
      {
          assert(N > 0);

          if (_cols == 1)
          { _rows = N; }
          else if (_rows == 1)
          { _cols = N; }
          else
          {
              _cols = 1;
              _rows = N;
          }

          _val.resize(num_elements());
      }
      /// @}

      /// @{ -------------------------------------------------- SET ALIGNMENT
      void set_alignment(MatrixAlignment alignment)
      {
          assert(alignment == MatrixAlignment::RowMajor || alignment == MatrixAlignment::ColMajor);
          _alignment = alignment;
      }
      /// @}

      /// @{ -------------------------------------------------- RESIZE
      /*!
       * - If the new size is greater than or equal the current size, existing values will
       *   stay at their current position.
       * - If the new size is smaller than the current size, corresponding values will
       *   be cropped, i.e., the values of the removed row/col will be missing afterwards
       */
      void resize_existing(unsigned int rows, unsigned int cols)
      {
          assert(rows > 0 && cols > 0);

          std::vector<value_type> x(rows * cols);

          for (unsigned int r = 0; r < std::min(_rows, rows); ++r)
          { // min is used since new size could be smaller
              for (unsigned int c = 0; c < std::min(_cols, cols); ++c)
              { // min is used since new size could be smaller
                  x[deriv()->listId_from_rowId_colId(r, c, rows, cols, deriv()->is_col_major())] = deriv()->operator()(r, c);
              }
          }

          _rows = rows;
          _cols = cols;
          _val = std::move(x);
      }
      /// @}

      /// @{ -------------------------------------------------- ADD/REMOVE ROW/COL
      /*!
       * - cols can be added to the left or right of the matrix
       * - rows can be added to the top or bottom of the matrix
       * - new rows/cols are initialized with 0
       * - existing entries are preserved, e.g.:
       *       A = (1,2,3)                      A' = (0,1,2,3)
       *           (4,5,6)  ---add col left-->       (0,4,5,6)
       *           (7,8,9)                           (0,7,8,9)
       */
      void add_col_left()
      {
          std::vector<value_type> x(_rows * (_cols + 1));

          for (unsigned int r = 0; r < num_rows(); ++r)
          {
              for (unsigned int c = 0; c < num_cols(); ++c)
              { x[deriv()->listId_from_rowId_colId(r, c + 1, _rows, _cols + 1, deriv()->is_col_major())] = deriv()->operator()(r, c); }
          }

          ++_cols;
          _val = std::move(x);
      }

      void add_col_right()
      { resize_existing(_rows, _cols + 1); }

      void remove_col_left()
      {
          assert(num_cols() >= 2 && "matrix is too small to remove a col");

          std::vector<value_type> x(_rows * (_cols - 1));

          for (unsigned int r = 0; r < _rows; ++r)
          {
              for (unsigned int c = 1; c < _cols; ++c)
              { x[deriv()->listId_from_rowId_colId(r, c - 1, _rows, _cols - 1, deriv()->is_col_major())] = deriv()->operator()(r, c); }
          }

          --_cols;
          _val = std::move(x);
      }

      void remove_col_right()
      {
          assert(_cols >= 2 && "matrix is too small to remove a col");
          resize_existing(_rows, _cols - 1);
      }

      void add_row_top()
      {
          std::vector<value_type> x((_rows + 1) * _cols);

          for (unsigned int r = 0; r < _rows; ++r)
          {
              for (unsigned int c = 0; c < _cols; ++c)
              { x[deriv()->listId_from_rowId_colId(r + 1, c, _rows + 1, _cols, deriv()->is_col_major())] = deriv()->operator()(r, c); }
          }

          ++_rows;
          _val = std::move(x);
      }

      void add_row_bottom()
      { resize_existing(_rows + 1, _cols); }

      void remove_row_top()
      {
          assert(_rows >= 2 && "matrix is too small to remove a row");

          std::vector<value_type> x((_rows - 1) * _cols);

          for (unsigned int r = 1; r < _rows; ++r)
          {
              for (unsigned int c = 0; c < _cols; ++c)
              { x[deriv()->listId_from_rowId_colId(r - 1, c, _rows - 1, _cols, deriv()->is_col_major())] = deriv()->operator()(r, c); }
          }

          --_rows;
          _val = std::move(x);
      }

      void remove_row_bottom()
      {
          assert(_rows >= 2 && "matrix is too small to remove a row");

          resize_existing(_rows - 1, _cols);
      }
      /// @}

  }; // class MatrixMemoryDynamic
} // namespace bk::details

#endif //BK_MATRIXMEMORYDYNAMIC_H


