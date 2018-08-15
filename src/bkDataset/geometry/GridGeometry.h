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

#ifndef BK_GRIDGEOMETRY_H
#define BK_GRIDGEOMETRY_H

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include <bk/Matrix>
#include <bkMath/functions/list_grid_id_conversion.h>
#include <bk/KDTree>
#include <bkTypeTraits/has_index_operator.h>

#include <bkDataset/helpers/GridSize.h>
#include <bkDataset/geometry/GridGeometryIterator.h>

namespace bk
{
  template<int TDims = -1, typename TValue = double> class GridGeometry : public GridSize<TDims>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GridGeometry<TDims, TValue>;
      using base_type = GridSize<TDims>;
      using size_vector_type = typename base_type::size_vector_type;
    public:
      using value_type = TValue;
      using point_type = Vec<TDims, value_type>;

      /// @{ -------------------------------------------------- IS EXPLICIT GEOMETRY
      [[nodiscard]] static constexpr bool IsExplicit() noexcept
      { return false; }
      /// @}

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      GridGeometry() = default;
      GridGeometry(const self_type&) = default;
      GridGeometry(self_type&&) noexcept = default;

      template<typename... TIds, std::enable_if_t<std::conjunction_v<std::is_integral<TIds>...>>* = nullptr>
      GridGeometry(TIds... sizes_per_dimension)
          : base_type(sizes_per_dimension...)
      { static_assert(TDims == -1 || sizeof...(TIds) == TDims, "invalid number of arguments"); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GridGeometry() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET POINT
      [[nodiscard]] point_type point(unsigned int id)
      { return point(bk::list_to_grid_id(this->size(), id)); }

      [[nodiscard]] point_type point(unsigned int id) const
      { return point(bk::list_to_grid_id(this->size(), id)); }

      template<typename... TVecOrNumbers, std::enable_if_t<(sizeof...(TVecOrNumbers) > 1) || std::conjunction_v<bk::has_index_operator<std::decay_t<TVecOrNumbers>>...>>* = nullptr>
      [[nodiscard]] point_type point(TVecOrNumbers&& ... gidvec_or_ids)
      { return point_type(std::forward<TVecOrNumbers>(gidvec_or_ids)...); }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] GridGeometryIterator<TDims, TValue> begin()
      { return GridGeometryIterator<TDims, TValue>(*this, 0); }

      [[nodiscard]] GridGeometryIterator<TDims, TValue> begin() const
      { return GridGeometryIterator<TDims, TValue>(*this, 0); }

      [[nodiscard]] GridGeometryIterator<TDims, TValue> end()
      { return GridGeometryIterator<TDims, TValue>(*this, this->num_points()); }

      [[nodiscard]] GridGeometryIterator<TDims, TValue> end() const
      { return GridGeometryIterator<TDims, TValue>(*this, this->num_points()); }

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
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CALC CENTER
      [[nodiscard]] point_type center() const
      {
          const unsigned int N = this->num_points();

          if (N > 0)
          {
              auto c = bk::MatrixFactory::create(point(0));

              for (unsigned int i = 0; i < N; ++i)
              { c += point(i); }

              c /= N;

              return c;
          }

          return point_type();
      }
      /// @}

      /// @{ -------------------------------------------------- NEIGHBORHOOD QUERIES
      template<typename TVec>
      [[nodiscard]] bk::KDPointInfo<point_type> closest_point(const TVec& pos) const
      {
          static_assert(bk::has_index_operator_v<TVec>);

          bk::KDPointInfo<point_type> cp;
          cp.point = bk::MatrixFactory::create<value_type, TDims, 1>(this->num_dimensions(), 1);
          cp.distance_to_query = 0;

          for (unsigned int d = 0; d < this->num_dimensions(); ++d)
          {
              cp.point[d] = static_cast<unsigned int>(std::round(pos[d]));
              cp.point[d] = std::max(static_cast<value_type>(0), std::min(cp.point[d], static_cast<value_type>(this->size(d) - 1)));
              cp.distance_to_query += (cp.point[d] - pos[d]) * (cp.point[d] - pos[d]);
          }

          cp.distance_to_query = std::sqrt(cp.distance_to_query);
          cp.point_id = bk::grid_to_list_id(this->size(), cp.point);

          return cp;
      }
      /// @}
  }; // class GridGeometry
} // namespace bk

#endif //BK_GRIDGEOMETRY_H
