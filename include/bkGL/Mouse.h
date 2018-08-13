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

#ifndef BKGL_MOUSE_H
#define BKGL_MOUSE_H

#include <memory>

#include "gl_definitions.h"
#include "lib/export.h"
#include "EMouseButton.h"

namespace bk
{
  class BK_GL_EXPORT Mouse
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Mouse;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      Mouse();
      Mouse(const self_type& other);
      Mouse(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Mouse();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET POSITION
      [[nodiscard]] GLint x() const;
      [[nodiscard]] GLint y() const;
      /// @}

      /// @{ -------------------------------------------------- GET DIFFERENCE
      //! difference to last position
      [[nodiscard]] GLint dx() const;
      [[nodiscard]] GLint dy() const;
      /// @}

      /// @{ -------------------------------------------------- GET MOVEMENT INFORMATION
      [[nodiscard]] bool last_move_was_up() const;
      [[nodiscard]] bool last_move_was_down() const;
      [[nodiscard]] bool last_move_was_right() const;
      [[nodiscard]] bool last_move_was_left() const;
      /// @}

      /// @{ -------------------------------------------------- GET BUTTON INFORMATION
      [[nodiscard]] bool button_is_pressed(MouseButton btn) const;
      [[nodiscard]] bool left_button_is_pressed() const;
      [[nodiscard]] bool right_button_is_pressed() const;
      [[nodiscard]] bool middle_button_is_pressed() const;
      [[nodiscard]] bool forward_button_is_pressed() const;
      [[nodiscard]] bool backward_button_is_pressed() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET POSITION
      void set_pos(GLint x, GLint y);
      /// @}

      /// @{ -------------------------------------------------- SET BUTTON PRESSED
      void set_button_pressed(MouseButton btn, bool pressed);
      void set_left_button_pressed();
      void set_right_button_pressed();
      void set_middle_button_pressed();
      void set_forward_button_pressed();
      void set_backward_button_pressed();
      /// @}

      /// @{ -------------------------------------------------- SET BUTTON RELEASED
      void set_left_button_released();
      void set_right_button_released();
      void set_middle_button_released();
      void set_forward_button_released();
      void set_backward_button_released();
      /// @}
  }; // class Mouse
} // namespace bk

#endif //BKGL_MOUSE_H


