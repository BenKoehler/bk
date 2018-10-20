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

#ifndef BK_PLOTAREA_H
#define BK_PLOTAREA_H

#include <bk/CopyablePIMPL>

#include <bkGL/renderable/plot/PlotAbstractDataVectorView.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;

  class AreaPlotDataVector;

  template<typename...> class Signal;
  // -------------------- forward declaration END

  class BKGL_EXPORT PlotArea : public details::PlotAbstractDataVectorView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = details::PlotAbstractDataVectorView;

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
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE
      PlotArea();
      #else
      explicit PlotArea(qt_gl_functions* gl);
      #endif
      PlotArea(const PlotArea&) = delete;
      PlotArea(PlotArea&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotArea();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] const ColorRGBA& color_above() const;
      [[nodiscard]] const ColorRGBA& color_below() const;
      [[nodiscard]] const ColorRGBA& color_line() const;
      [[nodiscard]] GLfloat line_width() const;
      [[nodiscard]] const AreaPlotDataVector& data_vector() const;
      [[nodiscard]] AreaPlotDataVector& data_vector();
      [[nodiscard]] virtual GLfloat x_min() const override;
      [[nodiscard]] virtual GLfloat x_max() const override;
      [[nodiscard]] virtual GLfloat y_min() const override;
      [[nodiscard]] virtual GLfloat y_max() const override;

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const PlotArea&) -> PlotArea& = delete;
      [[maybe_unused]] auto operator=(PlotArea&&) noexcept -> PlotArea&;
      /// @}

      void set_color_area_above(const ColorRGBA& col);
      void set_color_area_above(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
      void set_color_area_below(const ColorRGBA& col);
      void set_color_area_below(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
      void set_color_line(const ColorRGBA& col);
      void set_color_line(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

      void set_line_width(GLfloat w);

      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    private:
      [[maybe_unused]] bool init_shader();
      [[maybe_unused]] bool init_vbo_vao();
      [[maybe_unused]] bool init_ubo();
    public:
      [[maybe_unused]] virtual bool init() override;
      /// @}

      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_vbo_vao();
      void clear_ubo();
    public:
      virtual void clear() override;
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

      virtual void draw_impl() override;
  }; // class PlotArea
} // namespace bk

#endif //BK_PLOTAREA_H
