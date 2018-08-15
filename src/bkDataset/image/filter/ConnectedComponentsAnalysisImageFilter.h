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

#ifndef BK_CONNECTEDCOMPONENTSANALYSISIMAGEFILTER_H
#define BK_CONNECTEDCOMPONENTSANALYSISIMAGEFILTER_H

#include <map>
#include <queue>
#include <vector>

#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT ConnectedComponentAnalysisImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ConnectedComponentAnalysisImageFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::map<unsigned int/*id*/, unsigned int/*num pixels*/> _labels;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ConnectedComponentAnalysisImageFilter();
      ConnectedComponentAnalysisImageFilter(const self_type&);
      ConnectedComponentAnalysisImageFilter(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ConnectedComponentAnalysisImageFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM LABELS
      [[nodiscard]] unsigned int num_labels() const;
      /// @}

      /// @{ -------------------------------------------------- GET LABELS
      [[nodiscard]] const std::map<unsigned int, unsigned int>& labels() const;
      /// @}

      /// @{ -------------------------------------------------- GET REGION SIZES
      [[nodiscard]] unsigned int num_pixels_with_label(unsigned int labelId) const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] typename TImage::template self_template_type<int> apply(const TImage& img)
      {
          const unsigned int numDimensions = img.num_dimensions();
          const unsigned int numValues = img.num_values();
          const auto size = img.size();

          std::vector<int> stride(numDimensions, 0);
          for (unsigned int dimId = 0; dimId < numDimensions; ++dimId)
          { stride[dimId] = bk::stride_of_dim(size, dimId, img.num_dimensions()); }

          typename TImage::template self_template_type<int> labels;
          labels.set_size(size);
          labels.set_constant(-1);

          unsigned int currentLabel = 0;

          _labels.clear();

          for (unsigned int lid = 0; lid < numValues; ++lid)
          {
              if (labels[lid] != -1) // already visited and labeled
              { continue; }

              if (img[lid] == 0) // not in segmentation
              {
                  labels[lid] = 0;
                  continue;
              }

              ++currentLabel; // create new label
              labels[lid] = currentLabel; // label current pixel

              unsigned int regionSizeCnt = 1;
              std::queue<unsigned int> q; // queue with list ids
              q.push(lid);

              while (!q.empty())
              {
                  const unsigned int currentLid = q.front();

                  // todo 4 vs 8 neighborhood

                  for (unsigned int dimId = 0; dimId < numDimensions; ++dimId)
                  {
                      for (int neighborLid: {static_cast<int>(currentLid) - stride[dimId], static_cast<int>(currentLid) + stride[dimId]})
                      {
                          if (neighborLid >= 0 && neighborLid < static_cast<int>(numValues))
                          { // neighbor is inside image bounds
                              if (labels[neighborLid] == -1)
                              { // neighbor was not yet visited
                                  const auto neighborGid = bk::list_to_grid_id(size, neighborLid);

                                  if (img(neighborGid) != 0)
                                  {
                                      q.push(neighborLid);
                                      labels[neighborLid] = currentLabel;
                                      ++regionSizeCnt;
                                  }
                                  else
                                  { labels[neighborLid] = 0; }
                              }
                          }
                      } // for neighborLid
                  } // for dimId

                  q.pop();
              } // while

              _labels.insert(std::make_pair(currentLabel, regionSizeCnt));
          } // for lid

          return labels;
      }
      /// @}
  }; // class ConnectedComponentAnalysisImageFilter
} // namespace bk

#endif //BK_CONNECTEDCOMPONENTSANALYSISIMAGEFILTER_H
