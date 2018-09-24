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

#include <bkCMR/MagTMIPImageFilter.h>

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
  namespace
  {
    template<auto Dims>
    static std::unique_ptr<DicomImage<double, 3>> _apply(const DicomImage<double, Dims>& mag)
    {
        static_assert(Dims == 4 || Dims == -1, "invalid number of dimensions");

        const auto& size = mag.size();

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(mag.num_values() / size[3], ___("calculating magnitude tmip"));
        #endif

        auto res = std::make_unique<DicomImage<double, 3>>();
        res->set_size(size[0], size[1], size[2]);
        res->geometry().transformation().set_world_matrix(mag.geometry().transformation().world_matrix());
        res->geometry().transformation().set_dicom_image_type_3d();

        const unsigned int stride_t = bk::stride_of_dim(size, 3, 4);

        #pragma omp parallel for
        for (unsigned int x = 0; x < size[0]; ++x)
        {
            double maxval = 0;

            for (unsigned int y = 0; y < size[1]; ++y)
            {
                for (unsigned int z = 0; z < size[2]; ++z)
                {
                    unsigned int lid = bk::grid_to_list_id(size, x, y, z, 0);

                    maxval = 0;

                    for (unsigned int t = 0; t < size[3]; ++t, lid += stride_t)
                    { maxval = std::max(maxval, mag[lid]); }

                    (*res)(x, y, z) = maxval;
                }
            }

            #ifdef BK_EMIT_PROGRESS
                #pragma omp critical(MagTMIPImageFilter_progress)
            { prog.increment(size[1] * size[2]); }
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return res;
    }

    template<auto Dims>
    static std::unique_ptr<DicomImage<double, 3>> _apply(const DicomImage<double, Dims>& mag_x, const DicomImage<double, Dims>& mag_y, const DicomImage<double, Dims>& mag_z)
    {
        const auto& size = mag_x.size();

        assert(size[0] == mag_y.size(0) && size[0] == mag_z.size(0) && "MagTMIPImageFilter::apply - magnitude image size x mismatch");
        assert(size[1] == mag_y.size(1) && size[1] == mag_z.size(1) && "MagTMIPImageFilter::apply - magnitude image size y mismatch");
        assert(size[2] == mag_y.size(2) && size[2] == mag_z.size(2) && "MagTMIPImageFilter::apply - magnitude image size z mismatch");
        assert(size[3] == mag_y.size(3) && size[3] == mag_z.size(3) && "MagTMIPImageFilter::apply - magnitude image size t mismatch");

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(mag_x.num_values() / size[3], ___("calculating magnitude tmip"));
        #endif

        auto res = std::make_unique<DicomImage<double, 3>>();
        res->set_size(size[0], size[1], size[2]);
        res->geometry().transformation().set_world_matrix(mag_x.geometry().transformation().world_matrix());
        res->geometry().transformation().set_dicom_image_type_3d();

        const unsigned int stride_t = bk::stride_of_dim(size, 3, 4);

        #pragma omp parallel for
        for (unsigned int x = 0; x < size[0]; ++x)
        {
            double maxval = 0;

            for (unsigned int y = 0; y < size[1]; ++y)
            {
                for (unsigned int z = 0; z < size[2]; ++z)
                {
                    unsigned int lid = bk::grid_to_list_id(size, x, y, z, 0);

                    maxval = 0;

                    for (unsigned int t = 0; t < size[3]; ++t, lid += stride_t)
                    { maxval = std::max(maxval, Vec3d(mag_x[lid], mag_y[lid], mag_z[lid]).norm_squared()); }

                    (*res)(x, y, z) = std::sqrt(maxval);
                }
            }

            #ifdef BK_EMIT_PROGRESS
                #pragma omp critical(MagTMIPImageFilter_progress)
            { prog.increment(size[1] * size[2]); }
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return res;
    }
  } // anonymous namespace

  std::unique_ptr<DicomImage<double, 3>> MagTMIPImageFilter::apply(const DicomImage<double, 4>& mag)
  { return _apply(mag); }

  std::unique_ptr<DicomImage<double, 3>> MagTMIPImageFilter::apply(const DicomImage<double, -1>& mag)
  { return _apply(mag); }

  std::unique_ptr<DicomImage<double, 3>> MagTMIPImageFilter::apply(const DicomImage<double, 4>& mag_x, const DicomImage<double, 4>& mag_y, const DicomImage<double, 4>& mag_z)
  { return _apply(mag_x, mag_y, mag_z); }

  std::unique_ptr<DicomImage<double, 3>> MagTMIPImageFilter::apply(const DicomImage<double, -1>& mag_x, const DicomImage<double, -1>& mag_y, const DicomImage<double, -1>& mag_z)
  { return _apply(mag_x, mag_y, mag_z); }
} // namespace bk
