#pragma once

#ifndef BK_GRAYIMAGESLICEVIEW_IMPL_H
#define BK_GRAYIMAGESLICEVIEW_IMPL_H

#include <algorithm>

#include "GrayImageSliceView.h"
#include "../../../bkDataset/type_traits/is_image.h"

namespace bk
{
  template<typename Image_>
  inline bool GrayImageSliceView::set_image(const Image_& img)
  {
      static_assert(bk::is_image_v<Image_>, "PARAMETER MUST BE AN IMAGE");

      const unsigned int N = img.num_dimensions();
      Vec4<GLuint> size_ = MatrixFactory::One_Vec_4D<GLuint>();
      Vec4<GLfloat> scale_ = MatrixFactory::One_Vec_4D<GLfloat>();
      for (unsigned int i = 0; i < N; ++i)
      {
          size_[i] = img.geometry().size(i);
          scale_[i] = img.geometry().transformation().scale(i);
      }

      _image.set_size(size_);
      _image.geometry().transformation().set_scale(scale_);
      _intensitymax() = -std::numeric_limits<GLfloat>::max();
      _intensitymin() = std::numeric_limits<GLfloat>::max();

      #pragma omp parallel for
      for (unsigned int t = 0; t < size_[3]; ++t)
      {
          Vec4<GLuint> gid(0,0,0,t);
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
                      const GLfloat val = static_cast<GLfloat>(img(gid.sub_vector(0, N - 1)));
                      _image.operator()(gid) = val;

                      #pragma omp critical
                      {
                          _intensitymin() = std::min(intensitymin(), val);
                          _intensitymax() = std::max(intensitymax(), val);
                      }
                  }
              }
          }
      }

      return _image.num_values() > 1;
  }
} // namespace bk

#endif // BK_GRAYIMAGESLICEVIEW_IMPL_H


