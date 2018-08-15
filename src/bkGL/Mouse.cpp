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

#include <bkGL/Mouse.h>

#include <array>
#include <unordered_set>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class Mouse::Impl
  {
    public:
      std::array<GLint, 2> pos;
      std::array<GLint, 2> last_pos;
      std::unordered_set<MouseButton> pressed_buttons;

      Impl()
          : pos{{0, 0}},
            last_pos{{0, 0}}
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Mouse::Mouse()
      : _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  Mouse::Mouse(const self_type& other)
      : _pdata(new Impl(*other._pdata.get()))
  { /* do nothing */ }

  Mouse::Mouse(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Mouse::~Mouse() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET POSITION
  GLint Mouse::x() const
  { return _pdata->pos[0]; }

  GLint Mouse::y() const
  { return _pdata->pos[1]; }
  /// @}

  /// @{ -------------------------------------------------- GET DIFFERENCE
  GLint Mouse::dx() const
  { return _pdata->pos[0] - _pdata->last_pos[0]; }

  GLint Mouse::dy() const
  { return _pdata->pos[1] - _pdata->last_pos[1]; }
  /// @}

  /// @{ -------------------------------------------------- GET MOVEMENT INFORMATION
  bool Mouse::last_move_was_up() const
  { return dy() > 0; }

  bool Mouse::last_move_was_down() const
  { return dy() < 0; }

  bool Mouse::last_move_was_right() const
  { return dx() > 0; }

  bool Mouse::last_move_was_left() const
  { return dx() < 0; }
  /// @}

  /// @{ -------------------------------------------------- GET BUTTON INFORMATION
  bool Mouse::button_is_pressed(MouseButton btn) const
  { return _pdata->pressed_buttons.find(btn) != _pdata->pressed_buttons.end(); }
  
  bool Mouse::left_button_is_pressed() const
  { return button_is_pressed(MouseButton::Left); }

  bool Mouse::right_button_is_pressed() const
  { return button_is_pressed(MouseButton::Right); }

  bool Mouse::middle_button_is_pressed() const
  { return button_is_pressed(MouseButton::Middle); }

  bool Mouse::forward_button_is_pressed() const
  { return button_is_pressed(MouseButton::Forward); }

  bool Mouse::backward_button_is_pressed() const
  { return button_is_pressed(MouseButton::Backward); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Mouse::operator=(const self_type& other) -> self_type&
  {
      _pdata->pos[0] = other._pdata->pos[0];
      _pdata->pos[1] = other._pdata->pos[1];
      _pdata->last_pos[0] = other._pdata->last_pos[0];
      _pdata->last_pos[1] = other._pdata->last_pos[1];
      _pdata->pressed_buttons = other._pdata->pressed_buttons;

      return *this;
  }

  auto Mouse::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET POSITION
  void Mouse::set_pos(GLint x, GLint y)
  {
      _pdata->last_pos = _pdata->pos;
      _pdata->pos[0] = x;
      _pdata->pos[1] = y;
  }
  /// @}

  /// @{ -------------------------------------------------- SET BUTTON PRESSED
  void Mouse::set_button_pressed(MouseButton btn, bool pressed)
  {
      if (pressed)
      { _pdata->pressed_buttons.insert(btn); }
      else
      { _pdata->pressed_buttons.erase(btn); }
  }
      
  void Mouse::set_left_button_pressed()
  { set_button_pressed(MouseButton::Left, true); }

  void Mouse::set_right_button_pressed()
  { set_button_pressed(MouseButton::Right, true); }

  void Mouse::set_middle_button_pressed()
  { set_button_pressed(MouseButton::Middle, true); }

  void Mouse::set_forward_button_pressed()
  { set_button_pressed(MouseButton::Forward, true); }

  void Mouse::set_backward_button_pressed()
  { set_button_pressed(MouseButton::Backward, true); }
  /// @}

  /// @{ -------------------------------------------------- SET BUTTON RELEASED
  void Mouse::set_left_button_released()
  { set_button_pressed(MouseButton::Left, false); }

  void Mouse::set_right_button_released()
  { set_button_pressed(MouseButton::Right, false); }

  void Mouse::set_middle_button_released()
  { set_button_pressed(MouseButton::Middle, false); }

  void Mouse::set_forward_button_released()
  { set_button_pressed(MouseButton::Forward, false); }

  void Mouse::set_backward_button_released()
  { set_button_pressed(MouseButton::Backward, false); }
  /// @}
} // namespace bk


