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

#pragma once

#ifndef BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_IMPL_H
#define BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_IMPL_H

#include <bkGL/renderable/image/GrayImagePlusSegmentationSliceView.h>

#include <bkDataset/type_traits/is_image.h>

namespace bk
{
  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  template<typename Segmentation_>
  inline void GrayImagePlusSegmentationSliceView::set_segmentation(const Segmentation_& seg)
  {
      static_assert(is_image_v<Segmentation_>, "PARAMETER MUST BE AN IMAGE");

      _seg().set_size(seg.geometry().size(0), seg.geometry().size(1), seg.geometry().size(2));
      _seg().geometry().transformation().set_scale(seg.geometry().transformation().scale(0), seg.geometry().transformation().scale(1), seg.geometry().transformation().scale(2));

      #pragma omp parallel for
      for (unsigned int i = 0; i < seg.num_values(); ++i)
      { _seg()[i] = seg[i] != 0 ? 1 : 0; }

      init_ssbo();
  }
} // namespace bk

#endif // BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_IMPL_H


