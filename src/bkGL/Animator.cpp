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

#include <bkGL/Animator.h>

#include <algorithm>
#include <cassert>

#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bk/Clock>
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class Animator::Impl
  {
    public:
      bool enabled;
      bool is_paused;
      GLfloat current_time;
      GLfloat max_time; // heartbeat_in_ms
      GLfloat time_delta;
      GLfloat speed_factor; // 1 = double time, 0.5 = half speed, 2 = double speed, ...
      GLfloat target_fps;
      GLfloat update_interval_in_ms;
      
      bk::Signal<GLfloat> s_current_time_changed;
      bk::Signal<GLfloat> s_max_time_changed;
      bk::Signal<bool> s_enabled_changed;
      bk::Signal<bool> s_paused_changed;
      bk::Signal<> s_speed_settings_changed;
      bk::Clock update_timer;

      Impl():
          enabled(false),
          is_paused(false),
          current_time(0),
          max_time(0),
          time_delta(0),
          speed_factor(0.3),
          target_fps(60),
          update_interval_in_ms(static_cast<GLfloat>(1000) * speed_factor / target_fps)
      { /* do nothing */ }
      
      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;

      ~Impl()
      {
          s_current_time_changed.disconnect_all();
          s_max_time_changed.disconnect_all();
          s_enabled_changed.disconnect_all();
          s_paused_changed.disconnect_all();
          s_speed_settings_changed.disconnect_all();
      }

      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Animator::Animator() = default;

  Animator::Animator(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Animator::~Animator() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS ENABLED
  bool Animator::is_enabled() const
  { return _pdata->enabled; }
  /// @}

  /// @{ -------------------------------------------------- IS PAUSED
  bool Animator::is_paused() const
  { return _pdata->is_paused; }
  /// @}

  /// @{ -------------------------------------------------- GET CURRENT TIME
  GLfloat Animator::current_time() const
  { return _pdata->current_time; }  
  /// @}

  /// @{ -------------------------------------------------- GET MAX TIME
  GLfloat Animator::max_time() const
  { return _pdata->max_time; }
  /// @}

  /// @{ -------------------------------------------------- GET SPEED FACTOR
  GLfloat Animator::speed_factor() const
  { return _pdata->speed_factor; }
  /// @}

  /// @{ -------------------------------------------------- GET TARGET FPS
  GLfloat Animator::target_fps() const
  { return _pdata->target_fps; }
  /// @}

  /// @{ -------------------------------------------------- GET UPDATE INTERVAL
  GLfloat Animator::update_interval_in_ms() const
  { return _pdata->update_interval_in_ms; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<GLfloat>& Animator::signal_current_time_changed()
  { return _pdata->s_current_time_changed; }

  const bk::Signal<GLfloat>& Animator::signal_current_time_changed() const
  { return _pdata->s_current_time_changed; }

  bk::Signal<GLfloat>& Animator::signal_max_time_changed()
  { return _pdata->s_max_time_changed; }

  const bk::Signal<GLfloat>& Animator::signal_max_time_changed() const
  { return _pdata->s_max_time_changed; }

  bk::Signal<bool>& Animator::signal_enabled_changed()
  { return _pdata->s_enabled_changed; }

  const bk::Signal<bool>& Animator::signal_enabled_changed() const
  { return _pdata->s_enabled_changed; }

  bk::Signal<bool>& Animator::signal_paused_changed()
  { return _pdata->s_paused_changed; }

  const bk::Signal<bool>& Animator::signal_paused_changed() const
  { return _pdata->s_paused_changed; }

  bk::Signal<>& Animator::signal_speed_settings_changed()
  { return _pdata->s_speed_settings_changed; }

  const bk::Signal<>& Animator::signal_speed_settings_changed() const
  { return _pdata->s_speed_settings_changed; }

  /// @}
  
  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Animator::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET ENABLED
  void Animator::set_enabled(bool b)
  {
      if (_pdata->enabled != b)
      {
          _pdata->enabled = b;
          _pdata->is_paused = false;
          set_current_time(0);

          if (_pdata->enabled)
          { _pdata->update_timer.start(); }

          _pdata->s_enabled_changed.emit_signal(_pdata->enabled);
      }
  }

  void Animator::enable()
  { set_enabled(true); }

  void Animator::disable()
  { set_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET PAUSED
  void Animator::set_paused(bool b)
  {
      if (_pdata->is_paused != b)
      {
          _pdata->is_paused = b;
          _pdata->s_paused_changed.emit_signal(_pdata->is_paused);
      }
  }

  void Animator::pause()
  { set_paused(true); }

  void Animator::unpause()
  { set_paused(false); }
  /// @}

  /// @{ -------------------------------------------------- SET CURRENT TIME
  void Animator::set_current_time(GLfloat ms)
  {
      _pdata->current_time = ms;
      _pdata->s_current_time_changed.emit_signal(_pdata->current_time);
  }
  /// @}

  /// @{ -------------------------------------------------- SET MAX TIME
  void Animator::set_max_time(GLfloat ms)
  { _pdata->max_time = ms; }
  /// @}

  /// @{ -------------------------------------------------- SET PARAMETERS
  void Animator::set_parameters(GLfloat speed_factor, GLfloat target_fps)
  {
      assert(speed_factor != 0 && "speed factor must not be 0!");

      _pdata->speed_factor = speed_factor;
      _pdata->target_fps = std::max(target_fps, static_cast<GLfloat>(0.25));
      _pdata->update_interval_in_ms = static_cast<GLfloat>(1000) / _pdata->target_fps;
      _pdata->time_delta = static_cast<GLfloat>(1000) * _pdata->speed_factor / _pdata->target_fps;
  }
  /// @}

  /// @{ -------------------------------------------------- SET SIGNALS MUTED
  void Animator::set_signals_muted(bool setMuted)
  {
      _pdata->s_current_time_changed.mute(setMuted);
      _pdata->s_max_time_changed.mute(setMuted);
      _pdata->s_enabled_changed.mute(setMuted);
      _pdata->s_paused_changed.mute(setMuted);
      _pdata->s_speed_settings_changed.mute(setMuted);
  }
  /// @}
  
  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CONNECT SIGNALS
  void Animator::connect_signals(std::shared_ptr<details::AbstractRenderable>& r)
  {
      _pdata->s_enabled_changed.connect([=](bool enabled)
                                        {r->set_animation_is_enabled(enabled);});
  }

  void Animator::connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r)
  {
      _pdata->s_enabled_changed.connect([=](bool enabled)
                                        {r->set_animation_is_enabled(enabled);});
  }
  /// @}

  /// @{ -------------------------------------------------- ADVANCE CURRENT TIME
  void Animator::advance_current_time()
  {
      GLfloat temp = _pdata->current_time + _pdata->time_delta;

      if (temp >= _pdata->max_time)
      { temp -= _pdata->max_time; }

      set_current_time(temp);
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE
  //! update function called in render loop
  void Animator::update()
  {
      if (_pdata->enabled && !_pdata->is_paused)
      {
          _pdata->update_timer.stop();

          if (_pdata->update_timer.time_in_milli_sec() >= _pdata->update_interval_in_ms)
          {
              _pdata->update_timer.start(); // reset timer

              advance_current_time();
          }
      }
  }
  /// @}
} // namespace bk