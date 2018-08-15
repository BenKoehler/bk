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

#ifndef BK_NEARESTNEIGHBORIMAGEINTERPOLATION_H
#define BK_NEARESTNEIGHBORIMAGEINTERPOLATION_H

#include <algorithm>
#include <cmath>
#include <type_traits>

#include <bkTypeTraits/has_index_operator.h>
#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT NearestNeighborImageInterpolation
  {
    public:
      template<typename TImage, typename TIndexAccessible>
      constexpr auto operator()(const TImage& img, TIndexAccessible&& grid_pos) const
      {
          using IndexAccessible = std::decay_t<TIndexAccessible>;
          static_assert(bk::has_index_operator_v<IndexAccessible>, "grid_pos must provide operator[]");

          IndexAccessible grid_pos_rounded_and_clamped = std::forward<TIndexAccessible>(grid_pos);

          unsigned int dimId = 0;
          for (auto& x: grid_pos_rounded_and_clamped)
          { x = std::clamp(static_cast<int>(std::round(x)), 0, static_cast<int>(img.size(dimId++) - 1)); }

          return img(grid_pos_rounded_and_clamped);
      }
  };
} // namespace bk

#endif //BK_NEARESTNEIGHBORIMAGEINTERPOLATION_H
