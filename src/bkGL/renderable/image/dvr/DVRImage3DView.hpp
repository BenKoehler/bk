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

#ifndef BKGL_DVRIMAGE3DVIEW_HPP
#define BKGL_DVRIMAGE3DVIEW_HPP

#include <bkGL/renderable/image/dvr/DVRImage3DView.h>

#include <bk/Image>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/texture/Texture3D.h>
#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>
#include <bkTools/color/WindowingTransferFunction.h>

namespace bk
{
  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  template<typename TImage3D>
  void DVRImage3DView::init_image(const TImage3D& img)
  {
      const bk::Vec3<GLuint> image_size = img.geometry().size();

      //------------------------------------------------------------------------------------------------------
      // cuboid
      //------------------------------------------------------------------------------------------------------
      const bk::Vec3<GLfloat> p000 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(0, 0, 0));
      const bk::Vec3<GLfloat> p001 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(0, 0, image_size[2] - 1));
      const bk::Vec3<GLfloat> p010 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(0, image_size[1] - 1, 0));
      const bk::Vec3<GLfloat> p011 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(0, image_size[1] - 1, image_size[2] - 1));
      const bk::Vec3<GLfloat> p100 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(image_size[0] - 1, 0, 0));
      const bk::Vec3<GLfloat> p101 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(image_size[0] - 1, 0, image_size[2] - 1));
      const bk::Vec3<GLfloat> p110 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(image_size[0] - 1, image_size[1] - 1, 0));
      const bk::Vec3<GLfloat> p111 = img.geometry().transformation().to_world_coordinates(bk::Vec3<GLfloat>(image_size[0] - 1, image_size[1] - 1, image_size[2] - 1));

      _center() = /* 1/8 = */0.125 * (p000 + p001 + p010 + p011 + p100 + p101 + p110 + p111);

      constexpr unsigned int NUM_VALUES_VBO = 48;

      const GLfloat vertCol[NUM_VALUES_VBO] = {
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

      constexpr unsigned int NUM_INDICES = 36;

      //constexpr const GLuint indices[NUM_INDICES] = {
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
      constexpr const GLuint indices[NUM_INDICES] = {
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


      size_ind() = NUM_INDICES;

      vbo_cuboid().init(static_cast<const GLvoid*>(vertCol), NUM_VALUES_VBO * sizeof(GLfloat));
      ibo_cuboid().init(static_cast<const GLvoid*>(indices), NUM_INDICES * sizeof(GLuint));
      vao_cuboid().init(vbo_cuboid(), ibo_cuboid());

      //------------------------------------------------------------------------------------------------------
      // image texture
      //------------------------------------------------------------------------------------------------------
      this->image_tex().clear();

      this->image_tex().set_width(image_size[0]);
      this->image_tex().set_height(image_size[1]);
      this->image_tex().set_depth(image_size[2]);

      this->image_tex().set_internal_format_R32F();
      this->image_tex().set_format_RED();
      this->image_tex().set_pixel_data_type_FLOAT();

      const unsigned int N = image_size[0] * image_size[1] * image_size[2];
      std::vector<GLfloat> texVals(N);
      GLfloat minVal = std::numeric_limits<GLfloat>::max();
      GLfloat maxVal = -minVal;

      #pragma omp parallel for
      for (unsigned int z = 0; z < image_size[2]; ++z)
      {
          unsigned int cnt = z * image_size[1] * image_size[0];

          for (unsigned int y = 0; y < image_size[1]; ++y)
          {
              for (unsigned int x = 0; x < image_size[0]; ++x)
              {
                  const GLfloat val = static_cast<GLfloat>(img(x, y, z));
                  texVals[cnt++] = val;

                  #pragma omp critical
                  {
                      minVal = std::min(minVal, val);
                      maxVal = std::max(maxVal, val);
                  }
              }
          }
      }

      this->image_tex().set_texture_unit_number(2);
      this->image_tex().init(static_cast<GLvoid*>(texVals.data()));

      this->image_tex().bind();
      this->image_tex().set_interpolation_LINEAR();
      this->image_tex().set_texture_coordinates_CLAMP();
      this->image_tex().release();

      this->image_size().set(image_size[0], image_size[1], image_size[2]);
      this->image_scale().set(img.geometry().transformation().scale(0), img.geometry().transformation().scale(1), img.geometry().transformation().scale(2));

      tf().set_intensity_min_max(minVal, maxVal);
      tf().reset();
      tf_view().update_tf();
  }

  template<typename TImage3D>
  void DVRImage3DView::init(const TImage3D& img, GLuint window_width, GLuint window_height)
  {
      tf_view().init(&tf());
      init_image(img);
      init_shader();
      init_ubo();
      init_fbo(window_width, window_height);

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}
} // namespace bk

#endif // BKGL_DVRIMAGE3DVIEW_HPP


