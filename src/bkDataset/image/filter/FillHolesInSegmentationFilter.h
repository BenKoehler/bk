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

#ifndef BK_FILLHOLESINSEGMENTATIONFILTER_H
#define BK_FILLHOLESINSEGMENTATIONFILTER_H

#include <bkDataset/image/filter/ConnectedComponentsAnalysisKeepLargestRegionImageFilter.h>
#include <bkDataset/image/filter/InvertIntensityImageFilter.h>

#ifdef BK_EMIT_PROGRESS
    #include <bk/Progress>
    #include <bk/Localization>
#endif

namespace bk
{
  class FillHolesInSegmentationFilter
  {
      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr FillHolesInSegmentationFilter() = default;
      constexpr FillHolesInSegmentationFilter(const FillHolesInSegmentationFilter&) = default;
      constexpr FillHolesInSegmentationFilter(FillHolesInSegmentationFilter&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~FillHolesInSegmentationFilter() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const FillHolesInSegmentationFilter&) -> FillHolesInSegmentationFilter& = default;
      [[maybe_unused]] constexpr auto operator=(FillHolesInSegmentationFilter&&) noexcept -> FillHolesInSegmentationFilter& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TSegmentation>
      [[nodiscard]] static constexpr TSegmentation apply(const TSegmentation& seg)
      {
          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(9, ___("Filling holes in segmentation"));
          #endif

          auto segInverted = InvertIntensityImageFilter::apply(seg);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(2);
          #endif

          auto temp = ConnectedComponentAnalysisKeepLargestRegionImageFilter::apply(segInverted);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(5);
          #endif

          TSegmentation res = InvertIntensityImageFilter::apply(temp);

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return std::move(res);
      }
      /// @}
  }; // class FillHolesInSegmentationFilter
} // namespace bk

#endif //BK_FILLHOLESINSEGMENTATIONFILTER_H
