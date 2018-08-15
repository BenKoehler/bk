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

#ifndef BKGL_FRAMEBUFFER_H
#define BKGL_FRAMEBUFFER_H

#include <memory>

#include <bkGL/AbstractBindable.h>

namespace bk
{
  // -------------------- forward declaration
  class Texture2D;
  // -------------------- forward declaration END

  class BKGL_EXPORT FBO : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = FBO;
      using base_type = details::AbstractBindable;

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
      FBO();
      #else
      explicit FBO(qt_gl_functions* gl);
      #endif
      FBO(const self_type& other) = delete;
      FBO(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~FBO();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET ID
      using base_type::id;
      /// @}

      /// @{ -------------------------------------------------- GET ENUMS
      [[nodiscard]] static constexpr GLenum COLOR_ATTACHMENT(GLuint id) noexcept
      {
          switch (id)
          {
              case 0: return GL_COLOR_ATTACHMENT0;
              case 1: return GL_COLOR_ATTACHMENT1;
              case 2: return GL_COLOR_ATTACHMENT2;
              case 3: return GL_COLOR_ATTACHMENT3;
              case 4: return GL_COLOR_ATTACHMENT4;
              case 5: return GL_COLOR_ATTACHMENT5;
              case 6: return GL_COLOR_ATTACHMENT6;
              case 7: return GL_COLOR_ATTACHMENT7;
              case 8: return GL_COLOR_ATTACHMENT8;
              case 9: return GL_COLOR_ATTACHMENT9;
              case 10: return GL_COLOR_ATTACHMENT10;
              case 11: return GL_COLOR_ATTACHMENT11;
              case 12: return GL_COLOR_ATTACHMENT12;
              case 13: return GL_COLOR_ATTACHMENT13;
              case 14: return GL_COLOR_ATTACHMENT14;
              case 15: return GL_COLOR_ATTACHMENT15;
              default: return GL_COLOR_ATTACHMENT0;
          }
      }

      [[nodiscard]] static constexpr GLenum DEPTH_ATTACHMENT() noexcept
      { return GL_DEPTH_ATTACHMENT; }

      [[nodiscard]] static constexpr GLenum STENCIL_ATTACHMENT() noexcept
      { return GL_STENCIL_ATTACHMENT; }

      [[nodiscard]] static constexpr GLenum DEPTH_STENCIL_ATTACHMENT() noexcept
      { return GL_DEPTH_STENCIL_ATTACHMENT; }
      /// @}

      /// @{ -------------------------------------------------- GET/HAS ATTACHMENT
      [[nodiscard]] Texture2D* attachment(GLenum att);
      [[nodiscard]] const Texture2D* attachment(GLenum att) const;
      [[nodiscard]] bool has_attachment(GLenum att) const;
      /// @}

      /// @{ -------------------------------------------------- GET/HAS COLOR ATTACHMENT
      [[nodiscard]] Texture2D* color_attachment(GLuint id);
      [[nodiscard]] const Texture2D* color_attachment(GLuint id) const;
      [[nodiscard]] bool has_color_attachment(GLuint id) const;
      /// @}

      /// @{ -------------------------------------------------- GET/HAS DEPTH ATTACHMENT
      [[nodiscard]] Texture2D* depth_attachment();
      [[nodiscard]] const Texture2D* depth_attachment() const;
      [[nodiscard]] bool has_depth_attachment() const;
      /// @}

      /// @{ -------------------------------------------------- GET/HAS STENCIL ATTACHMENT
      [[nodiscard]] Texture2D* stencil_attachment();
      [[nodiscard]] const Texture2D* stencil_attachment() const;
      [[nodiscard]] bool has_stencil_attachment() const;
      /// @}

      /// @{ -------------------------------------------------- GET/HAS DEPTH STENCIL ATTACHMENT
      [[nodiscard]] Texture2D* depth_stencil_attachment();
      [[nodiscard]] const Texture2D* depth_stencil_attachment() const;
      [[nodiscard]] bool has_depth_stencil_attachment() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET/ADD TEXTURE
      //! add a texture to the FBO
      /*!
       * @param tex initialized texture
       * @param attachment GL_COLOR_ATTACHMENT0-15, GL_DEPTH_ATTACHMENT, or GL_STENCIL_ATTACHMENT.
       */
      void add_texture(Texture2D&& tex, GLenum att);
      void set_texture_DEPTH_ATTACHMENT(Texture2D&& tex);
      void set_texture_STENCIL_ATTACHMENT(Texture2D&& tex);
      void set_texture_DEPTH_STENCIL_ATTACHMENT(Texture2D&& tex);
      void set_texture_COLOR_ATTACHMENT(Texture2D&& tex, GLuint id);
      /// @}

      /// @{ -------------------------------------------------- SET TO DEFAULT CONFIG
      void set_default_config_deep_frame_buffer(GLuint w, GLuint h);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init();
      /// @}

      /// @{ -------------------------------------------------- CLEAR TEXTURES
      void clear_textures();
      /// @}

      /// @{ -------------------------------------------------- VIRTUAL FUNCTIONS
    protected:
      void clear_impl() override;
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class FBO
} // namespace bk

#endif //BKGL_FRAMEBUFFER_H


