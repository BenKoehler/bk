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

#ifndef BKMATH_LIST_GRID_ID_CONVERSION_H
#define BKMATH_LIST_GRID_ID_CONVERSION_H

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

#include <bkTypeTraits/has_num_elements_at_compile_time.h>

namespace bk
{
  /// @{ -------------------------------------------------- LIST TO GRID ID
  //! convert a list id (lid) to grid coordinates in a regular grid
  /*!
   *  - dimensionality is automatically obtained from the given size vector
   *  - last dimension has lowest stride, e.g. for a 2D 3x3 image:
   *          -> x0y0 x0y1 x0y2 x1y0 x1y1 x1y2 x2y0 x2y1 x2y2
   * 
   *  \param lid 1D list id
   *  \param size size per dimension of the regular grid
   * 
   *  \return int-vector with grid coordinates
   * 
   */
  namespace
  {
    template<typename TIndexAccessible0, typename TIndexAccessible1, typename TIndexAccessible2>
    constexpr TIndexAccessible2 _list_to_grid_id(const TIndexAccessible0& size, unsigned int nDims, TIndexAccessible1&& temp, TIndexAccessible2&& gid, unsigned int lid)
    {
        assert(nDims != 0);

        for (unsigned int i = 0; i < nDims - 1; ++i)
        {
            temp[i] = 1;

            for (unsigned int k = i + 1; k < nDims; ++k)
            { temp[i] *= size[k]; }
        }

        for (unsigned int i = 0; i < nDims - 1; ++i)
        {
            gid[i] = lid;

            for (unsigned int k = 0; k < i; ++k)
            { gid[i] -= temp[k] * gid[k]; }

            gid[i] /= temp[i] != 0 ? temp[i] : 1;
        }

        gid[nDims - 1] = lid;
        for (unsigned int i = 0; i < nDims - 1; ++i)
        { gid[nDims - 1] -= temp[i] * gid[i]; }

        return std::forward<TIndexAccessible2>(gid);
    }
  } // anonymous namespace

  template<typename TRandomAccessIterator>
  constexpr auto list_to_grid_id(TRandomAccessIterator sizeIteratorFirst, TRandomAccessIterator sizeIteratorLast, unsigned int lid)
  {
      const std::vector<unsigned int> size(sizeIteratorFirst, sizeIteratorLast);
      const unsigned int nDims = size.size();
      return _list_to_grid_id(size, nDims, std::vector<unsigned int>(nDims), std::vector<unsigned int>(nDims), lid);
  }

  template<std::size_t N, typename T>
  constexpr std::array<unsigned int, N> list_to_grid_id(const std::array<T, N>& size, unsigned int lid)
  { return _list_to_grid_id(size, N, std::array<unsigned int, N>(), std::array<unsigned int, N>(), lid); }

  template<typename T>
  std::vector<unsigned int> list_to_grid_id(const std::vector<T>& size, unsigned int lid)
  {
      const unsigned int N = size.size();
      return _list_to_grid_id(size, N, std::vector<unsigned int>(N), std::vector<unsigned int>(N), lid);
  }

  template<typename T>
  constexpr auto list_to_grid_id(const T& size, unsigned int lid)
  {
      if constexpr (has_num_elements_at_compile_time_v<T>)
      {
          constexpr int N = T::NumElementsAtCompileTime();
          return _list_to_grid_id(size, N, std::array<unsigned int, N>(), std::array<unsigned int, N>(), lid);
      }
      else
      { return list_to_grid_id(size.begin(), size.end(), lid); }
  }
  /// @}

  /// @{ -------------------------------------------------- GRID COORDINATES TO LIST ID
  //! convert ND grid coordinates to list index
  /*!
   *  - dimensionality is automatically obtained from the given size vector
   * 
   *  \param size size per dimension of the regular grid
   *  \param gid grid coordinates as vector
   * 
   *  \return list id
   */
  namespace
  {
    template<typename TIndexAccessible, typename T0_>
    constexpr unsigned int _grid_to_list_id_var_tpl(unsigned int d, const TIndexAccessible& s, unsigned int N, T0_ gid0)
    {
        auto templid = std::round(gid0);

        for (unsigned int i = d + 1; i < N; ++i)
        { templid *= s[i]; }

        return templid;
    }

    template<typename TIndexAccessible, typename T0_, typename... T_>
    constexpr unsigned int _grid_to_list_id_var_tpl(unsigned int d, const TIndexAccessible& s, unsigned int N, T0_ gid0, T_... gidn)
    {
        auto templid = std::round(gid0);

        for (unsigned int i = d + 1; i < N; ++i)
        { templid *= s[i]; }

        return templid + _grid_to_list_id_var_tpl(d + 1, s, N, gidn...);
    }
  } // anonymous namespace

  template<typename TIndexAccessible, typename... T_, std::enable_if_t<std::is_class_v<TIndexAccessible> && std::conjunction_v<std::is_arithmetic<std::decay_t<T_>>...>>* = nullptr>
  constexpr unsigned int grid_to_list_id(const TIndexAccessible& size, T_... gidn)
  { return _grid_to_list_id_var_tpl(0, size, sizeof...(gidn), gidn...); }

  // ------------------

  namespace
  {
    template<typename TIndexAccessible0, typename TIndexAccessible1>
    constexpr unsigned int _grid_to_list_id(const TIndexAccessible0& size, unsigned int nDims, const TIndexAccessible1& gid)
    {
        unsigned int lid = 0;

        for (unsigned int id = 0; id < nDims; ++id)
        {
            auto temp = std::round(gid[id]);

            for (unsigned int i = id + 1; i < nDims; ++i)
            { temp *= size[i]; }

            lid += temp;
        }

        return lid;
    }
  } // anonymous namespace

  template<typename TIndexAccessible0, typename TIndexAccessible1, std::enable_if_t<std::is_class_v<TIndexAccessible0> && std::is_class_v<TIndexAccessible1>>* = nullptr>
  constexpr unsigned int grid_to_list_id(const TIndexAccessible0& size, const TIndexAccessible1& gid, unsigned int numDimensions)
  { return _grid_to_list_id(size, numDimensions, gid); }
  /// @}

  /// @{ -------------------------------------------------- STRIDE OF DIMENSION
  //! calculates the stride of the given dimension in a regular grid of given size
  /*!
   * - Determines index offsets.
   * - In general, the stride of a dimension is the product of all further dimensions' sizes.
   * - E.g. the following 2D 3x3-sized image:
   *             (2,5,8)
   *             (1,4,7)  is stored internally as (0,1,2,3,4,5,6,7,8)
   *             (0,3,6)
   *     -> y-elements have a stride of 1
   *     -> x-elements have a stride of size.y = 3
   *
   * - In the bk library, the dimension with the lowest index always has the largest stride.
   *   E.g. the 3D case, this is equivalent to:
   *         for(x = 0; x < size[0]; ++x) {
   *             for(y = 0; y < size[1]; ++y) {
   *                 for(z = 0; z < size[2]; ++z) {
   *                     [...]
   *                 }
   *             }
   *         } 
   */
  template<typename TIndexAccessible>
  constexpr unsigned int stride_of_dim(const TIndexAccessible& size, unsigned int dimId, unsigned int numDims)
  {
      unsigned int s = 1;

      for (unsigned int i = dimId + 1; i < numDims; ++i)
      { s *= size[i]; }

      return s;
  }
  /// @}
} // namespace bk

#endif //BKMATH_LIST_GRID_ID_CONVERSION_H


