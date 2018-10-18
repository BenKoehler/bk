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

#include <bkCMR/dataset/transformation/MeasuringPlaneTransformation.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    MeasuringPlaneTransformation::MeasuringPlaneTransformation()
        : _center(0, 0, 0),
          _nx(1, 0, 0),
          _ny(0, 1, 0),
          _nz(0, 0, 1),
          _grid_size(1, 1, 1),
          _scale(1, 1, 1)
    { /* do nothing */ }

    MeasuringPlaneTransformation::MeasuringPlaneTransformation(const MeasuringPlaneTransformation&) = default;
    MeasuringPlaneTransformation::MeasuringPlaneTransformation(MeasuringPlaneTransformation&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    MeasuringPlaneTransformation::~MeasuringPlaneTransformation() = default;
    /// @}

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const Vec3d& MeasuringPlaneTransformation::center() const
    { return _center; }

    double MeasuringPlaneTransformation::center(unsigned int i) const
    { return i < 3 ? _center[i] : 0; }

    const Vec3d& MeasuringPlaneTransformation::nx() const
    { return _nx; }

    const Vec3d& MeasuringPlaneTransformation::ny() const
    { return _ny; }

    const Vec3d& MeasuringPlaneTransformation::nz() const
    { return _nz; }

    const Vec3i& MeasuringPlaneTransformation::grid_size() const
    { return _grid_size; }

    unsigned int MeasuringPlaneTransformation::grid_size(unsigned int i) const
    { return i < 3 ? _grid_size[i] : 0; }

    const Vec3d& MeasuringPlaneTransformation::scale() const
    { return _scale; }

    double MeasuringPlaneTransformation::scale(unsigned int i) const
    { return i < 3 ? _scale[i] : 0; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    MeasuringPlaneTransformation& MeasuringPlaneTransformation::operator=(const MeasuringPlaneTransformation&) = default;
    MeasuringPlaneTransformation& MeasuringPlaneTransformation::operator=(MeasuringPlaneTransformation&&) noexcept = default;
    /// @}

    void MeasuringPlaneTransformation::set_center(const Vec3d& _c)
    { _center = _c; }

    void MeasuringPlaneTransformation::set_center(double cx, double cy, double cz)
    { _center.set(cx, cy, cz); }

    void MeasuringPlaneTransformation::set_nx(const Vec3d& _nx)
    { this->_nx = _nx.normalize(); }

    void MeasuringPlaneTransformation::set_nx(double nxx, double nxy, double nxz)
    {
        _nx.set(nxx, nxy, nxz);
        this->_nx.normalize_internal();
    }

    void MeasuringPlaneTransformation::set_ny(const Vec3d& _ny)
    { this->_ny = _ny.normalize(); }

    void MeasuringPlaneTransformation::set_ny(double nyx, double nyy, double nyz)
    {
        _ny.set(nyx, nyy, nyz);
        this->_ny.normalize_internal();
    }

    void MeasuringPlaneTransformation::set_nz(const Vec3d& _nz)
    { this->_nz = _nz.normalize(); }

    void MeasuringPlaneTransformation::set_nz(double nzx, double nzy, double nzz)
    {
        _nz.set(nzx, nzy, nzz);
        this->_nz.normalize_internal();
    }

    void MeasuringPlaneTransformation::set_grid_size(const Vec3i& _gs)
    { this->_grid_size = _gs; }

    void MeasuringPlaneTransformation::set_grid_size(int grid_sizex, int grid_sizey, int grid_sizez)
    { _grid_size.set(grid_sizex, grid_sizey, grid_sizez); }

    void MeasuringPlaneTransformation::set_scale(const Vec3d& _scale)
    { this->_scale = _scale; }

    void MeasuringPlaneTransformation::set_scale(double scalex, double scaley, double scalez)
    { _scale.set(scalex, scaley, scalez); }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    /// @{ -------------------------------------------------- TO WORLD COORDINATES
    Vec4d MeasuringPlaneTransformation::to_world_coordinates(double x, double y, double t) const
    {
        Vec4d res;

        res.sub_vector_ref<0, 2>() = _center + (_nx * _scale[0] * (x - (_grid_size[0] >> 1))) + (_ny * _scale[1] * (y - (_grid_size[1] >> 1)));
        res[3] = t * _scale[2];

        return res;
    }
    /// @}

    /// @{ -------------------------------------------------- TO OBJECT COORDINATES
    Vec3d MeasuringPlaneTransformation::to_object_coordinates(double x, double y, double z, double t) const
    {
        const Vec3d temp(Vec3d(x, y, z) - _center);

        Vec3d res;

        res[0] = (_nx.dot(temp) / _scale[0]) + (_grid_size[0] >> 1);
        res[1] = (_ny.dot(temp) / _scale[1]) + (_grid_size[1] >> 1);
        res[2] = t / _scale[2];

        return res;
    }
    /// @}
  } // inline namespace cmr
} // namespace bk