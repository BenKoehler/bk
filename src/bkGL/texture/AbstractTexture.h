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

#ifndef BKGL_ABSTRACTTEXTURE_H
#define BKGL_ABSTRACTTEXTURE_H

#include <memory>

#include <bkGL/AbstractBindable.h>

namespace bk::details
{
  class BKGL_EXPORT AbstractTexture : public AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractTexture;
      using base_type = AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      AbstractTexture();
      AbstractTexture(GLuint w = 1, GLuint h = 1);
      #else
      explicit AbstractTexture(bk::qt_gl_functions* gl);
      explicit AbstractTexture(GLuint w, GLuint h, bk::qt_gl_functions* gl);
      #endif
      AbstractTexture(const self_type&) = delete;
      AbstractTexture(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractTexture();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TEXTURE UNIT
      [[nodiscard]] GLenum texture_unit() const;
      [[nodiscard]] GLuint texture_unit_number() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] GLsizei width() const;
      [[nodiscard]] GLsizei height() const;
      /// @}

      /// @{ -------------------------------------------------- GET MIPMAP LEVEL
      [[nodiscard]] GLint mipmap_level() const;
      /// @}

      /// @{ -------------------------------------------------- GET BORDER WIDTH
      [[nodiscard]] GLint border_width() const;
      /// @}

      /// @{ -------------------------------------------------- GET INTERNAL FORMAT
      [[nodiscard]] GLint internal_format() const;
      /// @}

      /// @{ -------------------------------------------------- GET FORMAT
      [[nodiscard]] GLenum format() const;
      /// @}

      /// @{ -------------------------------------------------- GET PIXEL DATA TYPE
      [[nodiscard]] GLenum pixel_data_type() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET TARGET
    protected:
      void set_target_TEXTURE2D();
      void set_target_TEXTURE3D();
    public:
      void set_target_TEXTURE_CUBE_MAP_POSITIVE_X();
      void set_target_TEXTURE_CUBE_MAP_NEGATIVE_X();
      void set_target_TEXTURE_CUBE_MAP_POSITIVE_Y();
      void set_target_TEXTURE_CUBE_MAP_NEGATIVE_Y();
      void set_target_TEXTURE_CUBE_MAP_POSITIVE_Z();
      void set_target_TEXTURE_CUBE_MAP_NEGATIVE_Z();
      /// @}

      /// @{ -------------------------------------------------- SET TEXTURE UNIT
      void set_texture_unit(GLenum t);
      void set_texture_unit_number(GLuint i);
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      void set_width(GLsizei w);
      void set_height(GLsizei h);
      /// @}

      /// @{ -------------------------------------------------- SET MIPMAP LEVEL
      //! @brief Specifies the level-of-detail number.
      //!        Level 0 is the base image level.
      //!        Level n is the nth mipmap reduction image.
      void set_mipmap_level(GLint l);
      /// @}

      /// @{ -------------------------------------------------- SET BORDER WIDTH
      //! @brief Specifies the width of the border. Must be 0 or 1.
      void set_border_width(GLint w);
      /// @}

      /// @{ -------------------------------------------------- SET INTERNAL FORMAT
      //! @brief Specifies the number of color components in the texture.
      void set_internal_format(GLint fmt);
      void set_internal_format_DEPTH_COMPONENT();
      void set_internal_format_DEPTH_COMPONENT16();
      void set_internal_format_DEPTH_COMPONENT24();
      void set_internal_format_DEPTH_COMPONENT32();
      void set_internal_format_DEPTH_COMPONENT32F();
      void set_internal_format_DEPTH_STENCIL();
      void set_internal_format_DEPTH24_STENCIL8();
      void set_internal_format_DEPTH32F_STENCIL8();
      void set_internal_format_ALPHA();
      void set_internal_format_ALPHA4();
      void set_internal_format_ALPHA8();
      void set_internal_format_ALPHA16();
      void set_internal_format_LUMINANCE();
      void set_internal_format_LUMINANCE4();
      void set_internal_format_LUMINANCE8();
      void set_internal_format_LUMINANCE16();
      void set_internal_format_INTENSITY();
      void set_internal_format_INTENSITY4();
      void set_internal_format_INTENSITY8();
      void set_internal_format_INTENSITY12();
      void set_internal_format_INTENSITY16();
      void set_internal_format_RED();
      void set_internal_format_R8();
      void set_internal_format_R8I();
      void set_internal_format_R8UI();
      void set_internal_format_R16();
      void set_internal_format_R16I();
      void set_internal_format_R16UI();
      void set_internal_format_R16F();
      void set_internal_format_R32I();
      void set_internal_format_R32UI();
      void set_internal_format_R32F();
      void set_internal_format_RG();
      void set_internal_format_RG8();
      void set_internal_format_RG8I();
      void set_internal_format_RG8UI();
      void set_internal_format_RG16();
      void set_internal_format_RG16I();
      void set_internal_format_RG16UI();
      void set_internal_format_RG16F();
      void set_internal_format_RG32I();
      void set_internal_format_RG32UI();
      void set_internal_format_RG32F();
      void set_internal_format_RGB();
      void set_internal_format_RGB4();
      void set_internal_format_RGB8();
      void set_internal_format_RGB8I();
      void set_internal_format_RGB8UI();
      void set_internal_format_RGB12();
      void set_internal_format_RGB16();
      void set_internal_format_RGB16I();
      void set_internal_format_RGB16UI();
      void set_internal_format_RGB16F();
      void set_internal_format_RGB32I();
      void set_internal_format_RGB32UI();
      void set_internal_format_RGB32F();
      void set_internal_format_RGBA();
      void set_internal_format_RGBA4();
      void set_internal_format_RGBA8();
      void set_internal_format_RGBA8I();
      void set_internal_format_RGBA8UI();
      void set_internal_format_RGBA12();
      void set_internal_format_RGBA16();
      void set_internal_format_RGBA16I();
      void set_internal_format_RGBA16UI();
      void set_internal_format_RGBA16F();
      void set_internal_format_RGBA32I();
      void set_internal_format_RGBA32UI();
      void set_internal_format_RGBA32F();
      /// @}

      /// @{ -------------------------------------------------- SET FORMAT
      //! @brief Specifies the format of the pixel data.
      void set_format(GLenum fmt);
      void set_format_RED();
      void set_format_RED_INTEGER();
      void set_format_RG();
      void set_format_RG_INTEGER();
      void set_format_RGB();
      void set_format_RGB_INTEGER();
      void set_format_RGBA();
      void set_format_RGBA_INTEGER();
      void set_format_DEPTH_COMPONENT();
      void set_format_STENCIL_INDEX();
      void set_format_DEPTH_STENCIL();
      /// @}

      /// @{ -------------------------------------------------- SET PIXEL DATA TYPE
      //! @brief Specifies the data type of the pixel data.
      void set_pixel_data_type(GLenum type);
      void set_pixel_data_type_UNSIGNED_BYTE();
      void set_pixel_data_type_BYTE();
      void set_pixel_data_type_UNSIGNED_SHORT();
      void set_pixel_data_type_SHORT();
      void set_pixel_data_type_UNSIGNED_INT();
      void set_pixel_data_type_UNSIGNED_INT_24_8();
      void set_pixel_data_type_INT();
      void set_pixel_data_type_FLOAT();
      /// @}

      /// @{ -------------------------------------------------- SET INTERPOLATION MODE
      void set_interpolation_NEAREST() const;
      void set_interpolation_LINEAR() const;
      /// @}

      /// @{ -------------------------------------------------- SET TEXTURE COORDINATES
      void set_texture_coordinates_CLAMP();
      void set_texture_coordinates_CLAMP_TO_EDGE();
      void set_texture_coordinates_REPEAT();
      void set_texture_coordinates_CLAMP_TO_BORDER(GLfloat r = 0, GLfloat g = 0, GLfloat b = 0, GLfloat a = 0);
      /// @}

      /// @{ -------------------------------------------------- SET DEFAULT CONFIG
      void set_default_config_gray_tex();
      void set_default_config_depth_tex();
      void set_default_config_depth_stencil_tex();
      void set_default_config_r_tex();
      void set_default_config_rg_tex();
      void set_default_config_rgb_tex();
      void set_default_config_rgba_tex();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    protected:
      void clear_impl() override;
    public:
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init(GLvoid* pixel_data = nullptr);
    protected:
      virtual void init_impl(GLvoid* pixel_data) = 0;
    public:
      /// @}

      /// @{ -------------------------------------------------- VIRTUAL
    protected:
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class AbstractTexture
} // namespace bk::details

#endif //BKGL_ABSTRACTTEXTURE_H


