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

#include <bkCMR/MagTMIPImageFilter.h>

#include <bkMath/functions/list_grid_id_conversion.h>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

#include <bkCMR/FlowImage3DT.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  MagTMIPImageFilter::MagTMIPImageFilter() = default;
  MagTMIPImageFilter::MagTMIPImageFilter(const MagTMIPImageFilter&) = default;
  MagTMIPImageFilter::MagTMIPImageFilter(MagTMIPImageFilter&&) noexcept = default;
  MagTMIPImageFilter::~MagTMIPImageFilter() = default;

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  MagTMIPImageFilter& MagTMIPImageFilter::operator=(const MagTMIPImageFilter&) = default;
  MagTMIPImageFilter& MagTMIPImageFilter::operator=(MagTMIPImageFilter&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  std::unique_ptr<DicomImage<double, 3>> apply(const DicomImage<double, 4>& mag)
  {
      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog = bk_progress.emplace_task(mag.num_values() / mag.size(3), ___("calculating magnitude tmip"));
      #endif

      auto res = std::make_unique<DicomImage<double, 3>>();
      res->set_size(mag.size(0), mag.size(1), mag.size(2));
      res->geometry().transformation().set_world_matrix(mag.geometry().transformation().world_matrix());

      const unsigned int stride_t = bk::stride_of_dim(mag.size(), 3, 4);

      #pragma omp parallel for
      for (unsigned int x = 0; x < mag.size(0); ++x)
      {
          double maxval = 0;

          for (unsigned int y = 0; y < mag.size(1); ++y)
          {
              for (unsigned int z = 0; z < mag.size(2); ++z)
              {
                  unsigned int lid = bk::grid_to_list_id(mag.size(), x, y, z, 0);

                  maxval = 0;

                  for (unsigned int t = 0; t < mag.size(3); ++t, lid += stride_t)
                  { maxval = std::max(maxval, mag[lid]); }

                  (*res)(x, y, z) = maxval;
              }
          }

          #ifdef BK_EMIT_PROGRESS
              #pragma omp critical(MagTMIPImageFilter_progress)
          { prog.increment(mag.size(1) * mag.size(2)); }
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return res;
  }
} // namespace bk
