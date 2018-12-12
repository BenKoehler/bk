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

#include <bkGL/texture/AbstractTexture.h>

#include <algorithm>
#include <cassert>

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AbstractTexture::Impl
  {
      GLenum target;
      GLsizei width;
      GLsizei height;
      GLint mipmap_level;
      GLint border_width;
      GLint internal_format;
      GLenum format;
      GLenum pixel_data_type;
      GLenum texture_unit;
      GLuint texture_unit_number;

      Impl(GLuint w, GLuint h)
          : target(GL_TEXTURE_2D),
            width(w),
            height(h),
            mipmap_level(0),
            border_width(0),
            texture_unit(GL_TEXTURE0),
            texture_unit_number(0)
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

  AbstractTexture::AbstractTexture()
      : AbstractTexture(1, 1)
      #else

  AbstractTexture::AbstractTexture(bk::qt_gl_functions* gl)
        : AbstractTexture(1, 1, gl)
      #endif
  { /* do nothing */ }

      #ifndef BK_LIB_QT_AVAILABLE

  AbstractTexture::AbstractTexture(GLuint w, GLuint h)
      : base_type(),
  #else

  AbstractTexture::AbstractTexture(GLuint w, GLuint h, bk::qt_gl_functions* gl)
        : base_type(gl),
  #endif
        _pdata(w, h)
  { set_default_config_gray_tex(); }

  AbstractTexture::AbstractTexture(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractTexture::~AbstractTexture()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TEXTURE UNIT
  GLenum AbstractTexture::texture_unit() const
  { return _pdata->texture_unit; }

  GLuint AbstractTexture::texture_unit_number() const
  { return _pdata->texture_unit_number; }
  /// @}

  /// @{ -------------------------------------------------- GET SIZE
  GLsizei AbstractTexture::width() const
  { return _pdata->width; }

  GLsizei AbstractTexture::height() const
  { return _pdata->height; }
  /// @}

  /// @{ -------------------------------------------------- GET MIPMAP LEVEL
  GLint AbstractTexture::mipmap_level() const
  { return _pdata->mipmap_level; }
  /// @}

  /// @{ -------------------------------------------------- GET BORDER WIDTH
  GLint AbstractTexture::border_width() const
  { return _pdata->border_width; }
  /// @}

  /// @{ -------------------------------------------------- GET INTERNAL FORMAT
  GLint AbstractTexture::internal_format() const
  { return _pdata->internal_format; }
  /// @}

  /// @{ -------------------------------------------------- GET FORMAT
  GLenum AbstractTexture::format() const
  { return _pdata->format; }
  /// @}

  /// @{ -------------------------------------------------- GET PIXEL DATA TYPE
  GLenum AbstractTexture::pixel_data_type() const
  { return _pdata->pixel_data_type; }
  /// @}

  /// @{ -------------------------------------------------- GET TARGET
  GLenum AbstractTexture::target() const
  { return _pdata->target; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractTexture::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET TARGET
  void AbstractTexture::set_target_TEXTURE2D()
  { _pdata->target = GL_TEXTURE_2D; }

  void AbstractTexture::set_target_TEXTURE3D()
  { _pdata->target = GL_TEXTURE_3D; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_POSITIVE_X()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_POSITIVE_X; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_NEGATIVE_X()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_POSITIVE_Y()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_NEGATIVE_Y()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_POSITIVE_Z()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; }

  void AbstractTexture::set_target_TEXTURE_CUBE_MAP_NEGATIVE_Z()
  { _pdata->target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; }
  /// @}

  /// @{ -------------------------------------------------- SET TEXTURE UNIT
  void AbstractTexture::set_texture_unit(GLenum t)
  {
      assert(t == GL_TEXTURE0 || t == GL_TEXTURE1 || t == GL_TEXTURE2 || t == GL_TEXTURE3 || t == GL_TEXTURE4 || t == GL_TEXTURE5 || t == GL_TEXTURE6 || t == GL_TEXTURE7 || t == GL_TEXTURE8 || t == GL_TEXTURE9 || t == GL_TEXTURE10 || t == GL_TEXTURE11 || t == GL_TEXTURE12 || t == GL_TEXTURE13 || t == GL_TEXTURE14 || t == GL_TEXTURE15 || t == GL_TEXTURE16 || t == GL_TEXTURE17 || t == GL_TEXTURE18 || t == GL_TEXTURE19 || t == GL_TEXTURE20 || t == GL_TEXTURE21 || t == GL_TEXTURE22 || t == GL_TEXTURE23 || t == GL_TEXTURE24 || t == GL_TEXTURE25 || t == GL_TEXTURE26 || t == GL_TEXTURE27 || t == GL_TEXTURE28 || t == GL_TEXTURE29 || t == GL_TEXTURE30 || t == GL_TEXTURE31);

      _pdata->texture_unit = t;

      switch (_pdata->texture_unit)
      {
          case GL_TEXTURE0: _pdata->texture_unit_number = 0;
              break;
          case GL_TEXTURE1: _pdata->texture_unit_number = 1;
              break;
          case GL_TEXTURE2: _pdata->texture_unit_number = 2;
              break;
          case GL_TEXTURE3: _pdata->texture_unit_number = 3;
              break;
          case GL_TEXTURE4: _pdata->texture_unit_number = 4;
              break;
          case GL_TEXTURE5: _pdata->texture_unit_number = 5;
              break;
          case GL_TEXTURE6: _pdata->texture_unit_number = 6;
              break;
          case GL_TEXTURE7: _pdata->texture_unit_number = 7;
              break;
          case GL_TEXTURE8: _pdata->texture_unit_number = 8;
              break;
          case GL_TEXTURE9: _pdata->texture_unit_number = 9;
              break;
          case GL_TEXTURE10: _pdata->texture_unit_number = 10;
              break;
          case GL_TEXTURE11: _pdata->texture_unit_number = 11;
              break;
          case GL_TEXTURE12: _pdata->texture_unit_number = 12;
              break;
          case GL_TEXTURE13: _pdata->texture_unit_number = 13;
              break;
          case GL_TEXTURE14: _pdata->texture_unit_number = 14;
              break;
          case GL_TEXTURE15: _pdata->texture_unit_number = 15;
              break;
          case GL_TEXTURE16: _pdata->texture_unit_number = 16;
              break;
          case GL_TEXTURE17: _pdata->texture_unit_number = 17;
              break;
          case GL_TEXTURE18: _pdata->texture_unit_number = 18;
              break;
          case GL_TEXTURE19: _pdata->texture_unit_number = 19;
              break;
          case GL_TEXTURE20: _pdata->texture_unit_number = 20;
              break;
          case GL_TEXTURE21: _pdata->texture_unit_number = 21;
              break;
          case GL_TEXTURE22: _pdata->texture_unit_number = 22;
              break;
          case GL_TEXTURE23: _pdata->texture_unit_number = 23;
              break;
          case GL_TEXTURE24: _pdata->texture_unit_number = 24;
              break;
          case GL_TEXTURE25: _pdata->texture_unit_number = 25;
              break;
          case GL_TEXTURE26: _pdata->texture_unit_number = 26;
              break;
          case GL_TEXTURE27: _pdata->texture_unit_number = 27;
              break;
          case GL_TEXTURE28: _pdata->texture_unit_number = 28;
              break;
          case GL_TEXTURE29: _pdata->texture_unit_number = 29;
              break;
          case GL_TEXTURE30: _pdata->texture_unit_number = 30;
              break;
          case GL_TEXTURE31: _pdata->texture_unit_number = 31;
              break;
      }
  }

  void AbstractTexture::set_texture_unit_number(GLuint i)
  {
      assert(i < 32);

      _pdata->texture_unit_number = i;

      switch (_pdata->texture_unit_number)
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

  /// @{ -------------------------------------------------- SET SIZE
  void AbstractTexture::set_width(GLsizei w)
  { _pdata->width = std::max(1, w); }

  void AbstractTexture::set_height(GLsizei h)
  { _pdata->height = std::max(1, h); }
  /// @}

  /// @{ -------------------------------------------------- SET MIPMAP LEVEL
  void AbstractTexture::set_mipmap_level(GLint l)
  { _pdata->mipmap_level = std::max(0, l); }
  /// @}

  /// @{ -------------------------------------------------- SET BORDER WIDTH
  void AbstractTexture::set_border_width(GLint w)
  { _pdata->border_width = std::clamp(w, static_cast<GLint>(0), static_cast<GLint>(1)); }
  /// @}

  /// @{ -------------------------------------------------- SET INTERNAL FORMAT
  void AbstractTexture::set_internal_format(GLint fmt)
  { _pdata->internal_format = fmt; }

  void AbstractTexture::set_internal_format_DEPTH_COMPONENT()
  { set_internal_format(GL_DEPTH_COMPONENT); }

  void AbstractTexture::set_internal_format_DEPTH_COMPONENT16()
  { set_internal_format(GL_DEPTH_COMPONENT16); }

  void AbstractTexture::set_internal_format_DEPTH_COMPONENT24()
  { set_internal_format(GL_DEPTH_COMPONENT24); }

  void AbstractTexture::set_internal_format_DEPTH_COMPONENT32()
  { set_internal_format(GL_DEPTH_COMPONENT32); }

  void AbstractTexture::set_internal_format_DEPTH_COMPONENT32F()
  { set_internal_format(GL_DEPTH_COMPONENT32F); }

  void AbstractTexture::set_internal_format_DEPTH_STENCIL()
  { set_internal_format(GL_DEPTH_STENCIL); }

  void AbstractTexture::set_internal_format_DEPTH24_STENCIL8()
  { set_internal_format(GL_DEPTH24_STENCIL8); }

  void AbstractTexture::set_internal_format_DEPTH32F_STENCIL8()
  { set_internal_format(GL_DEPTH32F_STENCIL8); }

  void AbstractTexture::set_internal_format_ALPHA()
  { set_internal_format(GL_ALPHA); }

  void AbstractTexture::set_internal_format_ALPHA4()
  { set_internal_format(GL_ALPHA4); }

  void AbstractTexture::set_internal_format_ALPHA8()
  { set_internal_format(GL_ALPHA8); }

  void AbstractTexture::set_internal_format_ALPHA16()
  { set_internal_format(GL_ALPHA16); }

  void AbstractTexture::set_internal_format_LUMINANCE()
  { set_internal_format(GL_LUMINANCE); }

  void AbstractTexture::set_internal_format_LUMINANCE4()
  { set_internal_format(GL_LUMINANCE4); }

  void AbstractTexture::set_internal_format_LUMINANCE8()
  { set_internal_format(GL_LUMINANCE8); }

  void AbstractTexture::set_internal_format_LUMINANCE16()
  { set_internal_format(GL_LUMINANCE16); }

  void AbstractTexture::set_internal_format_INTENSITY()
  { set_internal_format(GL_INTENSITY); }

  void AbstractTexture::set_internal_format_INTENSITY4()
  { set_internal_format(GL_INTENSITY4); }

  void AbstractTexture::set_internal_format_INTENSITY8()
  { set_internal_format(GL_INTENSITY8); }

  void AbstractTexture::set_internal_format_INTENSITY12()
  { set_internal_format(GL_INTENSITY12); }

  void AbstractTexture::set_internal_format_INTENSITY16()
  { set_internal_format(GL_INTENSITY16); }

  void AbstractTexture::set_internal_format_RED()
  { set_internal_format(GL_RED); }

  void AbstractTexture::set_internal_format_R8()
  { set_internal_format(GL_R8); }

  void AbstractTexture::set_internal_format_R8I()
  { set_internal_format(GL_R8I); }

  void AbstractTexture::set_internal_format_R8UI()
  { set_internal_format(GL_R8UI); }

  void AbstractTexture::set_internal_format_R16()
  { set_internal_format(GL_R16); }

  void AbstractTexture::set_internal_format_R16I()
  { set_internal_format(GL_R16I); }

  void AbstractTexture::set_internal_format_R16UI()
  { set_internal_format(GL_R16UI); }

  void AbstractTexture::set_internal_format_R16F()
  { set_internal_format(GL_R16F); }

  void AbstractTexture::set_internal_format_R32I()
  { set_internal_format(GL_R32I); }

  void AbstractTexture::set_internal_format_R32UI()
  { set_internal_format(GL_R32UI); }

  void AbstractTexture::set_internal_format_R32F()
  { set_internal_format(GL_R32F); }

  void AbstractTexture::set_internal_format_RG()
  { set_internal_format(GL_RG); }

  void AbstractTexture::set_internal_format_RG8()
  { set_internal_format(GL_RG8); }

  void AbstractTexture::set_internal_format_RG8I()
  { set_internal_format(GL_RG8I); }

  void AbstractTexture::set_internal_format_RG8UI()
  { set_internal_format(GL_RG8UI); }

  void AbstractTexture::set_internal_format_RG16()
  { set_internal_format(GL_RG16); }

  void AbstractTexture::set_internal_format_RG16I()
  { set_internal_format(GL_RG16I); }

  void AbstractTexture::set_internal_format_RG16UI()
  { set_internal_format(GL_RG16UI); }

  void AbstractTexture::set_internal_format_RG16F()
  { set_internal_format(GL_RG16F); }

  void AbstractTexture::set_internal_format_RG32I()
  { set_internal_format(GL_RG32I); }

  void AbstractTexture::set_internal_format_RG32UI()
  { set_internal_format(GL_RG32UI); }

  void AbstractTexture::set_internal_format_RG32F()
  { set_internal_format(GL_RG32F); }

  void AbstractTexture::set_internal_format_RGB()
  { set_internal_format(GL_RGB); }

  void AbstractTexture::set_internal_format_RGB4()
  { set_internal_format(GL_RGB4); }

  void AbstractTexture::set_internal_format_RGB8()
  { set_internal_format(GL_RGB8); }

  void AbstractTexture::set_internal_format_RGB8I()
  { set_internal_format(GL_RGB8I); }

  void AbstractTexture::set_internal_format_RGB8UI()
  { set_internal_format(GL_RGB8UI); }

  void AbstractTexture::set_internal_format_RGB12()
  { set_internal_format(GL_RGB12); }

  void AbstractTexture::set_internal_format_RGB16()
  { set_internal_format(GL_RGB16); }

  void AbstractTexture::set_internal_format_RGB16I()
  { set_internal_format(GL_RGB16I); }

  void AbstractTexture::set_internal_format_RGB16UI()
  { set_internal_format(GL_RGB16UI); }

  void AbstractTexture::set_internal_format_RGB16F()
  { set_internal_format(GL_RGB16F); }

  void AbstractTexture::set_internal_format_RGB32I()
  { set_internal_format(GL_RGB32I); }

  void AbstractTexture::set_internal_format_RGB32UI()
  { set_internal_format(GL_RGB32UI); }

  void AbstractTexture::set_internal_format_RGB32F()
  { set_internal_format(GL_RGB32F); }

  void AbstractTexture::set_internal_format_RGBA()
  { set_internal_format(GL_RGBA); }

  void AbstractTexture::set_internal_format_RGBA4()
  { set_internal_format(GL_RGBA4); }

  void AbstractTexture::set_internal_format_RGBA8()
  { set_internal_format(GL_RGBA8); }

  void AbstractTexture::set_internal_format_RGBA8I()
  { set_internal_format(GL_RGBA8I); }

  void AbstractTexture::set_internal_format_RGBA8UI()
  { set_internal_format(GL_RGBA8UI); }

  void AbstractTexture::set_internal_format_RGBA12()
  { set_internal_format(GL_RGBA12); }

  void AbstractTexture::set_internal_format_RGBA16()
  { set_internal_format(GL_RGBA16); }

  void AbstractTexture::set_internal_format_RGBA16I()
  { set_internal_format(GL_RGBA16I); }

  void AbstractTexture::set_internal_format_RGBA16UI()
  { set_internal_format(GL_RGBA16UI); }

  void AbstractTexture::set_internal_format_RGBA16F()
  { set_internal_format(GL_RGBA16F); }

  void AbstractTexture::set_internal_format_RGBA32I()
  { set_internal_format(GL_RGBA32I); }

  void AbstractTexture::set_internal_format_RGBA32UI()
  { set_internal_format(GL_RGBA32UI); }

  void AbstractTexture::set_internal_format_RGBA32F()
  { set_internal_format(GL_RGBA32F); }
  /// @}

  /// @{ -------------------------------------------------- SET FORMAT
  void AbstractTexture::set_format(GLenum fmt)
  { _pdata->format = fmt; }

  void AbstractTexture::set_format_RED()
  { set_format(GL_RED); }

  void AbstractTexture::set_format_RED_INTEGER()
  { set_format(GL_RED_INTEGER); }

  void AbstractTexture::set_format_RG()
  { set_format(GL_RG); }

  void AbstractTexture::set_format_RG_INTEGER()
  { set_format(GL_RG_INTEGER); }

  void AbstractTexture::set_format_RGB()
  { set_format(GL_RGB); }

  void AbstractTexture::set_format_RGB_INTEGER()
  { set_format(GL_RGB_INTEGER); }

  void AbstractTexture::set_format_RGBA()
  { set_format(GL_RGBA); }

  void AbstractTexture::set_format_RGBA_INTEGER()
  { set_format(GL_RGBA_INTEGER); }

  void AbstractTexture::set_format_DEPTH_COMPONENT()
  { set_format(GL_DEPTH_COMPONENT); }

  void AbstractTexture::set_format_STENCIL_INDEX()
  { set_format(GL_STENCIL_INDEX); }

  void AbstractTexture::set_format_DEPTH_STENCIL()
  { set_format(GL_DEPTH_STENCIL); }
  /// @}

  /// @{ -------------------------------------------------- SET PIXEL DATA TYPE
  void AbstractTexture::set_pixel_data_type(GLenum type)
  { _pdata->pixel_data_type = type; }

  void AbstractTexture::set_pixel_data_type_UNSIGNED_BYTE()
  { set_pixel_data_type(GL_UNSIGNED_BYTE); }

  void AbstractTexture::set_pixel_data_type_BYTE()
  { set_pixel_data_type(GL_BYTE); }

  void AbstractTexture::set_pixel_data_type_UNSIGNED_SHORT()
  { set_pixel_data_type(GL_UNSIGNED_SHORT); }

  void AbstractTexture::set_pixel_data_type_SHORT()
  { set_pixel_data_type(GL_SHORT); }

  void AbstractTexture::set_pixel_data_type_UNSIGNED_INT()
  { set_pixel_data_type(GL_UNSIGNED_INT); }

  void AbstractTexture::set_pixel_data_type_UNSIGNED_INT_24_8()
  { set_pixel_data_type(GL_UNSIGNED_INT_24_8); }

  void AbstractTexture::set_pixel_data_type_INT()
  { set_pixel_data_type(GL_INT); }

  void AbstractTexture::set_pixel_data_type_FLOAT()
  { set_pixel_data_type(GL_FLOAT); }
  /// @}

  /// @{ -------------------------------------------------- SET INTERPOLATION MODE
  void AbstractTexture::set_interpolation_NEAREST() const
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  }

  void AbstractTexture::set_interpolation_LINEAR() const
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  /// @}

  /// @{ -------------------------------------------------- SET TEXTURE COORDINATES
  void AbstractTexture::set_texture_coordinates_CLAMP()
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_T, GL_CLAMP);

      if (_pdata->target == GL_TEXTURE_3D)
      { BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_R, GL_CLAMP); }
  }

  void AbstractTexture::set_texture_coordinates_CLAMP_TO_EDGE()
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      if (_pdata->target == GL_TEXTURE_3D)
      { BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); }
  }

  void AbstractTexture::set_texture_coordinates_REPEAT()
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_T, GL_REPEAT);

      if (_pdata->target == GL_TEXTURE_3D)
      { BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_R, GL_REPEAT); }
  }

  void AbstractTexture::set_texture_coordinates_CLAMP_TO_BORDER(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      assert(_is_bound && "texture must be bound; use bind() first");

      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

      if (_pdata->target == GL_TEXTURE_3D)
      { BK_QT_GL glTexParameteri(_pdata->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER); }

      const GLfloat color[4] = {r, g, b, a};
      BK_QT_GL glTexParameterfv(_pdata->target, GL_TEXTURE_BORDER_COLOR, color);
  }
  /// @}

  /// @{ -------------------------------------------------- SET DEFAULT CONFIG
  void AbstractTexture::set_default_config_gray_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_LUMINANCE8();
      set_format_RED();
      set_pixel_data_type_UNSIGNED_INT();
  }

  void AbstractTexture::set_default_config_depth_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);

      //set_internal_format_DEPTH_COMPONENT24();
      //set_internal_format_DEPTH_COMPONENT32();
      //set_pixel_data_type_UNSIGNED_INT();

      set_internal_format_DEPTH_COMPONENT32F();
      set_pixel_data_type_FLOAT();

      set_format_DEPTH_COMPONENT();
  }

  void AbstractTexture::set_default_config_depth_stencil_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_DEPTH24_STENCIL8();
      //set_internal_format_DEPTH32F_STENCIL8();
      set_format_DEPTH_STENCIL();
      //set_pixel_data_type_UNSIGNED_BYTE();
      set_pixel_data_type_UNSIGNED_INT_24_8();
      //set_pixel_data_type_FLOAT();
  }

  void AbstractTexture::set_default_config_r_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_R32F();
      set_format_RED();
      set_pixel_data_type_FLOAT();
  }

  void AbstractTexture::set_default_config_rg_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_RG32F();
      set_format_RG();
      set_pixel_data_type_FLOAT();
  }

  void AbstractTexture::set_default_config_rgb_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_RGB32F();
      set_format_RGB();
      set_pixel_data_type_FLOAT();
  }

  void AbstractTexture::set_default_config_rgba_tex()
  {
      set_mipmap_level(0);
      set_border_width(0);
      set_internal_format_RGBA32F();
      set_format_RGBA();
      set_pixel_data_type_FLOAT();
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void AbstractTexture::clear_impl()
  {
      if (_id != 0)
      { BK_QT_GL glDeleteTextures(1, &_id); }
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool AbstractTexture::init(GLvoid* pixel_data)
  {
      clear();
      [[maybe_unused]] GLenum err = BK_QT_GL glGetError(); // erase error codes from previous operations

      BK_QT_GL glActiveTexture(_pdata->texture_unit);
      BK_QT_GL glEnable(_pdata->target);
      BK_QT_GL glGenTextures(1, &_id);
      err = BK_QT_GL glGetError();

      if (err != GL_NO_ERROR)
      {
          clear();
          return false;
      }

      bind();
      init_impl(pixel_data);

      err = BK_QT_GL glGetError();
      if (err == GL_INVALID_OPERATION || err == GL_INVALID_VALUE)
      {
          clear();
          return false;
      }

      set_interpolation_LINEAR();
      set_texture_coordinates_CLAMP();
      release();

      return true;
  }
  /// @}

  /// @{ -------------------------------------------------- VIRTUAL
  void AbstractTexture::bind_impl()
  {
      BK_QT_GL glActiveTexture(_pdata->texture_unit);
      //BK_QT_GL glEnable(_pdata->target);
      BK_QT_GL glBindTexture(_pdata->target, _id);
  }

  void AbstractTexture::release_impl()
  {
      BK_QT_GL glActiveTexture(_pdata->texture_unit);
      BK_QT_GL glBindTexture(_pdata->target, 0);
  }
  /// @}
} // namespace bk::details


