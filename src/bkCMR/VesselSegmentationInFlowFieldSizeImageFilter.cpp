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

#include <bkCMR/VesselSegmentationInFlowFieldSizeImageFilter.h>

#include <bkCMR/FlowImage3DT.h>
#include <bkCMR/Vessel.h>
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
  VesselSegmentationInFlowFieldSizeImageFilter::VesselSegmentationInFlowFieldSizeImageFilter() = default;
  VesselSegmentationInFlowFieldSizeImageFilter::VesselSegmentationInFlowFieldSizeImageFilter(const VesselSegmentationInFlowFieldSizeImageFilter&) = default;
  VesselSegmentationInFlowFieldSizeImageFilter::VesselSegmentationInFlowFieldSizeImageFilter(VesselSegmentationInFlowFieldSizeImageFilter&&) noexcept = default;
  VesselSegmentationInFlowFieldSizeImageFilter::~VesselSegmentationInFlowFieldSizeImageFilter() = default;

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  VesselSegmentationInFlowFieldSizeImageFilter& VesselSegmentationInFlowFieldSizeImageFilter::operator=(const VesselSegmentationInFlowFieldSizeImageFilter&) = default;
  VesselSegmentationInFlowFieldSizeImageFilter& VesselSegmentationInFlowFieldSizeImageFilter::operator=(VesselSegmentationInFlowFieldSizeImageFilter&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  std::unique_ptr<DicomImage<double, 3>> VesselSegmentationInFlowFieldSizeImageFilter::apply(const FlowImage3DT& ff, const Vessel& v)
  {
      const auto& size = ff.size();

      auto res = std::make_unique<DicomImage<double, 3>>();
      res->set_size(size[0], size[1], size[2]);
      res->geometry().transformation().set_world_matrix(ff.geometry().transformation().world_matrix());

      const auto& vseg = v.segmentation3D();
      const auto vsegsize = vseg.size();

      if (vsegsize[0] == size[0] && vsegsize[1] == size[1] && vsegsize[2] == size[2])
      {
          // flow field and segmentation have same size
          // -> it's very likely that the segmentation was performed on a tmip
          // => copy values of vessel segmentation

          #pragma omp parallel for
          for (unsigned int i = 0; i < vseg.num_values(); ++i)
          { (*res)[i] = vseg[i]; }
      }
      else
      {
          // flow field and segmentation have different sizes
          // => sample new segmentation

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(ff.num_values() / size[3], ___("sampling vessel segmentation in flow field size"));
          #endif

          #pragma omp parallel for
          for (unsigned int x = 0; x < size[0]; ++x)
          {
              for (unsigned int y = 0; y < size[1]; ++y)
              {
                  for (unsigned int z = 0; z < size[2]; ++z)
                  { (*res)(x, y, z) = std::round(vseg.interpolate(res->geometry().transformation().to_world_coordinates(Vec3d(x, y, z)))); }
              }

              #ifdef BK_EMIT_PROGRESS
                  #pragma omp critical(VesselSegmentationInFlowFieldSizeImageFilter_progress)
              { prog.increment(size[1] * size[2]); }
              #endif
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif
      }

      return res;
  }
} // namespace bk
