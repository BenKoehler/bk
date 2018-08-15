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

#ifndef BKCMR_FLOWJETPOINT_H
#define BKCMR_FLOWJETPOINT_H

#include <bk/Matrix>

namespace bk
{
  inline namespace cmr
  {
    struct FlowJetPoint
    {
        bk::Vec3d local_coord_sys_x;
        bk::Vec3d local_coord_sys_y;

        // glyph
        bk::Vec3d peak_velocity_position;
        double peak_velocity;

        // area
        bk::Vec3d area_center;
        double area_radius_x;
        double area_radius_y;
        bk::Vec3d area_dir_x;
        bk::Vec3d area_dir_y;

        // vessel
        bk::Vec3d vessel_center;
        double vessel_radius;
    };
  } // inline namespace cmr
} // namespace bk

#endif //BKCMR_FLOWJETPOINT_H
