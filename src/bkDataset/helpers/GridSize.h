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

#ifndef BK_GRIDSIZE_H
#define BK_GRIDSIZE_H

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include <bkTypeTraits/has_index_operator.h>
#include <bk/StringUtils>

namespace bk
{
  template<int TDims = -1> class GridSize
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || (TDims > 0), "invalid num dimensions (TDims > 0 or TDims == -1 required)");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GridSize<TDims>;
    public:
      using size_vector_type = std::conditional_t<(TDims > 0), std::array<unsigned int, TDims>, std::vector<unsigned int>>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      size_vector_type _size;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GridSize()
      { std::fill(_size.begin(), _size.end(), 0); }

      GridSize(const self_type&) = default;
      GridSize(self_type&&) noexcept = default;

      template<typename... TIds, std::enable_if_t<std::conjunction_v<std::is_integral<TIds>...>>* = nullptr>
      GridSize(TIds... sizes_per_dimension)
          : _size({static_cast<unsigned int>(sizes_per_dimension)...})
      { static_assert(TDims == -1 || sizeof...(TIds) == TDims, "invalid number of arguments"); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GridSize() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] unsigned int num_dimensions() const
      { return _size.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE (OF DIMENSION)
      [[nodiscard]] const size_vector_type& size() const
      { return _size; }

      [[nodiscard]] unsigned int& size(unsigned int dimId)
      {
          assert(dimId < num_dimensions() && "dimId out of bounds");
          return _size[dimId];
      }

      [[nodiscard]] unsigned int size(unsigned int dimId) const
      {
          assert(dimId < num_dimensions() && "dimId out of bounds");
          return _size[dimId];
      }
      /// @}

      /// @{ -------------------------------------------------- GET NUM POINTS
      [[nodiscard]] unsigned int num_points() const
      {
          return std::accumulate(_size.begin(), _size.end(), 1U, [](unsigned int x, unsigned int y)
          { return x * y; });
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR==
      template<int D>
      [[nodiscard]] bool operator==(const GridSize<D>& other)
      {
          if (num_dimensions() != other.num_dimensions())
          { return false; }

          for (unsigned int i = 0; i < num_dimensions(); ++i)
          {
              if (_size[i] != other._size[i])
              { return false; }
          }

          return true;
      }

      template<int D>
      [[nodiscard]] bool operator!=(const GridSize<D>& other)
      { return !operator==(other); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      template<typename TVec, std::enable_if_t<bk::has_index_operator_v<TVec>>* = nullptr>
      void set_size(const TVec& sizes)
      {
          const unsigned int N = std::distance(sizes.begin(), sizes.end());

          if constexpr (TDims == -1)
          { _size.resize(N); }

          assert(N == num_dimensions() && "invalid vector size");

          for (unsigned int i = 0; i < N; ++i)
          { _size[i] = sizes[i]; }
      }

      template<typename... TSizes, std::enable_if_t<std::conjunction_v<std::is_integral<std::decay_t<TSizes>>...>>* = nullptr>
      void set_size(TSizes... sizes)
      {
          static_assert(TDims == -1 || sizeof...(TSizes) == TDims, "invalid number of arguments");

          _size = {static_cast<unsigned int>(sizes)...};
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(std::string_view filename) const
      {
          /*
           * check filename
           */
          std::string fname(filename);
          const std::string suffix = ".gsize";

          if (fname.empty())
          { fname = "grid" + suffix; }
          else if (!bk::string_utils::ends_with(fname, suffix))
          { fname.append(suffix); }

          /*
           * create file
           */
          std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);

          /*
           * save
           */
          const bool success = save(file);

          if (success)
          { file.close(); }

          return success;
      }

      [[maybe_unused]] bool save(std::ofstream& file) const
      {
          if (!file.is_open() || !file.good())
          { return false; }

          std::uint32_t numDimensions = num_dimensions();
          file.write(reinterpret_cast<char*>(&numDimensions), sizeof(std::uint32_t));

          for (std::uint32_t dimId = 0; dimId < numDimensions; ++dimId)
          {
              std::uint32_t uitemp = _size[dimId];
              file.write(reinterpret_cast<char*>(&uitemp), sizeof(std::uint32_t));
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filename)
      {
          /*
           * check file ending
           */
          if (!string_utils::ends_with(filename.data(), ".gsize"))
          { return false; }

          /*
           * open file
           */
          std::ifstream file(filename.data(), std::ios_base::in | std::ios_base::binary);

          /*
           * load
           */
          const bool success = load(file);

          if (success)
          { file.close(); }

          return success;
      }

      [[maybe_unused]] bool load(std::ifstream& file)
      {
          if (!file.is_open() || !file.good())
          { return false; }

          std::uint32_t numDimensions;
          file.read(reinterpret_cast<char*>(&numDimensions), sizeof(std::uint32_t));

          assert((TDims <= 0 || (TDims > 0 && TDims == static_cast<int>(numDimensions))) && "GridSize is loaded from an object (this) with wrong TDims template argument");

          if constexpr (TDims <= 0)
          { _size.resize(numDimensions); }

          for (std::uint32_t dimId = 0; dimId < numDimensions; ++dimId)
          {
              std::uint32_t uitemp;
              file.read(reinterpret_cast<char*>(&uitemp), sizeof(std::uint32_t));

              _size[dimId] = uitemp;
          }

          return num_points() != 0;
      }
      /// @}
  }; // class GridSize
} // namespace bk

#endif //BK_GRIDSIZE_H
