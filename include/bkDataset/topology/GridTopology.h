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

#ifndef BK_GRIDTOPOLOGY_H
#define BK_GRIDTOPOLOGY_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

#include "../../bkMath/functions/list_grid_id_conversion.h"
#include "../../bkTypeTraits/variadic_template_helpers.h"
#include "../../bkTypeTraits/has_index_operator.h"
#include "../../bkTools/NDContainer"

#include "Cell.h"
#include "GridTopologyIterator.h"
#include "../helpers/GridSize.h"

namespace bk
{
  template<int TDims = -1> class GridTopology : public GridSize<TDims>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GridTopology<TDims>;
      using base_type = GridSize<TDims>;
      using size_vector_type = typename base_type::size_vector_type;
    public:
      using cell_type = std::conditional_t<(TDims > 0), Cell<static_cast<int>(std::pow(2, TDims))>, Cell<-1>>;

      /// @{ -------------------------------------------------- IS EXPLICIT TOPOLOGY
      [[nodiscard]] static constexpr bool IsExplicit() noexcept
      { return false; }
      /// @}

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      GridTopology() = default;
      GridTopology(const GridTopology&) = default;
      GridTopology(GridTopology&&) = default;

      template<typename... TIds, std::enable_if_t<std::conjunction_v<std::is_integral<TIds>...>>* = nullptr>
      GridTopology(TIds... sizes_per_dimension)
          : base_type(sizes_per_dimension...)
      { static_assert(TDims == -1 || sizeof...(TIds) == TDims, "invalid number of arguments"); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GridTopology() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM CELLS
      [[nodiscard]] unsigned int num_cells(unsigned int dimId) const
      {
          return this->size(dimId) - 1;

          // -1 because there is a line/row/... break at the end of each dimension.
          //
          // E.g., the 3x3 2D image with indices as follows
          //          (2,5,8)
          //          (1,4,7)
          //          (0,3,6)
          // has 4 cells:
          //          0 3 1 4
          //          1 4 2 5
          //          3 6 4 7
          //          4 7 5 8
          //
          // There are no cells above 2,5 or 5,8
      }

      [[nodiscard]] unsigned int num_cells() const
      {
          unsigned int N = 1;

          for (unsigned int dimId = 0; dimId < this->num_dimensions(); ++dimId)
          { N *= num_cells(dimId); }

          return N;
      }
      /// @}

      /// @{ -------------------------------------------------- GET CELL SIZE
      [[nodiscard]] auto cell_size() const
      {
          auto s = this->size();

          for (auto& x: s)
          { x -= 1; }

          return s;
      }

      [[nodiscard]] unsigned int cell_size(unsigned int dimId) const
      { return num_cells(dimId); }
      /// @}

      /// @{ -------------------------------------------------- GET NUM IDS PER CELL
      [[nodiscard]] unsigned int num_pointIds_per_cell() const
      { return std::pow(2, this->num_dimensions()); }
      /// @}

    private:
      /// @{ -------------------------------------------------- HELPERS: GET CELL
      template<typename TVec>
      [[nodiscard]] unsigned int _cell_id_of_grid_pos(const TVec& gid) const
      {
          size_vector_type s(this->size());
          for (unsigned int& dimSize: s)
          { --dimSize; }

          size_vector_type res;

          if constexpr (TDims == -1)
          { res.resize(this->num_dimensions()); }

          for (unsigned int dimId = 0; dimId < this->num_dimensions(); ++dimId)
          { res[dimId] = std::clamp(static_cast<int>(gid[dimId]), 0, static_cast<int>(this->size(dimId)) - 2); }

          return bk::grid_to_list_id(s, res, this->num_dimensions());
      }

      void _compose_cell(unsigned int dimId, cell_type& c, const size_vector_type& gidbase, std::vector<unsigned int>& gidoff) const
      {
          if (dimId != 0)
          {
              for (unsigned int i = 0; i < 2; ++i)
              {
                  gidoff[dimId] = i;
                  _compose_cell(dimId - 1, c, gidbase, gidoff);
              }
          }
          else
          {
              for (unsigned int i = 0; i < 2; ++i)
              {
                  gidoff[dimId] = i;

                  // calculate cellid
                  // - interpret offset as binary number -> convert to index
                  // - e.g. 3D offset = (1,0,1) -> index = 1*2^2 + 0*2^1 + 1*2^0 = 5
                  unsigned int cellid = 0;
                  for (unsigned int d = 0; d < gidbase.size(); ++d)
                  { cellid += gidoff[d] * std::pow(2, d); }

                  std::vector<unsigned int> tempgid;
                  tempgid.reserve(gidbase.size());

                  std::transform(gidbase.begin(), gidbase.end(), gidoff.begin(), std::back_inserter(tempgid), [](unsigned int x, unsigned int y)
                  { return x + y; });

                  c[cellid] = bk::grid_to_list_id(this->size(), tempgid, this->num_dimensions());
              }
          }
      }
      /// @}
    public:

      /// @{ -------------------------------------------------- GET CELL
      [[nodiscard]] cell_type cell(unsigned int cellId) const
      {
          assert(cellId < num_cells() && "cellId out of bounds");

          // - goal is to obtain the lower left front (etc.) point (gid) of the cell
          // - size-1 is used to calculate the gid since there is always a break above the top row, rightest col etc.
          // - for illustration see the comment in num_cells()
          size_vector_type sod(this->size());
          for (unsigned int& dimSize: sod)
          { --dimSize; }

          const size_vector_type gidbase = bk::list_to_grid_id(sod, cellId);
          std::vector<unsigned int> gidoff(this->num_dimensions(), 0);

          cell_type c;

          if constexpr (TDims == -1)
          { c.set_size(num_pointIds_per_cell()); }

          _compose_cell(this->num_dimensions() - 1, c, gidbase, gidoff);
          return c;
      }

      template<typename TVec, std::enable_if_t<bk::has_index_operator_v<TVec>>* = nullptr>
      [[nodiscard]] cell_type cell(const TVec& gridPosVector)
      { return cell(_cell_id_of_grid_pos(gridPosVector)); }

      template<typename TVec, std::enable_if_t<bk::has_index_operator_v<TVec>>* = nullptr>
      [[nodiscard]] cell_type cell(const TVec& gridPosVector) const
      { return cell(_cell_id_of_grid_pos(gridPosVector)); }

      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      [[nodiscard]] cell_type cell(TIds... ids)
      {
          static_assert(TDims == -1 || TDims == sizeof...(TIds), "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_integral<TIds>...>, "all ids must be integral types");
          assert(sizeof...(TIds) == this->num_dimensions() && "invalid number of arguments");
          assert(bk::index_pack_all_greater_or_equal(0, ids...) && "all ids must be >= 0");

          return cell(_cell_id_of_grid_pos(std::array<unsigned int, sizeof...(TIds)>{static_cast<unsigned int>(ids)...}));
      }

      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      [[nodiscard]] cell_type cell(TIds... ids) const
      {
          static_assert(TDims == -1 || TDims == sizeof...(TIds), "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_integral<TIds>...>, "all ids must be integral types");
          assert(sizeof...(TIds) == this->num_dimensions() && "invalid number of arguments");
          assert(bk::index_pack_all_greater_or_equal(0, ids...) && "all ids must be >= 0");

          return cell(_cell_id_of_grid_pos(std::array<unsigned int, sizeof...(TIds)>{static_cast<unsigned int>(ids)...}));
      }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] GridTopologyIterator<TDims> begin()
      { return GridTopologyIterator<TDims>(*this, 0); }

      [[nodiscard]] GridTopologyIterator<TDims> begin() const
      { return GridTopologyIterator<TDims>(*this, 0); }

      [[nodiscard]] GridTopologyIterator<TDims> end()
      { return GridTopologyIterator<TDims>(*this, num_cells()); }

      [[nodiscard]] GridTopologyIterator<TDims> end() const
      { return GridTopologyIterator<TDims>(*this, num_cells()); }

      [[nodiscard]] auto rbegin() -> std::reverse_iterator<decltype(begin())>
      { return std::reverse_iterator<decltype(begin())>(begin()); }

      [[nodiscard]] auto rbegin() const -> std::reverse_iterator<decltype(begin())>
      { return std::reverse_iterator<decltype(begin())>(begin()); }

      [[nodiscard]] auto rend() -> std::reverse_iterator<decltype(end())>
      { return std::reverse_iterator<decltype(end())>(end()); }

      [[nodiscard]] auto rend() const -> std::reverse_iterator<decltype(end())>
      { return std::reverse_iterator<decltype(end())>(end()); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] GridTopology& operator=(const GridTopology&) = default;
      [[maybe_unused]] GridTopology& operator=(GridTopology&&) = default;
      /// @}

      //====================================================================================================
      //===== CONNECTIVITY
      //====================================================================================================
      /// @{ -------------------------------------------------- NEIGHBORS OF POINT
      [[nodiscard]] std::vector<unsigned int> neighbors_of_point(unsigned int pointId) const
      {
          std::vector<unsigned int> ids;

          const size_vector_type _gid = bk::list_to_grid_id(this->size(), pointId);

          std::vector<int> gid(this->num_dimensions());
          std::copy(_gid.begin(), _gid.end(), gid.begin());

          std::vector<int> temp(gid);

          for (unsigned int d = 0; d < this->num_dimensions(); ++d)
          {
              if (gid[d] > 0)
              {
                  temp[d] = static_cast<int>(gid[d]) - 1;
                  ids.push_back(grid_to_list_id(this->size(), temp, this->num_dimensions()));
              }

              if (gid[d] < static_cast<int>(this->size(d)) - 1)
              {
                  temp[d] = static_cast<int>(gid[d]) + 1;
                  ids.push_back(grid_to_list_id(this->size(), temp, this->num_dimensions()));
              }

              temp[d] = gid[d]; //reset
          }

          std::sort(ids.begin(), ids.end());

          const unsigned int numPoints = this->num_points();
          ids.erase(std::remove_if(ids.begin(), ids.end(), [&](unsigned int i) -> bool
          { return i >= numPoints; }), ids.end());

          return ids;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: CELLS OF POINT
    private:
      void _cells_of_point(int dimId, const size_vector_type& cell_grid_size, const size_vector_type& gidbase, std::vector<int>& gidoff, std::vector<unsigned int>& ids) const
      {
          if (dimId != 0)
          {
              for (int i = -1; i < 1; ++i)
              {
                  gidoff[dimId] = i;
                  _cells_of_point(dimId - 1, cell_grid_size, gidbase, gidoff, ids);
              }
          }
          else
          {
              for (int i = -1; i < 1; ++i)
              {
                  gidoff[dimId] = i;

                  std::vector<int> tempgid;
                  tempgid.reserve(gidbase.size());

                  std::transform(gidbase.begin(), gidbase.end(), gidoff.begin(), std::back_inserter(tempgid), [](unsigned int x, int y)
                  { return static_cast<int>(x) + y; });

                  bool valid = true;

                  for (unsigned int k = 0; k < this->num_dimensions(); ++k)
                  {
                      if (tempgid[k] < 0 || tempgid[k] >= static_cast<int>(this->size(k)) - 1)
                      {
                          valid = false;
                          break;
                      }
                  }

                  if (valid)
                  { ids.push_back(bk::grid_to_list_id(cell_grid_size, tempgid, this->num_dimensions())); }
              }

              // remove invalid cells of boundary points
              ids.erase(std::remove_if(ids.begin(), ids.end(), [&](unsigned int i) -> bool
              { return i >= num_cells(); }), ids.end());
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- CELLS OF POINT
      [[nodiscard]] std::vector<unsigned int> cells_of_point(unsigned int pointId) const
      {
          assert(pointId < this->num_points() && "pointId out of bounds");

          std::vector<unsigned int> ids;

          const size_vector_type gidbase = bk::list_to_grid_id(this->size(), pointId);

          size_vector_type sod(this->size());
          for (unsigned int& dimSize: sod)
          { --dimSize; }

          std::vector<int> gidoff(this->num_dimensions(), 0);

          _cells_of_point(this->num_dimensions() - 1, sod, gidbase, gidoff, ids);

          std::sort(ids.begin(), ids.end());

          return ids;
      }

      template<typename TVec, std::enable_if_t<bk::has_index_operator_v<TVec>>* = nullptr>
      [[nodiscard]] std::vector<unsigned int> cells_of_point(const TVec& gridPosVector) const
      { return cells_of_point(grid_to_list_id(this->size(), gridPosVector, this->num_dimensions())); }

      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      [[nodiscard]] std::vector<unsigned int> cells_of_point(TIds... ids) const
      {
          static_assert(TDims == -1 || TDims == sizeof...(TIds), "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_integral<TIds>...>, "all ids must be integral types");
          assert(sizeof...(TIds) == this->num_dimensions() && "invalid number of arguments");
          assert(bk::index_pack_all_greater_or_equal(0, ids...) && "all ids must be >= 0");

          return cells_of_point(grid_to_list_id(this->size(), ids...));
      }
      /// @}
  }; // class GridTopology
} // namespace bk

#endif //BK_GRIDTOPOLOGY_H
