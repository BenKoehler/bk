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

#include <bk/CopyablePIMPL>

#include <bkGL/renderable/plot/PlotAbstractDataVectorView.h>

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
      PlotLine();
      #else
      explicit PlotLine(qt_gl_functions* gl);
      #endif
      PlotLine(const PlotLine&) = delete;
      PlotLine(PlotLine&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotLine();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] const ColorRGBA& color() const;
      [[nodiscard]] GLfloat line_width() const;
      [[nodiscard]] const PlotDataVector& data_vector() const;
      [[nodiscard]] PlotDataVector& data_vector();
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
      [[maybe_unused]] auto operator=(const PlotLine&) -> PlotLine& = delete;
      [[maybe_unused]] auto operator=(PlotLine&&) noexcept -> PlotLine&;
      /// @}

      void set_color(const ColorRGBA& col);
      void set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
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

      virtual void draw_impl() override;
  }; // class PlotLine
} // namespace bk

#endif //BK_PLOTLINE_H


