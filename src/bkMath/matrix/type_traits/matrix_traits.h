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

#ifndef BK_MATRIXTRAITS_H
#define BK_MATRIXTRAITS_H

#include <memory>
#include <type_traits>

#include <bkMath/matrix/MatrixAlignment.h>

namespace bk
{
  //------------------------------------------------------------------------------------------------------
  // is_matrix
  //------------------------------------------------------------------------------------------------------

  /*
   * This is a workaround for is_ref_matrix_v because the commented version
   * below does not work (for whatever reason...)
   */
  namespace details
  {
    /// @{ -------------------------------------------------- IS MATRIX TEMPLATE
    template<typename T> struct _is_matrix_template : std::false_type
    {
    };

    template<template<typename, int, int, MatrixAlignment> class T, typename V, int r, int c, MatrixAlignment a> struct _is_matrix_template<T<V, r, c, a>> : std::true_type
    {
    };

    template<typename T> constexpr bool _is_matrix_template_v = _is_matrix_template<T>::value;
    /// @}
  } // namespace details

  /// @{ -------------------------------------------------- IS REFERENCE MATRIX
  template<typename T, typename E = void> struct is_ref_matrix : std::false_type
  {
  };

  template<template<typename, int, int, int, int> class T, class M, int r0, int r1, int c0, int c1>
  struct is_ref_matrix<T<M, r0, r1, c0, c1>, std::enable_if_t<details::_is_matrix_template_v<M>>> : std::true_type
  {
  };

  template<typename T> constexpr bool is_ref_matrix_v = is_ref_matrix<T>::value;

  //template<typename T> class is_ref_matrix
  //{
  //    using yes = char[1];
  //    using no = char[2];
  //    template<typename C_> static yes& test(char (*)(typename C_::RefMatrix));
  //    template<typename C_> static no& test(...);
  //
  //public:
  //    static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
  //}; // class is_ref_matrix
  //
  //template<typename T> constexpr bool is_ref_matrix_v = is_ref_matrix<T>::value;
  /// @}

  /// @{ -------------------------------------------------- IS MATRIX
  namespace details
  {
    template<typename T> class is_mat
    {
        using yes = char[1];
        using no = char[2];
        template<typename C_>
        static yes& test(char (*)(typename C_::Matrix));
        template<typename C_>
        static no& test(...);

      public:
        static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
    }; // class is_mat
  } // namespace details

  // shortcut
  template<typename T> constexpr bool is_matrix_v = std::is_class_v<std::decay_t<T>> ? details::is_mat<T>::value || is_ref_matrix_v<T> : false;
  /// @}

  //------------------------------------------------------------------------------------------------------
  // class matrix_row_col_traits
  //------------------------------------------------------------------------------------------------------
  template<int TRows, int TCols> struct matrix_row_col_traits
  {
      /// @{ -------------------------------------------------- STATIC / DYNAMIC
      static constexpr bool is_static = TRows > 0 && TCols > 0;
      static constexpr bool is_dynamic = !is_static;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC
      template<int R, int C> static constexpr bool is_static_of_size = R > 0 && C > 0 && TRows == R && TCols == C;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC SQUARE
      static constexpr bool is_static_square = is_static && TRows == TCols;
      template<int N> static constexpr bool is_static_square_of_size = is_static_of_size<N, N>;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC VECTOR
      // row vector
      static constexpr bool is_static_row_vector = is_static && TRows == 1;
      template<int N> static constexpr bool is_static_row_vector_of_size = is_static_of_size<1, N>;

      // col vector
      static constexpr bool is_static_col_vector = is_static && TCols == 1;
      template<int N> static constexpr bool is_static_col_vector_of_size = is_static_of_size<N, 1>;

      // vector
      static constexpr bool is_static_vector = is_static_row_vector || is_static_col_vector;
      template<int N> static constexpr bool is_static_vector_of_size = is_static_row_vector_of_size<N> || is_static_col_vector_of_size<N>;
      /// @}

      /// @{ -------------------------------------------------- DEFAULT ALIGNMENT
      static constexpr MatrixAlignment default_alignment()
      { return is_dynamic ? MatrixAlignment::Dynamic : MatrixAlignment::ColMajor; }
      /// @}
  };

  //------------------------------------------------------------------------------------------------------
  // matrix_row_col_traits shortcuts
  //------------------------------------------------------------------------------------------------------
  /// @{ -------------------------------------------------- STATIC / DYNAMIC
  template<int TRows, int TCols> constexpr bool is_static_matrix_row_col_v = matrix_row_col_traits<TRows, TCols>::is_static;

  template<int TRows, int TCols> constexpr bool is_dynamic_matrix_row_col_v = matrix_row_col_traits<TRows, TCols>::is_dynamic;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC
  template<int TRows, int TCols, int R, int C> constexpr bool is_static_matrix_row_col_of_size_v = matrix_row_col_traits<TRows, TCols>::template is_static_of_size<R, C>;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC SQUARE
  template<int TRows, int TCols> constexpr bool is_static_square_matrix_row_col_v = matrix_row_col_traits<TRows, TCols>::is_static_square;

  template<int TRows, int TCols, int N> constexpr bool is_static_square_matrix_row_col_of_size_v = matrix_row_col_traits<TRows, TCols>::template is_static_square_of_size<N>;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC VECTOR
  // row vector
  template<int TRows, int TCols> constexpr bool is_static_row_vector_row_col_v = matrix_row_col_traits<TRows, TCols>::is_static_row_vector;

  template<int TRows, int TCols, int N> constexpr bool is_static_row_vector_row_col_of_size_v = matrix_row_col_traits<TRows, TCols>::template is_static_row_vector_of_size<N>;

  // col vector
  template<int TRows, int TCols> constexpr bool is_static_col_vector_row_col_v = matrix_row_col_traits<TRows, TCols>::is_static_col_vector;

  template<int TRows, int TCols, int N> constexpr bool is_static_col_vector_row_col_of_size_v = matrix_row_col_traits<TRows, TCols>::template is_static_col_vector_of_size<N>;

  // vector
  template<int TRows, int TCols> constexpr bool is_static_vector_row_col_v = matrix_row_col_traits<TRows, TCols>::is_static_vector;

  template<int TRows, int TCols, int N> constexpr bool is_static_vector_row_col_of_size_v = matrix_row_col_traits<TRows, TCols>::template is_static_vector_of_size<N>;
  /// @}

  //------------------------------------------------------------------------------------------------------
  // class matrix_traits for non-matrix types
  //------------------------------------------------------------------------------------------------------
  template<typename TMatrix, bool isMatrix = is_matrix_v<TMatrix>> struct matrix_traits
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using matrix_type = std::decay_t<TMatrix>;
      using value_type = void;

      /// @{ -------------------------------------------------- PARAMETERS
      static constexpr int RowsAtCompileTime = 0;
      static constexpr int ColsAtCompileTime = 0;
      /// @}

      /// @{ -------------------------------------------------- STATIC / DYNAMIC
      static constexpr bool is_static = false;
      static constexpr bool is_dynamic = false;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC
      template<int R, int C> static constexpr bool is_static_of_size = false;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC SQUARE
      static constexpr bool is_static_square = false;
      template<int N> static constexpr bool is_static_square_of_size = false;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC VECTOR
      // row vector
      static constexpr bool is_static_row_vector = false;
      template<int N> static constexpr bool is_static_row_vector_of_size = false;

      // col vector
      static constexpr bool is_static_col_vector = false;
      template<int N> static constexpr bool is_static_col_vector_of_size = false;

      // vector
      static constexpr bool is_static_vector = false;
      template<int N> static constexpr bool is_static_vector_of_size = false;
      /// @}

    private:
      using self_type = matrix_traits<TMatrix>;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr matrix_traits() = delete;
      constexpr matrix_traits(const self_type&) = delete;
      constexpr matrix_traits(self_type&&) = delete;

      constexpr matrix_traits(const TMatrix& /*m*/)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~matrix_traits() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&) = delete;
      constexpr self_type& operator=(self_type&&) noexcept = delete;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] constexpr bool is_of_size(unsigned int /*rows*/, unsigned int /*cols*/) const noexcept
      { return false; }

      /// @{ -------------------------------------------------- IS SQUARE
      [[nodiscard]] constexpr bool is_square() const noexcept
      { return false; }

      [[nodiscard]] constexpr bool is_square_of_size(unsigned int /*N*/) const noexcept
      { return false; }
      /// @}

      /// @{ -------------------------------------------------- IS VECTOR
      // row vector
      [[nodiscard]] constexpr bool is_row_vector() const noexcept
      { return false; }

      [[nodiscard]] constexpr bool is_row_vector_of_size(unsigned int /*N*/) const noexcept
      { return false; }

      // col vector
      [[nodiscard]] constexpr bool is_col_vector() const noexcept
      { return false; }

      [[nodiscard]] constexpr bool is_col_vector_of_size(unsigned int /*N*/) const noexcept
      { return false; }

      // vector
      [[nodiscard]] constexpr bool is_vector() const noexcept
      { return false; }

      [[nodiscard]] constexpr bool is_vector_of_size(unsigned int /*N*/) const noexcept
      { return false; }
      /// @}
  };

  //------------------------------------------------------------------------------------------------------
  // class matrix_traits
  //------------------------------------------------------------------------------------------------------
  template<typename TMatrix> struct matrix_traits<TMatrix, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using matrix_type = std::decay_t<TMatrix>;
      using value_type = typename matrix_type::value_type;

      /// @{ -------------------------------------------------- PARAMETERS
      static constexpr int RowsAtCompileTime = matrix_type::RowsAtCompileTime();
      static constexpr int ColsAtCompileTime = matrix_type::ColsAtCompileTime();
      /// @}

      /// @{ -------------------------------------------------- STATIC / DYNAMIC
      static constexpr bool is_static = is_static_matrix_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      static constexpr bool is_dynamic = is_dynamic_matrix_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC
      template<int R, int C> static constexpr bool is_static_of_size = (R > 0 && C > 0) && is_static_matrix_row_col_of_size_v<RowsAtCompileTime, ColsAtCompileTime, R, C>;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC SQUARE
      static constexpr bool is_static_square = is_static_square_matrix_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      template<int N> static constexpr bool is_static_square_of_size = (N > 0) && is_static_square_matrix_row_col_of_size_v<RowsAtCompileTime, ColsAtCompileTime, N>;
      /// @}

      /// @{ -------------------------------------------------- IS STATIC VECTOR
      // row vector
      static constexpr bool is_static_row_vector = is_static_row_vector_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      template<int N> static constexpr bool is_static_row_vector_of_size = is_static_row_vector_row_col_of_size_v<RowsAtCompileTime, ColsAtCompileTime, N>;

      // col vector
      static constexpr bool is_static_col_vector = is_static_col_vector_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      template<int N> static constexpr bool is_static_col_vector_of_size = is_static_col_vector_row_col_of_size_v<RowsAtCompileTime, ColsAtCompileTime, N>;

      // vector
      static constexpr bool is_static_vector = is_static_vector_row_col_v<RowsAtCompileTime, ColsAtCompileTime>;
      template<int N> static constexpr bool is_static_vector_of_size = is_static_vector_row_col_of_size_v<RowsAtCompileTime, ColsAtCompileTime, N>;
      /// @}

    private:
      using self_type = matrix_traits<TMatrix>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      const matrix_type* _matrix_ptr;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr matrix_traits() = delete;
      constexpr matrix_traits(const self_type&) = delete;
      constexpr matrix_traits(self_type&&) = delete;

      constexpr matrix_traits(const TMatrix& m)
          : _matrix_ptr(std::addressof(m))
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~matrix_traits() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type& other) = delete;
      constexpr self_type& operator=(self_type&& other) noexcept = delete;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] constexpr bool is_of_size(unsigned int rows, unsigned int cols) const
      { return _matrix_ptr->num_rows() == rows && _matrix_ptr->num_cols() == cols; }

      /// @{ -------------------------------------------------- IS SQUARE
      [[nodiscard]] constexpr bool is_square() const
      { return _matrix_ptr->num_rows() == _matrix_ptr->num_cols(); }

      [[nodiscard]] constexpr bool is_square_of_size(unsigned int N) const
      { return is_of_size(N, N); }
      /// @}

      /// @{ -------------------------------------------------- IS VECTOR
      // row vector
      [[nodiscard]] constexpr bool is_row_vector() const
      { return _matrix_ptr->num_rows() == 1; }

      [[nodiscard]] constexpr bool is_row_vector_of_size(unsigned int N) const
      { return is_row_vector() && _matrix_ptr->num_cols() == N; }

      // col vector
      [[nodiscard]] constexpr bool is_col_vector() const
      { return _matrix_ptr->num_cols() == 1; }

      [[nodiscard]] constexpr bool is_col_vector_of_size(unsigned int N) const
      { return is_col_vector() && _matrix_ptr->num_rows() == N; }

      // vector
      [[nodiscard]] constexpr bool is_vector() const
      { return is_row_vector() || is_col_vector(); }

      [[nodiscard]] constexpr bool is_vector_of_size(unsigned int N) const
      { return is_row_vector_of_size(N) || is_col_vector_of_size(N); }
      /// @}
  }; // struct matrix_traits

  //------------------------------------------------------------------------------------------------------
  // matrix_traits shortcuts
  //------------------------------------------------------------------------------------------------------
  /// @{ -------------------------------------------------- STATIC/DYNAMIC
  template<typename TMatrix> constexpr bool is_static_matrix_v = matrix_traits<TMatrix>::is_static;

  template<typename TMatrix> constexpr bool is_dynamic_matrix_v = matrix_traits<TMatrix>::is_dynamic;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC
  template<typename TMatrix, int R, int C> constexpr bool is_static_matrix_of_size_v = matrix_traits<TMatrix>::template is_static_of_size<R, C>;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC SQUARE
  template<typename TMatrix> constexpr bool is_static_square_matrix_v = matrix_traits<TMatrix>::is_static_square;

  template<typename TMatrix, int N> constexpr bool is_static_square_matrix_of_size_v = matrix_traits<TMatrix>::template is_static_square_of_size<N>;
  /// @}

  /// @{ -------------------------------------------------- IS STATIC VECTOR
  // row vector
  template<typename TMatrix> constexpr bool is_static_row_vector_v = matrix_traits<TMatrix>::is_static_row_vector;

  template<typename TMatrix, int N> constexpr bool is_static_row_vector_of_size_v = matrix_traits<TMatrix>::template is_static_row_vector_of_size<N>;

  // col vector
  template<typename TMatrix> constexpr bool is_static_col_vector_v = matrix_traits<TMatrix>::is_static_col_vector;

  template<typename TMatrix, int N> constexpr bool is_static_col_vector_of_size_v = matrix_traits<TMatrix>::template is_static_col_vector_of_size<N>;

  // vector
  template<typename TMatrix> constexpr bool is_static_vector_v = matrix_traits<TMatrix>::is_static_vector;

  template<typename TMatrix, int N> constexpr bool is_static_vector_of_size_v = matrix_traits<TMatrix>::template is_static_vector_of_size<N>;
  /// @}

  //------------------------------------------------------------------------------------------------------
  // matrix_traits for comparison of 2 matrices
  //------------------------------------------------------------------------------------------------------
  template<typename TMatrixA, typename TMatrixB, std::enable_if_t<is_matrix_v<TMatrixA>>* = nullptr, std::enable_if_t<is_matrix_v<TMatrixB>>* = nullptr> struct matrix_traits_comp
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
    private:
      using self_type = matrix_traits_comp<TMatrixA, TMatrixB>;
    public:
      using matrix_A_type = std::decay_t<TMatrixA>;
      using matrix_B_type = std::decay_t<TMatrixB>;
    private:
      using A = matrix_traits<matrix_A_type>;
      using B = matrix_traits<matrix_B_type>;
    public:

      /// @{ -------------------------------------------------- STATIC / DYNAMIC
      static constexpr bool are_both_static = A::is_static && B::is_static;
      static constexpr bool are_both_dynamic = A::is_dynamic && B::is_dynamic;
      /// @}

      /// @{ -------------------------------------------------- STATIC SIZE
      static constexpr bool static_size_matches = !are_both_static || (A::RowsAtCompileTime == B::RowsAtCompileTime && A::ColsAtCompileTime == B::ColsAtCompileTime);

      static constexpr bool static_num_elements_matches = !are_both_static || (A::RowsAtCompileTime == B::RowsAtCompileTime && A::ColsAtCompileTime == B::ColsAtCompileTime) || (A::RowsAtCompileTime == B::ColsAtCompileTime && A::ColsAtCompileTime == B::RowsAtCompileTime);
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      const matrix_A_type* _A_ptr;
      const matrix_B_type* _B_ptr;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr matrix_traits_comp() = delete;
      constexpr matrix_traits_comp(const self_type&) = delete;
      constexpr matrix_traits_comp(self_type&&) = delete;

      constexpr matrix_traits_comp(const TMatrixA& ma, const TMatrixB& mb)
          : _A_ptr(std::addressof(ma)),
            _B_ptr(std::addressof(mb))
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~matrix_traits_comp() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type& other) = delete;
      constexpr self_type& operator=(self_type&& other) noexcept = delete;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- DYNAMIC SIZE
      [[nodiscard]] constexpr bool size_matches() const
      { return _A_ptr->num_rows() == _B_ptr->num_rows() && _A_ptr->num_cols() == _B_ptr->num_cols(); }

      [[nodiscard]] constexpr bool num_elements_matches() const
      { return _A_ptr->num_elements() == _B_ptr->num_elements(); }
      /// @}
  };
} // namespace bk

#endif //BK_MATRIXTRAITS_H


