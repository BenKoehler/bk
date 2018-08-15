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

#ifndef BK_TRIANGULARMESH3D_H
#define BK_TRIANGULARMESH3D_H

#include <bkDataset/mesh/TriangularMeshBase.h>
#include <bkDataset/mesh/TriangularMesh.h>
#include <bkDataset/attributes/attribute_info.h>

namespace bk
{
  template<> class TriangularMesh<3> : public details::TriangularMeshBase<3>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TriangularMesh<3>;
      using base_type = details::TriangularMeshBase<3>;
    public:
      using point_type = typename base_type::point_type;
      using cell_type = typename base_type::cell_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      TriangularMesh() = default;
      TriangularMesh(const self_type&) = default;
      TriangularMesh(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TriangularMesh() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HAS NORMALS
      [[nodiscard]] bool has_point_normals() const
      { return this->point_attribute_map().has_attribute(attribute_info::normal3d()); }

      [[nodiscard]] bool has_triangle_normals() const
      { return this->cell_attribute_map().has_attribute(attribute_info::normal3d()); }

      [[nodiscard]] bool has_normals() const
      { return has_point_normals() && has_triangle_normals(); }
      /// @}

      /// @{ -------------------------------------------------- GET POINT NORMALS
      Vec3d& normal_of_point(unsigned int pointId)
      { return normals_of_points()[pointId]; }

      const Vec3d& normal_of_point(unsigned int pointId) const
      { return normals_of_points()[pointId]; }

    private:
      std::vector<attribute_info::normal3d_value_type>& normals_of_points()
      {
          assert(has_point_normals() && "call calc_normals() first");
          return this->point_attribute_vector<attribute_info::normal3d()>();
      }

    public:
      const std::vector<attribute_info::normal3d_value_type>& normals_of_points() const
      {
          assert(has_point_normals() && "call calc_normals() first");
          return this->point_attribute_vector<attribute_info::normal3d()>();
      }
      /// @}

      /// @{ -------------------------------------------------- GET CELL NORMALS
      Vec3d& normal_of_triangle(unsigned int cellId)
      { return normals_of_triangles()[cellId]; }

      const Vec3d& normal_of_triangle(unsigned int cellId) const
      { return normals_of_triangles()[cellId]; }

    private:
      std::vector<attribute_info::normal3d_value_type>& normals_of_triangles()
      {
          assert(has_triangle_normals() && "call calc_normals() first");
          return this->cell_attribute_vector<attribute_info::normal3d()>();
      }

    public:
      const std::vector<attribute_info::normal3d_value_type>& normals_of_triangles() const
      {
          assert(has_triangle_normals() && "call calc_normals() first");
          return this->cell_attribute_vector<attribute_info::normal3d()>();
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init()
      {
          base_type::init();
          calc_normals();
      }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: INITIALIZATION
    protected:
      void calc_normals_per_triangle()
      {
          std::vector<attribute_info::normal3d_value_type>& cell_normals = this->add_cell_attribute_vector<attribute_info::normal3d()>();

          #pragma omp parallel for
          for (unsigned int cellId = 0; cellId < this->topology().num_cells(); ++cellId)
          {
              const Cell<3>& c = this->topology().cell(cellId);

              const Vec3d diff0 = this->geometry().point(c[1]) - this->geometry().point(c[0]);
              const Vec3d diff1 = this->geometry().point(c[2]) - this->geometry().point(c[0]);

              cell_normals[cellId] = (diff0).cross(diff1).normalize();
          }
      }

      void calc_normals_per_point()
      {
          const std::vector<attribute_info::normal3d_value_type>& cell_normals = this->cell_attribute_vector<attribute_info::normal3d()>();
          assert(!cell_normals.empty() && "call calc_normals_per_triangle() first");

          std::vector<attribute_info::normal3d_value_type>& point_normals = this->add_point_attribute_vector<attribute_info::normal3d()>();

          #pragma omp parallel for
          for (unsigned int pointId = 0; pointId < this->geometry().num_points(); ++pointId)
          {
              const std::vector<unsigned int>& triangles = this->topology().cells_of_point(pointId);
              assert(!triangles.empty() && "this point does not belong to any triangle");

              point_normals[pointId].set_zero();

              for (unsigned int trianglePointId = 0; trianglePointId < triangles.size(); ++trianglePointId)
              { point_normals[pointId] += cell_normals[triangles[trianglePointId]]; }

              point_normals[pointId] /= triangles.size();
              //point_normals[pointId].normalize_internal(); // not necessary because the cell normals are already normalized
          }
      }

      //! heuristic: majority of normals points away from center of points
      [[maybe_unused]] bool consistent_normal_orientation()
      {
          std::vector<attribute_info::normal3d_value_type>& point_normals = normals_of_points();
          std::vector<attribute_info::normal3d_value_type>& cell_normals = normals_of_triangles();

          if (!point_normals.empty() && !cell_normals.empty())
          { return false; }

          const auto meshCenter = this->geometry().center();

          if (point_normals.empty())
          {
              const unsigned int numPoints = this->geometry().num_points();
              unsigned int cntAway = 0;
              unsigned int cntTowards = 0;

              #pragma omp parallel for reduction(+:cntAway) reduction(+:cntTowards)
              for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
              {
                  Vec3d dirToCenter = meshCenter - this->geometry().point(pointId);
                  const double n = dirToCenter.norm();

                  if (bk::equals_approx(n, 0))
                  { continue; }

                  dirToCenter /= n;

                  if (dirToCenter.dot(point_normals[pointId]) < 0)
                  { ++cntAway; }
                  else
                  { ++cntTowards; }
              }

              if (cntTowards > cntAway)
              {
                  // more normals point towards the center than away from it -> flip normals

                  for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
                  { point_normals[pointId].negate_internal(); }
              }
          }

          if (cell_normals.empty())
          {
              const unsigned int numCells = this->topology().num_cells();
              unsigned int cntAway = 0;
              unsigned int cntTowards = 0;

              #pragma omp parallel for reduction(+:cntAway) reduction(+:cntTowards)
              for (unsigned int cellId = 0; cellId < numCells; ++cellId)
              {
                  const Cell<3>& triangle = this->topology().cell(cellId);
                  const Vec3d triangleCenter = (this->geometry().point(triangle[0]) + this->geometry().point(triangle[1]) + this->geometry().point(triangle[2])) / 3;
                  Vec3d dirToCenter = meshCenter - triangleCenter;
                  const double n = dirToCenter.norm();

                  if (bk::equals_approx(n, 0))
                  { continue; }

                  dirToCenter /= n;

                  if (dirToCenter.dot(cell_normals[cellId]) < 0)
                  { ++cntAway; }
                  else
                  { ++cntTowards; }
              }

              // more normals point towards the center than away from it -> flip normals
              if (cntTowards > cntAway)
              {
                  for (unsigned int cellId = 0; cellId < numCells; ++cellId)
                  { cell_normals[cellId].negate_internal(); }
              }
          }

          return true;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- CALC NORMALS
      void calc_normals()
      {
          calc_normals_per_triangle();
          calc_normals_per_point();
          consistent_normal_orientation();
      }
      /// @}

      /// @{ -------------------------------------------------- IS INSIDE
      [[nodiscard]] bool is_inside(const point_type& queryPoint) const
      {
          const bk::KDPointInfo<point_type> closestPoint = this->geometry().closest_point(queryPoint);
          const attribute_info::normal3d_value_type& nrml = normal_of_point(closestPoint.point_id);
          const Vec3d dirToPoint = closestPoint.point - queryPoint;

          return nrml.dot(dirToPoint) >= 0;
      }
      /// @}

      /// @{ -------------------------------------------------- DISTANCE TO SURFACE
      [[nodiscard]] double distance_to_surface(const point_type& queryPoint) const
      {
          const bk::KDPointInfo<point_type> closestPoint = this->geometry().closest_point(queryPoint);
          const std::vector<unsigned int>& closestTriangles = this->topology().cells_of_point(closestPoint.point_id);

          if (closestTriangles.empty())
          { return closestPoint.distance_to_query; }

          bool foundProjection = false;
          double minSqDist = std::numeric_limits<double>::max();
          Vec3d minDistProjection(0, 0, 0);

          //const std::vector<Vec3d>& point_normals = normals_of_points();
          const std::vector<attribute_info::normal3d_value_type>& cell_normals = normals_of_triangles();

          for (unsigned int t = 0; t < closestTriangles.size(); ++t)
          {
              const Cell<3> trianglePointsIds = this->topology().cell(closestTriangles[t]);

              const point_type& point0 = this->geometry().point(trianglePointsIds[0]);
              const point_type& point1 = this->geometry().point(trianglePointsIds[1]);
              const point_type& point2 = this->geometry().point(trianglePointsIds[2]);

              //const Vec3d& normal0 = point_normals[trianglePointsIds[0]];
              //const Vec3d& normal1 = point_normals[trianglePointsIds[1]];
              //const Vec3d& normal2 = point_normals[trianglePointsIds[2]];

              //const Vec3d cellNormal(((*normal0) + (*normal1) + (*normal2)) / 3.0);
              const attribute_info::normal3d_value_type& cellNormal = cell_normals[closestTriangles[t]];

              /*
               *  ray-triangle intersection by haines & moeller
               */
              // B - A
              const Vec3d u(point1 - point0);

              // C - A
              const Vec3d v(point2 - point0);

              // P - A
              const Vec3d w(queryPoint - point0);
              const Vec3d dvCross(cellNormal.cross(v));
              const Vec3d wuCross(w.cross(u));
              const double dvCrossUDot = dvCross.dot(u);
              const double dvCrossWDot = dvCross.dot(w);
              const double wuCrossDDot = wuCross.dot(cellNormal);
              const double norm = 1.0 / dvCrossUDot;
              const double res_r = norm * dvCrossWDot;
              const double res_s = norm * wuCrossDDot;

              // projected point in triangle?
              double sqDist = std::numeric_limits<double>::max();

              if (res_r >= 0.0 && res_s >= 0.0 && res_r + res_s <= 1.0)
              {
                  const Vec3d projection(point0 + res_r * u + res_s * v);

                  sqDist = (projection - queryPoint).norm_squared();

                  if (sqDist < minSqDist)
                  {
                      minSqDist = sqDist;
                      minDistProjection = projection;
                      foundProjection = true;
                  }
              }
          } // for triangles of closest point

          return foundProjection ? queryPoint.distance(minDistProjection) : closestPoint.distance_to_query;
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save_matlab(std::string_view filename) const
      {
          std::string fname(filename);
          const std::string matlab_suffix = ".m";

          if (fname.empty())
          { fname = "trimesh_matlab" + matlab_suffix; }
          else if (fname.compare(fname.length() - 2, 2, matlab_suffix) != 0)
          { fname.append(matlab_suffix); }

          std::ofstream file(fname, std::ios_base::out);

          if (!file.good())
          { return false; }

          file.precision(4);
          file << std::fixed;

          std::stringstream ss_x;
          ss_x.precision(4);
          ss_x << std::fixed;

          std::stringstream ss_y;
          ss_y.precision(4);
          ss_y << std::fixed;

          std::stringstream ss_z;
          ss_z.precision(4);
          ss_z << std::fixed;

          const unsigned int numVert = this->geometry().num_points();

          for (unsigned int i = 0; i < numVert; ++i)
          {
              const point_type& v = this->geometry().point(i);
              ss_x << v[0];
              ss_y << v[1];
              ss_z << v[2];

              if (i != numVert - 1)
              {
                  ss_x << " ";
                  ss_y << " ";
                  ss_z << " ";
              }
          }

          std::stringstream ss_tri;
          ss_tri.precision(4);
          ss_tri << std::fixed;

          const unsigned int numTri = this->topology().num_cells();

          for (unsigned int i = 0; i < numTri; ++i)
          {
              const Cell<3>& c = this->topology().cell(i);
              ss_tri << (c[0] + 1) << " " << (c[1] + 1) << " " << (c[2] + 1);

              if (i != numTri - 1)
              { ss_tri << "; "; }
          }

          file << "clc; clear all; close all;" << std::endl;
          file << std::endl;
          file << "x = [" << ss_x.str() << "];" << std::endl;
          file << "y = [" << ss_y.str() << "];" << std::endl;
          file << "z = [" << ss_z.str() << "];" << std::endl;
          file << "tri = [" << ss_tri.str() << "];" << std::endl;
          file << std::endl;
          file << "figure('Renderer', 'OpenGL');" << std::endl;
          file << "hold on;" << std::endl;
          file << "colormap([0.5 0.5 0.5]);" << std::endl;
          file << "trisurf(tri, x, y, z);" << std::endl;
          file << "axis tight equal off;" << std::endl;
          file << "shading interp;" << std::endl;
          file << "light('Position', [0 0 -50]);" << std::endl;
          file << "light('Position', [0 0 50]);" << std::endl;
          file << "lighting phong;" << std::endl;
          //file << "view(0, -90)" << std::endl;
          file << "hold off;" << std::endl;
          file.close();

          return true;
      }

      [[maybe_unused]] bool save_obj(std::string_view filename) const
      {
          std::string fname(filename);
          const std::string obj_suffix = ".obj";

          if (fname.empty())
          { fname = "trimesh" + obj_suffix; }
          else if (fname.compare(fname.length() - 2, 2, obj_suffix) != 0)
          { fname.append(obj_suffix); }

          std::ofstream file(fname, std::ios_base::out);

          if (!file.good())
          { return false; }

          file.precision(8);
          file << std::fixed;

          // object name

          file << "o TriMesh" << std::endl;

          // vertices

          const unsigned int numVert = this->geometry().num_points();

          for (unsigned int i = 0; i < numVert; ++i)
          {
              const point_type& v = this->geometry().point(i);
              file << "v ";
              file << v[0] << " ";
              file << v[1] << " ";
              file << v[2] << std::endl;
          }

          // normals

          const std::vector<attribute_info::normal3d_value_type>& pointNormals = normals_of_points();

          for (unsigned int i = 0; i < numVert; ++i)
          {
              const attribute_info::normal3d_value_type& n = pointNormals[i];
              file << "vn ";
              file << n[0] << " ";
              file << n[1] << " ";
              file << n[2] << std::endl;
          }

          // triangles

          for (unsigned int i = 0; i < this->topology().num_cells(); ++i)
          {
              const Cell<3>& c = this->topology().cell(i);
              file << "f ";
              file << (c[0] + 1) << " ";
              file << (c[1] + 1) << " ";
              file << (c[2] + 1) << std::endl;
          }

          file.close();

          return true;
      }

      [[maybe_unused]] bool save_stl(std::string_view filename) const
      {
          static_assert(sizeof(float) == 4, "BINARY STL CAN ONLY BE SAVED IF FLOAT IS IMPLEMENTED AS 32 BIT TYPE");

          std::string fname(filename);
          const std::string stl_suffix = ".stl";

          if (fname.empty())
          { fname = "trimesh" + stl_suffix; }
          else if (fname.compare(fname.length() - 2, 2, stl_suffix) != 0)
          { fname.append(stl_suffix); }

          std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);

          if (!file.good())
          { return false; }

          //file << "solid TriMesh" << std::endl;
          std::uint8_t header[80];
          file.write(reinterpret_cast<char*>(header), 80 * sizeof(std::uint8_t));

          std::uint32_t numTri = static_cast<std::uint32_t>(this->topology().num_cells());
          file.write(reinterpret_cast<char*>(&numTri), sizeof(std::uint32_t));

          float temp;
          std::uint16_t attribute_byte_count = 0;

          const std::vector<attribute_info::normal3d_value_type>& cellNormals = normals_of_triangles();

          for (unsigned int i = 0; i < numTri; ++i)
          {
              const Cell<3>& tri = this->topology().cell(i);

              // triangle normal
              const attribute_info::normal3d_value_type& trinormal = cellNormals[i];

              for (int n = 0; n < 3; ++n)
              {
                  temp = static_cast<float>(trinormal[n]);
                  file.write(reinterpret_cast<char*>(&temp), sizeof(float));
              }


              // 3x vertex
              for (int k = 0; k < 3; ++k)
              {
                  const point_type& vert = this->geometry().point(tri[k]);

                  for (int n = 0; n < 3; ++n)
                  {
                      temp = static_cast<float>(vert[n]);
                      file.write(reinterpret_cast<char*>(&temp), sizeof(float));
                  }
              }

              // attribute byte count (0 in most applications)
              file.write(reinterpret_cast<char*>(&attribute_byte_count), sizeof(std::uint16_t));
          }

          file.close();
          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filename)
      {
          const bool success = base_type::load(filename);
          this->init(); // calc normals
          return success;
      }
      /// @}
  }; // class TriangularMesh
} // namespace bk

#endif //BK_TRIANGULARMESH3D_H


