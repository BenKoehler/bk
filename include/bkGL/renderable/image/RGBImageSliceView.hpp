#pragma once
#ifndef BK_RGBIMAGESLICEVIEW_IMPL_H
#define BK_RGBIMAGESLICEVIEW_IMPL_H

#include <algorithm>
#include <limits>

#include <bkDataset/type_traits/is_image.h>

#include "RGBImageSliceView.h"


namespace bk
{
  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  template<typename Image_>
  bool RGBImageSliceView::set_image(const Image_& img)
  {
      static_assert(is_image_v<Image_>, "PARAMETER MUST BE AN IMAGE");

      const unsigned int N = img.num_dimensions();
      Vec4<GLuint> size_ = Vec4<GLuint>::One();
      Vec4<GLfloat> scale_ = Vec4<GLfloat>::One();

      for (unsigned int i = 0; i < N; ++i)
      {
          size_[i] = img.size(i);
          scale_[i] = img.scale(i);
      }

      _image.set_size(size_);
      _image.geometry().transformation().set_scale(scale_);

      _intensitymax() = -std::numeric_limits<GLfloat>::max();
      _intensitymin() = std::numeric_limits<GLfloat>::max();

      Vec4<GLuint> gid;

      for (unsigned int t = 0; t < size_[3]; ++t)
      {
          gid[3] = t;
          for (unsigned int z = 0; z < size_[2]; ++z)
          {
              gid[2] = z;
              for (unsigned int y = 0; y < size_[1]; ++y)
              {
                  gid[1] = y;
                  for (unsigned int x = 0; x < size_[0]; ++x)
                  {
                      gid[0] = x;
                      //const rgb_type val(img(gid.block(0, N - 1, 0, 0)));
                      const rgb_type val(img(gid.sub_matrix(0, N - 1, 0, 0)));
                      _image(gid) = val;
                      for (unsigned int k = 0; k < 3; ++k)
                      {
                          _intensitymin = std::min(_intensitymin(), val[k]);
                          _intensitymax = std::max(_intensitymax(), val[k]);
                      }
                  }
              }
          }
      }

      return _image.num_values() > 1;
  }
} // namespace bk

#endif // BK_RGBIMAGESLICEVIEW_IMPL_H


