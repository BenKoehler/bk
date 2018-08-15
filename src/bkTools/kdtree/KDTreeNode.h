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

#ifndef BKTOOLS_KDTREENODE_H
#define BKTOOLS_KDTREENODE_H

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>

namespace bk
{
  template<typename TPoint, typename TValue = typename TPoint::value_type> class KDTreeNode
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = KDTreeNode<TPoint, TValue>;

    public:
      using point_type = TPoint;
      using value_type = TValue;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      unsigned int _split_dim;
      value_type _split_value;
      std::unique_ptr<self_type> _left;
      std::unique_ptr<self_type> _right;
      std::unique_ptr<point_type> _point;
      unsigned int _point_id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      KDTreeNode()
          : _split_dim(0),
            _split_value(0),
            _left(nullptr),
            _right(nullptr),
            _point(nullptr),
            _point_id(0)
      { /* do nothing */ }

      KDTreeNode(const self_type&) = delete;
      KDTreeNode(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET SPLIT DIM
      [[nodiscard]] unsigned int split_dim() const
      { return _split_dim; }
      /// @}

      /// @{ -------------------------------------------------- GET SPLIT VALUE
      [[nodiscard]] value_type split_value() const
      { return _split_value; }
      /// @}

      /// @{ -------------------------------------------------- GET CHILD NODES
      [[nodiscard]] std::unique_ptr<self_type>& left()
      { return _left; }

      [[nodiscard]] const std::unique_ptr<self_type>& left() const
      { return _left; }

      [[nodiscard]] std::unique_ptr<self_type>& right()
      { return _right; }

      [[nodiscard]] const std::unique_ptr<self_type>& right() const
      { return _right; }

      [[nodiscard]] bool has_left() const
      { return _left != nullptr; }

      [[nodiscard]] bool has_right() const
      { return _right != nullptr; }

      [[nodiscard]] bool is_leaf() const
      { return !has_left() && !has_right(); }
      /// @}

      /// @{ -------------------------------------------------- GET POINT
      [[nodiscard]] std::unique_ptr<point_type>& point()
      { return _point; }

      [[nodiscard]] const std::unique_ptr<point_type>& point() const
      { return _point; }

      [[nodiscard]] unsigned int& point_id()
      { return _point_id; }

      [[nodiscard]] const unsigned int& point_id() const
      { return _point_id; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&)  = delete;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET SPLIT VALUE
      void set_split_value(value_type x)
      { _split_value = x; }
      /// @}

      /// @{ -------------------------------------------------- SET SPLIT DIM
      void set_split_dim(unsigned int d)
      { _split_dim = d; }
      /// @}
      
      //====================================================================================================
      //===== SETUP
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCT
      template<typename TIterator>
      void construct(TIterator first, TIterator last, unsigned int numDimensions)
      {
          // http://andrewd.ces.clemson.edu/courses/cpsc805/references/nearest_search.pdf
          // https://courses.cs.washington.edu/courses/cse373/02au/lectures/lecture22l.pdf
          // https://www.cise.ufl.edu/class/cot5520fa09/CG_RangeKDtrees.pdf

          // determine number of input points
          const unsigned int N = static_cast<unsigned int>(std::distance(first, last));

          if (N > 1)
          {
              double maxDimDiff = 0;
              _split_dim = 0;

              for (unsigned int d = 0; d < numDimensions; ++d)
              {
                  const auto minmax = std::minmax_element(first, last, [=](const std::pair<point_type, unsigned int>& pt0, const std::pair<point_type, unsigned int>& pt1) -> bool
                  { return pt0.first[d] < pt1.first[d]; });

                  const double dimdiff = std::abs(minmax.second->first[d] - minmax.first->first[d]);

                  if (dimdiff > maxDimDiff)
                  {
                      maxDimDiff = dimdiff;
                      _split_dim = d;
                  }
              }

              #if defined(__GNUC__) && defined(_OPENMP)
              __gnu_parallel
              #else
              std
              #endif
              ::sort(first, last, [=](const std::pair<point_type, unsigned int>& p0, const std::pair<point_type, unsigned int>& p1) -> bool
              { return p0.first[_split_dim] < p1.first[_split_dim]; });

              // determine split value as median
              _split_value = (*(first + (N / 2))).first[_split_dim];
              _left = std::make_unique<KDTreeNode>();
              _left->construct(first, first + N / 2, numDimensions);
              _right = std::make_unique<KDTreeNode>();
              _right->construct(first + N / 2, last, numDimensions);
          }
          else // if (N == 1)
          {
              _point = std::make_unique<point_type>(first->first);
              _point_id = first->second;
          }
      }
      /// @}
  }; // class KDTreeNode
} // namespace bk

#endif //BKTOOLS_KDTREENODE_H


