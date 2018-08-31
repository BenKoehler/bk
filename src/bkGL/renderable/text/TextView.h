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

#ifndef BKGL_TEXTVIEW_H
#define BKGL_TEXTVIEW_H

#include <map>
#include <string>
#include <string_view>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/texture/Texture2D.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  //====================================================================================================
  //===== STRUCTS
  //====================================================================================================
  namespace details
  {
    struct BKGL_EXPORT FreeTypeCharacter
    {
        Texture2D tex;
        GLint size_x;
        GLint size_y;
        GLint bearing_x;
        GLint bearing_y;
        GLint advance;

        #ifndef BK_LIB_QT_AVAILABLE
        FreeTypeCharacter();
        #else
        FreeTypeCharacter(bk::qt_gl_functions* gl);
        #endif

        FreeTypeCharacter(const FreeTypeCharacter&) = delete;
        FreeTypeCharacter(FreeTypeCharacter&&);
        ~FreeTypeCharacter();
        [[maybe_unused]] FreeTypeCharacter& operator=(const FreeTypeCharacter&) = delete;
        [[maybe_unused]] FreeTypeCharacter& operator=(FreeTypeCharacter&&);
    };
  } // namespace details

  class BKGL_EXPORT TextView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TextView;
      using base_type = details::AbstractRenderable;
    public:
      using color_type = ColorRGBA;
    private:

      //====================================================================================================
      //===== STATIC MEMBERS
      //====================================================================================================
      static bool ft_initialized;
      static FT_Library ft_lib;
      static FT_Face ft_face;
      static std::map<GLchar, details::FreeTypeCharacter> ft_map;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      TextView();
      #else
      TextView(bk::qt_gl_functions* gl);
      #endif
      TextView(const self_type&) = delete;
      TextView(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TextView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TEXT
      [[nodiscard]] const std::string& text() const;
      /// @}

      /// @{ -------------------------------------------------- GET POSITION
      [[nodiscard]] GLfloat position_x() const;
      [[nodiscard]] GLfloat position_y() const;
      /// @}

      /// @{ -------------------------------------------------- GET POSITION MODE
      [[nodiscard]] bool position_mode_is_absolute() const;
      [[nodiscard]] bool position_mode_is_relative() const;
      /// @}

      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] const ColorRGBA& color_text() const;
      [[nodiscard]] const ColorRGBA& color_background() const;
      /// @}

      /// @{ -------------------------------------------------- BACKGROUND IS ENABLED
      [[nodiscard]] bool background_is_enabled() const;
      /// @}

      /// @{ -------------------------------------------------- GET SCALE
      [[nodiscard]] GLfloat scale_x() const;
      [[nodiscard]] GLfloat scale_y() const;
      /// @}

      /// @{ -------------------------------------------------- GET CURRENT TEXT WIDTH IN PIXEL
      [[nodiscard]] GLfloat text_pixel_width_absolute() const;
      [[nodiscard]] GLfloat text_pixel_width_relative() const;
      [[nodiscard]] GLfloat text_pixel_width() const;
      [[nodiscard]] GLfloat text_pixel_height_absolute() const;
      [[nodiscard]] GLfloat text_pixel_height_relative() const;
      [[nodiscard]] GLfloat text_pixel_height() const;
    private:
      [[nodiscard]] GLfloat text_max_bearing_y_absolute() const;
      [[nodiscard]] GLfloat text_max_bearing_y_relative() const;
      [[nodiscard]] GLfloat text_max_bearing_y() const;

      [[nodiscard]] GLfloat text_max_off_y_absolute() const;
      [[nodiscard]] GLfloat text_max_off_y_relative() const;
      [[nodiscard]] GLfloat text_max_off_y() const;
    public:
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET TEXT
      void set_text(std::string_view txt);
      /// @}

      /// @{ -------------------------------------------------- SET POSITION
      void set_position(GLfloat x, GLfloat y);
      /// @}

      /// @{ -------------------------------------------------- SET POSITION MODE
    private:
      void _set_position_mode_absolute(bool b);
    public:
      void set_position_mode_absolute();
      void set_position_mode_relative();
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
      void set_color_text(const ColorRGBA& c);
      void set_color_text(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);

      void set_color_background(const ColorRGBA& c);
      void set_color_background(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
      /// @}

      /// @{ -------------------------------------------------- SET BACKGROUND ENABLED
      void set_background_enabled(bool b);
      void enable_background();
      void disable_background();
      /// @}

      /// @{ -------------------------------------------------- SET SCALE
      void set_scale(GLfloat sx, GLfloat sy);
      void set_scale(GLfloat sxy);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shaders();
      void clear_buffers();
    public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] static bool init_freetype(std::string_view path_to_font);
    private:
      void init_shader();
      void init_buffers();
    public:
      void init();
      void init(std::string_view txt, std::string_view path_to_font = BK_GL_FONT_PATH );
      /// @}

      /// @{ -------------------------------------------------- UPDATE POSITION
    private:
      [[nodiscard]] GLfloat update_vbo_text(GLfloat x, GLfloat y, const details::FreeTypeCharacter& character);
      void update_vbo_background();
    public:
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_changed(bool) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_mouse_button_released(MouseButton_ btn) override;
      virtual void on_key_pressed(Key_ k) override;
      virtual void on_key_released(Key_ k) override;
      virtual void on_mouse_wheel_up() override;
      virtual void on_mouse_wheel_down() override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_impl() override;
    public:
      /// @}
  }; // class TextView
} // namespace bk

#endif //BKGL_TEXTVIEW_H
