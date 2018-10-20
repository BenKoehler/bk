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

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  enum PlotMarkerOrientation_ : unsigned int
  {
      PlotMarkerOrientation_Horizontal = 0,//
      PlotMarkerOrientation_Vertical = 1//
  };

  class BKGL_EXPORT PlotMarker : public details::AbstractRenderable
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
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      PlotMarker();
      #else
      explicit PlotMarker(qt_gl_functions* gl);
      #endif
      PlotMarker(const PlotMarker&);
      PlotMarker(PlotMarker&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~PlotMarker();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] const ColorRGBA& color() const;
      [[nodiscard]] GLfloat line_width() const;
      [[nodiscard]] GLfloat data_value() const;
      [[nodiscard]] const PlotMarkerOrientation_& orientation() const;
      [[nodiscard]] bool orientation_is_horizontal() const;
      [[nodiscard]] bool orientation_is_vertical() const;

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] PlotMarker& operator=(const PlotMarker&);
      [[maybe_unused]] PlotMarker& operator=(PlotMarker&&) noexcept;
      /// @}

      void set_color(const ColorRGBA& col);
      void set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
      void set_line_width(GLfloat w);
      void set_data_value(GLfloat x);
      void set_x_min(GLfloat xmin);
      void set_x_max(GLfloat xmax);
      void set_y_min(GLfloat ymin);
      void set_y_max(GLfloat ymax);
    private:
      void set_orientation(PlotMarkerOrientation_ orientation);
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

      virtual void draw_impl() override;
  }; // class PlotMarker
} // namespace bk

#endif //BK_PLOTMARKER_H


