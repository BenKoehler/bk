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

#ifndef BK_VECTORFUNCTIONS_H
#define BK_VECTORFUNCTIONS_H

#include <algorithm>
#include <cassert>
#include <cmath>

#include <bkMath/matrix/type_traits/matrix_traits.h>

namespace bk::details
{
  template<typename TValue, typename TDerived, bool isVec> class VectorFunctions
  { /* empty class */ };

  template<typename TValue, typename TDerived> class VectorFunctions<TValue, TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = VectorFunctions<TValue, TDerived, true>;
      using derived_type = TDerived;
    public:
      using value_type = TValue;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CTOR
      constexpr VectorFunctions() noexcept = default;
      constexpr VectorFunctions(const self_type& other) noexcept = default;
      constexpr VectorFunctions(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~VectorFunctions() noexcept = default;
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

      /// @{ -------------------------------------------------- HELPER: COPY BLOCK
    private:
      template<typename TSubVector>
      constexpr void _copy_vec(TSubVector& res, unsigned int fromId, unsigned int toId) const
      {
          for (unsigned int i = fromId; i <= toId; ++i)
          { res[i - fromId] = (*deriv())[i]; }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET SUB VECTOR
      template<unsigned int fromId, unsigned int toId>
      [[nodiscard]] constexpr auto sub_vector() const
      {
          static_assert(fromId <= toId, "invalid from/to ids");
          static_assert(bk::is_dynamic_matrix_v<derived_type> || (bk::is_static_matrix_v<derived_type> && toId < derived_type::NumElementsAtCompileTime()), "invalid from/to ids");
          assert(toId < deriv()->num_elements());

          constexpr int NumEl = toId - fromId + 1;
          constexpr int R = bk::is_static_row_vector_v<derived_type> ? 1 : NumEl;
          constexpr int C = bk::is_static_row_vector_v<derived_type> ? NumEl : 1;

          using result_type = typename derived_type::template self_template_type<value_type, R, C>;

          result_type res;
          _copy_vec(res, fromId, toId);
          return res;
      }

      [[nodiscard]] constexpr auto sub_vector(unsigned int fromId, unsigned int toId) const
      {
          assert(fromId <= toId);
          assert(toId < deriv()->num_elements());

          using result_type = typename derived_type::template self_template_type<value_type, -1, -1>;
          result_type res;

          if (bk::matrix_traits(*deriv()).is_row_vector())
          { res.set_size(1, toId - fromId + 1); }
          else // if (bk::matrix_traits(*deriv()).is_col_vector())
          { res.set_size(toId - fromId + 1, 1); }

          res.set_alignment(deriv()->alignment());

          _copy_vec(res, fromId, toId);

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- SUB VECTOR REF
      template<unsigned int fromId, unsigned int toId>
      [[nodiscard]] constexpr auto sub_vector_ref()
      {
          assert(toId < deriv()->num_elements());

          constexpr bool isRowVecAtCompileTime = bk::is_static_row_vector_v<derived_type>;

          constexpr int R0 = isRowVecAtCompileTime ? 0 : fromId;
          constexpr int R1 = isRowVecAtCompileTime ? 0 : toId;
          constexpr int C0 = isRowVecAtCompileTime ? fromId : 0;
          constexpr int C1 = isRowVecAtCompileTime ? toId : 0;

          return deriv()->template sub_matrix_ref<R0, R1, C0, C1>();
      }

      [[nodiscard]] auto sub_vector_ref(unsigned int fromId, unsigned int toId)
      {
          assert(toId < deriv()->num_elements());

          const bool isRowVecAtRunTime = bk::matrix_traits(*deriv()).is_row_vector();

          const int R0 = isRowVecAtRunTime ? 0 : fromId;
          const int R1 = isRowVecAtRunTime ? 0 : toId;
          const int C0 = isRowVecAtRunTime ? fromId : 0;
          const int C1 = isRowVecAtRunTime ? toId : 0;

          return deriv()->sub_matrix_ref(R0, R1, C0, C1);
      }
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
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- TO HOMOGENEOUS COORDINATES
      [[nodiscard]] constexpr auto to_homogeneous_coordinates() const
      {
          constexpr int R = bk::is_static_row_vector_v<derived_type> ? 1 : bk::is_static_col_vector_v<derived_type> ? derived_type::RowsAtCompileTime() + 1 : -1;
          constexpr int C = bk::is_static_row_vector_v<derived_type> ? derived_type::ColsAtCompileTime() + 1 : bk::is_static_col_vector_v<derived_type> ? 1 : -1;
          using result_type = typename derived_type::template self_template_type<value_type, R, C, derived_type::AlignmentAtCompileTime()>;

          result_type h;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              if (bk::matrix_traits(*deriv()).is_row_vector())
              { h.set_size(1, deriv()->num_cols() + 1); }
              else
              { h.set_size(deriv()->num_rows() + 1, 1); }
          }

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          { h[i] = deriv()->operator[](i); }

          h[deriv()->num_elements()] = 1;

          return h;
      }
      /// @}

      /// @{ -------------------------------------------------- FROM HOMOGENEOUS COORDINATES
      [[nodiscard]] constexpr auto from_homogeneous_coordinates() const
      {
          constexpr int R = bk::is_static_row_vector_v<derived_type> ? 1 : bk::is_static_col_vector_v<derived_type> ? derived_type::RowsAtCompileTime() - 1 : -1;
          constexpr int C = bk::is_static_row_vector_v<derived_type> ? derived_type::ColsAtCompileTime() - 1 : bk::is_static_col_vector_v<derived_type> ? 1 : -1;
          using result_type = typename derived_type::template self_template_type<value_type, R, C, derived_type::AlignmentAtCompileTime()>;

          result_type h;

          if constexpr (bk::is_dynamic_matrix_v<result_type>)
          {
              if (bk::matrix_traits(*deriv()).is_row_vector())
              { h.set_size(1, deriv()->num_cols() - 1); }
              else
              { h.set_size(deriv()->num_rows() - 1, 1); }
          }

          const auto w = deriv()->operator[](deriv()->num_elements() - 1);
          assert(w != 0);

          for (unsigned int i = 0; i < h.num_elements(); ++i)
          { h[i] = deriv()->operator[](i) / w; }

          return h;
      }
      /// @}

      //====================================================================================================
      //===== MATH
      //====================================================================================================
      /// @{ -------------------------------------------------- DISTANCE (SQUARED)
      template<typename TVec>
      [[nodiscard]] constexpr double distance_squared(const TVec& v) const
      {
          static_assert(bk::is_static_vector_v<TVec> || bk::is_dynamic_matrix_v<TVec>);
          assert(bk::matrix_traits_comp(*deriv(), v).num_elements_matches());

          double d = 0;

          for (unsigned int i = 0; i < deriv()->num_elements(); ++i)
          {
              double temp = deriv()->operator[](i) - v[i];
              d += temp * temp;
          }

          return d;
      }

      template<typename TVec>
      [[nodiscard]] constexpr auto distance(const TVec& v) const
      { return std::sqrt(distance_squared(v)); }
      /// @}

      /// @{ -------------------------------------------------- ANGLE (RAD)
      template<typename TVec>
      [[nodiscard]] constexpr double angle(const TVec& v) const
      {
          static_assert(bk::is_static_vector_v<TVec> || bk::is_dynamic_matrix_v<TVec>);
          assert(bk::matrix_traits_comp(*deriv(), v).num_elements_matches());

          const double a = deriv()->dot(v) / (deriv()->norm() * v.norm());
          return std::acos(std::clamp(a, -1.0, 1.0));
      }
      /// @}
  }; // class VectorFunctions
} // namespace bk::details

#endif //BK_VECTORFUNCTIONS_H


