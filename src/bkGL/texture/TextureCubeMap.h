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

#ifndef BKGL_TEXTURECUBEMAP_H
#define BKGL_TEXTURECUBEMAP_H

#include <string_view>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bkGL/AbstractBindable.h>

namespace bk
{
  class BKGL_EXPORT TextureCubeMap : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TextureCubeMap;
      using base_type = details::AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      TextureCubeMap();
      #else
      TextureCubeMap(bk::qt_gl_functions* gl);
      #endif
      TextureCubeMap(const self_type& other) = delete;
      TextureCubeMap(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TextureCubeMap();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TEXTURE UNIT
      [[nodiscard]] GLenum texture_unit() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET TEXTURE UNIT
      void set_texture_unit(GLenum t);
      void set_texture_unit_number(GLuint i);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type& other) = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
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
      #ifdef BK_LIB_PNG_AVAILABLE
    private:
      [[nodiscard]] std::vector<GLfloat> _make_texture(std::string_view path) const;
    public:

      void init_from_rgb_images(std::string_view x_pos_path, std::string_view x_neg_path, std::string_view y_pos_path, std::string_view y_neg_path, std::string_view z_pos_path, std::string_view z_neg_path);
      #endif
      /// @}

      /// @{ -------------------------------------------------- VIRTUAL
    protected:
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class TextureCubeMap
} // namespace bk

#endif //BKGL_TEXTURECUBEMAP_H


