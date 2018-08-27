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

#ifndef BKGL_ANIMATOR_H
#define BKGL_ANIMATOR_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class Clock;
  template<typename...> class Signal;

  namespace details
  {
    class AbstractRenderable;
    class AbstractSceneRenderable;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT Animator
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Animator;
    public:

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;
      bk::cpimpl <Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      Animator();
      Animator(const self_type&) = delete;
      Animator(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Animator();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS ENABLED
      [[nodiscard]] bool is_enabled() const;
      /// @}

      /// @{ -------------------------------------------------- IS PAUSED
      [[nodiscard]] bool is_paused() const;
      /// @}

      /// @{ -------------------------------------------------- GET CURRENT TIME
      [[nodiscard]] GLfloat current_time() const;
      /// @}

      /// @{ -------------------------------------------------- GET MAX TIME
      [[nodiscard]] GLfloat max_time() const;
      /// @}

      /// @{ -------------------------------------------------- GET SPEED FACTOR
      [[nodiscard]] GLfloat speed_factor() const;
      /// @}

      /// @{ -------------------------------------------------- GET TARGET FPS
      [[nodiscard]] GLfloat target_fps() const;
      /// @}

      /// @{ -------------------------------------------------- GET UPDATE INTERVAL
      [[nodiscard]] GLfloat update_interval_in_ms() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<GLfloat>& signal_current_time_changed();
      [[nodiscard]] const bk::Signal<GLfloat>& signal_current_time_changed() const;

      [[nodiscard]] bk::Signal<GLfloat>& signal_max_time_changed();
      [[nodiscard]] const bk::Signal<GLfloat>& signal_max_time_changed() const;

      [[nodiscard]] bk::Signal<bool>& signal_enabled_changed();
      [[nodiscard]] const bk::Signal<bool>& signal_enabled_changed() const;

      [[nodiscard]] bk::Signal<bool>& signal_paused_changed();
      [[nodiscard]] const bk::Signal<bool>& signal_paused_changed() const;

      [[nodiscard]] bk::Signal<>& signal_speed_settings_changed();
      [[nodiscard]] const bk::Signal<>& signal_speed_settings_changed() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET ENABLED
      void set_enabled(bool b);
      void enable();
      void disable();
      /// @}

      /// @{ -------------------------------------------------- SET PAUSED
      void set_paused(bool b);
      void pause();
      void unpause();
      /// @}

      /// @{ -------------------------------------------------- SET CURRENT TIME
      void set_current_time(GLfloat ms);
      /// @}

      /// @{ -------------------------------------------------- SET MAX TIME
      void set_max_time(GLfloat ms);
      /// @}

      /// @{ -------------------------------------------------- SET PARAMETERS
      void set_parameters(GLfloat speed_factor, GLfloat target_fps);
      /// @}

      /// @{ -------------------------------------------------- SET SIGNALS MUTED
      void set_signals_muted(bool setMuted);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CONNECT SIGNALS
      void connect_signals(std::shared_ptr<details::AbstractRenderable>& r);
      void connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r);
      /// @}

      /// @{ -------------------------------------------------- ADVANCE CURRENT TIME
      //! increase current time by internally determined delta t
      void advance_current_time();
      /// @}

      /// @{ -------------------------------------------------- UPDATE
      //! update function called in render loop
      void update();
      /// @}
  }; // class Animator
} // namespace bk

#endif //BKGL_ANIMATOR_H
