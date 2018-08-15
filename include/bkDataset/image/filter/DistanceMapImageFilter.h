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

#ifndef BK_DISTANCEMAPIMAGEFILTER_H
#define BK_DISTANCEMAPIMAGEFILTER_H

#include <algorithm>
#include <vector>

#include "../../../bkMath/functions/list_grid_id_conversion.h"

#include "../../lib/bkDataset_export.h"

namespace bk
{
  class BKDATASET_EXPORT DistanceMapImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DistanceMapImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      double _val;
      bool _value_was_set;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      DistanceMapImageFilter();
      DistanceMapImageFilter(const self_type&);
      DistanceMapImageFilter(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DistanceMapImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TARGET VALUE
      [[nodiscard]] double value() const;
      [[nodiscard]] bool custom_value_was_set() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET VALUE
      //! distance to pixels with this value will be determined
      void set_value(double val);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER
    private:
      template<typename TDistanceMap, typename TVec>
      void _distance_map(TDistanceMap& dst, unsigned int d, unsigned int dimId, unsigned int stride, TVec& gid, bool first = false) const
      {
          if (d < dst.num_dimensions())
          {
              if (d != dimId)
              {
                  if (first)
                  {
                      #pragma omp parallel for
                      for (unsigned int i = 0; i < dst.size(d); ++i)
                      {
                          TVec gid2 = gid;
                          gid2[d] = i;
                          _distance_map(dst, d + 1, dimId, stride, gid2);
                      }
                  }
                  else
                  {
                      for (unsigned int i = 0; i < dst.size(d); ++i)
                      {
                          gid[d] = i;
                          _distance_map(dst, d + 1, dimId, stride, gid);
                      }
                  }
              }
              else
              {
                  // low to high
                  gid[d] = 1;
                  unsigned int lid = bk::grid_to_list_id(dst.size(), gid, dst.num_dimensions());

                  for (unsigned int i = 1; i < dst.size(d); ++i, lid += stride)
                  { dst[lid] = std::min(dst[lid - stride] + 1, dst[lid]); }

                  // high to low
                  gid[d] = dst.size(d) - 2;
                  lid = bk::grid_to_list_id(dst.size(), gid, dst.num_dimensions());

                  for (int i = static_cast<int>(dst.size(d)) - 2; i >= 0; --i, lid -= stride)
                  { dst[lid] = std::min(dst[lid + stride] + 1, dst[lid]); }

                  _distance_map(dst, d + 1, dimId, stride, gid);
              }
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] typename TImage::template self_template_type<unsigned int> apply(const TImage& img)
      {
          if (!_value_was_set)
          { _val = img.max_value(); }

          const unsigned int numValues = img.num_values();
          const unsigned int maxdst = numValues;

          typename TImage::template self_template_type<unsigned int> dst;
          dst.set_size(img.size());

          // init
          #pragma omp parallel for
          for (unsigned int i = 0; i < numValues; ++i)
          { dst[i] = img[i] == _val ? 0 : maxdst; }

          // process dims
          for (unsigned int dimId = 0; dimId < img.num_dimensions(); ++dimId)
          {
              const unsigned int stride = bk::stride_of_dim(img.size(), dimId, img.num_dimensions());
              std::vector<unsigned int> gid(dst.num_dimensions(), 0);

              _distance_map(dst, 0, dimId, stride, gid, true);
          } // for dimId

          return dst;
      }
      /// @}
  }; // class DistanceMapImageFilter
} // namespace bk

#endif //BK_DISTANCEMAPIMAGEFILTER_H
