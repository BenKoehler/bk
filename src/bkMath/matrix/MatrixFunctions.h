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

#ifndef BK_MATRIXFUNCTIONS_H
#define BK_MATRIXFUNCTIONS_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <random>

#include <bkMath/functions/equals_approx.h>
#include <bkMath/matrix/MatrixAlignment.h>
#include <bkMath/matrix/eigen_wrappers/QRDecomposition.h>
#include <bkMath/matrix/eigen_wrappers/SVDecomposition.h>
#include <bkMath/matrix/type_traits/matrix_traits.h>
#include <bkMath/matrix/type_traits/common_type.h>
#include <bkMath/matrix/type_traits/signed_type.h>
#include <bkTools/random/random_ct.h>
#include <bkTools/random/random.h>
#include <bkTypeTraits/complex_traits.h>
#include <bkTypeTraits/floating_point.h>

namespace bk::details
{
  template<typename TValue, typename TDerived> class MatrixFunctions
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = MatrixFunctions<TValue, TDerived>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr MatrixFunctions() noexcept = default;
      constexpr MatrixFunctions(const self_type& other) noexcept = default;
      constexpr MatrixFunctions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~MatrixFunctions() noexcept = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- TO DERIVED
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }

      [[nodiscard]] constexpr const self_type* const_this()
      { return static_cast<const self_type*>(this); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: ASSERTIONS
      template<typename TMatrix>
      [[nodiscard]] static constexpr bool _matrix_is_same_size_static(const TMatrix&)
      { return bk::matrix_traits_comp<derived_type, TMatrix>::static_size_matches || !bk::matrix_traits_comp<derived_type, TMatrix>::are_both_static; }

      template<typename TMatrix>
      [[nodiscard]] bool _matrix_is_same_size_runtime(const TMatrix& m) const
      {
          if (!bk::matrix_traits_comp(*deriv(), m).size_matches())
          {
              #pragma omp critical
              { std::cout << " this matrix size: " << deriv()->num_rows() << " x " << deriv()->num_cols() << " ; other matrix size: " << m.num_rows() << " x " << m.num_cols() << std::endl; }

              return false;
          }

          return true;
      }
      /// @}

    public:
      /// @{ -------------------------------------------------- SIZE
      template<typename TMatrix>
      [[nodiscard]] constexpr bool has_same_size(const TMatrix& rhs) const noexcept
      { return deriv()->num_rows() == rhs.num_rows() && deriv()->num_cols() == rhs.num_cols(); }
      /// @}

      /// @{ -------------------------------------------------- ALIGNMENT
      [[nodiscard]] constexpr bool is_row_major() const noexcept
      { return deriv()->alignment() == MatrixAlignment::RowMajor; }

      [[nodiscard]] constexpr bool is_col_major() const noexcept
      { return deriv()->alignment() == MatrixAlignment::ColMajor; }

      template<typename TMatrix>
      [[nodiscard]] constexpr bool has_same_alignment(const TMatrix& rhs) const noexcept
      { return deriv()->alignment() == rhs.alignment(); }
      /// @}

      /// @{ -------------------------------------------------- ROWID/COLID <> LISTID
      //! get rowId from listId of internal data vector
      [[nodiscard]] static constexpr unsigned int rowId_from_listId(unsigned int listId, const unsigned int num_rows, const unsigned int num_cols, const bool is_colmajor) noexcept
      { return is_colmajor ? listId % num_rows : listId / num_cols; }

      [[nodiscard]] constexpr unsigned int rowId_from_listId(unsigned int listId) const
      { return rowId_from_listId(listId, deriv()->num_rows(), deriv()->num_cols(), this->is_col_major()); }

      //! get colId from listId of internal data vector
      [[nodiscard]] static constexpr unsigned int colId_from_listId(unsigned int listId, const unsigned int num_rows, const unsigned int num_cols, const bool is_colmajor) noexcept
      { return is_colmajor ? listId / num_rows : listId % num_cols; }

      [[nodiscard]] constexpr unsigned int colId_from_listId(unsigned int listId) const
      { return colId_from_listId(listId, deriv()->num_rows(), deriv()->num_cols(), this->is_col_major()); }

      //! convert rowId/colId to listId of internal data vector
      [[nodiscard]] static constexpr unsigned int listId_from_rowId_colId(const unsigned int rowId, const unsigned int colId, const unsigned int num_rows, const unsigned int num_cols, const bool is_colmajor) noexcept
      { return is_colmajor ? colId * num_rows + rowId : rowId * num_cols + colId; }

      [[nodiscard]] constexpr unsigned int listId_from_rowId_colId(unsigned int rowId, unsigned int colId) const noexcept
      { return listId_from_rowId_colId(rowId, colId, deriv()->num_rows(), deriv()->num_cols(), this->is_col_major()); }
      /// @}

      /// @{ -------------------------------------------------- GET COPY
      //! allocate same-typed matrix and copy all values
      template<typename TCopyValue = value_type>
      [[nodiscard]] constexpr auto copy() const
      { return typename derived_type::template self_template_type<TCopyValue>(*deriv()); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: COPY BLOCK
    private:
      template<typename TSubMatrix>
      constexpr void _copy_block(TSubMatrix& res, unsigned int fromRowId, unsigned int toRowId, unsigned int fromColId, unsigned int toColId) const
      {
          for (unsigned int r = fromRowId; r <= toRowId; ++r)
          {
              for (unsigned int c = fromColId; c <= toColId; ++c)
              { res(r - fromRowId, c - fromColId) = (*deriv())(r, c); }
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET SUB MATRIX
      template<unsigned int fromRowId, unsigned int toRowId, unsigned int fromColId, unsigned int toColId>
      [[nodiscard]] constexpr auto sub_matrix() const
      {
          static_assert(fromRowId <= toRowId && fromColId <= toColId, "invalid from/to ids");
          static_assert(bk::is_dynamic_matrix_v<derived_type> || (bk::is_static_matrix_v<derived_type> && toRowId < derived_type::RowsAtCompileTime() && toColId < derived_type::ColsAtCompileTime()), "invalid from/to ids");
          assert(toRowId < deriv()->num_rows() && toColId < deriv()->num_cols());

          using result_type = typename derived_type::template self_template_type<value_type, toRowId - fromRowId + 1, toColId - fromColId + 1>;
          result_type res;
          _copy_block(res, fromRowId, toRowId, fromColId, toColId);
          return res;
      }

      [[nodiscard]] constexpr auto sub_matrix(unsigned int fromRowId, unsigned int toRowId, unsigned int fromColId, unsigned int toColId) const
      {
          assert(fromRowId <= toRowId && fromColId <= toColId);
          assert(toRowId < deriv()->num_rows() && toColId < deriv()->num_cols());

          using result_type = typename derived_type::template self_template_type<value_type, -1, -1>;
          result_type res;
          res.set_size(toRowId - fromRowId + 1, toColId - fromColId + 1);
          _copy_block(res, fromRowId, toRowId, fromColId, toColId);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- GET ROW
      template<unsigned int Id>
      [[nodiscard]] constexpr auto row() const
      { return this->row(Id); }

      [[nodiscard]] constexpr auto row(unsigned int Id) const
      {
          using result_type = typename derived_type::template self_template_type<value_type, 1, derived_type::ColsAtCompileTime(), derived_type::AlignmentAtCompileTime()>;
          result_type res;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              res.set_size(1, deriv()->num_cols());
              res.set_alignment(deriv()->alignment());
          }

          this->_copy_block(res, Id, Id, 0, deriv()->num_cols() - 1);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- GET COL
      template<unsigned int Id>
      [[nodiscard]] constexpr auto col() const
      { return this->col(Id); }

      [[nodiscard]] constexpr auto col(unsigned int Id) const
      {
          using result_type = typename derived_type::template self_template_type<value_type, derived_type::RowsAtCompileTime(), 1, derived_type::AlignmentAtCompileTime()>;
          result_type res;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              res.set_size(deriv()->num_rows(), 1);
              res.set_alignment(deriv()->alignment());
          }

          this->_copy_block(res, 0, deriv()->num_rows() - 1, Id, Id);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- GET DIAGONAL
      //! Extract diagonal (x_ii) as col-vector starting from top left (0,0)
      [[nodiscard]] constexpr auto diagonal() const
      {
          using result_type = typename derived_type::template self_template_type<value_type, std::min(derived_type::RowsAtCompileTime(), derived_type::ColsAtCompileTime()), 1>;
          result_type res;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              res.set_size(std::min(deriv()->num_rows(), deriv()->num_cols()), 1);
              res.set_alignment(deriv()->alignment());
          }

          for (unsigned int i = 0; i < res.num_elements(); ++i)
          { res[i] = (*deriv())(i, i); }

          return res;
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;

      template<typename T>
      [[maybe_unused]] constexpr self_type& operator=(T&& rhs)
      {
          this->set(std::forward<T>(rhs));
          return *this;
      }

      template<typename T>
      [[maybe_unused]] constexpr self_type& operator=(std::initializer_list <T> rhs)
      {   // if this overloading is not defined, a = {1,2,3,4} invokes a constructor call
          this->set(std::move(rhs));
          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: RESIZE MATRIX TO THIS SIZE
      template<typename TMatrix>
      void constexpr _resize_if_dynamic([[maybe_unused]] TMatrix& m, [[maybe_unused]] int rows = -1, [[maybe_unused]] int cols = -1, [[maybe_unused]] MatrixAlignment alignment = MatrixAlignment::ColMajor) const
      {
          if constexpr (bk::is_dynamic_matrix_v<TMatrix>)
          {
              if (rows == -1 || cols == -1)
              {
                  m.set_size(deriv()->num_rows(), deriv()->num_cols());
                  m.set_alignment(deriv()->alignment());
              }
              else
              {
                  m.set_size(rows, cols);
                  m.set_alignment(alignment);
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: SET TO VALUE(S)
    protected:
      template<unsigned int I, typename T>
      constexpr void _set(T&& x)
      { (*deriv())[I] = std::forward<T>(x); }

      template<unsigned int I, typename T, typename... TL>
      constexpr void _set(T&& x, TL&& ... xi)
      {
          _set<I>(std::forward<T>(x));
          _set<I + 1>(std::forward<TL>(xi)...);
      }
      /// @}
    public:

      /// @{ -------------------------------------------------- SET TO VALUE(S)
      template<typename T, typename... TL>
      constexpr void set(T&& x0, TL&& ... xi)
      {
          using T_ = std::decay_t<T>;
          constexpr unsigned int N = 1 + sizeof...(xi);

          if constexpr (N > 1 && !bk::is_matrix_v<T_>)
          {
              //------------------------------------------------------------------------------------------------------
              // parameters are single values
              //------------------------------------------------------------------------------------------------------
              static_assert(bk::is_dynamic_matrix_v<derived_type> || N == derived_type::NumElementsAtCompileTime(), "invalid number of arguments");
              assert((bk::is_dynamic_matrix_v<derived_type> || N == deriv()->num_elements()) && "invalid number of arguments");

              if constexpr (bk::is_dynamic_matrix_v<derived_type>)
              { // this == dynamic matrix: resize if necessary
                  if (deriv()->num_elements() != N)
                  { deriv()->set_size(N); }
              }
              else
              { // this == static matrix: check size
                  static_assert(derived_type::NumElementsAtCompileTime() == N, "SIZE MISMATCH");
              }

              _set<0U>(std::forward<T>(x0), std::forward<TL>(xi)...);
          }
          else if constexpr (N == 1)
          {
              //------------------------------------------------------------------------------------------------------
              // parameter is matrix or constant value
              //------------------------------------------------------------------------------------------------------
              if constexpr (bk::is_matrix_v<T_>)
              { // parameter is matrix
                  if constexpr (bk::is_dynamic_matrix_v<derived_type>)
                  { // this == dynamic matrix: resize if necessary
                      if (!this->has_same_size(x0))
                      { deriv()->set_size(x0.num_rows(), x0.num_cols()); }
                  }
                  else
                  { // this == static matrix: check size
                      static_assert(_matrix_is_same_size_static(x0));
                      assert(_matrix_is_same_size_runtime(x0));
                  }

                  // copy values
                  if (this->has_same_alignment(x0))
                  {
                      for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
                      { (*deriv())[i] = x0[i]; }
                  }
                  else
                  {
                      for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
                      {
                          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                          { (*deriv())(r, c) = x0(r, c); }
                      }
                  }
              }
              else if constexpr (std::is_class_v < T_ >)
              { std::copy(x0.begin(), x0.end(), deriv()->begin()); }
              else // parameter is constant value
              { this->set_constant(std::forward<T>(x0)); }
          }
      }

      //! set: single values are given in initializer list
      void set(std::initializer_list <value_type> rhs)
      {
          assert(deriv()->num_elements() == rhs.size() && "invalid number of elements in initializer list");
          std::copy(rhs.begin(), rhs.end(), deriv()->begin());
      }
      /// @}

      /// @{ -------------------------------------------------- SET CONSTANT
      constexpr void set_constant(const value_type x)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = x; }
      }

      constexpr void set_one()
      { this->set_constant(1); }

      constexpr void set_zero()
      { this->set_constant(0); }
      /// @}

      /// @{ -------------------------------------------------- SET IDENTTY
      //! set to identity matrix (all values on diagonal are 1, rest is 0)
      constexpr void set_identity()
      {
          this->set_zero();

          for (unsigned int i = 0; i < std::min(deriv()->num_rows(), deriv()->num_cols()); ++i)
          { (*deriv())(i, i) = 1; }
      }
      /// @}

      /// @{ -------------------------------------------------- SET RANDOM
      void set_random_int(int rmin, int rmax)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = bk::random::make_int(rmin, rmax); }
      }

      void set_random_float(double rmin = 0.0, double rmax = 1.0)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = bk::random::make_double(rmin, rmax); }
      }

    private:
      template<int I, typename RandEng>
      constexpr void _set_random_ct()
      {
          if constexpr (I == 0)
          { (*deriv())[I] = RandEng::value; }
          else
          {
              (*deriv())[I] = RandEng::value;
              this->_set_random_ct<I - 1, typename RandEng::next>();
          }
      }

    public:
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
      constexpr void set_random_ct()
      { this->_set_random_ct<derived_type::RowsAtCompileTime() * derived_type::ColsAtCompileTime() - 1, bk::randi<0UL, (1UL << 31) - 1, bk::seed_from_buildtime() + seedAdd>>(); }
      /// @}

      /// @{ -------------------------------------------------- MODIFY LOWER/UPPER TRIANGULAR MATRIX
      constexpr void set_lower_triangle_constant(value_type x)
      {
          for (unsigned int c = 0; c < deriv()->num_cols() - 1; ++c)
          { // num_cols()-1 since there is no element below (end,end)
              for (unsigned int r = c + 1; r < deriv()->num_rows(); ++r)
              { (*deriv())(r, c) = x; }
          }
      }

      constexpr void set_lower_triangle_zeros()
      { this->set_lower_triangle_constant(0); }

      constexpr void set_lower_triangle_ones()
      { this->set_lower_triangle_constant(1); }

      constexpr void set_upper_triangle_constant(value_type x)
      {
          for (unsigned int c = 1; c < deriv()->num_cols(); ++c)
          { // c starts at second col (c=1) since there is no element above (0,0)
              for (unsigned int r = 0; r < c; ++r)
              { (*deriv())(r, c) = x; }
          }
      }

      constexpr void set_upper_triangle_zeros()
      { this->set_upper_triangle_constant(0); }

      constexpr void set_upper_triangle_ones()
      { this->set_upper_triangle_constant(1); }
      /// @}

      /// @{ -------------------------------------------------- MODIFY DIAGONAL
      constexpr void set_diagonal_constant(value_type x)
      {
          for (unsigned int i = 0; i < std::min(deriv()->num_rows(), deriv()->num_cols()); ++i)
          { (*deriv())(i, i) = x; }
      }

      constexpr void set_diagonal_zeros()
      { this->set_diagonal_constant(0); }

      constexpr void set_diagonal_ones()
      { this->set_diagonal_constant(1); }

      template<typename TVec>
      constexpr void set_diagonal(const TVec& v)
      {
          assert(((v.num_rows() == 1 && v.num_cols() == std::min(deriv()->num_rows(), deriv()->num_cols())) || (v.num_rows() == std::min(deriv()->num_rows(), deriv()->num_cols()) && v.num_cols() == 1)) && "set diagonal: invalid parameter (wrong size)");

          for (unsigned int i = 0; i < std::min(deriv()->num_rows(), deriv()->num_cols()); ++i)
          { (*deriv())(i, i) = v[i]; }
      }

      /// @}

      //====================================================================================================
      //===== PROPERTIES
      //====================================================================================================
      /// @{ -------------------------------------------------- IS SYMMETRIC
      [[nodiscard]] constexpr bool is_symmetric(double precision = _precision_of<double>()) const
      {
          if (!bk::matrix_traits(*deriv()).is_square())
          { return false; }

          // iterate over lower triangular matrix
          for (unsigned int c = 0; c < deriv()->num_cols() - 1; ++c)
          { // num_cols()-1 since there is no element below (end,end)
              for (unsigned int r = c + 1; r < deriv()->num_rows(); ++r)
              {
                  if (!bk::equals_approx((*deriv())(r, c), (*deriv())(c, r), precision))
                  { return false; }
              }
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- IS DIAGONAL
      //! all elements except diagonal are zero
      [[nodiscard]] constexpr bool is_diagonal(double precision = _precision_of<double>()) const
      { return bk::matrix_traits(*deriv()).is_square() && is_lower_triangular(precision) && is_upper_triangular(precision); }
      /// @}

      /// @{ -------------------------------------------------- IS TRIANGULAR
      //! check if lower triangle is zero
      [[nodiscard]] constexpr bool is_upper_triangular(double precision = _precision_of<double>()) const
      {
          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          {
              for (unsigned int r = c + 1; r < deriv()->num_rows(); ++r)
              {
                  if (!bk::equals_approx((*deriv())(r, c), 0, precision))
                  { return false; }
              }
          }

          return true;
      }

      //! check if upper triangle is zero
      [[nodiscard]] constexpr bool is_lower_triangular(double precision = _precision_of<double>()) const
      {
          for (unsigned int c = 1; c < deriv()->num_cols(); ++c)
          { // c=1 since there is no element above (0,0)
              for (unsigned int r = 0; r < c; ++r)
              {
                  if (!bk::equals_approx((*deriv())(r, c), 0, precision))
                  { return false; }
              }
          }

          return true;
      }

      [[nodiscard]] constexpr bool is_triangular(double precision = _precision_of<double>()) const
      { return is_upper_triangular(precision) || is_lower_triangular(precision); }
      /// @}

      /// @{ -------------------------------------------------- IS HESSENBERG
      //! check if lower triangle (except extra diagonal) is zero
      [[nodiscard]] constexpr bool is_upper_hessenberg(double precision = _precision_of<double>()) const
      {
          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          {
              for (unsigned int r = c + 2; r < deriv()->num_rows(); ++r)
              {
                  if (!bk::equals_approx((*deriv())(r, c), 0, precision))
                  { return false; }
              }
          }

          return true;
      }

      //! check if upper triangle (except extra diagonal) is zero
      [[nodiscard]] constexpr bool is_lower_hessenberg(double precision = _precision_of<double>()) const
      {
          for (unsigned int c = 1; c < deriv()->num_cols(); ++c)
          { // c=1 since there is no element above (0,0)
              for (unsigned int r = 0; r < c - 2; ++r)
              { // c-2 since non-zero (0,1) and (1,1) are valid
                  if (!bk::equals_approx((*deriv())(r, c), 0, precision))
                  { return false; }
              }
          }

          return true;
      }

      [[nodiscard]] constexpr bool is_hessenberg(double precision = _precision_of<double>()) const
      { return is_upper_hessenberg(precision) || is_lower_hessenberg(precision); }
      /// @}

      /// @{ -------------------------------------------------- IS ORTHOGONAL
      [[nodiscard]] constexpr bool is_orthogonal(double precision = _precision_of<double>()) const
      {
          if (!bk::matrix_traits(*deriv()).is_square())
          { return false; }

          for (unsigned int c = 0; c < deriv()->num_cols() - 1; ++c)
          { // for all but last cols
              for (unsigned int cr = c + 1; cr < deriv()->num_cols(); ++cr)
              { // for cols right of current col
                  if (!bk::equals_approx(col(c).dot(col(cr)), 0, precision) /*not perpendicular*/)
                  { return false; }
              }
          }

          return true;
      }

      [[nodiscard]] constexpr bool is_orthonormal(double precision = _precision_of<double>()) const
      {
          if (!bk::matrix_traits(*deriv()).is_square())
          { return false; }

          for (unsigned int c = 0; c < deriv()->num_cols() - 1; ++c) // for all but last col
          {
              for (unsigned int cr = c + 1; cr < deriv()->num_cols(); ++cr) // for cols right of current col
              {
                  const auto current_col = col(c);

                  if (!bk::equals_approx(current_col.norm(), 1, precision) /*length not normalized*/ || !bk::equals_approx(current_col.dot(col(cr)), 0, precision) /*not perpendicular*/)
                  { return false; }
              }
          }

          return true;
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- SWAP MATRIX
      //! exchange values with other matrix
      /*!
       *  Both matrices are intact afterwards. Swap is equivalent to an _internal function,
       *  i.e. 'this' will be changed by 'swap' 
       */
      template<typename TMatrix>
      constexpr void swap(TMatrix& other)
      {
          static_assert(_matrix_is_same_size_static(other));
          assert(_matrix_is_same_size_runtime(other));

          if (has_same_alignment(other))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              {
                  value_type temp = (*deriv())[i];
                  (*deriv())[i] = other[i];
                  other[i] = std::move(temp);
              }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  {
                      typename TMatrix::value_type temp = (*deriv())(r, c);
                      (*deriv())(r, c) = other(r, c);
                      other(r, c) = std::move(temp);
                  }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- SWAP ROWS
      [[nodiscard]] constexpr auto swap_rows(unsigned int i, unsigned int k) const&
      {
          auto res = copy();
          res.swap_rows_internal(i, k);
          return res;
      }

      [[nodiscard]] constexpr auto swap_rows(unsigned int i, unsigned int k)&&
      {
          swap_rows_internal(i, k);
          return std::move(*deriv());
      }

      constexpr void swap_rows_internal(unsigned int i, unsigned int k)
      {
          assert(i < deriv()->num_rows() && k < deriv()->num_rows() && "swap_rows_internal: invalid row ids");
          value_type temp = 0;
          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          {
              temp = (*deriv())(i, c);
              (*deriv())(i, c) = (*deriv())(k, c);
              (*deriv())(k, c) = temp;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- SWAP COLS
      [[nodiscard]] constexpr auto swap_cols(unsigned int i, unsigned int k) const&
      {
          auto res = copy();
          res.swap_cols_internal(i, k);
          return res;
      }

      [[nodiscard]] constexpr auto swap_cols(unsigned int i, unsigned int k)&&
      {
          swap_cols_internal(i, k);
          return std::move(*deriv());
      }

      constexpr void swap_cols_internal(unsigned int i, unsigned int k)
      {
          assert(i >= 0 && k >= 0 && i < deriv()->num_cols() && k < deriv()->num_cols() && "swap_cols_internal: invalid col ids");
          value_type temp = 0;
          for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
          {
              temp = (*deriv())(r, i);
              (*deriv())(r, i) = (*deriv())(r, k);
              (*deriv())(r, k) = temp;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- NORMALIZE
      //! scales the vector to length 1
      [[nodiscard]] constexpr auto normalize() const&
      {
          auto res = this->copy<bk::make_floating_point_t<value_type>>();
          res /= norm();
          return res;
      }

      [[nodiscard]] constexpr auto normalize()&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          {
              normalize_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->normalize(); }
      }

      constexpr void normalize_internal()
      {
          static_assert(bk::is_floating_point_v<value_type>, "DO NOT USE NORMALIZE_INTERNAL() WITH INTEGRAL TYPES");

          if (!bk::equals_approx(norm(), 0, _precision_of<double>()))
          { operator/=(norm()); }
      }
      /// @}

      /// @{ -------------------------------------------------- NEGATE
      //! Multiply each element by -1
      [[nodiscard]] constexpr auto negate() const&
      {
          auto res = copy<bk::make_signed_t<value_type>>();
          res.negate_internal();
          return res;
      }

      [[nodiscard]] constexpr auto negate()&&
      {
          if constexpr (bk::is_signed_v<value_type>)
          {
              negate_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->negate(); }
      }

      constexpr void negate_internal()
      {
          static_assert(bk::is_signed_v<value_type>, "DO NOT USE NEGATE_INTERNAL() WITH UNSIGNED TYPES");

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = -(*deriv())[i]; }
      }
      /// @}

      /// @{ -------------------------------------------------- TRANSPOSE
      [[nodiscard]] constexpr auto transpose() const&
      {
          if constexpr (bk::is_static_matrix_v<derived_type>)
          {
              using result_type = typename derived_type::template self_template_type<value_type, derived_type::ColsAtCompileTime(), derived_type::RowsAtCompileTime()>;
              result_type res;

              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { res(c, r) = (*deriv())(r, c); }
              }

              return res;
          }
          else // if constexpr (bk::is_dynamic_matrix_v<derived_type>)
          {
              auto res = copy();
              res.transpose_internal();
              return res;
          }
      }

      [[nodiscard]] auto transpose()&&
      {
          if constexpr (bk::is_dynamic_matrix_v<derived_type>)
          {
              transpose_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->transpose(); }
      }

      constexpr void transpose_internal()
      {
          static_assert(bk::is_static_square_matrix_v<derived_type> || bk::is_dynamic_matrix_v<derived_type>);

          if constexpr (bk::is_dynamic_matrix_v<derived_type>)
          {
              deriv()->set_size(deriv()->num_cols(), deriv()->num_rows());
              deriv()->set_alignment(is_row_major() ? MatrixAlignment::ColMajor : MatrixAlignment::RowMajor);
          }
          else if constexpr (bk::is_static_square_matrix_v<derived_type>)
          {
              for (unsigned int r = 1; r < deriv()->num_rows(); ++r)
              { // r starts at 1 since there is no need to swap (0,0)
                  for (unsigned int c = 0; c < r; ++c)
                  {
                      value_type temp = (*deriv())(r, c);
                      (*deriv())(r, c) = (*deriv())(c, r);
                      (*deriv())(c, r) = std::move(temp);
                  }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- DOT PRODUCT
      //! sum of element-wise multiplications
      template<typename TMatrix>
      [[nodiscard]] constexpr auto dot(const TMatrix& rhs) const -> bk::signed_common_type_t<value_type, typename TMatrix::value_type>
      {
          static_assert(bk::matrix_traits_comp<derived_type, TMatrix>::static_num_elements_matches || !bk::matrix_traits_comp<derived_type, TMatrix>::are_both_static);
          assert(bk::matrix_traits_comp(*deriv(), rhs).num_elements_matches());

          using result_type = bk::signed_common_type_t<value_type, typename TMatrix::value_type>;
          result_type temp = 0;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { temp += (*deriv())[i] * rhs[i]; }

          return temp;
      }

      //! alias for dot product
      template<typename TMatrix>
      [[nodiscard]] constexpr auto inner_product(const TMatrix& rhs) const
      { return dot(rhs); }
      /// @}

      /// @{ -------------------------------------------------- NORM
      //! maximum column sum where absolute of each element is used
      [[nodiscard]] constexpr value_type norm1() const
      {
          value_type n = 0;

          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          {
              value_type temp = 0;

              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  if constexpr (bk::is_signed_v<value_type>)
                  { temp += std::abs((*deriv())(r, c)); }
                  else
                  { temp += (*deriv())(r, c); }
              }

              n = std::max(n, temp);
          }

          return n;
      }

      //! maximum row sum where absolute of each element is used
      [[nodiscard]] constexpr value_type normInf() const
      {
          value_type n = 0;

          for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
          {
              value_type temp = 0;

              for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
              {
                  if constexpr (bk::is_signed_v<value_type>)
                  { temp += std::abs((*deriv())(r, c)); }
                  else
                  { temp += (*deriv())(r, c); }
              }

              n = std::max(n, temp);
          }

          return n;
      }

      //! square root of sum of all elements squared (default)
      [[nodiscard]] constexpr auto norm2() const
      { return std::sqrt(norm2_squared()); }

      [[nodiscard]] constexpr value_type norm2_squared() const
      {
          value_type n = 0;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          {
              // additional abs() is adjustment for complex value_type
              if constexpr (bk::is_signed_v<value_type>)
              { n += std::abs((*deriv())[i]) * std::abs((*deriv())[i]); }
              else
              { n += (*deriv())[i] * (*deriv())[i]; }
          }

          return n;
      }

      //! alias for norm2(_squared)
      [[nodiscard]] constexpr auto norm() const
      { return norm2(); }

      [[nodiscard]] constexpr value_type norm_squared() const
      { return norm2_squared(); }
      /// @}

      /// @{ -------------------------------------------------- NORMALIZE COLS
      //! normalize each column/row vector to length 1
      [[nodiscard]] constexpr auto normalize_cols() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.normalize_cols_internal();
          return res;
      }

      [[nodiscard]] constexpr auto normalize_cols()&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          {
              normalize_cols_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->normalize_cols(); }
      }

      constexpr void normalize_cols_internal()
      {
          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          {
              const auto n = this->col(c).norm();

              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              { (*deriv())(r, c) /= n; }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- NORMALIZE ROWS
      [[nodiscard]] constexpr auto normalize_rows() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.normalize_rows_internal();
          return res;
      }

      [[nodiscard]] constexpr auto normalize_rows()&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          {
              normalize_rows_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->normalize_rows(); }
      }

      constexpr void normalize_rows_internal()
      {
          for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
          {
              const auto n = this->row(r).norm();
              for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
              { (*deriv())(r, c) /= n; }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- RANK
      //! determines the rank of the matrix using QR decomposition (counting zero-rows in R)
      [[nodiscard]] int rank_via_qr() const
      { return qr().rank(); }

      [[nodiscard]] int rank_via_qr(double tolerance) const
      { return qr().rank(tolerance); }

      [[nodiscard]] int rank_via_svd() const
      { return svd().rank(); }

      [[nodiscard]] int rank_via_svd(double tolerance) const
      { return svd().rank(tolerance); }

      [[nodiscard]] int rank() const
      { return rank_via_svd(); }

      [[nodiscard]] int rank(double tolerance) const
      { return rank_via_svd(tolerance); }
      /// @}

      /// @{ -------------------------------------------------- QR DECOMPOSITION
      /*!
       * Decompose the matrix so that A = Q*R, where Q is orthogonal and R is an upper triangular matrix.
       * Can be used to solve linear equation systems.
       * 
       * \return Object that contains Q/R and allows to solve regarding rhs vector 
       */
      [[nodiscard]] auto qr() const
      { return QRDecomposition<derived_type>(*deriv()); }
      /// @}

      /// @{ -------------------------------------------------- SV DECOMPOSITION
      /*!
       *  Decompose the matrix so that A[r,c] = U[r,r] * S[r,c] * V[c,c].transpose().
       *  
       *  U are the eigenvectors of A*A.transpose() and V are the eigenvectors of A.transpose()*A.
       *  Both have the same eigenvalues. S contains the sqrt of these eigenvalues in descending order
       *  so that the largest eigenvalues is at (0,0)
       *  
       *  https://www.ling.ohio-state.edu/~kbaker/pubs/Singular_TValueDecomposition_Tutorial.pdf 
       */
      [[nodiscard]] auto svd(bool computeFullU = false, bool computeFullV = false) const
      { return SVDecomposition<derived_type>(*deriv(), computeFullU, computeFullV); }
      /// @}

      /// @{ -------------------------------------------------- PSEUDO INVERSE
      //! pseudo inverse via svd
      [[nodiscard]] auto pseudo_inverse(bk::make_floating_point_t<value_type> precision = _precision_of<bk::make_floating_point_t<value_type>>()) const
      { return svd(true, true).pseudo_inverse(precision); }
      /// @}

      /// @{ -------------------------------------------------- CONDITION
      //! condition number via svd
      /*!
       * Calculates the ratio of the largest and the smallest (non-zero) singular value.
       * Lower conditions (min = 0) are better
       */
      auto condition(bk::make_floating_point_t<value_type> precision = _precision_of<bk::make_floating_point_t<value_type>>()) const
      { return svd().condition(precision); }
      /// @}

      /// @{ -------------------------------------------------- INTERPOLATE LINEAR
      //! (1-alpha)*this + alpha*rhs
      /*!
       * The weight should be between 0 and 1. However, other values
       * are also excepted which yields extrapolation
       */
      template<typename TMatrix>
      constexpr void interpolate_linear_internal(const TMatrix& rhs, double alpha)
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              {
                  (*deriv())[i] *= 1.0 - alpha;
                  (*deriv())[i] += alpha * rhs[i];
              }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  {
                      (*deriv())(r, c) *= 1.0 - alpha;
                      (*deriv())(r, c) += alpha * rhs(r, c);
                  }
              }
          }
      }

      template<typename TMatrix>
      [[nodiscard]] constexpr auto interpolate_linear(const TMatrix& rhs, double alpha) const&
      {
          auto res = this->copy<bk::make_floating_point_t<value_type>>();
          res.interpolate_linear_internal(rhs, alpha);
          return res;
      }

      template<typename TMatrix>
      [[nodiscard]] constexpr auto interpolate_linear(const TMatrix& rhs, double alpha)&&
      {
          if constexpr (bk::is_floating_point_v<value_type>)
          {
              interpolate_linear_internal(rhs, alpha);
              return std::move(*deriv());
          }
          else
          { return const_this()->interpolate_linear(rhs, alpha); }
      }
      /// @}

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      /// @{ -------------------------------------------------- EQUALITY
      template<typename TMatrix>
      [[nodiscard]] constexpr bool operator==(const TMatrix& other) const
      {
          static_assert(bk::is_matrix_v<TMatrix>);

          const unsigned int r = deriv()->num_rows();
          const unsigned int c = deriv()->num_cols();

          if (r != other.num_rows() || c != other.num_cols())
          { return false; }

          for (unsigned int ri = 0; ri < r; ++ri)
          {
              for (unsigned int ci = 0; ci < c; ++ci)
              {
                  if ((*deriv())(ri, ci) != other(ri, ci))
                  { return false; }
              }
          }

          return true;
      }

      template<typename TMatrix>
      [[nodiscard]] constexpr bool operator!=(const TMatrix& other) const
      { return !operator==(other); }
      /// @}

    private:
      /// @{ -------------------------------------------------- HELPER: GENERIC COMPARISON
      template<typename TScalar, typename TRes, typename TCompareFunction>
      constexpr void _compare_to_scalar(const TScalar& rhs, TRes& res, TCompareFunction cmp) const
      {
          using common_value_type = bk::signed_common_type_t<value_type, TScalar>;
          using res_value_type = typename std::decay_t<TRes>::value_type;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { res[i] = static_cast<res_value_type>(cmp(static_cast<common_value_type>((*deriv())[i]), static_cast<common_value_type>(rhs))); }
      }

      template<typename TMatrix, typename TRes, typename TCompareFunction>
      constexpr void _compare_to_matrix(const TMatrix& rhs, TRes& res, TCompareFunction cmp) const
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          using common_value_type = bk::signed_common_type_t<value_type, typename std::decay_t<TMatrix>::value_type>;
          using res_value_type = typename std::decay_t<TRes>::value_type;

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { res[i] = static_cast<res_value_type>(cmp(static_cast<common_value_type>((*deriv())[i]), static_cast<common_value_type>(rhs[i]))); }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { res(r, c) = static_cast<res_value_type>(cmp(static_cast<common_value_type>((*deriv())(r, c)), static_cast<common_value_type>(rhs(r, c)))); }
              }
          }
      }
      /// @}

    public:
      /// @{ -------------------------------------------------- EQUAL CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_equal_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::equal_to<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::equal_to<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- NOTM EQUAL CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_not_equal_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::not_equal_to<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::not_equal_to<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- LESSER CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_lesser_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::less<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::less<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- LESSER EQUAL CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_lesser_equal_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::less_equal<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::less_equal<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- GREATER CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_greater_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::greater<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::greater<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- GREATER EQUAL CWISE
      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto is_greater_equal_cwise(const TMatrixOrScalar& rhs) const
      {
          using result_type = typename derived_type::template self_template_type<bool>;
          result_type res;
          _resize_if_dynamic(res);

          if constexpr (std::is_arithmetic_v < TMatrixOrScalar > || bk::is_complex_v<TMatrixOrScalar>)
          { _compare_to_scalar(rhs, res, std::greater_equal<value_type>()); }
          else if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          { _compare_to_matrix(rhs, res, std::greater_equal<value_type>()); }
          else
          { res.set_constant(false); }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- MIN CWISE
      template<typename TMatrixOrScalar>
      constexpr void min_cwise_internal(const TMatrixOrScalar& rhs)
      {
          if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          {
              assert(_matrix_is_same_size_runtime(rhs));

              if (has_same_alignment(rhs))
              {
                  for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
                  { (*deriv())[i] = std::min((*deriv())[i], static_cast<value_type>(rhs[i])); }
              }
              else
              {
                  for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
                  {
                      for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                      { (*deriv())(r, c) = std::min((*deriv())(r, c), static_cast<value_type>(rhs(r, c))); }
                  }
              }
          }
          else
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] = std::min((*deriv())[i], static_cast<value_type>(rhs)); }
          }
      }

      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto min_cwise(const TMatrixOrScalar& rhs) const
      {
          using T = std::conditional_t<bk::is_matrix_v<TMatrixOrScalar>, typename TMatrixOrScalar::value_type, TMatrixOrScalar>;
          auto res = copy<bk::signed_common_type_t<value_type, T>>();
          res.min_cwise_internal(rhs);
          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- MAX CWISE
      template<typename TMatrixOrScalar>
      constexpr void max_cwise_internal(const TMatrixOrScalar& rhs)
      {
          if constexpr (bk::is_matrix_v<TMatrixOrScalar>)
          {
              assert(_matrix_is_same_size_runtime(rhs));

              if (has_same_alignment(rhs))
              {
                  for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
                  { (*deriv())[i] = std::max((*deriv())[i], static_cast<value_type>(rhs[i])); }
              }
              else
              {
                  for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
                  {
                      for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                      { (*deriv())(r, c) = std::max((*deriv())(r, c), static_cast<value_type>(rhs(r, c))); }
                  }
              }
          }
          else
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] = std::max((*deriv())[i], static_cast<value_type>(rhs)); }
          }
      }

      template<typename TMatrixOrScalar>
      [[nodiscard]] constexpr auto max_cwise(const TMatrixOrScalar& rhs) const
      {
          using T = std::conditional_t<bk::is_matrix_v<TMatrixOrScalar>, typename TMatrixOrScalar::value_type, TMatrixOrScalar>;
          auto res = copy<bk::signed_common_type_t<value_type, T>>();
          res.max_cwise_internal(rhs);
          return res;
      }
      /// @}

      //====================================================================================================
      //===== MATH OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- ADD SCALAR OR COMPLEX
      template<typename T>
      constexpr void add_scalar_cwise_internal(const T& rhs)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] += rhs; }
      }
      /// @}

      /// @{ -------------------------------------------------- ADD MATRIX
      template<typename TMatrix>
      constexpr void add_matrix_cwise_internal(const TMatrix& rhs)
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] += rhs[i]; }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { (*deriv())(r, c) += rhs(r, c); }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR +
      template<typename T>
      [[nodiscard]] constexpr auto operator+(T&& rhs) const&
      {
          using T_ = std::decay_t<T>;

          if constexpr (bk::is_matrix_v<T_>)
          {
              if constexpr (!std::is_lvalue_reference_v < T > && !bk::is_ref_matrix_v<T_> && !std::is_const_v < T > && bk::is_signed_common_type_v<typename T_::value_type, value_type>)
              {
                  /*
                   * const this& + Matrix&& (non-const non-ref common_t) -> rhs&&
                   */
                  rhs.add_matrix_cwise_internal(*deriv());
                  return rhs;
              }
              else
              {
                  using common_value_type = bk::signed_common_type_t<value_type, typename T_::value_type>;
                  auto res = copy<common_value_type>();
                  res.add_matrix_cwise_internal(rhs);
                  return res;
              }
          }
          else
          {
              using common_value_type = bk::signed_common_type_t<value_type, T_>;
              auto res = copy<common_value_type>();
              res.add_scalar_cwise_internal(rhs);
              return res;
          }
      }

      template<typename T>
      [[nodiscard]] constexpr auto operator+(T&& rhs)&&
      {
          using T_ = std::decay_t<T>;

          if constexpr (bk::is_matrix_v<T_>)
          {
              if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_signed_common_type_v<value_type, typename T_::value_type>)
              {
                  /*
                   * this&& (non-const non-ref common_t) + Matrix -> this&&
                   */
                  add_matrix_cwise_internal(rhs);
                  return std::move(*deriv());
              }
          }
          else if constexpr (bk::is_signed_common_type_v<value_type, T_>)
          {
              /*
               * this&& (non-const non-ref common_t) + (scalar or complex) -> this&&
               */
              add_scalar_cwise_internal(rhs);
              return std::move(*deriv());
          }

          return const_this()->operator+(std::forward<T>(rhs));
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR +=
      template<typename T>
      constexpr void operator+=(T&& rhs)
      {
          if constexpr (bk::is_matrix_v<std::decay_t < T>>)
          { add_matrix_cwise_internal(rhs); }
          else
          { add_scalar_cwise_internal(rhs); }
      }
      /// @}

      /// @{ -------------------------------------------------- SUBTRACT SCALAR OR COMPLEX
      template<typename T>
      constexpr void subtract_scalar_internal(const T& rhs)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] -= rhs; }
      }
      /// @}

      /// @{ -------------------------------------------------- SUBTRACT MATRIX
      template<typename TMatrix>
      constexpr void subtract_matrix_internal(const TMatrix& rhs)
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] -= rhs[i]; }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { (*deriv())(r, c) -= rhs(r, c); }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR -
      [[nodiscard]] constexpr auto operator-() const&
      { return negate(); }

      [[nodiscard]] constexpr auto operator-()&&
      {
          if constexpr (bk::is_signed_common_type_v<value_type, value_type, true /*enforceSign*/>)
          {
              negate_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->operator-(); }
      }

      template<typename T>
      [[nodiscard]] constexpr auto operator-(T&& rhs) const&
      {
          using T_ = std::decay_t<T>;

          if constexpr (bk::is_matrix_v<T_>)
          {
              if constexpr (!std::is_lvalue_reference_v < T > && !bk::is_ref_matrix_v<T> && !std::is_const_v < T > && bk::is_signed_common_type_v<typename T_::value_type, value_type, true /*enforceSign*/>)
              {
                  /*
                   * const this& - Matrix&& (non-const non-ref common_t) -> rhs&&
                   */
                  rhs.negate_internal();
                  rhs.add_matrix_cwise_internal(*deriv());
                  return rhs;
              }
              else
              {
                  using common_value_type = bk::signed_common_type_t<value_type, typename T_::value_type, true /*enforceSign*/>;
                  auto res = copy<common_value_type>();
                  res.subtract_matrix_internal(rhs);
                  return res;
              }
          }
          else
          {
              using common_value_type = bk::signed_common_type_t<value_type, T_, true /*enforceSign*/>;
              auto res = copy<common_value_type>();
              res.subtract_scalar_internal(rhs);
              return res;
          }
      }

      template<typename T>
      [[nodiscard]] constexpr auto operator-(T&& rhs)&&
      {
          using T_ = std::decay_t<T>;

          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              if constexpr (bk::is_matrix_v<T_> && bk::is_signed_common_type_v<value_type, typename T_::value_type, true /*enforceSign*/>)
              {
                  /*
                   * this&& (non-const non-ref common_t) - Matrix -> this&&
                   */
                  subtract_matrix_internal(std::forward<T>(rhs));
                  return std::move(*deriv());
              }
              else if constexpr (bk::is_signed_common_type_v<value_type, T_, true /*enforceSign*/>)
              {
                  /*
                   * this&& (non-const non-ref common_t) - (scalar or complex) -> this&&
                   */
                  subtract_scalar_internal(std::forward<T>(rhs));
                  return std::move(*deriv());
              }
          }

          return const_this()->operator-(std::forward<T>(rhs));
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR -=
      template<typename T>
      constexpr void operator-=(const T& rhs)
      {
          if constexpr (bk::is_matrix_v<T>)
          { subtract_matrix_internal(rhs); }
          else
          { subtract_scalar_internal(rhs); }
      }
      /// @}

      /// @{ -------------------------------------------------- MULTIPLY SCALAR OR COMPLEX
      template<typename T>
      constexpr void mult_scalar_internal(const T& rhs)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] *= rhs; }
      }
      /// @}

      /// @{ -------------------------------------------------- MULTIPLY MATRIX
    private:
      template<typename TMatrix>
      [[nodiscard]] constexpr auto _mult_matrix(const TMatrix& rhs) const
      {
          static_assert(bk::is_dynamic_matrix_v<derived_type> || bk::is_dynamic_matrix_v<TMatrix> || (derived_type::ColsAtCompileTime() == TMatrix::RowsAtCompileTime()), "invalid matrix sizes for multiplication");

          using common_value_type = bk::signed_common_type_t<value_type, typename TMatrix::value_type>;
          using result_type = typename derived_type::template self_template_type<common_value_type, derived_type::RowsAtCompileTime(), TMatrix::ColsAtCompileTime()>;
          result_type res;
          _resize_if_dynamic(res, deriv()->num_rows(), rhs.num_cols());

          res.set_zero();

          for (unsigned int c = 0; c < res.num_cols(); ++c)
          {
              for (unsigned int r = 0; r < res.num_rows(); ++r)
              {
                  for (unsigned int i = 0; i < deriv()->num_cols(); ++i)
                  { res(r, c) += (*deriv())(r, i) * rhs(i, c); }
              }
          }

          return res;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR *
      template<typename T>
      [[nodiscard]] constexpr auto operator*(const T& rhs) const&
      {
          if constexpr (bk::is_matrix_v<T>)
          { return _mult_matrix(rhs); }
          else
          {
              using common_value_type = bk::signed_common_type_t<value_type, T>;
              auto res = copy<common_value_type>();
              res.mult_scalar_internal(rhs);
              return res;
          }
      }

      template<typename T>
      [[nodiscard]] constexpr auto operator*(const T& rhs)&&
      {
          if constexpr (!bk::is_matrix_v<T> && !bk::is_ref_matrix_v<derived_type> && bk::is_signed_common_type_v<value_type, T>)
          {
              mult_scalar_internal(rhs);
              return std::move(*deriv());
          }
          else
          { return const_this()->operator*(rhs); }
      }
      /// @}

      /// @{ -------------------------------------------------- MULTIPLY MATRIX CWISE
      template<typename T>
      constexpr void mult_matrix_cwise_internal(const T& rhs)
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] *= rhs[i]; }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { (*deriv())(r, c) *= rhs(r, c); }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- MULTIPLY SCALAR CWISE
      constexpr void mult_scalar_cwise_internal(value_type rhs)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] *= rhs; }
      }
      /// @}

      /// @{ -------------------------------------------------- MULTIPLY CWISE
      template<typename T>
      [[nodiscard]] constexpr auto mult_cwise(T&& rhs) const&
      {
          using T_ = std::decay_t<T>;

          if constexpr (bk::is_matrix_v<T_>)
          {
              if constexpr (!std::is_lvalue_reference_v < T > && !bk::is_ref_matrix_v<T_> && !std::is_const_v < T > && bk::is_signed_common_type_v<typename T_::value_type, value_type>)
              {
                  /*
                   * const this& .* Matrix&& (non-const non-ref common_t) -> rhs&&
                   */
                  rhs.mult_matrix_cwise_internal(*deriv());
                  return rhs;
              }
              else
              {
                  using common_value_type = bk::signed_common_type_t<value_type, typename T_::value_type>;
                  auto res = copy<common_value_type>();
                  res.mult_matrix_cwise_internal(rhs);
                  return res;
              }
          }
          else
          {
              using common_value_type = bk::signed_common_type_t<value_type, T_>;
              auto res = copy<common_value_type>();
              res.mult_scalar_cwise_internal(rhs);
              return res;
          }
      }

      template<typename T>
      [[nodiscard]] constexpr auto mult_cwise(T&& rhs)&&
      {
          using T_ = std::decay_t<T>;

          if constexpr (bk::is_matrix_v<T_> && !bk::is_ref_matrix_v<derived_type> && is_common_type_v<value_type, typename T_::value_type>)
          {
              /*
               * this&& (non-const non-ref common_t) .* Matrix -> this&&
               */
              mult_matrix_cwise_internal(rhs);
              return std::move(*deriv());
          }
          else
          { return const_this()->mult_cwise(std::forward<T>(rhs)); }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR *=
      constexpr void operator*=(value_type rhs)
      { mult_scalar_internal(rhs); }
      /// @}

      /// @{ -------------------------------------------------- DIVIDE CWISE BY MATRIX
      template<typename T>
      constexpr void div_matrix_cwise_internal(const T& rhs)
      {
          static_assert(_matrix_is_same_size_static(rhs));
          assert(_matrix_is_same_size_runtime(rhs));

          if (has_same_alignment(rhs))
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              { (*deriv())[i] /= rhs[i]; }
          }
          else
          {
              for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
                  { (*deriv())(r, c) /= rhs(r, c); }
              }
          }
      }
      /// @}

      /// @{ -------------------------------------------------- DIVIDE CWISE BY SCALAR OR COMPLEX
      constexpr void div_scalar_cwise_internal(value_type rhs)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] /= rhs; }
      }
      /// @}

      /// @{ -------------------------------------------------- DIV CWISE
      template<typename T>
      [[nodiscard]] constexpr auto div_cwise(const T& rhs) const&
      {
          using common_value_type = std::conditional_t<bk::is_matrix_v<T>, signed_common_float_t<value_type, typename T::value_type>, signed_common_float_t<value_type, T>>;
          auto res = copy<common_value_type>();

          if constexpr (bk::is_matrix_v<T>)
          { res.div_matrix_cwise_internal(rhs); }
          else
          { res.div_scalar_cwise_internal(rhs); }

          return res;
      }

      template<typename T>
      [[nodiscard]] constexpr auto div_cwise(const T& rhs)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              if constexpr (bk::is_matrix_v<T>&& is_signed_common_float_v<value_type, typename T::value_type>)
              {
                  /*
                   * this&& (non-const non-ref floating_point) ./ Matrix -> this&&
                   */
                  div_matrix_cwise_internal(rhs);
                  return std::move(*deriv());
              }
              else if constexpr (is_signed_common_float_v < value_type, value_type >)
              {
                  /*
                   * this&& (non-const non-ref floating_point) ./ scalar -> this&&
                   */
                  div_scalar_cwise_internal(rhs);
                  return std::move(*deriv());
              }
          }
          else
          { return const_this()->div_cwise(rhs); }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR /
      [[nodiscard]] constexpr auto operator/(value_type rhs) const&
      {
          using common_value_type = bk::make_floating_point_t<value_type>;
          auto res = copy<common_value_type>();
          res.div_scalar_cwise_internal(rhs);
          return res;
      }

      [[nodiscard]] constexpr auto operator/(value_type rhs)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              div_scalar_cwise_internal(rhs);
              return std::move(*deriv());
          }
          else
          { return const_this()->operator/(rhs); }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR /=
      constexpr void operator/=(value_type rhs)
      { div_scalar_cwise_internal(rhs); }
      /// @}

      //====================================================================================================
      //===== MISC MATH FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- PRODUCT
      //! product of all elements
      constexpr value_type product() const
      {
          value_type s = 1;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { s *= (*deriv())[i]; }

          return s;
      }
      /// @}

      /// @{ -------------------------------------------------- SUM
      //!  sum of all elements
      constexpr value_type sum() const
      {
          value_type s = 0;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { s += (*deriv())[i]; }

          return s;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: SUM OF COLS
    private:
      constexpr value_type _sum_of_col(unsigned int colId) const
      {
          value_type s = 0;

          for (unsigned int r = 0; r < deriv()->num_rows(); ++r)
          { s += (*deriv())(r, colId); }

          return s;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SUM OF CUSTOM COLS
      //!  Calculates the sum of all elements in the specified columns.
      /*!
       * - duplicate colIds are not checked
       */
      template<typename... TUInts>
      constexpr value_type sum_of_cols(TUInts... colIds) const
      { return (_sum_of_col(colIds) + ...); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: SUM OF ROWS
    private:
      constexpr value_type _sum_of_row(unsigned int rowId) const
      {
          value_type s = 0;

          for (unsigned int c = 0; c < deriv()->num_cols(); ++c)
          { s += (*deriv())(rowId, c); }

          return s;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SUM OF CUSTOM ROWS
      //!  Calculates the sum of all elements in the specified rows.
      /*!
       * - duplicate rowIds are not checked
       */
      template<typename... TUInts>
      constexpr value_type sum_of_rows(TUInts... rowIds) const
      { return (_sum_of_row(rowIds) + ...); }
      /// @}

      /// @{ -------------------------------------------------- MEAN
      //! mean / average of all elements
      [[nodiscard]] constexpr auto mean() const
      {
          using float_type = bk::make_floating_point_t<value_type>;
          return static_cast<float_type>(sum()) / static_cast<float_type>(deriv()->num_elements());
      }
      /// @}

      /// @{ -------------------------------------------------- VARIANCE
      [[nodiscard]] constexpr auto variance() const
      {
          const auto m = mean();
          using float_type = std::decay_t<decltype(m)>;

          if constexpr (bk::is_floating_point_v<value_type>)
          {
              float_type s = 0;

              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              {
                  const float_type diff = static_cast<float_type>((*deriv())[i]) - m;
                  s += diff * diff;
              }

              return s / static_cast<float_type>(deriv()->num_elements());
          }
      }
      /// @}

      /// @{ -------------------------------------------------- STDEV
      [[nodiscard]] constexpr auto stdev() const
      { return std::sqrt(variance()); }
      /// @}

      /// @{ -------------------------------------------------- minimum element
      [[nodiscard]] constexpr auto min_element() const
      { return std::min_element(deriv()->begin(), deriv()->end()); }
      /// @}

      /// @{ -------------------------------------------------- MAXIMUM ELEMENT
      [[nodiscard]] constexpr auto max_element() const
      { return std::max_element(deriv()->begin(), deriv()->end()); }
      /// @}

      /// @{ -------------------------------------------------- ABS CWISE
      constexpr void abs_cwise_internal()
      {
          if constexpr (bk::is_signed_v<value_type>)
          {
              for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
              {
                  if constexpr (bk::is_signed_v<value_type>)
                  { (*deriv())[i] = std::abs((*deriv())[i]); }
                  else
                  { (*deriv())[i] = (*deriv())[i]; }
              }
          }
      }

      [[nodiscard]] constexpr auto abs_cwise() const&
      {
          auto res = copy();
          res.abs_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto abs_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              abs_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->abs_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- CBRT CWISE
      constexpr void cbrt_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::cbrt((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto cbrt_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.cbrt_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto cbrt_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              cbrt_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->cbrt_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- EXP CWISE
      constexpr void exp_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::exp((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto exp_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.exp_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto exp_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              exp_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->exp_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- LOGI CWISE
      constexpr void logi_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::log((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto logi_cwise() const&
      {
          auto res = copy<std::conditional_t < std::is_integral_v < value_type>, value_type,
          int >> ();
          res.logi_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto logi_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && std::is_integral_v < value_type >)
          {
              logi_cwise_internal();
              return std::move(*deriv());
          }
          { return const_this()->logi_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- MOD CWISE
      template<typename T>
      constexpr void mod_cwise_internal(const T& x)
      {
          static_assert(std::is_integral_v < value_type > , "mod is for integral values types only");

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = (*deriv())[i] % x; }
      }

      template<typename T>
      [[nodiscard]] constexpr auto mod_cwise(const T& x) const&
      {
          auto res = copy();
          res.mod_cwise_internal(x);
          return res;
      }

      template<typename T>
      [[nodiscard]] constexpr auto mod_cwise(const T& x)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              mod_cwise_internal(x);
              return std::move(*deriv());
          }
          { return const_this()->mod_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- POW CWISE
      constexpr void pow_cwise_internal(double exponent)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::pow((*deriv())[i], exponent); }
      }

      [[nodiscard]] constexpr auto pow_cwise(double exponent) const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.pow_cwise_internal(exponent);
          return res;
      }

      [[nodiscard]] constexpr auto pow_cwise(double exponent)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              pow_cwise_internal(exponent);
              return std::move(*deriv());
          }
          else
          { return const_this()->pow_cwise(exponent); }
      }
      /// @}

      /// @{ -------------------------------------------------- POWI CWISE
      constexpr void powi_cwise_internal(int exponent)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::pow((*deriv())[i], exponent); }
      }

      [[nodiscard]] constexpr auto powi_cwise(int exponent) const&
      {
          auto res = copy();
          res.powi_cwise_internal(exponent);
          return res;
      }

      [[nodiscard]] constexpr auto powi_cwise(int exponent)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              powi_cwise_internal(exponent);
              return std::move(*deriv());
          }
          else
          { return const_this()->powi_cwise(exponent); }
      }
      /// @}

      /// @{ -------------------------------------------------- SIGN CWISE
      constexpr void sign_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = static_cast<value_type>(!std::signbit((*deriv())[i])); }
      }

      [[nodiscard]] constexpr auto sign_cwise() const&
      {
          auto res = copy<std::conditional_t < bk::is_signed_v<value_type>, value_type, int>>
          ();
          res.sign_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sign_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_signed_v<value_type>)
          {
              sign_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->sign_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- SQR CWISE
      constexpr void sqr_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = (*deriv())[i] * (*deriv())[i]; }
      }

      [[nodiscard]] constexpr auto sqr_cwise() const&
      {
          auto res = copy();
          res.sqr_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sqr_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              sqr_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->sqr_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- SQRT CWISE
      constexpr void sqrt_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::sqrt((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto sqrt_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.sqrt_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sqrt_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              sqrt_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->sqrt_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- SIN CWISE
      constexpr void sin_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::sin((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto sin_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.sin_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sin_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              sin_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->sin_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- ASIN CWISE
      constexpr void asin_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::asin((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto asin_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.asin_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto asin_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              asin_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->asin_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- COS CWISE
      constexpr void cos_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::cos((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto cos_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.cos_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto cos_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              cos_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->cos_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- ACOS CWISE
      constexpr void acos_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::acos((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto acos_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.acos_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto acos_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              acos_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->acos_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- TAN CWISE
      constexpr void tan_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::tan((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto tan_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.tan_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto tan_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              tan_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->tan_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- ATAN CWISE
      constexpr void atan_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::atan((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto atan_cwise() const&
      {
          auto res = copy<bk::make_floating_point_t<value_type>>();
          res.atan_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto atan_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type> && bk::is_floating_point_v<value_type>)
          {
              atan_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->atan_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- CEIL CWISE
      constexpr void ceil_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::ceil((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto ceil_cwise() const&
      {
          auto res = copy();
          res.ceil_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto ceil_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              ceil_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->ceil_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- FLOOR CWISE
      constexpr void floor_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::floor((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto floor_cwise() const&
      {
          auto res = copy();
          res.floor_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto floor_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              floor_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->floor_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- ROUND CWISE
      constexpr void round_cwise_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::round((*deriv())[i]); }
      }

      [[nodiscard]] constexpr auto round_cwise() const&
      {
          auto res = copy();
          res.round_cwise_internal();
          return res;
      }

      [[nodiscard]] constexpr auto round_cwise()&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              round_cwise_internal();
              return std::move(*deriv());
          }
          else
          { return const_this()->round_cwise(); }
      }
      /// @}

      /// @{ -------------------------------------------------- CLAMP CWISE
      constexpr void clamp_cwise_internal(value_type xmin, value_type xmax)
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { (*deriv())[i] = std::clamp((*deriv())[i], xmin, xmax); }
      }

      [[nodiscard]] constexpr auto clamp_cwise(value_type xmin, value_type xmax) const&
      {
          auto res = copy();
          res.clamp_cwise_internal(xmin, xmax);
          return res;
      }

      [[nodiscard]] constexpr auto clamp_cwise(value_type xmin, value_type xmax)&&
      {
          if constexpr (!bk::is_ref_matrix_v<derived_type>)
          {
              clamp_cwise_internal(xmin, xmax);
              return std::move(*deriv());
          }
          else
          { return const_this()->clamp_cwise(xmin, xmax); }
      }
      /// @}

      //====================================================================================================
      //===== OTHER
      //====================================================================================================
      /// @{ -------------------------------------------------- SORT ASCENDING
      //! sort values in ascending or descending order
      constexpr void sort_ascending_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          {
              const value_type temp = (*deriv())[i];
              int j = i - 1;

              while (j >= 0 && (*deriv())[j] > temp)
              {
                  (*deriv())[j + 1] = (*deriv())[j];
                  --j;
              }

              (*deriv())[j + 1] = temp;
          }
      }

      [[nodiscard]] constexpr auto sort_ascending() const&
      {
          auto res = copy();
          res.sort_ascending_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sort_ascending()&&
      {
          sort_ascending_internal();
          return std::move(*deriv());
      }
      /// @}

      /// @{ -------------------------------------------------- SORT DESCENDING
      constexpr void sort_descending_internal()
      {
          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          {
              const value_type temp = (*deriv())[i];
              int j = i - 1;

              while (j >= 0 && (*deriv())[j] < temp)
              {
                  (*deriv())[j + 1] = (*deriv())[j];
                  --j;
              }

              (*deriv())[j + 1] = temp;
          }
      }

      [[nodiscard]] constexpr auto sort_descending() const&
      {
          auto res = copy();
          res.sort_descending_internal();
          return res;
      }

      [[nodiscard]] constexpr auto sort_descending()&&
      {
          sort_descending_internal();
          return std::move(*deriv());
      }
      /// @}
  }; // class MatrixFunctions
} // namespace bk::details

#endif //BK_MATRIXFUNCTIONS_H