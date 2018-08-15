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

#ifndef BK_GRAYIMAGEGRAPHCUTVIEW_HPP
#define BK_GRAYIMAGEGRAPHCUTVIEW_HPP

#include <bkGL/renderable/image/segmenter/GrayImageGraphCutView.h>

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


