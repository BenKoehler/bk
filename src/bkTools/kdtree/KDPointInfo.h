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

#ifndef BKTOOLS_KDPOINTINFO_H
#define BKTOOLS_KDPOINTINFO_H

namespace bk
{
  template<typename TPoint> struct KDPointInfo
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = KDPointInfo<TPoint>;
    public:
      using point_type = TPoint;
      
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      point_type point;
      unsigned int point_id;
      double distance_to_query;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      constexpr KDPointInfo()
          : point(point_type()),
            point_id(0),
            distance_to_query(0)
      { /* do nothing */ }

      constexpr KDPointInfo(const self_type&) = default;
      constexpr KDPointInfo(self_type&&) noexcept = default;

      constexpr KDPointInfo(const point_type& point, unsigned int pointId, double dst)
          : point(point),
            point_id(pointId),
            distance_to_query(dst)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~KDPointInfo() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}
  };
} // namespace bk

#endif //BKTOOLS_KDPOINTINFO_H


