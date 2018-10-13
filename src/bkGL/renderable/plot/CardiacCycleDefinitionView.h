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

#ifndef BK_CARDIACCYCLEDEFINITIONVIEW_H
#define BK_CARDIACCYCLEDEFINITIONVIEW_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/plot/AreaPlotView.h>

namespace bk
{
  // -------------------- forward declaration
  class PlotMarker;

  template<typename...> class Signal;
  // -------------------- forward declaration END

  class BKGL_EXPORT CardiacCycleDefinitionView : public AreaPlotView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = AreaPlotView;

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
      CardiacCycleDefinitionView();
      #else
      explicit CardiacCycleDefinitionView(qt_gl_functions* gl);
      #endif
      CardiacCycleDefinitionView(const CardiacCycleDefinitionView&) = delete;
      CardiacCycleDefinitionView(CardiacCycleDefinitionView&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~CardiacCycleDefinitionView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] std::shared_ptr<PlotMarker>& marker_systole_begin();
      [[nodiscard]] const std::shared_ptr<PlotMarker>& marker_systole_begin() const;

      [[nodiscard]] std::shared_ptr<PlotMarker>& marker_systole_end();
      [[nodiscard]] const std::shared_ptr<PlotMarker>& marker_systole_end() const;

      [[nodiscard]] std::shared_ptr<PlotMarker>& marker_current();
      [[nodiscard]] const std::shared_ptr<PlotMarker>& marker_current() const;

      [[nodiscard]] unsigned int time_point_id_systole_begin() const;
      [[nodiscard]] unsigned int time_point_id_systole_end() const;
      [[nodiscard]] double time_systole_begin() const;
      [[nodiscard]] double time_systole_end() const;

      [[nodiscard]] Signal<unsigned int, unsigned int>& signal_times_changed();

    private:
      [[nodiscard]] unsigned int current_time_id() const;
    public:

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] CardiacCycleDefinitionView& operator=(const CardiacCycleDefinitionView&) = delete;
      [[maybe_unused]] CardiacCycleDefinitionView& operator=(CardiacCycleDefinitionView&&) noexcept;
      /// @}

      void set_temporal_resolution(double temporal_resolution);

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      void set_current_marker_pos();
      void scroll_current_marker(bool up);
    public:

      //====================================================================================================
      //===== GL
      //====================================================================================================
      /// @{ -------------------------------------------------- EVENTS
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_mouse_button_released(MouseButton_ btn) override;
      virtual void on_mouse_wheel_up() override;
      virtual void on_mouse_wheel_down() override;
      /// @}
  }; // class CardiacCycleDefinitionView
} // namespace bk

#endif //BK_CARDIACCYCLEDEFINITIONVIEW_H
