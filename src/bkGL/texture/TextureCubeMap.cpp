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

#include <bkGL/texture/TextureCubeMap.h>

#include <climits>
#include <future>
#include <string>

#include <bk/Image>
#include <bkGL/texture/Texture2D.h>
#include <bk/Matrix>
#include <bk/ThreadPool>

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
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  TextureCubeMap::TextureCubeMap()
      : base_type()
  #else
  TextureCubeMap::TextureCubeMap(bk::qt_gl_functions* gl)
      : base_type(gl)
  #endif
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
  std::vector<GLfloat> TextureCubeMap::_make_texture(std::string_view path) const
  {
      CartesianImage<Vec3d, 2> img; // temp image for png loading
      img.load_png(path);

      const GLuint width = img.geometry().size(0);
      const GLuint height = img.geometry().size(1);
      std::vector<GLfloat> tex_vals(width * height * 3, 0.0f);

          #pragma omp parallel for
      for (unsigned int y = 0; y < height; ++y)
      {
          unsigned int off = y * width * 3;

          for (unsigned int x = 0; x < width; ++x)
          {
              for (unsigned int v = 0; v < 3; ++v)
              { tex_vals[off++] = static_cast<GLfloat>(img(x, y)[v] / 255.0); }
          }
      }

      return tex_vals;
  }

  void TextureCubeMap::init_from_rgb_images(std::string_view x_pos_path, std::string_view x_neg_path, std::string_view y_pos_path, std::string_view y_neg_path, std::string_view z_pos_path, std::string_view z_neg_path)
  {
      this->clear();

      std::future<std::vector<GLfloat>> fut_x_pos = bk_threadpool.enqueue([&](){ return _make_texture(x_pos_path); });
      std::future<std::vector<GLfloat>> fut_x_neg = bk_threadpool.enqueue([&](){ return _make_texture(x_neg_path); });
      std::future<std::vector<GLfloat>> fut_y_pos = bk_threadpool.enqueue([&](){ return _make_texture(y_pos_path); });
      std::future<std::vector<GLfloat>> fut_y_neg = bk_threadpool.enqueue([&](){ return _make_texture(y_neg_path); });
      std::future<std::vector<GLfloat>> fut_z_pos = bk_threadpool.enqueue([&](){ return _make_texture(z_pos_path); });
      std::future<std::vector<GLfloat>> fut_z_neg = bk_threadpool.enqueue([&](){ return _make_texture(z_neg_path); });

      CartesianImage<Vec3d, 2> img; // temp image for png loading
      img.load_png(x_pos_path);
      const GLuint width = img.geometry().size(0);
      const GLuint height = img.geometry().size(1);

      BK_QT_GL glGenTextures(1, &_id);
      bind();

      std::vector<GLfloat> tex_vals_x_pos = fut_x_pos.get();
      std::vector<GLfloat> tex_vals_x_neg = fut_x_neg.get();
      std::vector<GLfloat> tex_vals_y_pos = fut_y_pos.get();
      std::vector<GLfloat> tex_vals_y_neg = fut_y_neg.get();
      std::vector<GLfloat> tex_vals_z_pos = fut_z_pos.get();
      std::vector<GLfloat> tex_vals_z_neg = fut_z_neg.get();

      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_x_pos.data()));
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_x_neg.data()));
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_y_pos.data()));
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_y_neg.data()));
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_z_pos.data()));
      BK_QT_GL glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, static_cast<const GLvoid*>(tex_vals_z_neg.data()));

      /*
       * cube map texture parameters
       */
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      BK_QT_GL glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      release();

      tex_vals_x_pos.clear();
      tex_vals_x_pos.shrink_to_fit();
      tex_vals_x_neg.clear();
      tex_vals_x_neg.shrink_to_fit();
      tex_vals_y_pos.clear();
      tex_vals_y_pos.shrink_to_fit();
      tex_vals_y_neg.clear();
      tex_vals_y_neg.shrink_to_fit();
      tex_vals_z_pos.clear();
      tex_vals_z_pos.shrink_to_fit();
      tex_vals_z_neg.clear();
      tex_vals_z_neg.shrink_to_fit();
  }
      #endif
  /// @}
} // namespace bk



