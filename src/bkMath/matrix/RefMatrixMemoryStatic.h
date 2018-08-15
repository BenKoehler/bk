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

#ifndef BK_REFMATRIXMEMORYSTATIC_H
#define BK_REFMATRIXMEMORYSTATIC_H

#include <cassert>

#include <bkMath/matrix/MatrixAlignment.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>

namespace bk::details
{
  template<typename TMatrix, int TRowIDBegin, int TRowIDEnd, int TColIDBegin, int TColIDEnd, bool isDynamic> class RefMatrixMemoryStatic
  { /* empty class */ };

  template<typename TMatrix, int TRowIDBegin, int TRowIDEnd, int TColIDBegin, int TColIDEnd> class RefMatrixMemoryStatic<TMatrix, TRowIDBegin, TRowIDEnd, TColIDBegin, TColIDEnd, true>
  {
      //==================================================================================================================================
      //===== DEFINITIONS
      //==================================================================================================================================
    private:
      using self_type = RefMatrixMemoryStatic<TMatrix, TRowIDBegin, TRowIDEnd, TColIDBegin, TColIDEnd, true>;
    public:
      using matrix_type = TMatrix;
      using value_type = typename matrix_type::value_type;

      //==================================================================================================================================
      //===== ASSERTIONS
      //==================================================================================================================================
      static_assert(bk::is_dynamic_matrix_v<matrix_type> || TRowIDBegin < matrix_type::RowsAtCompileTime(), "RefMatrixMemoryStatic: row ID out of bound");
      static_assert(bk::is_dynamic_matrix_v<matrix_type> || TColIDBegin < matrix_type::ColsAtCompileTime(), "RefMatrixMemoryStatic: col ID out of bound");
      static_assert(TRowIDBegin <= TRowIDEnd, "RefMatrixMemoryStatic: invalid row IDs");
      static_assert(TColIDBegin <= TColIDEnd, "RefMatrixMemoryStatic: invalid col IDs");

      //==================================================================================================================================
      //===== MEMBERS
      //==================================================================================================================================
    protected:
      matrix_type& _m;

      //==================================================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //==================================================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr RefMatrixMemoryStatic() = delete;
      constexpr RefMatrixMemoryStatic(const self_type&) = default;
      constexpr RefMatrixMemoryStatic(self_type&&) = default;

      constexpr RefMatrixMemoryStatic(matrix_type& m)
          : _m(m)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~RefMatrixMemoryStatic() noexcept = default;
      /// @}

      //==================================================================================================================================
      //===== GETTER
      //==================================================================================================================================
    public:
      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr int RowsAtCompileTime() noexcept
      { return TRowIDEnd - TRowIDBegin + 1; }

      [[nodiscard]] static constexpr int ColsAtCompileTime() noexcept
      { return TColIDEnd - TColIDBegin + 1; }

      [[nodiscard]] static constexpr int NumElementsAtCompileTime() noexcept
      { return RowsAtCompileTime() * ColsAtCompileTime(); }

      [[nodiscard]] static constexpr MatrixAlignment AlignmentAtCompileTime() noexcept
      { return matrix_type::AlignmentAtCompileTime(); }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] static constexpr unsigned int num_rows() noexcept
      { return static_cast<unsigned int>(RowsAtCompileTime()); }

      [[nodiscard]] static constexpr unsigned int num_cols() noexcept
      { return static_cast<unsigned int>(ColsAtCompileTime()); }

      [[nodiscard]] static constexpr unsigned int num_elements() noexcept
      { return static_cast<unsigned int>(NumElementsAtCompileTime()); }

      [[nodiscard]] static constexpr unsigned int rowID_begin() noexcept
      { return static_cast<unsigned int>(TRowIDBegin); }

      [[nodiscard]] static constexpr unsigned int rowID_end() noexcept
      { return static_cast<unsigned int>(TRowIDEnd); }

      [[nodiscard]] static constexpr unsigned int colID_begin() noexcept
      { return static_cast<unsigned int>(TColIDBegin); }

      [[nodiscard]] static constexpr unsigned int colID_end() noexcept
      { return static_cast<unsigned int>(TColIDEnd); }
      /// @}

      /// @{ -------------------------------------------------- GET ALIGNMENT
      [[nodiscard]] static constexpr MatrixAlignment alignment() noexcept
      { return AlignmentAtCompileTime(); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: ROWID/COLID OFFSET
    protected:
      [[nodiscard]] constexpr unsigned int _transform_list_id(unsigned int listId) const
      {
          const unsigned int rowID = TMatrix::rowId_from_listId(listId, this->num_rows(), this->num_cols(), this->alignment()) + this->rowID_begin();
          const unsigned int colID = TMatrix::colId_from_listId(listId, this->num_rows(), this->num_cols(), this->alignment()) + this->colID_begin();
          return this->_m.listId_from_rowId_colId(rowID, colID);
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR []
      [[nodiscard]] constexpr value_type& operator[](unsigned int listId)
      {
          assert(listId < num_elements() && "invalid listID");
          return _m[this->_transform_list_id(listId)];
      }

      [[nodiscard]] constexpr const value_type& operator[](unsigned int listId) const
      {
          assert(listId < num_elements() && "invalid listID");
          return _m[this->_transform_list_id(listId)];
      }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] constexpr auto begin() noexcept(noexcept(_m.begin()))
      { return _m.begin(); }

      [[nodiscard]] constexpr auto begin() const
      { return _m.begin(); }

      [[nodiscard]] constexpr auto end()
      { return _m.end(); }

      [[nodiscard]] constexpr auto end() const
      { return _m.end(); }

      [[nodiscard]] constexpr auto rbegin()
      { return _m.rbegin(); }

      [[nodiscard]] constexpr auto rbegin() const
      { return _m.rbegin(); }

      [[nodiscard]] constexpr auto rend()
      { return _m.rend(); }

      [[nodiscard]] constexpr auto rend() const
      { return _m.rend(); }
      /// @}

      //==================================================================================================================================
      //===== SETTER
      //==================================================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) = default;
      /// @}
    public:
  }; // class RefMatrixMemoryStatic
} // namespace bk::details

#endif //BK_REFMATRIXMEMORYSTATIC_H


