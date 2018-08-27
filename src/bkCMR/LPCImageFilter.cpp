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

#include <bkCMR/LPCImageFilter.h>

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
  LPCImageFilter::LPCImageFilter() = default;
  LPCImageFilter::LPCImageFilter(const LPCImageFilter&) = default;
  LPCImageFilter::LPCImageFilter(LPCImageFilter&&) noexcept = default;
  LPCImageFilter::~LPCImageFilter() = default;

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  LPCImageFilter& LPCImageFilter::operator=(const LPCImageFilter&) = default;
  LPCImageFilter& LPCImageFilter::operator=(LPCImageFilter&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  std::unique_ptr<DicomImage<double, 3>> apply(const FlowImage3DT& ff)
  {
      const auto& size = ff.size();

      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog = bk_progress.emplace_task(ff.num_values() / size[3], ___("calculating lpc"));
      #endif

      auto res = std::make_unique<DicomImage<double, 3>>();
      res->set_size(size[0], size[1], size[2]);
      res->geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix());

      const unsigned int stride_t = bk::stride_of_dim(size, 3, 4);

      #pragma omp parallel for
      for (unsigned int x = 0; x < size[0]; ++x)
      {
          for (unsigned int y = 0; y < size[1]; ++y)
          {
              for (unsigned int z = 0; z < size[2]; ++z)
              {
                  unsigned int lid = bk::grid_to_list_id(size, x, y, z, 0);

                  double temporalAverage = 0;

                  for (unsigned int t = 0; t < size[3]; ++t, lid += stride_t)
                  {
                      Vec3d vn(ff(x, y, z, t));
                      vn.normalize_internal();

                      double angleSum = 0;

                      for (int dx = -1; dx <= 1; ++dx)
                      {
                          const int ddx = static_cast<int>(x) + dx;

                          if (ddx < 0 || ddx >= static_cast<int>(size[0]))
                          { continue; }

                          for (int dy = -1; dy <= 1; ++dy)
                          {
                              const int ddy = static_cast<int>(y) + dy;

                              if (ddy < 0 || ddy >= static_cast<int>(size[1]))
                              { continue; }

                              for (int dz = -1; dz <= 1; ++dz)
                              {
                                  if (dx == 0 && dy == 0 && dz == 0)
                                  { continue; }

                                  const int ddz = static_cast<int>(z) + dz;

                                  if (ddz < 0 || ddz >= static_cast<int>(size[2]))
                                  { continue; }

                                  Vec3d neigh(ff(ddx, ddy, ddz, t));
                                  neigh.normalize_internal();
                                  angleSum += std::abs(vn.dot(neigh));
                              } // for dz
                          } // for dy
                      } // for dx

                      //temporalAverage += angleSum / 26 /*neighborhood size = 3*3*3 - 1*/;
                  }

                  //(*res)(x, y, z) = temporalAverage / size[3];
                  (*res)(x, y, z) = temporalAverage / (size[3] * 26 /*= 3*3*3 - 1 = neighborhood size*/);
              }
          }

          #ifdef BK_EMIT_PROGRESS
              #pragma omp critical(LPCImageFilter_progress)
          { prog.increment(size[1] * size[2]); }
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return res;
  }
} // namespace bk
