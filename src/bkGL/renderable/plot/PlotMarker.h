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

#ifndef BK_PLOTMARKER_H
#define BK_PLOTMARKER_H

#include "gl/renderable/Renderable.h"

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  namespace details
  {
    enum class PlotMarkerOrientation : unsigned int
    {
        Horizontal = 0, Vertical = 1
    };
  } // namespace details

  class BKGL_EXPORT PlotMarker: public details::Renderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = PlotMarker;
      using base_type = details::Renderable;

    public:
      using color_type = ColorRGBA;
      using value_type = GLfloat;
      using size_type = GLuint;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      std::unique_ptr <Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      PlotMarker();
      #else
      explicit PlotMarker(qt_gl_functions* gl);
      #endif
      PlotMarker(const self_type& other);
      PlotMarker(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~PlotMarker();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] auto get_color() const -> const color_type&;
      [[nodiscard]] auto get_line_width() const -> value_type;
      [[nodiscard]] auto get_data_value() const -> value_type;
      [[nodiscard]] const details::PlotMarkerOrientation& get_orientation() const;
      [[nodiscard]] bool orientation_is_horizontal() const;
      [[nodiscard]] bool orientation_is_vertical() const;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      auto operator=(const self_type& other) -> self_type&;
      auto operator=(self_type&& other) -> self_type&;
      /// @}

      void set_color(const color_type& col);
      void set_color(value_type r, value_type g, value_type b, value_type a);
      void set_line_width(value_type w);
      void set_data_value(value_type x);
      void set_x_min(value_type xmin);
      void set_x_max(value_type xmax);
      void set_y_min(value_type ymin);
      void set_y_max(value_type ymax);
    private:
      void set_orientation(details::PlotMarkerOrientation orientation);
    public:
      void set_orientation_horizontal();
      void set_orientation_vertical();

      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    private:
      [[maybe_unused]] bool init_shader();
      [[maybe_unused]] bool init_vbo_vao();
      [[maybe_unused]] bool init_ubo();
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
      virtual void on_modelview_changed(bool b) override;
      virtual void on_visible_changed(bool b) override;
      /// @}

      virtual void draw_impl() override;

  }; // class PlotMarker
} // namespace bk

#endif //BK_PLOTMARKER_H


