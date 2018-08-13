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

#ifndef BKTOOLS_KDTREE_H
#define BKTOOLS_KDTREE_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>

#else
    #include <algorithm>
#endif

#include <cmath>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "KDTreeNode.h"
#include "KDPointInfo.h"

namespace bk
{

  template<typename TPoint, typename TValue = typename TPoint::value_type> class KDTree
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = KDTree<TPoint, TValue>;
    public:
      using point_type = TPoint;
      using value_type = TValue;
      using node_type = KDTreeNode<point_type, value_type>;
      using kdpoint_info_type = KDPointInfo<point_type>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      node_type _root;
      unsigned int _num_dims;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      KDTree() = default;
      KDTree(const self_type&) = delete;
      KDTree(self_type&&) = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMS
      [[nodiscard]] unsigned int num_dims() const
      { return _num_dims; }
      /// @}
      
      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = delete;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

      //====================================================================================================
      //===== SETUP
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCT
      template<typename TPointIterator>
      void construct(TPointIterator first, TPointIterator last, unsigned int numDimensions)
      {
          _num_dims = numDimensions;

          // determine number of input points
          const unsigned int N = static_cast<unsigned int>(std::distance(first, last));

          // copy input point + index
          std::vector<std::pair<point_type, unsigned int>> p(N);

          for (unsigned int i = 0; i < N; ++i, ++first)
          {
              p[i].first = *first;
              p[i].second = i;
          }

          _root.construct(p.begin(), p.end(), _num_dims);
      }
      /// @}

      //====================================================================================================
      //===== QUERIES
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: DISTANCE
    private:
      [[nodiscard]] double _distance(const point_type& p, const point_type& q) const
      {
          double d = 0.0;

          for (unsigned int i = 0; i < _num_dims; ++i)
          {
              const double diff = p[i] - q[i];
              d += diff*diff;
          }

          return std::sqrt(d);
      }
      /// @}

      /// @{ -------------------------------------------------- NEAREST NEIGHBOR
    private:
      void _nearest_neighbor_search(const point_type& query, const node_type* n, kdpoint_info_type& p) const
      {
          // http://andrewd.ces.clemson.edu/courses/cpsc805/references/nearest_search.pdf
          // -> slide 15

          if (n->is_leaf())
          {
              const double w0 = _distance(query, *n->point());

              if (w0 < p.distance_to_query)
              {
                  p.distance_to_query = w0;
                  p.point = *n->point();
                  p.point_id = n->point_id();
              }
          }
          else
          {
              if (query[n->split_dim()] <= n->split_value())
              { // search left first
                  if (query[n->split_dim()] - p.distance_to_query <= n->split_value())
                  { _nearest_neighbor_search(query, n->left().get(), p); }

                  if (query[n->split_dim()] + p.distance_to_query > n->split_value())
                  { _nearest_neighbor_search(query, n->right().get(), p); }
              }
              else
              { // search right first
                  if (query[n->split_dim()] + p.distance_to_query > n->split_value())
                  { _nearest_neighbor_search(query, n->right().get(), p); }

                  if (query[n->split_dim()] - p.distance_to_query <= n->split_value())
                  { _nearest_neighbor_search(query, n->left().get(), p); }
              }
          }
      }

    public:

      [[nodiscard]] kdpoint_info_type nearest_neighbor(const point_type& query_pos) const
      {
          kdpoint_info_type nn;
          nn.distance_to_query = std::numeric_limits<double>::max();

          _nearest_neighbor_search(query_pos, &this->_root, nn);

          return nn;
      }
      /// @}

      /// @{ -------------------------------------------------- RADIUS SEARCH
    private:
      void _radius_search(const point_type& query, double radius, const node_type* n, std::vector<kdpoint_info_type>& p) const
      {
          if (n->is_leaf())
          {
              const double w0 = _distance(query, *n->point());

              if (w0 <= radius)
              {
                  kdpoint_info_type x;
                  x.point = *n->point();
                  x.point_id = n->point_id();
                  x.distance_to_query = w0;
                  p.emplace_back(std::move(x));
              }
          }
          else
          {
              if (query[n->split_dim()] <= n->split_value())
              { // search left first
                  if (query[n->split_dim()] - radius <= n->split_value())
                  { _radius_search(query, radius, n->left().get(), p); }

                  if (query[n->split_dim()] + radius > n->split_value())
                  { _radius_search(query, radius, n->right().get(), p); }
              }
              else
              { // search right first
                  if (query[n->split_dim()] + radius > n->split_value())
                  { _radius_search(query, radius, n->right().get(), p); }

                  if (query[n->split_dim()] - radius <= n->split_value())
                  { _radius_search(query, radius, n->left().get(), p); }
              }
          }
      }

    public:
      [[nodiscard]] std::vector<kdpoint_info_type> neighbors_within_radius(const point_type& query_pos, double radius) const
      {
          std::vector<kdpoint_info_type> knn;

          _radius_search(query_pos, radius, &this->_root, knn);

          // sort ascendingly by distance to query point
          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel::
          #else
          std::
          #endif
          sort(std::begin(knn), std::end(knn), [](const kdpoint_info_type& a, const kdpoint_info_type& b) -> bool
          { return a.distance_to_query < b.distance_to_query; });

          return knn;
      }
      /// @}

      /// @{ -------------------------------------------------- K NEAREST NEIGHBORS
    private:
      void _knn_search(const point_type& query, unsigned int k, const node_type* n, std::vector<kdpoint_info_type>& p, double& maxDist) const
      {
          if (n->is_leaf())
          {
              const double w0 = _distance(query, *n->point());

              if (w0 <= maxDist)
              {
                  kdpoint_info_type x;
                  x.point = *n->point();
                  x.point_id = n->point_id();
                  x.distance_to_query = w0;

                  typename std::vector<kdpoint_info_type>::iterator it = std::begin(p);

                  while (it != std::end(p) && it->distance_to_query <= w0)
                  { ++it; }

                  p.insert(it, std::move(x));

                  if (p.size() > k)
                  { p.pop_back(); }

                  if (p.size() == k)
                  { maxDist = p.back().distance_to_query; }
              }
          }
          else
          {
              if (query[n->split_dim()] <= n->split_value())
              { // search left first
                  if (query[n->split_dim()] - maxDist <= n->split_value())
                  { _knn_search(query, k, n->left().get(), p, maxDist); }

                  if (query[n->split_dim()] + maxDist > n->split_value())
                  { _knn_search(query, k, n->right().get(), p, maxDist); }
              }
              else
              { // search right first
                  if (query[n->split_dim()] + maxDist > n->split_value())
                  { _knn_search(query, k, n->right().get(), p, maxDist); }

                  if (query[n->split_dim()] - maxDist <= n->split_value())
                  { _knn_search(query, k, n->left().get(), p, maxDist); }
              }
          }
      }

    public:
      [[nodiscard]] std::vector<kdpoint_info_type> k_nearest_neighbors(const point_type& query_pos, unsigned int k) const
      {
          std::vector<kdpoint_info_type> knn;
          double maxDist = std::numeric_limits<double>::max();

          _knn_search(query_pos, k, &this->_root, knn, maxDist);

          // knn is already sorted ascendingly

          return knn;
      }
      /// @}
  }; // class KDTree
} // namespace bk

#endif //BKTOOLS_KDTREE_H


