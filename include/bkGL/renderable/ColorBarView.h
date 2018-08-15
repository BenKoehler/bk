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

#ifndef BKGL_COLORBARVIEW_H
#define BKGL_COLORBARVIEW_H

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "AbstractRenderable.h"
#include "../lib/bkGL_export.h"

namespace bk
{
  // -------------------- forward declaration
  class ColorBarRGBA;

  class ColorRGBA;

  template<typename...> class Signal;
  // -------------------- forward declaration END

  class BKGL_EXPORT ColorBarView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ColorBarView;
      using base_type = details::AbstractRenderable;
    public:
      using color_type = ColorRGBA;
      using colorbar_type = ColorBarRGBA;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      ColorBarView();
      #else
      ColorBarView(bk::qt_gl_functions* gl);
      #endif
      ColorBarView(const self_type& other) = delete;
      ColorBarView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ColorBarView();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      /// @{ -------------------------------------------------- GET POSITION
      [[nodiscard]] bool is_on_left_side() const;
      [[nodiscard]] bool is_on_right_side() const;
      [[nodiscard]] unsigned int position_vertical() const;
      /// @}

      /// @{ -------------------------------------------------- GET RANGE
      [[nodiscard]] GLfloat min_value() const;
      [[nodiscard]] GLfloat max_value() const;
      /// @}

      /// @{ -------------------------------------------------- GET CLAMPING
      [[nodiscard]] GLfloat min_clamp_value() const;
      [[nodiscard]] GLfloat max_clamp_value() const;
      /// @}

      /// @{ -------------------------------------------------- GET NUM TICKS
      //! number of intermediate steps between min and max value
      [[nodiscard]] unsigned int num_ticks() const;
      /// @}

      /// @{ -------------------------------------------------- GET VALUE PRECISION
      //! number of digits after comma
      [[nodiscard]] unsigned int value_precision() const;
      /// @}

      /// @{ -------------------------------------------------- GET TITLE
      [[nodiscard]] const std::string& title() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      //! percentaged colorbar size on the screen
      [[nodiscard]] GLfloat width_in_percent() const;
      [[nodiscard]] GLfloat height_in_percent() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<std::string>& signal_title_changed();
      [[nodiscard]] bk::Signal<double, double>& signal_min_max_value_changed();
      [[nodiscard]] bk::Signal<double, double>& signal_min_max_clamp_value_changed();
      /// @}

      /// @{ -------------------------------------------------- DISCRETE MODE
      //! no linear interpolation between the colors
      [[nodiscard]] bool linear_interpolation_is_enabled() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET POSITION
      void set_position_horizontal_left();
      void set_position_horizontal_right();

      void set_position_vertical(unsigned int posv);
      /// @}

      /// @{ -------------------------------------------------- SET RANGE
      void set_value_range(GLfloat rmin, GLfloat rmax, bool copytoClampValueRange = true);
      void set_min_value(GLfloat rmin, bool copytoClampValueRange = true);
      void set_max_value(GLfloat rmax, bool copytoClampValueRange = true);
      /// @}

      /// @{ -------------------------------------------------- SET CLAMPING
      //! clamp colorbar so that value below/above cmin/cmax are mapped to min/max color
      void set_clamp_value_range(GLfloat cmin, GLfloat cmax);
      void set_min_clamp_value(GLfloat cmin);
      void set_max_clamp_value(GLfloat cmax);
      /// @}

      /// @{ -------------------------------------------------- SET NUM TICKS
      //! number of intermediate steps between min and max value
      void set_num_ticks(unsigned int n);
      /// @}

      /// @{ -------------------------------------------------- GET VALUE PRECISION
      //! number of digits after comma
      void set_value_precision(unsigned int nDigitsAfterComma);
      /// @}

      /// @{ -------------------------------------------------- SET TITLE
      void set_title(std::string_view t);
      /// @}

    private:
      /// @{ -------------------------------------------------- SET SIZE
      //! percentaged colorbar size on the screen
      void set_width_in_percent(GLfloat s);
      void set_height_in_percent(GLfloat h);
      /// @}
    public:

      /// @{ -------------------------------------------------- LINEAR INTERPOLATION (DISCRETE MODE)
      void set_linear_color_interpolation_enabled(bool b);
      void enable_linear_color_interpolation();
      void disable_linear_color_interpolation();
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_buffers();
      void clear_shader();
      void clear_text();
    public:

      void clear();
      /// @}


      /// @{ -------------------------------------------------- HELPERS: MIN MAX SCREEN POS
    private:
      [[nodiscard]] std::tuple<double, double, double, double> _minx_maxx_miny_maxy_screen_pos() const;
      /// @}

      /// @{ -------------------------------------------------- HELPERS: UPDATE
      void _update_vbo_positions();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init_shader();
      void _init(const std::vector<GLfloat>& r, const std::vector<GLfloat>& g, const std::vector<GLfloat>& b);
    public:
      void init(const ColorBarRGBA& cb);
      void init_manual_num_colors(const ColorBarRGBA& cb, unsigned int nColors);
      void init_heat();
      void init_rainbow();
      void init_blue_to_red();
      void init_magenta();
      void init_uniform_yellow();
      void init_traffic_light();
      void init_cluster(int nClusters = -1);
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_changed(bool) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton btn) override;
      virtual void on_mouse_button_released(MouseButton btn) override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
      virtual void draw_impl() override;
      /// @}
    public:
  }; // class ColorBarView
} // namespace bk

#endif //BKGL_COLORBARVIEW_H


