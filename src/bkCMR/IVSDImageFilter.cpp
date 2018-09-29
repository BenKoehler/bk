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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#include <bkAlgorithm/standard_deviation.h>
#include <bkCMR/IVSDImageFilter.h>

#include <bkCMR/FlowImage3DT.h>
#include <bkMath/functions/list_grid_id_conversion.h>
#include <bk/Matrix>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  IVSDImageFilter::IVSDImageFilter() = default;
  IVSDImageFilter::IVSDImageFilter(const IVSDImageFilter&) = default;
  IVSDImageFilter::IVSDImageFilter(IVSDImageFilter&&) noexcept = default;
  IVSDImageFilter::~IVSDImageFilter() = default;

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  IVSDImageFilter& IVSDImageFilter::operator=(const IVSDImageFilter&) = default;
  IVSDImageFilter& IVSDImageFilter::operator=(IVSDImageFilter&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  std::unique_ptr<DicomImage<double, 3>> IVSDImageFilter::apply(const FlowImage3DT& ff)
  {
      const auto& size = ff.size();

      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog = bk_progress.emplace_task(ff.num_values() / size[3], ___("Calculating IVSD"));
      #endif

      auto res = std::make_unique<DicomImage<double, 3>>();
      res->set_size(size[0], size[1], size[2]);
      res->geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix());
      res->geometry().transformation().set_dicom_image_type_3d();

      const unsigned int stride_t = bk::stride_of_dim(size, 3, 4);

      #pragma omp parallel for
      for (unsigned int x = 0; x < size[0]; ++x)
      {
          std::vector<double> tempx(size[3]);
          std::vector<double> tempy(size[3]);
          std::vector<double> tempz(size[3]);

          for (unsigned int y = 0; y < size[1]; ++y)
          {
              for (unsigned int z = 0; z < size[2]; ++z)
              {
                  unsigned int lid = bk::grid_to_list_id(size, x, y, z, 0);

                  for (unsigned int t = 0; t < size[3]; ++t, lid += stride_t)
                  {
                      tempx[t] = ff(x, y, z, t)[0];
                      tempy[t] = ff(x, y, z, t)[1];
                      tempz[t] = ff(x, y, z, t)[2];
                  }

                  const double stdevx = bk::standard_deviation(tempx.begin(), tempx.end());
                  const double stdevy = bk::standard_deviation(tempy.begin(), tempy.end());
                  const double stdevz = bk::standard_deviation(tempz.begin(), tempz.end());

                  (*res)(x, y, z) = Vec3d(stdevx, stdevy, stdevz).norm();
              }
          }

          #ifdef BK_EMIT_PROGRESS
              #pragma omp critical(IVSDImageFilter_progress)
          { prog.increment(size[1] * size[2]); }
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return res;
  }
} // namespace bk
