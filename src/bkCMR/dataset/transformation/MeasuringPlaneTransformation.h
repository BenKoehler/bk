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

#ifndef BK_MEASURINGPLANETRANSFORMATION_H
#define BK_MEASURINGPLANETRANSFORMATION_H

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

#include <bk/Matrix>
#include <bkTypeTraits/has_index_operator.h>
#include <bkCMR/lib/bkCMR_export.h>
#include <bkDataset/transformation/EDicomImageType.h>

namespace bk
{
  inline namespace cmr
  {
    class BKCMR_EXPORT MeasuringPlaneTransformation
    {
        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
        Vec3d _center;
        Vec3d _nx;
        Vec3d _ny;
        Vec3d _nz;
        Vec3i _grid_size;
        Vec3d _scale;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CTOR
        MeasuringPlaneTransformation();
        MeasuringPlaneTransformation(const MeasuringPlaneTransformation&);
        MeasuringPlaneTransformation(MeasuringPlaneTransformation&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DTOR
        ~MeasuringPlaneTransformation();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] const Vec3d& center() const;
        [[nodiscard]] double center(unsigned int i) const;
        [[nodiscard]] const Vec3d& nx() const;
        [[nodiscard]] const Vec3d& ny() const;
        [[nodiscard]] const Vec3d& nz() const;
        [[nodiscard]] const Vec3i& grid_size() const;
        [[nodiscard]] unsigned int grid_size(unsigned int i) const;
        [[nodiscard]] const Vec3d& scale() const;
        [[nodiscard]] double scale(unsigned int i) const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] MeasuringPlaneTransformation& operator=(const MeasuringPlaneTransformation&);
        [[maybe_unused]] MeasuringPlaneTransformation& operator=(MeasuringPlaneTransformation&&) noexcept;
        /// @}

        void set_center(const Vec3d& _c);
        void set_center(double cx, double cy, double cz);
        void set_nx(const Vec3d& _nx);
        void set_nx(double nxx, double nxy, double nxz);
        void set_ny(const Vec3d& _ny);
        void set_ny(double nyx, double nyy, double nyz);
        void set_nz(const Vec3d& _nz);
        void set_nz(double nzx, double nzy, double nzz);
        void set_grid_size(const Vec3i& _gs);
        void set_grid_size(int gsx, int gsy, int gsz);
        void set_scale(const Vec3d& _scale);
        void set_scale(double sx, double sy, double sz);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- TO WORLD COORDINATES
        [[nodiscard]] Vec4d to_world_coordinates(double x, double y, double t) const;

        template<typename TIteratableClass>
        [[nodiscard]] Vec4d to_world_coordinates(const TIteratableClass& vec) const
        {
            std::array<double, 3> temp{{0, 0, 0}};
            auto ittemp = temp.begin();

            for (auto it = std::begin(vec); it != std::end(vec); ++it, ++ittemp)
            { *ittemp = *it; }

            return to_world_coordinates(temp[0], temp[1], temp[2]);
        }

        template<typename T>
        [[nodiscard]] Vec4d to_world_coordinates(std::initializer_list<T> ilist) const
        {
            std::array<double, 3> temp{{0, 0, 0}};
            auto ittemp = temp.begin();

            for (auto it = std::begin(ilist); it != std::end(ilist); ++it, ++ittemp)
            { *ittemp = *it; }

            return to_world_coordinates(temp[0], temp[1], temp[2]);
        }
        /// @}

        /// @{ -------------------------------------------------- TO OBJECT COORDINATES
        [[nodiscard]] Vec3d to_object_coordinates(double x, double y, double z, double t = 0.0) const;

        template<typename T>
        [[nodiscard]] Vec3d to_object_coordinates(const T& vec) const
        {
            std::array<double, 4> temp{{0, 0, 0, 0}};
            auto ittemp = temp.begin();

            for (auto it = std::begin(vec); it != std::end(vec); ++it, ++ittemp)
            { *ittemp = *it; }

            return to_object_coordinates(temp[0], temp[1], temp[2], temp[3]);
        }

        template<typename T>
        [[nodiscard]] Vec3d to_object_coordinates(std::initializer_list<T> ilist) const
        {
            std::array<double, 4> temp{{0, 0, 0, 0}};
            auto ittemp = temp.begin();

            for (auto it = std::begin(ilist); it != std::end(ilist); ++it, ++ittemp)
            { *ittemp = *it; }

            return to_object_coordinates(temp[0], temp[1], temp[2], temp[3]);
        }
        /// @}
    }; // class MeasuringPlaneTransformation
  } // inline namespace cmr
} // namespace bk

#endif //BK_MEASURINGPLANETRANSFORMATION_H
