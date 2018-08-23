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

#include <bkCMR/FlowImage3DT.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    FlowImage3DT::FlowImage3DT()
        : base_type(),
          _wmat_rot(MatrixFactory::Identity_Mat_3D()),
          wmat_rot_is_up2date(false)
    { /* do nothing */ }

    FlowImage3DT::FlowImage3DT(const FlowImage3DT&) = default;
    FlowImage3DT::FlowImage3DT(FlowImage3DT&&) noexcept = default;
    FlowImage3DT::~FlowImage3DT() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const Mat3d& FlowImage3DT::world_matrix_rotational_part() const
    { return _wmat_rot; }

    Vec3d FlowImage3DT::flow_vector_world_rotated_at_grid_pos(double x, double y, double z, double t) const
    { return _wmat_rot * this->interpolate_at_grid_pos(Vec4d(x, y, z, t)); }

    Vec3d FlowImage3DT::flow_vector_world_rotated_at_grid_pos(const Vec4d& xyzt) const
    { return flow_vector_world_rotated_at_grid_pos(xyzt[0], xyzt[1], xyzt[2], xyzt[3]); }

    Vec3d FlowImage3DT::flow_vector_world_rotated_at_world_pos(double x, double y, double z, double t) const
    { return flow_vector_world_rotated_at_world_pos(Vec4d(x, y, z, t)); }

    Vec3d FlowImage3DT::flow_vector_world_rotated_at_world_pos(const Vec4d& xyzt) const
    { return _wmat_rot * this->interpolate(xyzt); }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    FlowImage3DT& FlowImage3DT::operator=(const FlowImage3DT&) = default;
    FlowImage3DT& FlowImage3DT::operator=(FlowImage3DT&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void FlowImage3DT::clear()
    {
        this->set_size(1,1,1,1);

        _wmat_rot           = MatrixFactory::Identity_Mat_3D();
        wmat_rot_is_up2date = false;
    }

    void FlowImage3DT::calc_world_matrix_rotational_part()
    {
        _wmat_rot = this->geometry().transformation().world_matrix().sub_matrix<0, 2, 0, 2>();
        _wmat_rot.col_ref(0) /= this->geometry().transformation().scale(0);
        _wmat_rot.col_ref(1) /= this->geometry().transformation().scale(1);
        _wmat_rot.col_ref(2) /= this->geometry().transformation().scale(2);

        wmat_rot_is_up2date = true;
    }
  } // inline namespace cmr
} // namespace bk