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

#ifndef BK_ALGORITHMS_MINMAX_DIFFERENCE_H
#define BK_ALGORITHMS_MINMAX_DIFFERENCE_H

#include <algorithm>

#if defined(__GNUC__) && defined(_OPENMP)
    #include <parallel/algorithm>
#endif

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace bk
{
  /// @{ -------------------------------------------------- MINIMUM DIFFERENCE
  //! determines the minimum difference between 2 arbitrary elements in the range
  /*!
   * Example 1: [0,2,3,7]
   *      - minimum difference = 3-2 = 1
   * Example 2: [1,1,2,3,7]
   *      - minimum difference = 1-1 = 0
   */
  namespace details
  {
    template<typename TIter, typename SortLesser>
    [[nodiscard]] auto _min_difference_sorted_range(TIter first, TIter last, SortLesser compare)
    {
        using value_type = std::decay_t<decltype(*first)>;

        value_type minDist = -(*first) + *(first + 1);

        for (; first != last - 1; ++first)
        {
            const value_type d = -(*first) + *(first + 1);

            if (compare(d, minDist))
            { minDist = d; }
        }

        return minDist;
    }
  } // namespace details

  template<typename TIter, typename SortLesser>
  [[nodiscard]] auto min_difference(TIter first, TIter last, SortLesser compare)
  {
      using value_type = std::decay_t<decltype(*first)>;
      if (!std::is_sorted(TIter(first), TIter(last)))
      {
          std::vector<value_type> temp(first, last);
          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::sort(std::begin(temp), std::end(temp), compare);
          return details::_min_difference_sorted_range(std::begin(temp), std::end(temp), compare);
      }
      else
      { return details::_min_difference_sorted_range(first, last, compare); }
  }

  template<typename TIter>
  [[nodiscard]] auto min_difference(TIter first, TIter last)
  { return min_difference(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first)>>()); }
  /// @}

  /// @{ -------------------------------------------------- MAXIMUM DIFFERENCE
  //! determines the maximum difference between 2 arbitrary elements in the range
  /*!
   * Example 1: [0,2,3,7]
   *      - maximum difference = 7-0 = 7
   * Example 2: [1,1,2,3,7]
   *      - maximum difference = 7-1 = 6
   */
  namespace details
  {
    template<typename TIter>
    [[nodiscard]] auto _max_difference_sorted_range(TIter first, TIter last)
    { return *(last - 1) - *first; }
  } // namespace details

  template<typename TIter, typename SortLesser>
  [[nodiscard]] auto max_difference(TIter first, TIter last, SortLesser compare)
  {
      using value_type = std::decay_t<decltype(*first)>;

      if (!std::is_sorted(first, last))
      {
          std::vector<value_type> temp(first, last);

          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::sort(std::begin(temp), std::end(temp), compare);

          return details::_max_difference_sorted_range(temp.begin(), temp.end());
      }
      else
      { return details::_max_difference_sorted_range(first, last); }
  }

  template<typename TIter>
  [[nodiscard]] auto max_difference(TIter first, TIter last)
  { return max_difference(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first)>>()); }
  /// @}

  /// @{ -------------------------------------------------- MINIMUM/MAXIMUM DIFFERENCE
  namespace details
  {
    template<typename TIter, typename SortLesser>
    [[nodiscard]] auto _minmax_difference_sorted_range(TIter first, TIter last, SortLesser compare)
    { return std::make_pair(_min_difference_sorted_range(first, last, compare), _max_difference_sorted_range(first, last)); }
  } // namespace details

  template<typename TIter, typename SortLesser>
  [[nodiscard]] auto minmax_difference(TIter first, TIter last, SortLesser compare)
  {
      using value_type = std::decay_t<decltype(*first)>;

      if (!std::is_sorted(first, last))
      {
          std::vector<value_type> temp(first, last);

          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::sort(std::begin(temp), std::end(temp), compare);

          return details::_minmax_difference_sorted_range(temp.begin(), temp.end(), compare);
      }
      else
      { return details::_minmax_difference_sorted_range(first, last, compare); }
  }

  template<typename TIter>
  [[nodiscard]] auto minmax_difference(TIter first, TIter last)
  { return minmax_difference(std::move(first), std::move(last), std::less<std::decay_t<decltype(*first)>>()); }
  /// @}
} // namespace bk

#endif //BK_ALGORITHMS_MINMAX_DIFFERENCE_H


