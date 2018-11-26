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

#include <bkGL/Interactor.h>

#include <algorithm>

#include <bk/Matrix>
#include <bk/Signal>

#include <bkGL/KeyBoard.h>
#include <bkGL/Mouse.h>
#include <bkGL/WindowGeometry.h>
#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/trackball/TrackBall.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class Interactor::Impl
  {
    public:
      bool rotation_enabled;
      bool translation_enabled;
      bool zoom_enabled;
      bk::Vec3<GLfloat> translation;
      bk::Vec3<GLfloat> scale_manual;
      GLfloat scale_fixed;
      GLfloat translation_speed;
      KeyBoard keyboard;
      Mouse mouse;
      TrackBall trackball;
      bk::Signal<> s_do_update;
      bk::Signal<> s_require_modelview_update;
      bk::Signal<GLint, GLint> s_mouse_pos_changed;
      bk::Signal<MouseButton_> s_mouse_button_pressed;
      bk::Signal<MouseButton_> s_mouse_button_released;
      bk::Signal<Key_> s_key_pressed;
      bk::Signal<Key_> s_key_released;
      bk::Signal<> s_wheel_up;
      bk::Signal<> s_wheel_down;

      Impl()
          : rotation_enabled(true),
            translation_enabled(true),
            zoom_enabled(true),
            translation(0, 0, 0),
            scale_manual(1, 1, 1),
            scale_fixed(1),
            translation_speed(1)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Interactor::Interactor() = default;

  Interactor::Interactor(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Interactor::~Interactor() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS ENABLED
  bool Interactor::rotation_enabled() const
  { return _pdata->rotation_enabled; }

  bool Interactor::translation_enabled() const
  { return _pdata->translation_enabled; }

  bool Interactor::zoom_enabled() const
  { return _pdata->zoom_enabled; }
  /// @}

  /// @{ -------------------------------------------------- GET TRANSLATION
  const bk::Vec3<GLfloat>& Interactor::translation() const
  { return _pdata->translation; }
  /// @}

  /// @{ -------------------------------------------------- GET TRANSLATION SPEED FACTOR
  GLfloat Interactor::translation_speed_factor() const
  { return _pdata->translation_speed; }
  /// @}

  /// @{ -------------------------------------------------- GET SCALE
  const bk::Vec3<GLfloat>& Interactor::scale_manual() const
  { return _pdata->scale_manual; }

  GLfloat Interactor::scale_fixed() const
  { return _pdata->scale_fixed; }
  /// @}

  /// @{ -------------------------------------------------- GET KEYBOARD
  KeyBoard& Interactor::keyboard()
  { return _pdata->keyboard; }

  const KeyBoard& Interactor::keyboard() const
  { return _pdata->keyboard; }
  /// @}

  /// @{ -------------------------------------------------- GET MOUSE
  Mouse& Interactor::mouse()
  { return _pdata->mouse; }

  const Mouse& Interactor::mouse() const
  { return _pdata->mouse; }
  /// @}

  /// @{ -------------------------------------------------- GET TRACKBALL
  TrackBall& Interactor::trackball()
  { return _pdata->trackball; }

  const TrackBall& Interactor::trackball() const
  { return _pdata->trackball; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<>& Interactor::signal_do_update()
  { return _pdata->s_do_update; }

  const bk::Signal<>& Interactor::signal_do_update() const
  { return _pdata->s_do_update; }

  bk::Signal<>& Interactor::signal_require_modelview_update()
  { return _pdata->s_require_modelview_update; }

  const bk::Signal<>& Interactor::signal_require_modelview_update() const
  { return _pdata->s_require_modelview_update; }

  bk::Signal<GLint, GLint>& Interactor::signal_mouse_position_changed()
  { return _pdata->s_mouse_pos_changed; }

  const bk::Signal<GLint, GLint>& Interactor::signal_mouse_position_changed() const
  { return _pdata->s_mouse_pos_changed; }

  bk::Signal<MouseButton_>& Interactor::signal_mouse_button_pressed()
  { return _pdata->s_mouse_button_pressed; }

  const bk::Signal<MouseButton_>& Interactor::signal_mouse_button_pressed() const
  { return _pdata->s_mouse_button_pressed; }

  bk::Signal<MouseButton_>& Interactor::signal_mouse_button_released()
  { return _pdata->s_mouse_button_released; }

  const bk::Signal<MouseButton_>& Interactor::signal_mouse_button_released() const
  { return _pdata->s_mouse_button_released; }

  bk::Signal<Key_>& Interactor::signal_key_pressed()
  { return _pdata->s_key_pressed; }

  const bk::Signal<Key_>& Interactor::signal_key_pressed() const
  { return _pdata->s_key_pressed; }

  bk::Signal<Key_>& Interactor::signal_key_released()
  { return _pdata->s_key_released; }

  const bk::Signal<Key_>& Interactor::signal_key_released() const
  { return _pdata->s_key_released; }

  bk::Signal<>& Interactor::signal_wheel_down()
  { return _pdata->s_wheel_down; }

  const bk::Signal<>& Interactor::signal_wheel_down() const
  { return _pdata->s_wheel_down; }

  bk::Signal<>& Interactor::signal_wheel_up()
  { return _pdata->s_wheel_up; }

  const bk::Signal<>& Interactor::signal_wheel_up() const
  { return _pdata->s_wheel_up; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto Interactor::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET WINDOW SIZE
  void Interactor::set_window_size(unsigned int width, unsigned int height)
  { _pdata->trackball.set_window_size(width, height); }

  void Interactor::set_window_size(const WindowGeometry& window)
  { set_window_size(window.width(), window.height()); }
  /// @}

  /// @{ -------------------------------------------------- SET MOUSE POSITION
  void Interactor::set_mouse_position(GLint x, GLint y)
  {
      _pdata->mouse.set_pos(x, y);
      _pdata->trackball.set_current_mouse_position(x, y);

      _pdata->s_mouse_pos_changed.emit_signal(x, y);
  }
  /// @}

  /// @{ -------------------------------------------------- SET TRANSLATION ENABLED
  void Interactor::set_translation_enabled(bool b)
  { _pdata->translation_enabled = b; }

  void Interactor::enable_translation()
  { set_translation_enabled(true); }

  void Interactor::disable_translation()
  { set_translation_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET TRANSLATION SPEED FACTOR
  void Interactor::set_translation_speed_factor(GLfloat f)
  { _pdata->translation_speed = std::abs(f); }
  /// @}

  /// @{ -------------------------------------------------- SET ROTATION ENABLED
  void Interactor::set_rotation_enabled(bool b)
  { _pdata->rotation_enabled = b; }

  void Interactor::enable_rotation()
  { set_rotation_enabled(true); }

  void Interactor::disable_rotation()
  { set_rotation_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET ZOOM ENABLED
  void Interactor::set_zoom_enabled(bool b)
  { _pdata->zoom_enabled = b; }

  void Interactor::enable_zoom()
  { set_zoom_enabled(true); }

  void Interactor::disable_zoom()
  { set_zoom_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET ZOOM
  void Interactor::set_zoom(GLfloat zx, GLfloat zy, GLfloat zz)
  {
      _pdata->scale_manual[0] = zx;
      _pdata->scale_manual[1] = zy;
      _pdata->scale_manual[2] = zz;

      _pdata->s_require_modelview_update.emit_signal();
  }

  void Interactor::set_zoom(GLfloat zxyz)
  { set_zoom(zxyz, zxyz, zxyz); }
  /// @}

  /// @{ -------------------------------------------------- SET SCENE SCALE
  void Interactor::set_scale_scene(GLfloat s)
  {
      _pdata->scale_fixed = s;
      _pdata->s_require_modelview_update.emit_signal();
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void Interactor::clear()
  {
      _pdata->trackball.reset_rotation();
      _pdata->translation.set_zero();
      _pdata->scale_manual.set_one();
      _pdata->scale_fixed = 1;
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void Interactor::init()
  {
      _pdata->trackball.set_current_mouse_position(0, 0);
      _pdata->trackball.set_radius(1);
      _pdata->trackball.set_upvector_id(2);
  }
  /// @}

  /// @{ -------------------------------------------------- CONNECT SIGNALS
  void Interactor::connect_signals(std::shared_ptr<details::AbstractRenderable>& r)
  {
      _pdata->s_mouse_pos_changed.connect([=](GLint x, GLint y)
                                          { r->on_mouse_pos_changed(x, y); });

      _pdata->s_mouse_button_pressed.connect([=](MouseButton_ btn)
                                             { r->on_mouse_button_pressed(btn); });

      _pdata->s_mouse_button_released.connect([=](MouseButton_ btn)
                                              { r->on_mouse_button_released(btn); });

      _pdata->s_wheel_down.connect([=]()
                                   { r->on_mouse_wheel_down(); });

      _pdata->s_wheel_up.connect([=]()
                                 { r->on_mouse_wheel_up(); });

      _pdata->s_key_pressed.connect([=](Key_ k)
                                    { r->on_key_pressed(k); });

      _pdata->s_key_released.connect([=](Key_ k)
                                     { r->on_key_released(k); });
  }

  void Interactor::connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r)
  {
      _pdata->s_mouse_pos_changed.connect([=](GLint x, GLint y)
                                          { r->on_mouse_pos_changed(x, y); });

      _pdata->s_mouse_button_pressed.connect([=](MouseButton_ btn)
                                             { r->on_mouse_button_pressed(btn); });

      _pdata->s_mouse_button_released.connect([=](MouseButton_ btn)
                                              { r->on_mouse_button_released(btn); });

      _pdata->s_wheel_down.connect([=]()
                                   { r->on_mouse_wheel_down(); });

      _pdata->s_wheel_up.connect([=]()
                                 { r->on_mouse_wheel_up(); });

      _pdata->s_key_pressed.connect([=](Key_ k)
                                    { r->on_key_pressed(k); });

      _pdata->s_key_released.connect([=](Key_ k)
                                     { r->on_key_released(k); });
  }
  /// @}

  /// @{ -------------------------------------------------- MOUSE EVENTS
  void Interactor::mouse_move(GLint x, GLint y)
  {
      set_mouse_position(x, y);

      bool do_update = false;

      if (_pdata->mouse.left_button_is_pressed())
      {
          if (_pdata->rotation_enabled)
          { do_update = true; }
      }
      else if (_pdata->mouse.right_button_is_pressed())
      {
          if (_pdata->translation_enabled)
          {
              const GLfloat s = 0.5 * _pdata->scale_fixed * _pdata->translation_speed;
              _pdata->translation[0] += (s * _pdata->mouse.dx());
              _pdata->translation[2] -= (s * _pdata->mouse.dy());

              do_update = true;
          }
      }
      else if (_pdata->mouse.middle_button_is_pressed())
      {
          if (_pdata->zoom_enabled)
          {
              constexpr GLfloat minScale = static_cast<GLfloat>(0.05);
              constexpr GLfloat s = 0.02; // TODO: more sophisticated choice
              const GLfloat dy = s * static_cast<GLfloat>(_pdata->mouse.dy());
              _pdata->scale_manual[0] = std::max(_pdata->scale_manual[0] - dy, minScale);
              _pdata->scale_manual[1] = std::max(_pdata->scale_manual[1] - dy, minScale);
              _pdata->scale_manual[2] = std::max(_pdata->scale_manual[2] - dy, minScale);

              do_update = true;
          }
      }

      if (do_update)
      {
          _pdata->s_require_modelview_update.emit_signal();
          _pdata->s_do_update.emit_signal();
      }
  }

  void Interactor::mouse_pressed(MouseButton_ btn)
  {
      const bool emitSignal = !_pdata->mouse.button_is_pressed(btn);

      _pdata->mouse.set_button_pressed(btn, true);

      bool do_update = false;

      if (_pdata->mouse.left_button_is_pressed())
      {
          if (_pdata->rotation_enabled)
          {
              _pdata->trackball.set_current_mouse_position(_pdata->mouse.x(), _pdata->mouse.y());

              if (!_pdata->trackball.is_active())
              { _pdata->trackball.set_active(); }

              do_update = true;
          }
      }

      if (emitSignal)
      { _pdata->s_mouse_button_pressed.emit_signal(btn); }

      if (do_update)
      { _pdata->s_do_update.emit_signal(); }
  }

  void Interactor::mouse_released(MouseButton_ btn)
  {
      const bool emitSignal = _pdata->mouse.button_is_pressed(btn);

      _pdata->mouse.set_button_pressed(btn, false);

      bool do_update = false;
      if (!_pdata->mouse.left_button_is_pressed())
      {
          if (_pdata->rotation_enabled)
          {
              _pdata->trackball.set_current_mouse_position(_pdata->mouse.x(), _pdata->mouse.y());

              if (_pdata->trackball.is_active())
              { _pdata->trackball.set_inactive(); }

              do_update = true;
          }
      }

      if (emitSignal)
      { _pdata->s_mouse_button_released.emit_signal(btn); }

      if (do_update)
      { _pdata->s_do_update.emit_signal(); }
  }
  /// @}

  /// @{ -------------------------------------------------- WHEEL EVENTS
  void Interactor::wheel_up()
  { _pdata->s_wheel_up.emit_signal(); }

  void Interactor::wheel_down()
  { _pdata->s_wheel_down.emit_signal(); }
  /// @}

  /// @{ -------------------------------------------------- KEY EVENTS
  void Interactor::key_pressed(Key_ k)
  {
      if (!_pdata->keyboard.key_is_pressed(k))
      { _pdata->s_key_pressed.emit_signal(k); }
  }

  void Interactor::key_released(Key_ k)
  {
      if (_pdata->keyboard.key_is_pressed(k))
      { _pdata->s_key_released.emit_signal(k); }
  }
  /// @}
} // namespace bk