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

#include <bkGL/buffer/FBO.h>

#include <cassert>
#include <iostream>
#include <map>
#include <vector>

#include <bkGL/texture/Texture2D.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct FBO::Impl
  {
      std::map<GLenum, Texture2D> textures;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  FBO::FBO()
      : base_type()
  #else

  FBO::FBO(bk::qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /* do nothing */ }

  FBO::FBO(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  FBO::~FBO()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET/HAS ATTACHMENT
  Texture2D* FBO::attachment(GLenum att)
  {
      auto it = _pdata->textures.find(att);
      return it != _pdata->textures.end() ? &it->second : nullptr;
  }

  const Texture2D* FBO::attachment(GLenum att) const
  {
      auto it = _pdata->textures.find(att);
      return it != _pdata->textures.end() ? &it->second : nullptr;
  }

  bool FBO::has_attachment(GLenum att) const
  { return _pdata->textures.find(att) != _pdata->textures.end(); }
  /// @}

  /// @{ -------------------------------------------------- GET/HAS COLOR ATTACHMENT
  Texture2D* FBO::color_attachment(GLuint id)
  { return attachment(COLOR_ATTACHMENT(id)); }

  const Texture2D* FBO::color_attachment(GLuint id) const
  { return attachment(COLOR_ATTACHMENT(id)); }

  bool FBO::has_color_attachment(GLuint id) const
  { return color_attachment(id) != nullptr; }
  /// @}

  /// @{ -------------------------------------------------- GET/HAS DEPTH ATTACHMENT
  Texture2D* FBO::depth_attachment()
  { return attachment(DEPTH_ATTACHMENT()); }

  const Texture2D* FBO::depth_attachment() const
  { return attachment(DEPTH_ATTACHMENT()); }

  bool FBO::has_depth_attachment() const
  { return depth_attachment() != nullptr; }
  /// @}

  /// @{ -------------------------------------------------- GET/HAS STENCIL ATTACHMENT
  Texture2D* FBO::stencil_attachment()
  { return attachment(STENCIL_ATTACHMENT()); }

  const Texture2D* FBO::stencil_attachment() const
  { return attachment(STENCIL_ATTACHMENT()); }

  bool FBO::has_stencil_attachment() const
  { return stencil_attachment() != nullptr; }
  /// @}

  /// @{ -------------------------------------------------- GET/HAS DEPTH STENCIL ATTACHMENT
  Texture2D* FBO::depth_stencil_attachment()
  { return attachment(DEPTH_STENCIL_ATTACHMENT()); }

  const Texture2D* FBO::depth_stencil_attachment() const
  { return attachment(DEPTH_STENCIL_ATTACHMENT()); }

  bool FBO::has_depth_stencil_attachment() const
  { return depth_stencil_attachment() != nullptr; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto FBO::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET/ADD TEXTURE
  void FBO::add_texture(Texture2D&& tex, GLenum att)
  {
      assert((att == GL_COLOR_ATTACHMENT0 || att == GL_COLOR_ATTACHMENT1 || att == GL_COLOR_ATTACHMENT2 || att == GL_COLOR_ATTACHMENT3 || att == GL_COLOR_ATTACHMENT4 || att == GL_COLOR_ATTACHMENT5 || att == GL_COLOR_ATTACHMENT6 || att == GL_COLOR_ATTACHMENT7 || att == GL_COLOR_ATTACHMENT8 || att == GL_COLOR_ATTACHMENT9 || att == GL_COLOR_ATTACHMENT10 || att == GL_COLOR_ATTACHMENT11 || att == GL_COLOR_ATTACHMENT12 || att == GL_COLOR_ATTACHMENT13 || att == GL_COLOR_ATTACHMENT14 || att == GL_COLOR_ATTACHMENT15 || att == GL_DEPTH_ATTACHMENT || att == GL_STENCIL_ATTACHMENT || att == GL_DEPTH_STENCIL_ATTACHMENT) && "invalid attribute");
      auto it = _pdata->textures.find(att);
      if (it == _pdata->textures.end())
      { _pdata->textures.insert(std::make_pair(att, std::move(tex))); }
      else
      { it->second = std::move(tex); }
  }

  void FBO::set_texture_DEPTH_ATTACHMENT(Texture2D&& tex)
  { add_texture(std::move(tex), DEPTH_ATTACHMENT()); }

  void FBO::set_texture_STENCIL_ATTACHMENT(Texture2D&& tex)
  { add_texture(std::move(tex), STENCIL_ATTACHMENT()); }

  void FBO::set_texture_DEPTH_STENCIL_ATTACHMENT(Texture2D&& tex)
  { add_texture(std::move(tex), DEPTH_STENCIL_ATTACHMENT()); }

  void FBO::set_texture_COLOR_ATTACHMENT(Texture2D&& tex, GLuint id)
  { add_texture(std::move(tex), COLOR_ATTACHMENT(id)); }
  /// @}

  /// @{ -------------------------------------------------- SET TO DEFAULT CONFIG
  void FBO::set_default_config_deep_frame_buffer(GLuint w, GLuint h)
  {
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_depth(w, h);
      #else
      Texture2D tex_depth(w, h, _gl);
      #endif

      tex_depth.set_default_config_depth_tex();
      tex_depth.init();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_color(w, h);
      #else
      Texture2D tex_color(w, h, _gl);
      #endif

      tex_color.set_default_config_rgba_tex();
      tex_color.init();

      clear();

      set_texture_DEPTH_ATTACHMENT(std::move(tex_depth));
      set_texture_COLOR_ATTACHMENT(std::move(tex_color), 0);
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  bool FBO::init()
  {
      clear();
      BK_QT_GL glGenFramebuffers(1, &_id);
      bind();

      const Texture2D* temp = depth_stencil_attachment();

      if (temp != nullptr)
      { BK_QT_GL glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, temp->id(), temp->mipmap_level()); }
      else
      {
          temp = depth_attachment();
          if (temp != nullptr)
          { BK_QT_GL glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, temp->id(), temp->mipmap_level()); }

          temp = stencil_attachment();
          if (temp != nullptr)
          { BK_QT_GL glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, temp->id(), temp->mipmap_level()); }
      }

      std::vector<GLenum> col_att;
      for (GLuint i = 0; i < 16; ++i)
      {
          // 16 instead of GL_MAX_COLOR_ATTACHMENTS because FBO::COLOR_ATTACHMENT(GLuint id) is defined for up to 16 attachments;
          // otherwise the default case GL_COLOR_ATTACHMENT0 would be added GL_MAX_COLOR_ATTACHMENTS times, i.e., about 30k times

          temp = color_attachment(i);
          if (temp != nullptr)
          {
              const GLenum att = COLOR_ATTACHMENT(i);
              col_att.push_back(att);
              BK_QT_GL glFramebufferTexture2D(GL_FRAMEBUFFER, att, GL_TEXTURE_2D, temp->id(), temp->mipmap_level());
          }
      }

      BK_QT_GL glDrawBuffers(static_cast<GLsizei>(col_att.size()), col_att.data());

      if (BK_QT_GL glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
      {
          release();
          return true;
      }
      else
      {
          std::cerr << "FBO::init() FAILED" << std::endl;
          clear();
          return false;
      }
  }
  /// @}

  /// @{ -------------------------------------------------- CLEAR TEXTURES
  void FBO::clear_textures()
  { _pdata->textures.clear(); }
  /// @}

  /// @{ -------------------------------------------------- VIRTUAL FUNCTIONS
  void FBO::clear_impl()
  {
      if (_id != 0)
      {
          BK_QT_GL glDeleteFramebuffers(1, &_id);
          clear_textures();
      }
  }

  void FBO::bind_impl()
  { BK_QT_GL glBindFramebuffer(GL_FRAMEBUFFER, _id); }

  void FBO::release_impl()
  { BK_QT_GL glBindFramebuffer(GL_FRAMEBUFFER, 0); }
  /// @}
} // namespace bk


