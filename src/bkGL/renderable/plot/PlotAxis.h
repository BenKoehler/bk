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

#ifndef BK_PLOTAXIS_H
#define BK_PLOTAXIS_H

#include <memory>
#include <string>
#include <string_view>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;

  class TextView;
  // -------------------- forward declaration END

  enum PlotAxisOrientation_ : unsigned int
  {
      PlotAxisOrientation_Horizontal = 0,//
      PlotAxisOrientation_Vertical = 1//
  };

  using PlotAxisOrientation = unsigned int;

  class BKGL_EXPORT PlotAxis : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = details::AbstractRenderable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      PlotAxis();
      #else
      explicit PlotAxis(qt_gl_functions* gl);
      #endif
      PlotAxis(const PlotAxis&) = delete;
      PlotAxis(PlotAxis&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotAxis();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] const ColorRGBA& color() const;
      [[nodiscard]] GLfloat line_width() const;
      [[nodiscard]] GLfloat x_min() const;
      [[nodiscard]] GLfloat x_max() const;
      [[nodiscard]] GLfloat y_min() const;
      [[nodiscard]] GLfloat y_max() const;
      [[nodiscard]] GLuint num_ticks() const;
      [[nodiscard]] const std::string& label() const;
      [[nodiscard]] GLfloat border_width_x_in_percent() const;
      [[nodiscard]] GLfloat border_width_y_in_percent() const;
      [[nodiscard]] PlotAxisOrientation orientation() const;
      [[nodiscard]] bool orientation_is_horizontal() const;
      [[nodiscard]] bool orientation_is_vertical() const;
      [[nodiscard]] unsigned int tick_precision() const;
      [[nodiscard]] TextView& text_view_label();
      [[nodiscard]] const TextView& text_view_label() const;
      [[nodiscard]] bool draw_ticks_manually() const;

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] PlotAxis& operator=(const PlotAxis& other) = delete;
      [[maybe_unused]] PlotAxis& operator=(PlotAxis&& other) noexcept;
      /// @}

      void set_color(const ColorRGBA& col);
      void set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
      void set_line_width(GLfloat w);
      void set_x_min(GLfloat xmin);
      void set_x_max(GLfloat xmax);
      void set_y_min(GLfloat ymin);
      void set_y_max(GLfloat ymax);
      void set_num_ticks(GLuint numTicks);
      void set_label(std::string_view label);
      void set_border_width_x_in_percent(GLfloat p);
      void set_border_width_y_in_percent(GLfloat p);
    private:
      void set_orientation(PlotAxisOrientation orientation);
    public:
      void set_orientation_horizontal();
      void set_orientation_vertical();
      void set_tick_precision(unsigned int tickPrecision);
      void set_draw_ticks_manually(bool b);

      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    private:
      [[maybe_unused]] bool init_shader();
      [[maybe_unused]] bool init_vbo_vao();
      [[maybe_unused]] bool init_ubo();
      [[maybe_unused]] bool init_textview();
    public:
      [[maybe_unused]] bool init();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_vbo_vao();
      void clear_ubo();
    public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_matrix_changed(bool) override;
      virtual void on_new_modelview_matrix(const ColMat4<GLfloat>& m) override;
      virtual void on_new_projection_matrix(const ColMat4<GLfloat>& p) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_mouse_button_released(MouseButton_ btn) override;
      virtual void on_key_pressed(Key_ k) override;
      virtual void on_key_released(Key_ k) override;
      virtual void on_mouse_wheel_up() override;
      virtual void on_mouse_wheel_down() override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      virtual void on_animation_time_changed(GLfloat t) override;
      /// @}

      void draw_ticks();
      virtual void draw_impl() override;
  }; // class PlotAxis
} // namespace bk

#endif //BK_PLOTAXIS_H


