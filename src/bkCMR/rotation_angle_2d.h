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

#ifndef BKCMR_ROTATION_DIRECTION_2D_H
#define BKCMR_ROTATION_DIRECTION_2D_H

#include <bk/Matrix>

namespace bk
{
  [[nodiscard]] double rotation_angle_deg_2d(const bk::Vec2d& p0, const bk::Vec2d& p1);
  [[nodiscard]] double rotation_angle_deg_2d_of_3d_points_projected_onto_plane(const bk::Vec3d& p0, const bk::Vec3d& p1, const bk::Vec3d& planeCenter, const bk::Vec3d& planeDirX, const bk::Vec3d& planeDirY);
} // namespace bk

#endif //BKCMR_ROTATION_DIRECTION_2D_H