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

#ifndef BK_EXPLICITGEOMETRY_H
#define BK_EXPLICITGEOMETRY_H

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <bkAlgorithm/mean.h>
#include <bk/KDTree>
#include <bk/Matrix>
#include <bk/StringUtils>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  template<int TDims = -1, typename TValue = double> class ExplicitGeometry
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || TDims > 0);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ExplicitGeometry<TDims>;
    public:
      using value_type = TValue;
      using point_type = Vec<TDims, value_type>;
      using kdtree_type = KDTree<point_type>;
      using container_type = std::vector<point_type>;

      /// @{ -------------------------------------------------- IS EXPLICIT GEOMETRY
      [[nodiscard]] static constexpr bool IsExplicit() noexcept
      { return true; }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      std::vector<point_type> _points;
      std::unique_ptr<kdtree_type> _kdtree;
      bool _kdtree_is_up2date;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ExplicitGeometry()
          : _kdtree_is_up2date(false)
      { /* do nothing */ }

      ExplicitGeometry(const self_type& other)
          : _points(other._points),
            _kdtree_is_up2date(false)
      {
          if (other._kdtree_is_up2date)
          { construct_kd_tree(); }
      }

      ExplicitGeometry(self_type&& other)
          : _points(std::move(other._points)),
            _kdtree(std::move(other._kdtree)),
            _kdtree_is_up2date(other._kdtree_is_up2date)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ExplicitGeometry() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] unsigned int num_dimensions() const
      { return has_points() ? _points[0].num_elements() : 0U; }
      /// @}

      /// @{ -------------------------------------------------- GET NUM POINTS
      [[nodiscard]] unsigned int num_points() const
      { return _points.size(); }

      [[nodiscard]] bool has_points() const
      { return num_points() != 0; }
      /// @}

      /// @{ -------------------------------------------------- GET POINT
      [[nodiscard]] point_type& point(unsigned int id)
      { return _points[std::min(id, num_points() - 1)]; }

      [[nodiscard]] const point_type& point(unsigned int id) const
      { return _points[std::min(id, num_points() - 1)]; }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] typename container_type::iterator begin()
      { return _points.begin(); }

      [[nodiscard]] typename container_type::const_iterator begin() const
      { return _points.begin(); }

      [[nodiscard]] typename container_type::iterator end()
      { return _points.end(); }

      [[nodiscard]] typename container_type::const_iterator end() const
      { return _points.end(); }

      [[nodiscard]] typename container_type::reverse_iterator rbegin()
      { return _points.rbegin(); }

      [[nodiscard]] typename container_type::const_reverse_iterator rbegin() const
      { return _points.rbegin(); }

      [[nodiscard]] typename container_type::reverse_iterator rend()
      { return _points.rend(); }

      [[nodiscard]] typename container_type::const_reverse_iterator rend() const
      { return _points.rend(); }
      /// @}

      /// @{ -------------------------------------------------- GET KDTREE
      [[nodiscard]] const std::unique_ptr<kdtree_type>& kd_tree() const
      { return _kdtree; }

      [[nodiscard]] bool has_kdtree() const
      { return _kdtree_is_up2date && _kdtree.get() != nullptr; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type& other)
      {
          _points = other._points;
          _kdtree_is_up2date = false;

          if (other._kdtree_is_up2date)
          { construct_kd_tree(); }

          return *this;
      }

      [[maybe_unused]] self_type& operator=(self_type&& other) noexcept
      {
          _points = std::move(other._points);
          _kdtree = std::move(other._kdtree);
          _kdtree_is_up2date = other._kdtree_is_up2date;

          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- SET NUM POINTS
      void set_num_points(unsigned int n)
      {
          _points.resize(n);
          this->clear_kd_tree();
      }
      /// @}

      //====================================================================================================
      //===== ADD / REMOVE
      //====================================================================================================
      /// @{ -------------------------------------------------- PUSH BACK
      template<typename... TPoints>
      void push_back(TPoints&& ... vecs)
      {
          (_points.push_back(std::forward<TPoints>(vecs)), ...);
          clear_kd_tree();
      }
      /// @}

      /// @{ -------------------------------------------------- EMPLACE BACK
      template<typename... TArgs>
      void emplace_back(TArgs&& ... args)
      {
          _points.emplace_back(std::forward<TArgs>(args)...);
          clear_kd_tree();
      }
      /// @}

      /// @{ -------------------------------------------------- REMOVE
      [[maybe_unused]] bool remove(unsigned int i)
      {
          if (i < num_points())
          {
              _points.erase(_points.begin() + i);
              clear_kd_tree();
              return true;
          }

          return false;
      }
      /// @}

      //====================================================================================================
      //===== KD TREE
      //====================================================================================================
      /// @{ -------------------------------------------------- KDTREE CLEAR
      void clear_kd_tree()
      {
          _kdtree.reset();
          _kdtree_is_up2date = false;
      }
      /// @}

      /// @{ -------------------------------------------------- KDTREE INIT
      void construct_kd_tree()
      {
          clear_kd_tree();

          if (num_points() != 0)
          {
              _kdtree = std::make_unique<kdtree_type>();
              _kdtree->construct(begin(), end(), num_dimensions());
              _kdtree_is_up2date = true;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- KDTREE IS UP TO DATE
      bool kd_tree_is_up_to_date() const
      { return _kdtree_is_up2date && _kdtree.get() != nullptr; }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: KDTREE QUERIES
    private:
      template<typename TVec>
      [[nodiscard]] static constexpr bool _vector_is_same_size_static(const TVec& pos)
      { return bk::is_static_vector_of_size_v<TVec, point_type::NumElementsAtCompileTime()> || bk::is_dynamic_matrix_v<TVec>; }

    public:
      /// @}

      /// @{ -------------------------------------------------- CLOSEST POINT
      template<typename TVec>
      [[nodiscard]] bk::KDPointInfo<point_type> closest_point(const TVec& pos) const
      {
          //static_assert(_vector_is_same_size_static(pos));

          if (kd_tree_is_up_to_date())
          { return kd_tree()->nearest_neighbor(pos); }
          else
          {
              using kdpoint_info_type = typename kdtree_type::kdpoint_info_type;
              kdpoint_info_type closestPoint;
              closestPoint.distance_to_query = std::numeric_limits<double>::max();

              #pragma omp parallel for
              for (unsigned int pointId = 0; pointId < num_points(); ++pointId)
              {
                  const double sqDist = point(pointId).distance_squared(pos);

                  #pragma omp critical(closest_point_comparison)
                  {
                      if (sqDist < closestPoint.distance_to_query)
                      {
                          closestPoint.distance_to_query = sqDist;
                          closestPoint.point_id = pointId;
                      }
                  }
              }

              closestPoint.distance_to_query = std::sqrt(closestPoint.distance_to_query);
              closestPoint.point = point(closestPoint.point_id);

              return closestPoint;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- POINTS WITHIN RADIUS
      template<typename TVec> [[nodiscard]] std::vector<bk::KDPointInfo<point_type>> points_within_radius(const TVec& pos, double radius) const
      {
          static_assert(_vector_is_same_size_static(pos));

          if (kd_tree_is_up_to_date())
          { return kd_tree()->neighbors_within_radius(pos, radius); }
          else
          {
              using kdpoint_info_type = typename kdtree_type::kdpoint_info_type;
              std::vector<kdpoint_info_type> closestPoints;

              const double sqRadius = radius * radius;

              #pragma omp parallel for
              for (unsigned int pointId = 0; pointId < num_points(); ++pointId)
              {
                  const double sqDist = point(pointId).distance_squared(pos);

                  if (sqDist < sqRadius)
                  {
                      kdpoint_info_type closestPoint;
                      closestPoint.distance_to_query = std::sqrt(sqDist);
                      closestPoint.point_id = pointId;
                      closestPoint.point = point(pointId);

                      #pragma omp critical(points_within_radius_comparison)
                      { closestPoints.emplace_back(std::move(closestPoint)); }
                  }
              }

              return closestPoints;
          }
      }
      /// @}

      /// @{ -------------------------------------------------- CLOSEST N POINTS
      template<typename TVec> [[nodiscard]] std::vector<bk::KDPointInfo<point_type>> closest_n_points(const TVec& pos, unsigned int n) const
      {
          static_assert(_vector_is_same_size_static(pos));

          if (kd_tree_is_up_to_date())
          { return kd_tree()->k_nearest_neighbors(pos, n); }
          else
          {
              using pdist_pair_type = std::pair<unsigned int /*pointId*/, double /*distance squared*/>;

              std::vector<pdist_pair_type> points_distances(num_points());

              #pragma omp parallel for
              for (unsigned int pointId = 0; pointId < num_points(); ++pointId)
              {
                  const double sqDist = point(pointId).distance_squared(pos);

                  points_distances[pointId].first = pointId;
                  points_distances[pointId].second = sqDist;
              }

              std::sort(points_distances.begin(), points_distances.end(), [](const pdist_pair_type& a, const pdist_pair_type& b)
              { return a.second < b.second; });

              const unsigned int _n = std::min(n, static_cast<unsigned int>(points_distances.size()));

              using kdpoint_info_type = typename kdtree_type::kdpoint_info_type;
              std::vector<kdpoint_info_type> closestPoints(_n);

              #pragma omp parallel for
              for (unsigned int i = 0; i < _n; ++i)
              {
                  closestPoints[i].distance_to_query = std::sqrt(points_distances[i].second);
                  closestPoints[i].point_id = points_distances[i].first;
                  closestPoints[i].point = point(closestPoints[i].point_id);
              }

              return closestPoints;
          }
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear()
      {
          _points.clear();
          clear_kd_tree();
      }
      /// @}

      /// @{ -------------------------------------------------- RESERVE
      void reserve(unsigned int n)
      { _points.reserve(n); }
      /// @}

      /// @{ -------------------------------------------------- CALCULATE CENTER
      [[nodiscard]] auto center() const
      { return bk::mean(begin(), end()); }
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
          const std::string suffix = ".egeom";

          if (fname.empty())
          { fname = "explicit-geometry" + suffix; }
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

          std::uint8_t numDimensions = num_dimensions();
          file.write(reinterpret_cast<char*>(&numDimensions), sizeof(std::uint8_t));

          std::uint32_t numPoints = num_points();
          file.write(reinterpret_cast<char*>(&numPoints), sizeof(std::uint32_t));

          for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
          {
              const point_type& v = point(pointId);

              for (std::uint8_t dimId = 0; dimId < numDimensions; ++dimId)
              {
                  double dtemp = static_cast<double>(v[dimId]);
                  file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
              }
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
          if (!string_utils::ends_with(filename.data(), ".egeom"))
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
          clear();

          if (!file.is_open() || !file.good())
          { return false; }

          std::uint8_t numDimensions;
          file.read(reinterpret_cast<char*>(&numDimensions), sizeof(std::uint8_t));

          std::uint32_t numPoints;
          file.read(reinterpret_cast<char*>(&numPoints), sizeof(std::uint32_t));

          set_num_points(numPoints);

          std::vector<double> pointBuffer(numDimensions * numPoints);
          file.read(reinterpret_cast<char*>(pointBuffer.data()), numDimensions * numPoints * sizeof(double));

          #pragma omp parallel for
          for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
          {
              point_type& v = point(pointId);

              for (std::uint8_t dimId = 0; dimId < numDimensions; ++dimId)
              { v[dimId] = pointBuffer[numDimensions * pointId + dimId]; }
          }

          return num_points() != 0;
      }
      /// @}
  }; // class ExplicitGeometry
} // namespace bk

#endif //BK_EXPLICITGEOMETRY_H
