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

#include <bkGL/texture/TextureCubeMap.h>

#include <climits>

#include <bk/Image>
#include <bkGL/texture/Texture2D.h>
#include <bk/Matrix>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct TextureCubeMap::Impl
  {
      GLenum texture_unit;

      Impl()
          : texture_unit(GL_TEXTURE0)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  TextureCubeMap::TextureCubeMap()
      : base_type(),
  #else
  TextureCubeMap::TextureCubeMap(bk::qt_gl_functions* gl)
      : base_type(gl),
  #endif
        _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  TextureCubeMap::TextureCubeMap(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  TextureCubeMap::~TextureCubeMap() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TEXTURE UNIT
  GLenum TextureCubeMap::texture_unit() const
  { return _pdata->texture_unit; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto TextureCubeMap::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET TEXTURE UNIT
  void TextureCubeMap::set_texture_unit(GLenum t)
  {
      if (t == GL_TEXTURE0 || t == GL_TEXTURE1 || t == GL_TEXTURE2 || t == GL_TEXTURE3 || t == GL_TEXTURE4 || t == GL_TEXTURE5 || t == GL_TEXTURE6 || t == GL_TEXTURE7 || t == GL_TEXTURE8 || t == GL_TEXTURE9 || t == GL_TEXTURE10 || t == GL_TEXTURE11 || t == GL_TEXTURE12 || t == GL_TEXTURE13 || t == GL_TEXTURE14 || t == GL_TEXTURE15 || t == GL_TEXTURE16 || t == GL_TEXTURE17 || t == GL_TEXTURE18 || t == GL_TEXTURE19 || t == GL_TEXTURE20 || t == GL_TEXTURE21 || t == GL_TEXTURE22 || t == GL_TEXTURE23 || t == GL_TEXTURE24 || t == GL_TEXTURE25 || t == GL_TEXTURE26 || t == GL_TEXTURE27 || t == GL_TEXTURE28 || t == GL_TEXTURE29 || t == GL_TEXTURE30 || t == GL_TEXTURE31)
      { _pdata->texture_unit = t; }
  }

  void TextureCubeMap::set_texture_unit_number(GLuint i)
  {
      switch (i)
      {
          default: [[fallthrough]]
          case 0: _pdata->texture_unit = GL_TEXTURE0;
              break;
          case 1: _pdata->texture_unit = GL_TEXTURE1;
              break;
          case 2: _pdata->texture_unit = GL_TEXTURE2;
              break;
          case 3: _pdata->texture_unit = GL_TEXTURE3;
              break;
          case 4: _pdata->texture_unit = GL_TEXTURE4;
              break;
          case 5: _pdata->texture_unit = GL_TEXTURE5;
              break;
          case 6: _pdata->texture_unit = GL_TEXTURE6;
              break;
          case 7: _pdata->texture_unit = GL_TEXTURE7;
              break;
          case 8: _pdata->texture_unit = GL_TEXTURE8;
              break;
          case 9: _pdata->texture_unit = GL_TEXTURE9;
              break;
          case 10: _pdata->texture_unit = GL_TEXTURE10;
              break;
          case 11: _pdata->texture_unit = GL_TEXTURE11;
              break;
          case 12: _pdata->texture_unit = GL_TEXTURE12;
              break;
          case 13: _pdata->texture_unit = GL_TEXTURE13;
              break;
          case 14: _pdata->texture_unit = GL_TEXTURE14;
              break;
          case 15: _pdata->texture_unit = GL_TEXTURE15;
              break;
          case 16: _pdata->texture_unit = GL_TEXTURE16;
              break;
          case 17: _pdata->texture_unit = GL_TEXTURE17;
              break;
          case 18: _pdata->texture_unit = GL_TEXTURE18;
              break;
          case 19: _pdata->texture_unit = GL_TEXTURE19;
              break;
          case 20: _pdata->texture_unit = GL_TEXTURE20;
              break;
          case 21: _pdata->texture_unit = GL_TEXTURE21;
              break;
          case 22: _pdata->texture_unit = GL_TEXTURE22;
              break;
          case 23: _pdata->texture_unit = GL_TEXTURE23;
              break;
          case 24: _pdata->texture_unit = GL_TEXTURE24;
              break;
          case 25: _pdata->texture_unit = GL_TEXTURE25;
              break;
          case 26: _pdata->texture_unit = GL_TEXTURE26;
              break;
          case 27: _pdata->texture_unit = GL_TEXTURE27;
              break;
          case 28: _pdata->texture_unit = GL_TEXTURE28;
              break;
          case 29: _pdata->texture_unit = GL_TEXTURE29;
              break;
          case 30: _pdata->texture_unit = GL_TEXTURE30;
              break;
          case 31: _pdata->texture_unit = GL_TEXTURE31;
              break;
      }
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void TextureCubeMap::clear_impl()
  {
      if (_id != 0)
      { BK_QT_GL glDeleteTextures(1, &_id); }
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void TextureCubeMap::bind_impl()
  {
      BK_QT_GL glActiveTexture(_pdata->texture_unit);
      BK_QT_GL glEnable(GL_TEXTURE_CUBE_MAP);
      BK_QT_GL glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
  }

  void TextureCubeMap::release_impl()
  {
      BK_QT_GL glActiveTexture(_pdata->texture_unit);
      BK_QT_GL glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }
  /// @}

  /// @{ -------------------------------------------------- VIRTUAL
  #ifdef BK_LIB_PNG_AVAILABLE
  void TextureCubeMap::init_from_rgb_images(const std::string& x_pos_path, const std::string& x_neg_path, const std::string& y_pos_path, const std::string& y_neg_path, const std::string& z_pos_path, const std::string& z_neg_path)
  {
      this->clear();

      CartesianImage<Vec3d, 2> img; // temp image for png loading
      GLuint width = 0;
      GLuint height = 0;
      std::vector<GLfloat> tex_vals;

      auto copy_tex_vals = [&]()
      {
          width = img.geometry().size(0);
          height = img.geometry().size(1);

          tex_vals.resize(width * height * 3);

          //Vec3d minval = Vec3d::Constant(std::numeric_limits<double>::max());
          //Vec3d maxval = Vec3d::Constant(-std::numeric_limits<double>::max());
          //
          //for (unsigned int i = 0; i < img.num_values(); ++i)
          //{
          //    for (unsigned int v = 0; v < 3; ++v)
          //    {
          //        minval[v] = std::min(minval[v], img[i][v]);
          //        maxval[v] = std::max(maxval[v], img[i][v]);
          //    }
          //}

          //const Vec3<GLfloat> range(static_cast<GLfloat>(maxval[0]) - static_cast<GLfloat>(minval[0]), static_cast<GLfloat>(maxval[1]) - static_cast<GLfloat>(minval[1]), static_cast<GLfloat>(maxval[2]) - static_cast<GLfloat>(minval[2]));

          #pragma omp parallel for
          for (unsigned int y = 0; y < height; ++y)
          {
              for (unsigned int x = 0; x < width; ++x)
              {
                  for (unsigned int v = 0; v < 3; ++v)
                  {
                      const unsigned int off = y * width * 3 + x * 3 + v;
                      //tex_vals[off] = (static_cast<GLfloat>(img(x, y)[v]) - static_cast<GLfloat>(minval[v])) / range[v];

                      tex_vals[off] = static_cast<GLfloat>(img(x, y)[v] / static_cast<GLfloat>(255));
                  }
              }
          }
      };

      gl_clear_error();

      BK_QT_GL glGenTextures(1, &_id);
      //gl_print_error("glGenTextures");

      bind();
      //gl_print_error("bind");

      /*
       * x_pos
       */
      img.load_png(x_pos_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_POSITIVE_X");

      /*
       * x_neg
       */
      img.load_png(x_neg_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_NEGATIVE_X");

      /*
       * y_pos
       */
      img.load_png(y_pos_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_POSITIVE_Y");

      /*
       * y_neg
       */
      img.load_png(y_neg_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_NEGATIVE_Y");

      /*
       * z_pos
       */
      img.load_png(z_pos_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_POSITIVE_Z");

      /*
       * z_neg
       */
      img.load_png(z_neg_path);
      copy_tex_vals();
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals.data()));
      //gl_print_error("GL_TEXTURE_CUBE_MAP_NEGATIVE_Z");

      /*
       * cube map texture parameters
       */
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      release();
  }
      #endif
  /// @}
} // namespace bk



