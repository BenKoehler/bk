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

#ifndef BK_EIGENWRAPPERS_QRDECOMPOSITION_H
#define BK_EIGENWRAPPERS_QRDECOMPOSITION_H

#include <algorithm>
#include <optional>
#include <type_traits>
#include <utility>

#include <bkMath/matrix/type_traits/matrix_traits.h>

#include <eigen3/Eigen/QR>

namespace bk
{
  template<typename TMatrix> class QRDecomposition
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(bk::is_matrix_v<TMatrix>);
      static_assert(std::is_arithmetic_v < typename TMatrix::value_type > , "INVALID VALUE TYPE: ONLY REAL-VALUED MATRICES ARE ALLOWED");
      static_assert(bk::is_dynamic_matrix_v<TMatrix> || (TMatrix::RowsAtCompileTime() >= TMatrix::ColsAtCompileTime()), "INVALID MATRIX SIZE: NUM ROWS MUST BE >= NUM COLS");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = QRDecomposition<TMatrix>;
    public:
      using matrix_type = TMatrix;
      using Q_type = typename matrix_type::template self_template_type<typename matrix_type::value_type, matrix_type::RowsAtCompileTime(), matrix_type::RowsAtCompileTime()>;
      using eigen_matrix_type = std::conditional_t <bk::is_dynamic_matrix_v<matrix_type>, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Matrix<double, matrix_type::RowsAtCompileTime(), matrix_type::ColsAtCompileTime()>>;
      using eigen_qr_type = Eigen::FullPivHouseholderQR<eigen_matrix_type>;

    private:
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      eigen_qr_type _qr;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      QRDecomposition() = delete;
      QRDecomposition(const self_type&) = default;
      QRDecomposition(self_type&&) = default;

      QRDecomposition(const matrix_type& A)
          : _qr(A.num_rows(), A.num_cols())
      {
          eigen_matrix_type M;

          if (bk::is_dynamic_matrix_v<matrix_type>)
          { M.resize(A.num_rows(), A.num_cols()); }

          for (unsigned int r = 0; r < A.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < A.num_cols(); ++c)
              { M(r, c) = A(r, c); }
          }

          _qr.compute(M);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~QRDecomposition() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      //! returns the orthogonal matrix Q
      [[nodiscard]] Q_type Q() const
      {
          auto QEig = _qr.matrixQ();

          Q_type Q;

          if constexpr (bk::is_dynamic_matrix_v<Q_type>)
          { Q.set_size(QEig.rows(), QEig.cols()); }

          for (unsigned int r = 0; r < Q.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < Q.num_cols(); ++c)
              { Q(r, c) = QEig(r, c); }
          }

          return Q;
      }
      /// @}

      /// @{ -------------------------------------------------- VALID
      [[nodiscard]] bool is_valid() const
      { return _qr.isInvertible(); }
      /// @}

      /// @{ -------------------------------------------------- GET EIGEN QR DECOMPOSITION OBJECT
      [[nodiscard]] eigen_qr_type& qr()
      { return _qr; }

      [[nodiscard]] const eigen_qr_type& qr() const
      { return _qr; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- RANK
      [[nodiscard]] int rank() const
      { return _qr.rank(); }

      [[nodiscard]] int rank(const double tolerance = _precision_of<double>())
      {
          _qr.setThreshold(tolerance);
          return _qr.rank();
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

              const auto resEig = _qr.solve(bEig);

              for (unsigned int r = 0; r < b.num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < b.num_cols(); ++c)
                  { res(r, c) = resEig(r, c); }
              }

              return res;
          }
      }
      /// @}
  }; // class QRDecomposition
} //namespace bk

#endif //BK_EIGENWRAPPERS_QRDECOMPOSITION_H
