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

#ifndef BK_FLOWIMAGE3DT_H
#define BK_FLOWIMAGE3DT_H

#include <bk/Image>
#include <bk/Matrix>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    class BKCMR_EXPORT FlowImage3DT : public DicomImage<Vec3d, 4>
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using base_type = DicomImage<Vec3d, 4>;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
        Mat3d _wmat_rot; //!< the rotational part of the world matrix, i.e., the upper 3x3 matrix with normalized columns
        bool wmat_rot_is_up2date;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        FlowImage3DT();
        FlowImage3DT(const FlowImage3DT&);
        FlowImage3DT(FlowImage3DT&&) noexcept;
        ~FlowImage3DT();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] const Mat3d& world_matrix_rotational_part() const;

        [[nodiscard]] Vec3d flow_vector_world_rotated_at_grid_pos(double x, double y, double z, double t) const;
        [[nodiscard]] Vec3d flow_vector_world_rotated_at_grid_pos(const Vec4d& xyzt) const;

        [[nodiscard]] Vec3d flow_vector_world_rotated_at_world_pos(double x, double y, double z, double t) const;
        [[nodiscard]] Vec3d flow_vector_world_rotated_at_world_pos(const Vec4d& xyzt) const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] FlowImage3DT& operator=(const FlowImage3DT&);
        [[maybe_unused]] FlowImage3DT& operator=(FlowImage3DT&&) noexcept;

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();
        void calc_world_matrix_rotational_part();
    }; // class FlowImage3DT
  } // inline namespace cmr
} // namespace bk

#endif //BK_FLOWIMAGE3DT_H
