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

#ifndef BK_LINEARIMAGEINTERPOLATION_H
#define BK_LINEARIMAGEINTERPOLATION_H

#include <algorithm>
#include <cmath>
#include <type_traits>

#include <bkMath/functions/list_grid_id_conversion.h>
#include <bkTypeTraits/has_index_operator.h>

#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT LinearImageInterpolation
  {
    public:
      template<typename TImage, typename TIndexAccessible>
      constexpr auto operator()(const TImage& img, TIndexAccessible&& grid_pos) const
      {
          using IndexAccessible = std::decay_t<TIndexAccessible>;
          static_assert(bk::has_index_operator_v<IndexAccessible>, "grid_pos must provide operator[]");

          const IndexAccessible grid_pos_clamped = img.clamp_to_size(std::forward<TIndexAccessible>(grid_pos));

          IndexAccessible min_grid_pos = grid_pos_clamped;
          unsigned int dimId = 0;
          for (auto& x: min_grid_pos)
          {
              x = std::floor(x);
              assert(!std::signbit(x) && "grid_pos must not be negative");

              x = std::min(static_cast<int>(x), static_cast<int>(img.size(dimId++) - 2));
          }

          IndexAccessible max_grid_pos = min_grid_pos;
          for (auto& x: max_grid_pos)
          { x += 1; }

          const unsigned int nDims = img.num_dimensions();
          const auto cell = img.topology().cell(min_grid_pos);
          auto res = img.template allocate_value<double>();

          for (unsigned int pointId = 0; pointId < cell.size(); ++pointId)
          {
              const auto gid_of_cell_point = bk::list_to_grid_id(img.size(), cell[pointId]);
              double weight = 1.0;

              for (unsigned int dimId2 = 0; dimId2 < nDims; ++dimId2)
              {
                  if (gid_of_cell_point[dimId2] == min_grid_pos[dimId2])
                  { weight *= static_cast<double>(max_grid_pos[dimId2]) - static_cast<double>(grid_pos_clamped[dimId2]); }
                  else
                  { weight *= static_cast<double>(grid_pos_clamped[dimId2]) - static_cast<double>(min_grid_pos[dimId2]); }
              } // for dimId2

              res += img[cell[pointId]] * weight;
          } // for pointId

          return res;
      }
  };
} // namespace bk

#endif //BK_LINEARIMAGEINTERPOLATION_H
