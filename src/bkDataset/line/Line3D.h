/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BK_LINE3D_H
#define BK_LINE3D_H

#include <any>
#include <cmath>
#include <iostream>
#include <memory>

#include <bkAlgorithm/smooth.h>
#include <bk/Matrix>

#include <bkDataset/line/LineBase.h>
#include <bkDataset/line/Line.h>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  template<> class Line<3> : public details::LineBase<3>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Line<3>;
      using base_type = details::LineBase<3>;

      /// @{ -------------------------------------------------- ATTRIBUTES
      [[nodiscard]] static constexpr const char* LocalCoordinateSystemAttributeName()
      { return "Line<3>_attrib_lcs"; }
      /// @}

    public:
      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init()
      {
          base_type::init();
          calc_consistent_local_coordinate_systems();
      }
      /// @}

      /// @{ -------------------------------------------------- GET LOCAL COORDINATE SYSTEM AT POINT
      [[nodiscard]] bool has_local_coordinate_systems() const
      { return this->point_attribute_map().has_attribute(LocalCoordinateSystemAttributeName()); }

      [[nodiscard]] Mat3d local_coordinate_system_at_point(unsigned int pointId) const
      {
          if (has_local_coordinate_systems())
          { return this->point_attribute_value_of_type<Mat3d>(LocalCoordinateSystemAttributeName(), pointId); }

          const unsigned int nPoints = this->geometry().num_points();
          Mat3d lcs = MatrixFactory::Identity_Mat_3D<double>();

          if (nPoints <= 1)
          { return lcs; }

          lcs.col_ref<2>() = this->tangent_at_point(pointId);
          lcs.col_ref<2>().normalize_internal();

          Vec3d temp = MatrixFactory::X_Axis_3D<double>();
          lcs.col_ref<0>() = temp.cross(lcs.col_ref<2>());

          if (std::abs(lcs.col_ref<0>().norm()) < 1e-6)
          {
              temp = MatrixFactory::Y_Axis_3D<double>();
              lcs.col_ref<0>() = temp.cross(lcs.col_ref<2>());

              if (std::abs(lcs.col_ref<0>().norm()) < 1e-6)
              {
                  temp = MatrixFactory::Z_Axis_3D<double>();
                  lcs.col_ref<0>() = temp.cross(lcs.col_ref<2>());

                  if (std::abs(lcs.col_ref<0>().norm()) < 1e-6)
                  {
                      temp = MatrixFactory::Random_float_Vec_3D<double>(-1.0, +1.0);
                      lcs.col_ref<0>() = temp.cross(lcs.col_ref<2>());
                  }
              }
          }

          lcs.col_ref<0>().normalize_internal();

          lcs.col_ref<1>() = lcs.col_ref<0>().cross(lcs.col_ref<2>());
          lcs.col_ref<1>().normalize_internal();

          return lcs;
      }
      /// @}

      /// @{ -------------------------------------------------- CALC CONSISTENT LOCAL COORDINATE SYSTEM
      [[maybe_unused]] bool calc_consistent_local_coordinate_systems(unsigned int binomial_smooth_iterations = 25, unsigned int binomial_smooth_kernel_size = 5)
      {
          this->point_attribute_map().remove_attribute(LocalCoordinateSystemAttributeName());

          const unsigned int nPoints = this->geometry().num_points();
          if (nPoints == 0)
          {
              std::cerr << "Line3D::calc_consistent_local_coordinate_systems - empty line; abort!" << std::endl;
              return false;
          }

          const Mat3d lcs0 = local_coordinate_system_at_point(0);

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(3 * nPoints, ___("Calculating consistent local coordinate system"));
          #endif

          std::vector<Mat3d>& point_lcs = this->add_point_attribute_vector_of_type<Mat3d>(LocalCoordinateSystemAttributeName());

          // initialization: calculate lcs of first point
          point_lcs[0] = lcs0;

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          for (unsigned int pointId = 1; pointId < nPoints; ++pointId)
          {
              Mat3d& previous_lcs = point_lcs[pointId - 1];
              auto previous_x = previous_lcs.col_ref<0>();
              auto previous_y = previous_lcs.col_ref<1>();
              auto previous_z = previous_lcs.col_ref<2>();

              Mat3d& current_lcs = point_lcs[pointId];
              auto current_x = current_lcs.col_ref<0>();
              auto current_y = current_lcs.col_ref<1>();
              auto current_z = current_lcs.col_ref<2>();

              current_z = this->tangent_at_point(pointId);
              current_z.normalize_internal();

              Vec3d rotAxis(0, 0, 0);
              double rotAngle = 0.0;
              previous_z.rotate_onto_direction(current_z, &rotAngle, &rotAxis);

              current_x = previous_x.rotate_rad(rotAxis, rotAngle);
              current_y = previous_y.rotate_rad(rotAxis, rotAngle);
              bool validRotation = true;

              for (unsigned int vi = 0; vi < 3; ++vi)
              {
                  if (std::isnan(current_x[vi]) || std::isnan(current_y[vi]) || std::isinf(current_x[vi]) || std::isinf(current_y[vi]))
                  {
                      validRotation = false;
                      break;
                  }
              }

              if (!validRotation)
              {
                  current_x = previous_x;
                  current_y = previous_y;
              }

              current_x.normalize_internal();
              current_y.normalize_internal();

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif
          } // for pointId: nPoints

          /*
           * smoothing
           */
          bk::smooth_binomial(point_lcs.begin(), point_lcs.end(), binomial_smooth_iterations, binomial_smooth_kernel_size, MatrixFactory::Zero_Mat_3D());

          #ifdef BK_EMIT_PROGRESS
          prog.increment(nPoints);
          #endif

          #pragma omp parallel for
          for (unsigned int pointId = 0; pointId < nPoints; ++pointId)
          {
              point_lcs[pointId].normalize_cols_internal();

              #ifdef BK_EMIT_PROGRESS
              #pragma omp critical
              { prog.increment(1); }
              #endif
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return true;
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filename)
      {
          const bool success = base_type::load(filename);
          this->init(); // calc lcs
          return success;
      }
      /// @}
  }; // class Line  
} // namespace bk

#endif //BK_LINE3D_H
