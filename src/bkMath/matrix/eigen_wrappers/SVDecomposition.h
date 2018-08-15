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

#ifndef BK_EIGENWRAPPERS_SVDECOMPOSITION_H
#define BK_EIGENWRAPPERS_SVDECOMPOSITION_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <type_traits>
#include <utility>

#include <bkMath/matrix/type_traits/matrix_traits.h>

#include <eigen3/Eigen/SVD>

namespace bk
{
  template<typename TMatrix> class SVDecomposition
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(bk::is_matrix_v<TMatrix>);
      static_assert(std::is_arithmetic_v < typename TMatrix::value_type > , "INVALID VALUE TYPE: ONLY REAL-VALUED MATRICES ARE ALLOWED");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SVDecomposition<TMatrix>;
    public:
      using matrix_type = TMatrix;
      using U_type = typename matrix_type::template self_template_type<typename matrix_type::value_type, matrix_type::RowsAtCompileTime(), matrix_type::RowsAtCompileTime()>;
      using V_type = typename matrix_type::template self_template_type<typename matrix_type::value_type, matrix_type::ColsAtCompileTime(), matrix_type::ColsAtCompileTime()>;
      using S_type = matrix_type;
      using diagonal_vector_type = typename matrix_type::template self_template_type<typename matrix_type::value_type, std::min(matrix_type::RowsAtCompileTime(), matrix_type::ColsAtCompileTime()), 1>;
      using eigen_matrix_type = std::conditional_t <bk::is_dynamic_matrix_v<matrix_type>, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Matrix<double, matrix_type::RowsAtCompileTime(), matrix_type::ColsAtCompileTime()>>;
      using eigen_svd_type = Eigen::JacobiSVD<eigen_matrix_type, Eigen::FullPivHouseholderQRPreconditioner>;

    private:
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      eigen_svd_type _svd;
      unsigned int _rows;
      unsigned int _cols;
      bool _computeU;
      bool _computeV;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      SVDecomposition() = delete;
      SVDecomposition(const self_type&) = default;
      SVDecomposition(self_type&&) noexcept = default;

      SVDecomposition(const matrix_type& A, bool computeU = false, bool computeV = false)
          : _svd(A.num_rows(), A.num_cols()),
            _rows(A.num_rows()),
            _cols(A.num_cols()),
            _computeU(computeU),
            _computeV(computeV)
      {
          eigen_matrix_type M;

          if (bk::is_dynamic_matrix_v<matrix_type>)
          { M.resize(A.num_rows(), A.num_cols()); }

          for (unsigned int r = 0; r < A.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < A.num_cols(); ++c)
              { M(r, c) = A(r, c); }
          }

          unsigned int computationOptions = 0;

          if (computeU)
          { computationOptions |= Eigen::ComputeFullU; }
          else
          { computationOptions |= Eigen::ComputeThinU; }

          if (computeV)
          { computationOptions |= Eigen::ComputeFullV; }
          else
          { computationOptions |= Eigen::ComputeThinV; }

          _svd.compute(M, computationOptions);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SVDecomposition() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET U
      [[nodiscard]] U_type U() const
      {
          if (!_computeU)
          { std::cerr << "SVD: U was not computed!" << std::endl; }

          assert(_computeU && "U was not computed");

          auto UEig = _svd.matrixU();

          U_type U;

          if constexpr (bk::is_dynamic_matrix_v<U_type>)
          { U.set_size(UEig.rows(), UEig.cols()); }

          for (unsigned int r = 0; r < U.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < U.num_cols(); ++c)
              { U(r, c) = UEig(r, c); }
          }

          return U;
      }
      /// @}

      /// @{ -------------------------------------------------- GET V
      [[nodiscard]] V_type V() const
      {
          if (!_computeV)
          { std::cerr << "SVD: V was not computed!" << std::endl; }

          assert(_computeV && "V was not computed");

          auto VEig = _svd.matrixV();

          V_type V;

          if constexpr (bk::is_dynamic_matrix_v<V_type>)
          { V.set_size(VEig.rows(), VEig.cols()); }

          for (unsigned int r = 0; r < V.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < V.num_cols(); ++c)
              { V(r, c) = VEig(r, c); }
          }

          return V;
      }
      /// @}

      /// @{ -------------------------------------------------- GET S
      [[nodiscard]] S_type S() const
      {
          auto SEig = _svd.singularValues();

          S_type S;

          if constexpr (bk::is_dynamic_matrix_v<S_type>)
          { S.set_size(_rows, _cols); }

          for (unsigned int i = 0; i < SEig.size(); ++i)
          { S(i, i) = SEig[i]; }

          return S;
      }
      /// @}

      /// @{ -------------------------------------------------- GET SINGULAR VALUES
      [[nodiscard]] diagonal_vector_type singular_values() const
      {
          auto SEig = _svd.singularValues();

          diagonal_vector_type S;

          if constexpr (bk::is_dynamic_matrix_v<diagonal_vector_type>)
          { S.set_size(std::min(_rows, _cols), 1); }

          for (unsigned int i = 0; i < SEig.size(); ++i)
          { S[i] = SEig[i]; }

          return S;
      }
      /// @}

      /// @{ -------------------------------------------------- GET NUM NONZERO SINGULAR VALUES
      [[nodiscard]] unsigned int num_nonzero_singular_values() const
      { return static_cast<unsigned int>(_svd.nonzeroSingularValues()); }
      /// @}

      /// @{ -------------------------------------------------- GET EIGEN SVD OBJECT
      [[nodiscard]] eigen_svd_type& svd()
      { return _svd; }

      [[nodiscard]] const eigen_svd_type& svd() const
      { return _svd; }
      /// @}

      /// @{ -------------------------------------------------- IS VALID
      [[nodiscard]] bool is_valid() const
      { return static_cast<unsigned int>(_svd.nonzeroSingularValues()) == std::min(_rows, _cols); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- RANK
      [[nodiscard]] int rank() const
      { return _svd.rank(); }

      [[nodiscard]] int rank(const double tolerance = _precision_of<double>())
      {
          _svd.setThreshold(tolerance);
          return _svd.rank();
      }
      /// @}

      /// @{ -------------------------------------------------- CONDITION
      [[nodiscard]] double condition(double precision = _precision_of<double>()) const
      {
          const auto sing = singular_values();

          double smax = std::numeric_limits<double>::lowest();
          double smin = std::numeric_limits<double>::max();

          for (unsigned int i = 0; i < std::min(_rows, _cols); ++i)
          {
              smax = std::max(smax, static_cast<double>(sing[i]));

              if (!bk::equals_approx(sing[i], 0, precision))
              { smin = std::min(smin, static_cast<double>(sing[i])); }
          }

          return smax / smin;
      }
      /// @}

      /// @{ -------------------------------------------------- SOLVE
      template<typename TVector>
      [[nodiscard]] std::optional <TVector> solve(const TVector& b) const
      {
          if (!is_valid())
          { return {}; }
          else
          {
              constexpr int EigenRows = bk::is_dynamic_matrix_v<TVector> ? Eigen::Dynamic : TVector::RowsAtCompileTime();
              constexpr int EigenCols = bk::is_dynamic_matrix_v<TVector> ? Eigen::Dynamic : TVector::ColsAtCompileTime();
              Eigen::Matrix<typename TVector::value_type, EigenRows, EigenCols> bEig;

              TVector res;

              if constexpr (bk::is_dynamic_matrix_v<TVector>)
              {
                  bEig.resize(b.num_rows(), b.num_cols());
                  res.set_size(b.num_rows(), b.num_cols());
              }

              for (unsigned int r = 0; r < b.num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < b.num_cols(); ++c)
                  { bEig(r, c) = b(r, c); }
              }

              const auto resEig = _svd.solve(bEig);

              for (unsigned int r = 0; r < b.num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < b.num_cols(); ++c)
                  { res(r, c) = resEig(r, c); }
              }

              return res;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- PSEUDO INVERSE
      [[nodiscard]] auto pseudo_inverse(double precision = _precision_of<double>()) const
      {
          if (!_computeU || !_computeV)
          { std::cerr << "SVD: U and/or V were not computed!" << std::endl; }

          assert(_computeU && _computeV && "SVD: U and/or V were not computed!");

          auto Sinv = S();

          for (unsigned int i = 0; i < std::min(_rows, _cols); ++i)
          {
              if (!bk::equals_approx(Sinv(i, i), 0, precision))
              { Sinv(i, i) = 1.0 / Sinv(i, i); }
          }

          return (V() * std::move(Sinv) * U().transpose());
      }
      /// @}
  }; // class SVDecomposition
} //namespace bk

#endif //BK_EIGENWRAPPERS_SVDECOMPOSITION_H