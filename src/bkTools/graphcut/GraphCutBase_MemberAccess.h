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

#ifndef BKTOOLS_GRAPHCUTBASE_MEMBERACCESS_H
#define BKTOOLS_GRAPHCUTBASE_MEMBERACCESS_H

#include <array>

#include <bkTools/graphcut/gc_definitions.h>

namespace bk::gc_details
{
  template<int TDims, typename TDerived> class GraphCutBase_MemberAccess
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCutBase_MemberAccess<TDims, TDerived>;
      using derived_type = TDerived;
    protected:
      using gc = gcdef<TDims>;
      using ptNd_type = std::array<double, TDims>;
      using pt2Nd_type = std::array<double, 2 * TDims>;
      template<typename T> using vector_grid_type = wrap_vector_t<T, TDims>;

    public:
      using flag_type = typename gc::flag_type;
      using id_type = typename gc::id_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr GraphCutBase_MemberAccess() = default;
      constexpr GraphCutBase_MemberAccess(const self_type&) = default;
      constexpr GraphCutBase_MemberAccess(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCutBase_MemberAccess() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS: TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }

    public:
      /// @}

      /// @{ -------------------------------------------------- HELPER: GET VALUE OF ND INDEX ACCESSIBLE
    private:
      template<int I, typename TVec>
      [[nodiscard]] static constexpr decltype(auto) get_from_vector_grid(TVec& v, const id_type& n)
      {
          if constexpr (I != TDims - 1)
          { return get_from_vector_grid<I + 1>(v[n[I]], n); }
          else
          { return v[n[I]]; }
      }

    protected:

      template<typename TVec>
      [[nodiscard]] static constexpr decltype(auto) get_from_vector_grid(TVec& v, const id_type& n)
      { return get_from_vector_grid<0, TVec>(v, n); }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      //! returns size of dimension
      [[nodiscard]] constexpr int size_of_dim(unsigned int dimId) const
      { return deriv()->_size[dimId]; }
      /// @}



      /// @{ -------------------------------------------------- GET RESIDUAL
      [[nodiscard]] constexpr pt2Nd_type& residual(const id_type& n)
      { return get_from_vector_grid(deriv()->_residual, n); }

      [[nodiscard]] constexpr const pt2Nd_type& residual(const id_type& n) const
      { return get_from_vector_grid(deriv()->_residual, n); }
      /// @}

      /// @{ -------------------------------------------------- DISTANCE TO TERMINAL
      [[nodiscard]] constexpr int& distance_to_terminal(const id_type& n)
      { return get_from_vector_grid(deriv()->_distance_to_terminal, n); }

      [[nodiscard]] constexpr int distance_to_terminal(const id_type& n) const
      { return get_from_vector_grid(deriv()->_distance_to_terminal, n); }
      /// @}

      /// @{ -------------------------------------------------- TIMESTAMP
      [[nodiscard]] constexpr int& timestamp(const id_type& n)
      { return get_from_vector_grid(deriv()->_timestamp, n); }

      [[nodiscard]] constexpr int timestamp(const id_type& n) const
      { return get_from_vector_grid(deriv()->_timestamp, n); }
      /// @}

      /// @{ -------------------------------------------------- FLAG
      [[nodiscard]] constexpr flag_type& flag(const id_type& n)
      { return get_from_vector_grid(deriv()->_flags, n); }

      [[nodiscard]] constexpr const flag_type& flag(const id_type& n) const
      { return get_from_vector_grid(deriv()->_flags, n); }
      /// @}

      /// @{ -------------------------------------------------- NARROW BAND
      [[nodiscard]] constexpr decltype(auto) band(const id_type& n)
      { return get_from_vector_grid(deriv()->_band, n); }

      [[nodiscard]] constexpr bool band(const id_type& n) const
      { return get_from_vector_grid(deriv()->_band, n); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class GraphCutBase_MemberAccess
} // namespace bk

#endif //BKTOOLS_GRAPHCUTBASE_MEMBERACCESS_H
