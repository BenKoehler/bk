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

#ifndef BK_PRESSUREVIEW_HPP
#define BK_PRESSUREVIEW_HPP

#include "PressureView.h"
#include <bk/Image>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/texture/Texture3D.h>

namespace bk
{
  template<typename Mesh3D_, typename Img4D_, typename Seg3D_>
  void PressureView::init(const Mesh3D_& mesh, const Img4D_& pressureMap, const Seg3D_& vesselSeg, GLuint window_width, GLuint window_height)
  {
      //------------------------------------------------------------------------------------------------------
      // copy image
      //------------------------------------------------------------------------------------------------------
      const auto& image_size = pressureMap.geometry().size();
      const int stride_t = stride_of_dim(image_size, 3);

      img().set_size(image_size);
      img().geometry().transformation().set_scale(pressureMap.geometry().transformation().scale());

      minVal() = std::numeric_limits<GLfloat>::max();
      maxVal() = -minVal();

      #pragma omp parallel for
      for (unsigned int x = 0; x < image_size[0]; ++x)
      {
          for (unsigned int y = 0; y < image_size[1]; ++y)
          {
              for (unsigned int z = 0; z < image_size[2]; ++z)
              {
                  if (vesselSeg(x, y, z) != 0)
                  {
                      unsigned int lid = grid_to_list_id(image_size, x, y, z, 0);

                      for (unsigned int t = 0; t < image_size[3]; ++t)
                      {
                          img()[lid] = pressureMap[lid];

                          #pragma omp critical
                          {
                              minVal() = std::min(minVal(), static_cast<GLfloat>(img()[lid]));
                              maxVal() = std::max(maxVal(), static_cast<GLfloat>(img()[lid]));
                          };

                          lid += stride_t;
                      } // for t
                  }
              } // for z
          } // for y
      } // for x

      //------------------------------------------------------------------------------------------------------
      // image texture
      //------------------------------------------------------------------------------------------------------
      std::vector<GLfloat> texVals(image_size[0] * image_size[1] * image_size[2], 0);

      for (unsigned int i = 0; i < 2; ++i)
      {
          Texture3D& tex = image_tex(i);

          tex.clear();

          tex.set_width(image_size[0]);
          tex.set_height(image_size[1]);
          tex.set_depth(image_size[2]);

          tex.set_internal_format_R32F();
          tex.set_format_RED();
          tex.set_pixel_data_type_FLOAT();

          tex.set_texture_unit_number(2 + 2 * i); // yields: tex0 - >2, tex1 -> 4
          tex.init(static_cast<GLvoid*>(texVals.data()));

          tex.bind();
          tex.set_interpolation_LINEAR();
          tex.set_texture_coordinates_CLAMP();
          tex.release();
      }

      //------------------------------------------------------------------------------------------------------
      // mesh
      //------------------------------------------------------------------------------------------------------
      const unsigned int num_points = mesh.geometry().num_points();
      const unsigned int num_triangles = mesh.topology().num_cells();
      std::vector<GLfloat> vertices(3 * num_points);

      #pragma omp parallel for
      for (unsigned int i = 0; i < num_points; ++i)
      {
          const auto pt = mesh.geometry().point_in_world_coordinates(i);

          for (unsigned int k = 0; k < 3; ++k)
          { vertices[3 * i + k] = pt[k]; }
      }

      std::vector<GLuint> indices(3 * num_triangles);
      #pragma omp parallel for
      for (unsigned int i = 0; i < num_triangles; ++i)
      {
          const auto& tri = mesh.topology().cell(i);

          for (unsigned int k = 0; k < 3; ++k)
          { indices[3 * i + k] = tri[k]; }
      }

      sizeInd_mesh() = 3 * num_triangles;

      vbo_mesh().init(vertices);
      ibo_mesh().init(indices);
      vao_mesh().init(vbo_mesh(), ibo_mesh());

      //------------------------------------------------------------------------------------------------------
      // cuboid
      //------------------------------------------------------------------------------------------------------
      const Vec3<GLfloat> p000 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(0, 0, 0));
      const Vec3<GLfloat> p001 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(0, 0, image_size[2] - 1));
      const Vec3<GLfloat> p010 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(0, image_size[1] - 1, 0));
      const Vec3<GLfloat> p011 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(0, image_size[1] - 1, image_size[2] - 1));
      const Vec3<GLfloat> p100 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(image_size[0] - 1, 0, 0));
      const Vec3<GLfloat> p101 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(image_size[0] - 1, 0, image_size[2] - 1));
      const Vec3<GLfloat> p110 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(image_size[0] - 1, image_size[1] - 1, 0));
      const Vec3<GLfloat> p111 = pressureMap.geometry().transformation().to_world_coordinates(Vec3<GLfloat>(image_size[0] - 1, image_size[1] - 1, image_size[2] - 1));

      center() = 0.125 * (p000 + p001 + p010 + p011 + p100 + p101 + p110 + p111);

      const GLfloat vertCol_cuboid[48] = {
          // vert 0
          p000[0], p000[1], p000[2], 0, 0, 0,
          // vert 1
          p001[0], p001[1], p001[2], 0, 0, 1,
          // vert 2
          p010[0], p010[1], p010[2], 0, 1, 0,
          // vert 3
          p011[0], p011[1], p011[2], 0, 1, 1,
          // vert 4
          p100[0], p100[1], p100[2], 1, 0, 0,
          // vert 5
          p101[0], p101[1], p101[2], 1, 0, 1,
          // vert 6
          p110[0], p110[1], p110[2], 1, 1, 0,
          // vert 7
          p111[0], p111[1], p111[2], 1, 1, 1};

      //constexpr const GLuint indices_cuboid[36] = {
      //    // front (0 4 2 6)
      //    2, 4, 0, 6, 4, 2,
      //    // back (5 1 7 3)
      //    7, 1, 5, 3, 1, 7,
      //    // left (1 0 3 2)
      //    3, 0, 1, 2, 0, 3,
      //    // right (4 5 6 7)
      //    6, 5, 4, 7, 5, 6,
      //    // bottom (1 5 0 4)
      //    0, 5, 1, 4, 5, 0,
      //    // top (2 6 3 7)
      //    3, 6, 2, 7, 6, 3};

      // invert triangle ordering for correct culling
      constexpr const GLuint indices_cuboid[36] = {
          // front (0 4 2 6)
          0, 4, 2, 2, 4, 6,
          // back (5 1 7 3)
          5, 1, 7, 7, 1, 3,
          // left (1 0 3 2)
          1, 0, 3, 3, 0, 2,
          // right (4 5 6 7)
          4, 5, 6, 6, 5, 7,
          // bottom (1 5 0 4)
          1, 5, 0, 0, 5, 4,
          // top (2 6 3 7)
          2, 6, 3, 3, 6, 7};

      sizeInd_cuboid() = 36;

      vbo_cuboid().init(static_cast<const GLvoid*>(vertCol_cuboid), 48 * sizeof(GLfloat));
      ibo_cuboid().init(static_cast<const GLvoid*>(indices_cuboid), 36 * sizeof(GLuint));
      vao_cuboid().init(vbo_cuboid(), ibo_cuboid());

      //------------------------------------------------------------------------------------------------------
      // init
      //------------------------------------------------------------------------------------------------------
      init_shader();
      init_ubo();
      init_fbo(window_width, window_height);

      on_animation_time_changed(0); // upload tex of t0 and t1

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
} // namespace bk

#endif // BK_PRESSUREVIEW_HPP


