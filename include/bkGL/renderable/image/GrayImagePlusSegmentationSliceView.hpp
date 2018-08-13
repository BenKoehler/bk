#pragma once

#pragma once

#ifndef BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_IMPL_H
#define BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_IMPL_H

#include "GrayImagePlusSegmentationSliceView.h"

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


