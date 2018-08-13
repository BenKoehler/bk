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

#ifndef BK_MATRIXFACTORY_H
#define BK_MATRIXFACTORY_H

#include <cassert>
#include <random>
#include <type_traits>

#include "../../bkTypeTraits/complex_traits.h"
#include "../../bkTypeTraits/floating_point.h"

#include "Matrix.h"
#include "MatrixAlignment.h"
#include "type_traits/matrix_traits.h"

namespace bk
{
  struct MatrixFactory
  {
      //====================================================================================================
      //===== CREATE (ALLOCATE)
      //====================================================================================================
      /// @{ -------------------------------------------------- CREATE MATRIX FROM GIVEN SIZES
      template<typename TValue, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto create([[maybe_unused]] int rows = -1, [[maybe_unused]] int cols = -1, [[maybe_unused]] MatrixAlignment alignment = MatrixAlignment::ColMajor)
      {
          using matrix_type = Matrix<TValue, TRows, TCols, TAlignment>;

          matrix_type res;

          if constexpr (bk::is_dynamic_matrix_v<matrix_type>)
          {
              assert(rows > 0);
              assert(cols > 0);

              res.set_size(rows, cols);
              res.set_alignment(alignment);
          }

          if constexpr (bk::is_static_matrix_v<matrix_type>)
          {
              assert(rows < 0 || rows == TRows);
              assert(cols < 0 || cols == TCols);
          }

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- CREATE MATRIX FROM GIVEN EXAMPLE MATRIX
      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto create(const TMatrix& m)
      { return create<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TMatrix>
      [[nodiscard]] static constexpr auto create(const TMatrix& m)
      { return create<typename TMatrix::value_type>(m); }
      /// @}

      /// @{ -------------------------------------------------- CREATE COL VECTOR
      template<typename TValue, int N = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<N, 1>::default_alignment()>
      [[nodiscard]] static constexpr auto create_col_vector(int n = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor)
      { return create<TValue, N, 1, TAlignment>(n, 1, alignment); }
      /// @}

      /// @{ -------------------------------------------------- CREATE ROW VECTOR
      template<typename TValue, int N = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<N, 1>::default_alignment()>
      [[nodiscard]] static constexpr auto create_row_vector(int n = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor)
      { return create<TValue, 1, N, TAlignment>(1, n, alignment); }
      /// @}

      //====================================================================================================
      //===== COPY
      //====================================================================================================
      /// @{ -------------------------------------------------- CREATE AND COPY GIVEN MATRIX
      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto copy(const TMatrix& m)
      {
          auto res = create<TValue>(m);
          res = m;
          return res;
      }

      template<typename TMatrix>
      [[nodiscard]] static constexpr auto copy(const TMatrix& m)
      { return copy<typename TMatrix::value_type>(m); }
      /// @}

      //====================================================================================================
      //===== SPECIAL MATRICES
      //====================================================================================================
      /// @{ -------------------------------------------------- AXIS 2D
      //! create a vector representing the x/y coordinate axis in 2D
      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 2, 1> X_Axis_2D() noexcept
      { return Matrix<TValue, 2, 1>(1, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 2, 1> Y_Axis_2D() noexcept
      { return Matrix<TValue, 2, 1>(0, 1); }
      /// @}

      /// @{ -------------------------------------------------- AXIS 3D
      //! create a vector representing the x/y coordinate axis in 3D
      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 1> X_Axis_3D() noexcept
      { return Matrix<TValue, 3, 1>(1, 0, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 1> Y_Axis_3D() noexcept
      { return Matrix<TValue, 3, 1>(0, 1, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 1> Z_Axis_3D() noexcept
      { return Matrix<TValue, 3, 1>(0, 0, 1); }
      /// @}

      /// @{ -------------------------------------------------- AXIS 4D
      //! create a vector representing the x/y coordinate axis in 3D
      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 4, 1> X_Axis_4D() noexcept
      { return Matrix<TValue, 4, 1>(1, 0, 0, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 4, 1> Y_Axis_4D() noexcept
      { return Matrix<TValue, 4, 1>(0, 1, 0, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 4, 1> Z_Axis_4D() noexcept
      { return Matrix<TValue, 4, 1>(0, 0, 1, 0); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 4, 1> T_Axis_4D() noexcept
      { return Matrix<TValue, 4, 1>(0, 0, 0, 1); }
      /// @}

      /// @{ -------------------------------------------------- (COUNTER)CLOCKWISE 2D ROTATION MATRIX
      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 2, 2> Rotation_Matrix_2D_Counterclockwise(double angle_rad) noexcept
      {
          Matrix<TValue, 2, 2> R;
          R.set_rotation_matrix_2d_counterclockwise(angle_rad);
          return R;
      }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 2, 2> Rotation_Matrix_2D_Clockwise(double angle_rad) noexcept
      {
          Matrix<TValue, 2, 2> R;
          R.set_rotation_matrix_2d_clockwise(angle_rad);
          return R;
      }
      /// @}

      /// @{ -------------------------------------------------- GET GENERAL 3D ROTATION MATRIX
      template<typename TVec, typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 3> Rotation_Matrix_3D_General(const TVec& axis, double angle_rad, bool normalize_axis = true) noexcept
      {
          Matrix<TValue, 3, 3> R;
          R.set_rotation_matrix_3d_general(axis, angle_rad, normalize_axis);
          return R;
      }
      /// @}

      /// @{ -------------------------------------------------- GET X/Y/Z-AXIS 3D ROTATION MATRIX
      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 3> Rotation_Matrix_3D_X(double angle_rad) noexcept
      {
          Matrix<TValue, 3, 3> R;
          R.set_rotation_matrix_3d_x(angle_rad);
          return R;
      }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 3> Rotation_Matrix_3D_Y(double angle_rad) noexcept
      {
          Matrix<TValue, 3, 3> R;
          R.set_rotation_matrix_3d_y(angle_rad);
          return R;
      }

      template<typename TValue = double>
      [[nodiscard]] static constexpr Matrix<TValue, 3, 3> Rotation_Matrix_3D_Z(double angle_rad) noexcept
      {
          Matrix<TValue, 3, 3> R;
          R.set_rotation_matrix_3d_z(angle_rad);
          return R;
      }
      /// @}

      /// @{ -------------------------------------------------- CONSTANT VALUE
      template<typename TValue, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto Constant(TValue x, int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      {
          static_assert(std::is_arithmetic_v<TValue> || bk::is_complex_v<TValue>, "invalid value type");

          auto res = create<TValue, TRows, TCols, TAlignment>(rows, cols, alignment);
          res.set_constant(x);
          return res;
      }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto Constant(TValue x, const TMatrix& m)
      { return Constant<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(x, m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Vec_2D(TValue x) noexcept
      { return Constant<TValue, 2, 1>(x); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Vec_3D(TValue x) noexcept
      { return Constant<TValue, 3, 1>(x); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Vec_4D(TValue x) noexcept
      { return Constant<TValue, 4, 1>(x); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Mat_2D(TValue x) noexcept
      { return Constant<TValue, 2, 2>(x); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Mat_3D(TValue x) noexcept
      { return Constant<TValue, 3, 3>(x); }

      template<typename TValue>
      [[nodiscard]] static constexpr auto Constant_Mat_4D(TValue x) noexcept
      { return Constant<TValue, 4, 4>(x); }
      /// @}

      /// @{ -------------------------------------------------- CONSTANT ZERO
      template<typename TValue = double, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto Zero(int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      { return Constant<TValue, TRows, TCols, TAlignment>(static_cast<TValue>(0), rows, cols, alignment); }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto Zero(const TMatrix& m)
      { return Zero<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TMatrix>
      [[nodiscard]] static constexpr auto Zero(const TMatrix& m)
      { return Zero<typename TMatrix::value_type>(m); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Vec_2D() noexcept
      { return Zero<TValue, 2, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Vec_3D() noexcept
      { return Zero<TValue, 3, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Vec_4D() noexcept
      { return Zero<TValue, 4, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Mat_2D() noexcept
      { return Zero<TValue, 2, 2>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Mat_3D() noexcept
      { return Zero<TValue, 3, 3>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Zero_Mat_4D() noexcept
      { return Zero<TValue, 4, 4>(); }
      /// @}

      /// @{ -------------------------------------------------- CONSTANT ONE
      template<typename TValue = double, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto One(int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      { return Constant<TValue, TRows, TCols, TAlignment>(static_cast<TValue>(1), rows, cols, alignment); }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto One(const TMatrix& m)
      { return One<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TMatrix>
      [[nodiscard]] static constexpr auto One(const TMatrix& m)
      { return One<typename TMatrix::value_type>(m); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Vec_2D() noexcept
      { return One<TValue, 2, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Vec_3D() noexcept
      { return One<TValue, 3, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Vec_4D() noexcept
      { return One<TValue, 4, 1>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Mat_2D() noexcept
      { return One<TValue, 2, 2>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Mat_3D() noexcept
      { return One<TValue, 3, 3>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto One_Mat_4D() noexcept
      { return One<TValue, 4, 4>(); }
      /// @}

      /// @{ -------------------------------------------------- IDENTITY MATRIX
      template<typename TValue = double, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto Identity(int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      {
          auto res = create<TValue, TRows, TCols, TAlignment>(rows, cols, alignment);
          res.set_identity();
          return res;
      }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static constexpr auto Identity(const TMatrix& m)
      { return Identity<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TMatrix>
      [[nodiscard]] static constexpr auto Identity(const TMatrix& m)
      { return Identity<typename TMatrix::value_type>(m); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Identity_Mat_2D() noexcept
      { return Identity<TValue, 2, 2>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Identity_Mat_3D() noexcept
      { return Identity<TValue, 3, 3>(); }

      template<typename TValue = double>
      [[nodiscard]] static constexpr auto Identity_Mat_4D() noexcept
      { return Identity<TValue, 4, 4>(); }
      /// @}

      /// @{ -------------------------------------------------- RANDOM INT
      template<typename TValue = int, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static auto Random_int(TValue rmin, TValue rmax, int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      {
          auto res = create<TValue, TRows, TCols, TAlignment>(rows, cols, alignment);
          res.set_random_int(rmin, rmax);
          return res;
      }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static auto Random_int(TValue rmin, TValue rmax, const TMatrix& m)
      { return Random_int<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(rmin, rmax, m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Vec_2D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 2, 1>(rmin, rmax); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Vec_3D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 3, 1>(rmin, rmax); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Vec_4D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 4, 1>(rmin, rmax); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Mat_2D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 2, 2>(rmin, rmax); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Mat_3D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 3, 3>(rmin, rmax); }

      template<typename TValue = int>
      [[nodiscard]] static auto Random_int_Mat_4D(TValue rmin, TValue rmax) noexcept
      { return Random_int<TValue, 4, 4>(rmin, rmax); }
      /// @}

      /// @{ -------------------------------------------------- RANDOM FLOAT
      template<typename TValue = double, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static auto Random_float(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1), int rows = -1, int cols = -1, MatrixAlignment alignment = MatrixAlignment::ColMajor) noexcept(bk::is_static_matrix_row_col_v<TRows, TCols>)
      {
          auto res = create<TValue, TRows, TCols, TAlignment>(rows, cols, alignment);
          res.set_random_float(rmin, rmax);
          return res;
      }

      template<typename TValue, typename TMatrix>
      [[nodiscard]] static auto Random_float(TValue rmin, TValue rmax, const TMatrix& m)
      { return Random_float<TValue, TMatrix::RowsAtCompileTime(), TMatrix::ColsAtCompileTime(), TMatrix::AlignmentAtCompileTime()>(rmin, rmax, m.num_rows(), m.num_cols(), m.alignment()); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Vec_2D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 2, 1>(rmin, rmax); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Vec_3D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 3, 1>(rmin, rmax); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Vec_4D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 4, 1>(rmin, rmax); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Mat_2D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 2, 2>(rmin, rmax); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Mat_3D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 3, 3>(rmin, rmax); }

      template<typename TValue = double>
      [[nodiscard]] static auto Random_float_Mat_4D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_float<TValue, 4, 4>(rmin, rmax); }
      /// @}

      /// @{ -------------------------------------------------- RANDOM AT COMPILE TIME
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
      template<unsigned long long seedAdd, typename TValue = double, int TRows = -1, int TCols = -1, MatrixAlignment TAlignment = bk::matrix_row_col_traits<TRows, TCols>::default_alignment()>
      [[nodiscard]] static constexpr auto Random_ct(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1))  noexcept
      {
          static_assert(TRows > 0 && TCols > 0, "Random_ct() is for static vectors only! Template parameters TRows and TCols must be > 0!");

          Matrix<TValue, TRows, TCols, TAlignment> res;
          res.template set_random_ct<seedAdd>();

          const TValue range = rmax - rmin;

          for (unsigned int i = 0; i < TRows * TCols; ++i)
          {
              if constexpr (std::is_integral_v<TValue>)
              { res[i] = (res[i] % range) + rmin; }
              else if constexpr (bk::is_floating_point_v<TValue>)
              { res[i] = rmin + static_cast<TValue>(static_cast<long double>(res[i]) / static_cast<long double>((1UL << 31) - 1 /*uint32max*/)) * range; }
          }

          return res;
      }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Vec_2D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 2, 1>(rmin, rmax); }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Vec_3D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 3, 1>(rmin, rmax); }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Vec_4D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 4, 1>(rmin, rmax); }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Mat_2D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 2, 2>(rmin, rmax); }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Mat_3D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 3, 3>(rmin, rmax); }

      template<unsigned long long seedAdd, typename TValue = double>
      [[nodiscard]] static constexpr auto Random_ct_Mat_4D(TValue rmin = static_cast<TValue>(0), TValue rmax = static_cast<TValue>(1)) noexcept
      { return Random_ct<seedAdd, TValue, 4, 4>(rmin, rmax); }
      /// @}
  }; // class MatrixFactory
} // namespace bk

#endif //BK_MATRIXFACTORY_H


