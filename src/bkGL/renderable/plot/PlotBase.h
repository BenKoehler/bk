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

#ifndef BKGL_PLOTBASE_H
#define BKGL_PLOTBASE_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk
{
  // -------------------- forward declaration
  class PlotAxis;
  class PlotMarker;

  namespace details
  {
    class PlotAbstractDataVectorView;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT PlotBase : public details::AbstractRenderable
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
      PlotBase();
      #else
      explicit PlotBase(qt_gl_functions* gl);
      #endif
      PlotBase(const PlotBase&) = delete;
      PlotBase(PlotBase&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotBase();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] GLfloat window_width() const;
      [[nodiscard]] GLfloat window_height() const;

      [[nodiscard]] PlotAxis& x_axis();
      [[nodiscard]] const PlotAxis& x_axis() const;
      [[nodiscard]] PlotAxis& y_axis();
      [[nodiscard]] const PlotAxis& y_axis() const;

    private:
      [[nodiscard]] GLfloat _x_min_from_data() const;
      [[nodiscard]] GLfloat _x_max_from_data() const;
      [[nodiscard]] GLfloat _y_min_from_data() const;
      [[nodiscard]] GLfloat _y_max_from_data() const;
    public:

      [[nodiscard]] GLfloat x_min() const;
      [[nodiscard]] GLfloat x_max() const;
      [[nodiscard]] GLfloat y_min() const;
      [[nodiscard]] GLfloat y_max() const;

      [[nodiscard]] GLfloat border_width_x_in_percent() const;
      [[nodiscard]] GLfloat border_width_y_in_percent() const;
      [[nodiscard]] unsigned int num_datavectorviews() const;
      [[nodiscard]] std::shared_ptr<details::PlotAbstractDataVectorView>& datavectorview(unsigned int i);
      [[nodiscard]] const std::shared_ptr<details::PlotAbstractDataVectorView>& datavectorview(unsigned int i) const;

      /// @{ -------------------------------------------------- MARKERS
      [[nodiscard]] unsigned int num_markers() const;
      [[nodiscard]] unsigned int num_markers_horizontal() const;
      [[nodiscard]] unsigned int num_markers_vertical() const;

      [[nodiscard]] std::shared_ptr<PlotMarker>& marker_horizontal(unsigned int i);
      [[nodiscard]] const std::shared_ptr<PlotMarker>& marker_horizontal(unsigned int i) const;
      [[nodiscard]] std::shared_ptr<PlotMarker>& marker_vertical(unsigned int i);
      [[nodiscard]] const std::shared_ptr<PlotMarker>& marker_vertical(unsigned int i) const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] PlotBase& operator=(const PlotBase&) = delete;
      [[maybe_unused]] PlotBase& operator=(PlotBase&&) noexcept;
      /// @}

      [[maybe_unused]] std::shared_ptr<details::PlotAbstractDataVectorView>& add_datavectorview(std::shared_ptr<details::PlotAbstractDataVectorView> dvv);
      [[maybe_unused]] bool remove_datavectorview(std::shared_ptr<details::PlotAbstractDataVectorView> dvv);

      /// @{ -------------------------------------------------- MARKERS
    private:
      [[maybe_unused]] std::shared_ptr<PlotMarker>& _add_marker();
    public:
      [[maybe_unused]] std::shared_ptr<PlotMarker>& add_marker_horizontal();
      [[maybe_unused]] std::shared_ptr<PlotMarker>& add_marker_vertical();

      void clear_markers();
      void clear_markers_horizontal();
      void clear_markers_vertical();
      /// @}

      void set_x_min(GLfloat xmin);
      void set_x_max(GLfloat xmax);
      void set_y_min(GLfloat ymin);
      void set_y_max(GLfloat ymax);

    private:
      void _reset_xy_minmax();
    public:

      void set_xy_minmax_from_data();

      void set_border_width_x_in_percent(GLfloat b);
      void set_border_width_y_in_percent(GLfloat b);

      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
    private:
      [[maybe_unused]] bool init_ubo();
    public:
      [[maybe_unused]] bool init();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
    private:
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

      virtual void draw_impl() override;
  }; // class PlotBase
} // namespace bk

#endif //BKGL_PLOTBASE_H