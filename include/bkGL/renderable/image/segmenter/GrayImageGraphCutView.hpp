#pragma once

#ifndef BK_GRAYIMAGEGRAPHCUTVIEW_HPP
#define BK_GRAYIMAGEGRAPHCUTVIEW_HPP

#include "GrayImageGraphCutView.h"

namespace bk
{
  template<typename Img3_>
  void GrayImageGraphCutView::set_segmentation(const Img3_& seg)
  {
      for (unsigned int i = 0; i < seg.num_values(); ++i)
      { _seg()[i] = seg[i]; }

      _seg_changed() = true;
      update_ssbo_gc();
  }
} // namespace bk

#endif // BK_GRAYIMAGEGRAPHCUTVIEW_HPP


