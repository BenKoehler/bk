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

#ifndef BKGL_INTERACTOR_H
#define BKGL_INTERACTOR_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/gl_definitions.h>
#include <bkGL/EKey.h>
#include <bkGL/EMouseButton.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class KeyBoard;
  class Mouse;

  class TrackBall;

  class WindowGeometry;

  template<typename...> class Signal;

  namespace details
  {
    class AbstractRenderable;
    class AbstractSceneRenderable;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT Interactor
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Interactor;
    public:

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
      /// @{ -------------------------------------------------- CTOR
      Interactor();
      Interactor(const self_type&) = delete;
      Interactor(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Interactor();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS ENABLED
      [[nodiscard]] bool rotation_enabled() const;
      [[nodiscard]] bool translation_enabled() const;
      [[nodiscard]] bool zoom_enabled() const;
      /// @}

      /// @{ -------------------------------------------------- GET TRANSLATION
      [[nodiscard]] const bk::Vec3<GLfloat>& translation() const;
      /// @}

      /// @{ -------------------------------------------------- GET TRANSLATION SPEED FACTOR
      [[nodiscard]] GLfloat translation_speed_factor() const;
      /// @}

      /// @{ -------------------------------------------------- GET SCALE
      [[nodiscard]] const bk::Vec3<GLfloat>& scale_manual() const;
      [[nodiscard]] GLfloat scale_fixed() const;
      /// @}

      /// @{ -------------------------------------------------- GET KEYBOARD
      [[nodiscard]] KeyBoard& keyboard();
      [[nodiscard]] const KeyBoard& keyboard() const;
      /// @}

      /// @{ -------------------------------------------------- GET MOUSE
      [[nodiscard]] Mouse& mouse();
      [[nodiscard]] const Mouse& mouse() const;
      /// @}

      /// @{ -------------------------------------------------- GET TRACKBALL
      [[nodiscard]] TrackBall& trackball();
      [[nodiscard]] const TrackBall& trackball() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<>& signal_do_update();
      [[nodiscard]] const bk::Signal<>& signal_do_update() const;

      [[nodiscard]] bk::Signal<>& signal_require_modelview_update();
      [[nodiscard]] const bk::Signal<>& signal_require_modelview_update() const;

      [[nodiscard]] bk::Signal<GLint, GLint>& signal_mouse_position_changed();
      [[nodiscard]] const bk::Signal<GLint, GLint>& signal_mouse_position_changed() const;

      [[nodiscard]] bk::Signal<MouseButton_>& signal_mouse_button_pressed();
      [[nodiscard]] const bk::Signal<MouseButton_>& signal_mouse_button_pressed() const;

      [[nodiscard]] bk::Signal<MouseButton_>& signal_mouse_button_released();
      [[nodiscard]] const bk::Signal<MouseButton_>& signal_mouse_button_released() const;

      [[nodiscard]] bk::Signal<Key_>& signal_key_pressed();
      [[nodiscard]] const bk::Signal<Key_>& signal_key_pressed() const;

      [[nodiscard]] bk::Signal<Key_>& signal_key_released();
      [[nodiscard]] const bk::Signal<Key_>& signal_key_released() const;

      [[nodiscard]] bk::Signal<>& signal_wheel_down();
      [[nodiscard]] const bk::Signal<>& signal_wheel_down() const;

      [[nodiscard]] bk::Signal<>& signal_wheel_up();
      [[nodiscard]] const bk::Signal<>& signal_wheel_up() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET WINDOW SIZE
      void set_window_size(unsigned int width, unsigned int height);
      void set_window_size(const WindowGeometry& window);
      /// @}

      /// @{ -------------------------------------------------- SET MOUSE POSITION
      void set_mouse_position(GLint x, GLint y);
      /// @}

      /// @{ -------------------------------------------------- SET TRANSLATION ENABLED
      void set_translation_enabled(bool b);
      void enable_translation();
      void disable_translation();
      /// @}

      /// @{ -------------------------------------------------- SET TRANSLATION SPEED FACTOR
      void set_translation_speed_factor(GLfloat f);
      /// @}

      /// @{ -------------------------------------------------- SET ROTATION ENABLED
      void set_rotation_enabled(bool b);
      void enable_rotation();
      void disable_rotation();
      /// @}

      /// @{ -------------------------------------------------- SET ZOOM ENABLED
      void set_zoom_enabled(bool b);
      void enable_zoom();
      void disable_zoom();
      /// @}

      /// @{ -------------------------------------------------- SET ZOOM
      void set_zoom(GLfloat zx, GLfloat zy, GLfloat zz);
      void set_zoom(GLfloat zxyz);
      /// @}

      /// @{ -------------------------------------------------- SET TRANSLATION
      void set_translation(GLfloat tx, GLfloat ty, GLfloat tz);
      void set_translation(GLfloat txyz);
      /// @}

      /// @{ -------------------------------------------------- SET SCENE SCALE
      void set_scale_scene(GLfloat s);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init();
      /// @}

      /// @{ -------------------------------------------------- CONNECT SIGNALS
      void connect_signals(std::shared_ptr<details::AbstractRenderable>& r);
      void connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r);
      /// @}

      /// @{ -------------------------------------------------- MOUSE EVENTS
      void mouse_move(GLint x, GLint y);
      void mouse_pressed(MouseButton_ btn);
      void mouse_released(MouseButton_ btn);
      /// @}

      /// @{ -------------------------------------------------- WHEEL EVENTS
      void wheel_up();
      void wheel_down();
      /// @}

      /// @{ -------------------------------------------------- KEY EVENTS
      void key_pressed(Key_ k);
      void key_released(Key_ k);
      /// @}
  }; // class Interactor
} // namespace bk

#endif //BKGL_INTERACTOR_H
