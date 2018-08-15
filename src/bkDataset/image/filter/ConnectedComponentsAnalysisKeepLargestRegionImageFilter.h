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

#ifndef BK_CONNECTEDCOMPONENTSANALYSISKEEPLARGESTREGIONIMAGEFILTER_H
#define BK_CONNECTEDCOMPONENTSANALYSISKEEPLARGESTREGIONIMAGEFILTER_H

#include <map>
#include <queue>
#include <vector>

#include <bkDataset/image/filter/ConnectedComponentsAnalysisImageFilter.h>
#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT ConnectedComponentAnalysisKeepLargestRegionImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ConnectedComponentAnalysisKeepLargestRegionImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ConnectedComponentAnalysisKeepLargestRegionImageFilter();
      ConnectedComponentAnalysisKeepLargestRegionImageFilter(const self_type&);
      ConnectedComponentAnalysisKeepLargestRegionImageFilter(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ConnectedComponentAnalysisKeepLargestRegionImageFilter();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] static typename TImage::template self_template_type<int> apply(const TImage& img)
      {
          ConnectedComponentAnalysisImageFilter f_cca;
          auto labels = img.filter(f_cca);

          // determine max size label

          unsigned int maxSizeLabelId = 0;
          unsigned int maxSize = 0;

          for (const auto[id, num_pixels] : f_cca.labels())
          {
              if (num_pixels > maxSize)
              {
                  maxSize = num_pixels;
                  maxSizeLabelId = id;
              }
          }

          #pragma omp parallel for
          for (unsigned int i = 0; i < labels.num_values(); ++i)
          { labels[i] = labels[i] != static_cast<int>(maxSizeLabelId )? 0 : 1; }

          return labels;
      }
      /// @}
  }; // class ConnectedComponentAnalysisKeepLargestRegionImageFilter
} // namespace bk

#endif //BK_CONNECTEDCOMPONENTSANALYSISKEEPLARGESTREGIONIMAGEFILTER_H
