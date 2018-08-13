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

#ifndef BKTOOLS_GC_DEFINITIONS_H
#define BKTOOLS_GC_DEFINITIONS_H

#include <array>
#include <cmath>
#include <cstdint>
#include <type_traits>

namespace bk::gc_details
{
  //------------------------------------------------------------------------------------------------------
  // helper: wrap vector n times around type
  //------------------------------------------------------------------------------------------------------
  // e.g., wrap_vector_t<int,2> => std::vector<std::vector<int>>
  //       wrap_vector_t<int,3> => std::vector<std::vector<std::vector<int>>>
  template<typename T, int I, int N> struct _wrap_vector
  {
      using type = typename _wrap_vector<std::vector<T>, I + 1, N>::type;
  };

  template<typename T, int I> struct _wrap_vector<T, I, I>
  {
      using type = T;
  };

  template<typename T, int N> using wrap_vector_t = typename _wrap_vector<T, 0, N>::type;

  //------------------------------------------------------------------------------------------------------
  // resize wrapped vector
  //------------------------------------------------------------------------------------------------------
  template<int I, int TDims, typename TWrappedVec, typename TSizeVec, typename T>
  void _resize_wrapped_vector(TWrappedVec& v, const TSizeVec& size, const T& default_argument)
  {
      if constexpr (I != TDims - 1)
      {
          v.resize(size[I]);

          for (unsigned int k = 0; k < static_cast<unsigned int>(size[I]); ++k)
          { _resize_wrapped_vector<I + 1, TDims>(v[k], size, default_argument); }
      }
      else
      {
          v.resize(size[I], default_argument);
      }
  }

  template<int TDims, typename TWrappedVec, typename TSizeVec, typename T>
  void resize_wrapped_vector(TWrappedVec& v, const TSizeVec& size, const T& default_argument)
  { _resize_wrapped_vector<0, TDims>(v, size, default_argument); }

  //------------------------------------------------------------------------------------------------------
  // helper functions
  //------------------------------------------------------------------------------------------------------
  template<int TDims> struct gcdef
  {
      static_assert(TDims >= 2);

      using flag_type = std::conditional_t<(TDims <= 2), std::uint16_t, std::conditional_t<(TDims <= 7), std::uint32_t, std::uint64_t>>;
      using id_type = std::array<int, TDims>;
      using ptNd_type = std::array<double, TDims>;
      using pt2Nd_type = std::array<double, 2 * TDims>;
      template<typename T> using vector_grid_type = wrap_vector_t<T, TDims>;

      //====================================================================================================
      //===== NEIGHBORS
      //====================================================================================================
      [[nodiscard]] static constexpr unsigned int NUM_NEIGHBORS() noexcept
      { return 2 * TDims; }

      [[nodiscard]] static constexpr id_type NEIGHBOR_PREDECESSOR(id_type&& n, unsigned int dimId) noexcept
      {
          --n[dimId];
          return std::move(n);
      }

      [[nodiscard]] static constexpr id_type NEIGHBOR_PREDECESSOR(const id_type& n, unsigned int dimId) noexcept
      { return NEIGHBOR_PREDECESSOR(id_type(n), dimId); }

      [[nodiscard]] static constexpr id_type NEIGHBOR_SUCCESSOR(id_type&& n, unsigned int dimId) noexcept
      {
          ++n[dimId];
          return std::move(n);
      }

      [[nodiscard]] static constexpr id_type NEIGHBOR_SUCCESSOR(const id_type& n, unsigned int dimId) noexcept
      { return NEIGHBOR_SUCCESSOR(id_type(n), dimId); }

      //====================================================================================================
      //===== DIFFERENCE
      //====================================================================================================
      /// @{ -------------------------------------------------- PAIRWISE DIFFERENCE
      [[nodiscard]] static constexpr id_type PAIRWISE_DIFFERENCE(const id_type& p, const id_type& q) noexcept
      {
          id_type diff = p;

          for (int dimId = 0; dimId < TDims; ++dimId)
          { diff[dimId] -= q[dimId]; }

          return diff;
      }
      /// @}

      [[nodiscard]] static constexpr std::uint8_t DIFF_TO_EDGE_ID(const id_type& d) noexcept
      {
          std::uint8_t temp = 0;

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              // d[dimId] € {-1, 0, +1}
              const std::uint8_t switchDim = static_cast<std::uint8_t>(std::abs(d[dimId]));
              const std::uint8_t offDim = static_cast<std::uint8_t>(ID_EDGE_PREDECESSOR(dimId));
              const std::uint8_t offNeighborDir = static_cast<std::uint8_t>((d[dimId] + 1) >> 1); // maps {-1, 0, +1} to {0, 0, 1}

              temp += switchDim * (offDim + offNeighborDir);
          }

          return temp;
      }

      //====================================================================================================
      //=====
      //====================================================================================================
      // invalid distance to terminal
      static constexpr int invalid_distance = std::numeric_limits<int>::max() - 1;

      [[nodiscard]] static constexpr std::uint8_t ID_EDGE_PREDECESSOR(unsigned int dimId) noexcept
      { return 2 * dimId; }

      [[nodiscard]] static constexpr std::uint8_t ID_EDGE_SUCCESSOR(unsigned int dimId) noexcept
      { return 2 * dimId + 1; }

      //====================================================================================================
      //===== FLAGS (9 + 2*TDims + 2*TDims bits)
      //====================================================================================================
      // 2 bits terminal parent
      [[nodiscard]] static constexpr flag_type PARENT_IS_SOURCE() noexcept
      { return 0b1; }

      [[nodiscard]] static constexpr flag_type PARENT_IS_SINK() noexcept
      { return 0b10; }

      // 3 bits set
      [[nodiscard]] static constexpr flag_type FLAG_SOURCE_SET() noexcept
      { return 0b100; }

      [[nodiscard]] static constexpr flag_type FLAG_SINK_SET() noexcept
      { return 0b1000; }

      [[nodiscard]] static constexpr flag_type FLAG_FREE_SET() noexcept
      { return 0b1'0000; }

      // 2 bits state
      [[nodiscard]] static constexpr flag_type FLAG_ACTIVE() noexcept
      { return 0b10'0000; }

      [[nodiscard]] static constexpr flag_type FLAG_ORPHAN() noexcept
      { return 0b100'0000; }

      // 2*TDims bits parent
      [[nodiscard]] static constexpr flag_type FLAG_PARENT_IS_PREDECESSOR(unsigned int dimId) noexcept
      { return FLAG_ORPHAN() << (1 + 2 * dimId); }

      [[nodiscard]] static constexpr flag_type FLAG_PARENT_IS_SUCCESSOR(unsigned int dimId) noexcept
      { return FLAG_ORPHAN() << (2 + 2 * dimId); }

      // 2*TDims bits edge cap
      [[nodiscard]] static constexpr flag_type FLAG_EDGE_PREDECESSOR_IS_FULL(unsigned int dimId) noexcept
      { return FLAG_PARENT_IS_SUCCESSOR(TDims - 1) << (1 + 2 * dimId); }

      [[nodiscard]] static constexpr flag_type FLAG_EDGE_SUCCESSOR_IS_FULL(unsigned int dimId) noexcept
      { return FLAG_PARENT_IS_SUCCESSOR(TDims - 1) << (2 + 2 * dimId); }

      //====================================================================================================
      //===== SETTINGS
      //====================================================================================================
      // block size
      [[nodiscard]] static constexpr int BLOCK_SIZE(unsigned int i) noexcept
      {
          switch (i)
          {
              case 0U: return 64; // X
              case 1U: return 64; // Y
              case 2U: return 16; // Z
              case 3U: return 8; // Z
              default: return 64;
          }
      }

      [[nodiscard]] static constexpr unsigned int NUM_THREADS() noexcept
      { return 8U; }

      [[nodiscard]] static constexpr double weight_function(double x, double tolerance = 0.5) noexcept
      {
          //return std::abs(x) < 1e-10 ? std::numeric_limits<double>::max() : 255.0*255.0/(x*x);
          //return std::abs(x) < 1e-10 ? std::numeric_limits<double>::max() : 1.0/(x*x);
          //return 1.0/std::abs(x);
          return std::exp(-tolerance * std::abs(x));
      }
  }; // class struct gcdef
} // namespace bk::gc_details

#endif //BKTOOLS_GC_DEFINITIONS_H
