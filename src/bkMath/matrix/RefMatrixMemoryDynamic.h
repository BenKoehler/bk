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

#ifndef BK_REFMATRIXMEMORYDYNAMIC_H
#define BK_REFMATRIXMEMORYDYNAMIC_H

#include <algorithm>
#include <cassert>

namespace bk::details
{
  template<typename TMatrix, bool isDynamic> class RefMatrixMemoryDynamic
  { /* empty class */ };

  template<typename TMatrix> class RefMatrixMemoryDynamic<TMatrix, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
    private:
      using self_type = RefMatrixMemoryDynamic<TMatrix, true>;
    public:
      using matrix_type = TMatrix;
      using value_type = typename TMatrix::value_type;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      matrix_type& _m;
      unsigned int _rowIDBegin;
      unsigned int _rowIDEnd;
      unsigned int _colIDBegin;
      unsigned int _colIDEnd;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      RefMatrixMemoryDynamic() = delete;
      RefMatrixMemoryDynamic(const self_type&) = default;
      RefMatrixMemoryDynamic(self_type&&) noexcept = default;

      RefMatrixMemoryDynamic(matrix_type& m)
          : _m(m),
            _rowIDBegin(0),
            _rowIDEnd(m.num_rows() - 1),
            _colIDBegin(0),
            _colIDEnd(m.num_cols() - 1)
      { /* do nothing */ }

      RefMatrixMemoryDynamic(matrix_type& m, unsigned int rowIDBegin, unsigned int rowIDEnd, unsigned int colIDBegin, unsigned int colIDEnd)
          : _m(m),
            _rowIDBegin(std::min(rowIDBegin, m.num_rows() - 1)),
            _rowIDEnd(std::min(rowIDEnd, m.num_rows() - 1)),
            _colIDBegin(std::min(colIDBegin, m.num_cols() - 1)),
            _colIDEnd(std::min(colIDEnd, m.num_cols() - 1))
      {
          assert(rowIDEnd < _m.num_rows() && "rowID out of bound");
          assert(colIDEnd < _m.num_cols() && "colID out of bound");
          assert(rowIDBegin <= rowIDEnd && "invalid rowIDs");
          assert(colIDBegin <= colIDEnd && "invalid colIDs");
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~RefMatrixMemoryDynamic() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr int RowsAtCompileTime()
      { return 0; }

      [[nodiscard]] static constexpr int ColsAtCompileTime()
      { return 0; }

      [[nodiscard]] static constexpr int NumElementsAtCompileTime()
      { return 0; }

      [[nodiscard]] static constexpr MatrixAlignment AlignmentAtCompileTime()
      { return MatrixAlignment::Dynamic; }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] unsigned int num_rows() const
      { return _rowIDEnd - _rowIDBegin + 1; }

      [[nodiscard]] unsigned int num_cols() const
      { return _colIDEnd - _colIDBegin + 1; }

      [[nodiscard]] unsigned int num_elements() const
      { return num_rows() * num_cols(); }

      [[nodiscard]] unsigned int rowID_begin() const
      { return _rowIDBegin; }

      [[nodiscard]] unsigned int rowID_end() const
      { return _rowIDEnd; }

      [[nodiscard]] unsigned int colID_begin() const
      { return _colIDBegin; }

      [[nodiscard]] unsigned int colID_end() const
      { return _colIDEnd; }
      /// @}

      /// @{ -------------------------------------------------- GET ALIGNMENT
      [[nodiscard]] MatrixAlignment alignment() const
      { return _m.alignment(); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: ROWID/COLID OFFSET
    protected:
      [[nodiscard]] unsigned int _transform_list_id(unsigned int listId) const
      {
          const unsigned int rowID = TMatrix::rowId_from_listId(listId, this->num_rows(), this->num_cols(), this->alignment()) + this->rowID_begin();
          const unsigned int colID = TMatrix::colId_from_listId(listId, this->num_rows(), this->num_cols(), this->alignment()) + this->colID_begin();
          return this->_m.listId_from_rowId_colId(rowID, colID);
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR []
      [[nodiscard]] value_type& operator[](unsigned int listId)
      {
          assert(listId < num_elements() && "invalid listID");
          return _m[this->_transform_list_id(listId)];
      }

      [[nodiscard]] const value_type& operator[](unsigned int listId) const
      {
          assert(listId < num_elements() && "invalid listID");
          return _m[this->_transform_list_id(listId)];
      }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] auto begin()
      { return _m.begin(); }

      [[nodiscard]] auto begin() const
      { return _m.begin(); }

      [[nodiscard]] auto end()
      { return _m.end(); }

      [[nodiscard]] auto end() const
      { return _m.end(); }

      [[nodiscard]] auto rbegin()
      { return _m.rbegin(); }

      [[nodiscard]] auto rbegin() const
      { return _m.rbegin(); }

      [[nodiscard]] auto rend()
      { return _m.rend(); }

      [[nodiscard]] auto rend() const
      { return _m.rend(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      void set_size(unsigned int nRows, unsigned int nCols)
      { // origin remains the same in this version, just rowEnd and colEnd are adjusted
          assert(nRows < _m.num_rows() && "rowID out of bound");
          assert(nCols < _m.num_cols() && "colID out of bound");

          _rowIDEnd = std::min(_rowIDBegin + nRows - 1, _m.num_rows() - 1);
          _colIDEnd = std::min(_colIDBegin + nCols - 1, _m.num_cols() - 1);
      }

      void set_size(unsigned int rowIDBegin, unsigned int rowIDEnd, unsigned int colIDBegin, unsigned int colIDEnd)
      {
          assert(rowIDEnd < _m.num_rows() && "rowID out of bound");
          assert(colIDEnd < _m.num_cols() && "colID out of bound");
          assert(rowIDBegin <= rowIDEnd && "invalid rowIDs");
          assert(colIDBegin <= colIDEnd && "invalid colIDs");

          _rowIDBegin = std::min(rowIDBegin, _m.num_rows() - 1);
          _rowIDEnd = std::min(rowIDEnd, _m.num_rows() - 1);
          _colIDBegin = std::min(colIDBegin, _m.num_cols() - 1);
          _colIDEnd = std::min(colIDEnd, _m.num_cols() - 1);
      }

    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}
    public:
  }; // class RefMatrixMemoryDynamic
} // namespace bk::details

#endif //BK_REFMATRIXMEMORYDYNAMIC_H


