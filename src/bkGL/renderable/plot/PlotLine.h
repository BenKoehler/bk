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

#ifndef BK_PLOTLINE_H
#define BK_PLOTLINE_H

#include <memory>

#include "PlotAbstractDataVectorView.h"

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;

  class PlotDataVector;

  template<typename...> class Signal;
  // -------------------- forward declaration END

  class BKGL_EXPORT PlotLine : public details::PlotAbstractDataVectorView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = PlotLine;
      using base_type = details::PlotAbstractDataVectorView;

    public:
      using color_type = ColorRGBA;
      using value_type = GLfloat;
      using data_vector_type = PlotDataVector;

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
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      PlotLine();
      #else
      explicit PlotLine(qt_gl_functions* gl);
      #endif
      PlotLine(const self_type& other) = delete;
      PlotLine(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotLine();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] auto get_color() const -> const color_type&;
      [[nodiscard]] auto get_line_width() const -> value_type;
      [[nodiscard]] auto get_data_vector() const -> const data_vector_type&;
      [[nodiscard]] auto get_data_vector() -> data_vector_type&;
      virtual auto get_x_min() const -> value_type override;
      virtual auto get_x_max() const -> value_type override;
      virtual auto get_y_min() const -> value_type override;
      virtual auto get_y_max() const -> value_type override;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      void set_color(const color_type& col);
      void set_color(value_type r, value_type g, value_type b, value_type a);
      void set_line_width(value_type w);
      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    private:
      [[maybe_unused]] bool init_shader();
      [[maybe_unused]] bool init_vbo_vao();
      [[maybe_unused]] bool init_ubo();
    public:
      virtual bool init() override;
      /// @}

      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_vbo_vao();
      void clear_ubo();
    public:
      virtual void clear() override;
      /// @}

      virtual void draw_impl() override;
  }; // class PlotLine
} // namespace bk

#endif //BK_PLOTLINE_H


