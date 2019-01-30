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

#ifndef BK_EIGENWRAPPERS_EIGENANALYSISREALUNSYMMETRIC_H
#define BK_EIGENWRAPPERS_EIGENANALYSISREALUNSYMMETRIC_H

#include <algorithm>
#include <cassert>
#include <complex>
#include <optional>
#include <type_traits>
#include <utility>

#include <bkMath/matrix/type_traits/matrix_traits.h>

#include <eigen3/Eigen/Eigenvalues>

namespace bk
{
  template<typename TMatrix> class EigenAnalysisRealUnsymmetric
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(bk::is_matrix_v<TMatrix>);
      static_assert(std::is_arithmetic_v < typename TMatrix::value_type > , "INVALID VALUE TYPE: ONLY REAL-VALUED MATRICES ARE ALLOWED");
      static_assert(bk::is_dynamic_matrix_v<TMatrix> || (TMatrix::RowsAtCompileTime() == TMatrix::ColsAtCompileTime()), "INVALID MATRIX SIZE: MUST BE SQUARE");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = EigenAnalysisRealUnsymmetric<TMatrix>;
    public:
      using matrix_type = TMatrix;
      using eigenvectors_matrix_type = typename std::decay_t<TMatrix>::template self_template_type<std::complex < double>>;
      using eigenvalues_vector_type = typename matrix_type::template self_template_type<std::complex < double>, matrix_type::RowsAtCompileTime(), 1>;
      using eigen_matrix_type = std::conditional_t <bk::is_dynamic_matrix_v<matrix_type>, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Matrix<double, matrix_type::RowsAtCompileTime(), matrix_type::ColsAtCompileTime()>>;
      using eigen_solver_type = Eigen::EigenSolver<eigen_matrix_type>;

    private:
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      eigen_solver_type _solver;
      bool _computeEigenvectors;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      EigenAnalysisRealUnsymmetric() = delete;
      EigenAnalysisRealUnsymmetric(const self_type&) = default;
      EigenAnalysisRealUnsymmetric(self_type&&) noexcept = default;

      EigenAnalysisRealUnsymmetric(const matrix_type& A, bool computeEigenvectors = true)
          : _solver(A.num_rows()),
            _computeEigenvectors(computeEigenvectors)
      {
          eigen_matrix_type M;

          if (bk::is_dynamic_matrix_v<matrix_type>)
          { M.resize(A.num_rows(), A.num_cols()); }

          for (unsigned int r = 0; r < A.num_rows(); ++r)
          {
              for (unsigned int c = 0; c < A.num_cols(); ++c)
              { M(r, c) = A(r, c); }
          }

          _solver.compute(M, computeEigenvectors);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~EigenAnalysisRealUnsymmetric() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET EIGENVALUES
      //!  returns the eigenvalues as col-vector
      [[nodiscard]] std::optional <eigenvalues_vector_type> eigenvalues() const
      {
          if (!is_valid())
          { return {}; }
          else
          {
              auto VEig = _solver.eigenvalues();
              const unsigned int N = std::max(VEig.rows(), VEig.cols());

              eigenvalues_vector_type V;

              if constexpr (bk::is_dynamic_matrix_v<eigenvalues_vector_type>)
              { V.set_size(N, 1); }

              for (unsigned int i = 0; i < N; ++i)
              { V[i] = VEig[i]; }

              return V;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- GET EIGENVECTORS
      //!  returns a matrix where each column is a normalized eigenvector
      [[nodiscard]] std::optional <eigenvectors_matrix_type> eigenvectors() const
      {
          assert(_computeEigenvectors && "eigenvectors were not computed!");

          if (!is_valid())
          { return {}; }
          else
          {
              auto VEig = _solver.eigenvectors();

              eigenvectors_matrix_type V;

              if constexpr (bk::is_dynamic_matrix_v<eigenvectors_matrix_type>)
              { V.set_size(VEig.rows(), VEig.cols()); }

              for (unsigned int r = 0; r < V.num_rows(); ++r)
              {
                  for (unsigned int c = 0; c < V.num_cols(); ++c)
                  { V(r, c) = VEig(r, c); }
              }

              return V;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- VALID
      [[nodiscard]] bool is_valid() const
      { return _solver.info() == Eigen::ComputationInfo::Success; }
      /// @}

      /// @{ -------------------------------------------------- GET EIGEN SOLVER OBJECT
      [[nodiscard]] eigen_solver_type& solver()
      { return _solver; }

      [[nodiscard]] const eigen_solver_type& solver() const
      { return _solver; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class EigenAnalysisRealUnsymmetric
} //namespace bk

#endif //BK_EIGENWRAPPERS_EIGENANALYSISREALUNSYMMETRIC_H