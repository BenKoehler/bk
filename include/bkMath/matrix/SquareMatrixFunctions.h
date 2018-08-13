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

#ifndef BK_SQUAREMATRIXFUNCTIONS_H
#define BK_SQUAREMATRIXFUNCTIONS_H

#include "../../bkTypeTraits/complex_traits.h"

#include "eigen_wrappers/EigenvaluesRealSymmetric.h"
#include "eigen_wrappers/EigenvaluesRealUnsymmetric.h"
#include "eigen_wrappers/EigenAnalysisRealSymmetric.h"
#include "eigen_wrappers/EigenAnalysisRealUnsymmetric.h"
#include "type_traits/common_type.h"
#include "type_traits/signed_type.h"
#include "type_traits/matrix_traits.h"

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isSquare> class SquareMatrixFunctions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class SquareMatrixFunctions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SquareMatrixFunctions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr SquareMatrixFunctions() noexcept = default;
      constexpr SquareMatrixFunctions(const self_type& other) noexcept = default;
      constexpr SquareMatrixFunctions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SquareMatrixFunctions() noexcept = default;
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

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type& other) noexcept = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&& other) noexcept = default;
      /// @}
    public:

      //====================================================================================================
      //===== ANALYSIS
      //====================================================================================================
      /// @{ -------------------------------------------------- EIGEN ANALYSIS
      [[nodiscard]] constexpr auto eigenvalues_symmetric() const
      { return EigenvaluesRealSymmetric<const derived_type>(*deriv()).eigenvalues(); }

      [[nodiscard]] constexpr auto eigenvalues() const
      { return EigenvaluesRealUnsymmetric<const derived_type>(*deriv()).eigenvalues(); }

      [[nodiscard]] constexpr auto eigenanalysis_symmetric() const
      { return EigenAnalysisRealSymmetric<const derived_type>(*deriv()); }

      [[nodiscard]] constexpr auto eigenanalysis() const
      { return EigenAnalysisRealUnsymmetric<const derived_type>(*deriv()); }
      /// @}

      /// @{ -------------------------------------------------- DETERMINANT
      //! determinant as product of eigenvalues
      /*!
       * returns a complex value, although the imaginary part might be 0
       */
      [[nodiscard]] constexpr auto determinant_symmetric() const
      {
          static_assert(!bk::is_complex_v<value_type>, "only for real-valued matrices");
          assert(deriv()->is_symmetric());

          using signed_value_type = bk::make_signed_t<value_type>;

          const auto e = eigenvalues_symmetric().value();
          signed_value_type det = e[0];

          for (unsigned int i = 1; i < e.num_elements(); ++i)
          { det *= e[i]; }

          return det;
      }

      [[nodiscard]] constexpr auto determinant() const
      {
          static_assert(!bk::is_complex_v<value_type>, "only for real-valued matrices");

          using complex_signed_value_type = bk::make_complex_t<bk::make_signed_t<value_type>>;

          if (deriv()->is_symmetric())
          { return complex_signed_value_type(determinant_symmetric() /*real*/, 0 /*imag*/); }

          const auto e = eigenvalues().value();
          complex_signed_value_type det = e[0];

          for (unsigned int i = 1; i < e.num_elements(); ++i)
          { det *= e[i]; }

          return det;
      }
      /// @}

      /// @{ -------------------------------------------------- TRACE
      //! sum of diagonal elements
      [[nodiscard]] constexpr value_type trace() const
      {
          value_type tr = 0;

          for (unsigned int i = 0; i < deriv()->num_rows(); ++i)
          { tr += deriv()->operator()(i, i); }

          return tr;
      }
      /// @}
  }; // class SquareMatrixFunctions
} // namespace bk::details

#endif //BK_SQUAREMATRIXFUNCTIONS_H


