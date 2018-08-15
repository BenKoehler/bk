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

#include <bkCMR/rotation_angle_2d.h>

#include <cmath>

#include <bkMath/constants/pi.h>
#include <bkMath/functions/radians_degree_conversion.h>

namespace bk
{
  double rotation_angle_deg_2d(const bk::Vec2d& p0, const bk::Vec2d& p1)
  {
      constexpr const double ccw = -1;
      constexpr const double cw = +1;
      constexpr const double pi1 = bk::pi<double>;
      constexpr const double pi2 = pi1 / 2;
      const double alpha0 = std::atan2(p0[1], p0[0]);
      const double alpha1 = std::atan2(p1[1], p1[0]);
      const double rotangle_deg = std::fmod(bk::radians_to_degrees(std::abs(alpha0 - alpha1)), 180);

      // both on top or bottom half
      if ((alpha0 > 0 && alpha1 > 0) || (alpha0 < 0 && alpha1 < 0))
      { return (alpha0 < alpha1 ? ccw : cw) * rotangle_deg; }
      else
      {
          //both on left or right half
          if (alpha0 > 0 && alpha0 <= pi2 && alpha1 < 0 && alpha1 >= -pi2)
          {
              // top right to bottom right quadrant
              return cw * rotangle_deg;
          }
          else if (alpha1 > 0 && alpha1 < pi2 && alpha0 < 0 && alpha0 >= -pi2)
          {
              // bottom right to top right quadrant
              return ccw * rotangle_deg;
          }
          else if (alpha0 > pi2 && alpha0 <= pi1 && alpha1 < -pi2 && alpha1 >= -pi1)
          {
              // top left to bottom left quadrant
              return ccw * rotangle_deg;
          }
          else if (alpha1 > pi2 && alpha1 <= pi1 && alpha0 < -pi2 && alpha0 >= -pi1)
          {
              // bottom left to top left quadrant
              return cw * rotangle_deg;
          }
          else
          {
              //diagonal quadrants: evaluate y-axis intersection
              const bk::Vec2d d = p0 - p1;
              //const double l = p1[0] / d[0]; // todo: -p1[0] ?
              const double l = -p1[0] / d[0]; // todo: -p1[0] ?
              const double s = p1[1] + l * d[1];

              if ((alpha0 > 0 && alpha0 <= pi2) || (alpha0 <= 0 && alpha0 > -pi2))
              {
                  // qk on right half
                  return (s > 0 ? ccw : cw) * rotangle_deg;
              }
              else
              {
                  // qk on left half
                  return (s > 0 ? cw : ccw) * rotangle_deg;
              }
          }
      }
  }

  double rotation_angle_deg_2d_of_3d_points_projected_onto_plane(const bk::Vec3d& p0, const bk::Vec3d& p1, const bk::Vec3d& planeCenter, const bk::Vec3d& planeDirX, const bk::Vec3d& planeDirY)
  {
      const bk::Vec3d diff0 = p0 - planeCenter;
      const bk::Vec3d diff1 = p1 - planeCenter;

      return rotation_angle_deg_2d(bk::Vec2d(planeDirX.dot(diff0), planeDirY.dot(diff0)), bk::Vec2d(planeDirX.dot(diff1), planeDirY.dot(diff1)));
  }
} // namespace bk


