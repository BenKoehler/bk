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

#ifndef BKGL_ABSTRACTRENDERABLE_H
#define BKGL_ABSTRACTRENDERABLE_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/AbstractObject.h>
#include <bkGL/EKey.h>
#include <bkGL/EMouseButton.h>

namespace bk
{
  // ------ forward declaration -------
  template<typename... Args> class Signal;
  // ------ forward declaration -------
} // namespace bk

namespace bk::details
{
  class BKGL_EXPORT AbstractRenderable : public AbstractObject
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractRenderable;
      using base_type = AbstractObject;

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
      #ifndef BK_LIB_QT_AVAILABLE

      AbstractRenderable();
      #else
      AbstractRenderable(bk::qt_gl_functions* gl);
      #endif
      AbstractRenderable(const self_type& other) = delete;
      AbstractRenderable(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractRenderable();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET SETTINGS
      [[nodiscard]] bool oit_is_available() const;
      [[nodiscard]] bool animation_is_enabled() const;
      [[nodiscard]] bool modelview_matrix_changed() const;
      /// @}

      /// @{ -------------------------------------------------- GET VISIBILITY
      [[nodiscard]] bool is_visible() const;
      [[nodiscard]] bool is_hidden() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<>& signal_update_required();
      [[nodiscard]] bk::Signal<>& signal_bind_default_fbo();
      [[nodiscard]] bk::Signal<>& signal_bind_default_fbo_as_read();
      [[nodiscard]] bk::Signal<>& signal_bind_default_fbo_as_draw();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET SETTINGS
      void set_oit_available(bool b);
      void set_animation_is_enabled(bool b);
      void set_modelview_matrix_changed(bool b);
      void set_new_modelview_matrix(const ColMat4<GLfloat>& m);
      void set_new_projection_matrix(const ColMat4<GLfloat>& p);
      void set_animation_time(double t);
      /// @}

      /// @{ -------------------------------------------------- SET VISIBILITY
      void set_hidden(bool b = true);
      void set_visible(bool b = true);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- UPDATE SIGNAL
    protected:
      void emit_signal_update_required() const;
      void emit_signal_bind_default_fbo() const;
      void emit_signal_bind_default_fbo_as_read() const;
      void emit_signal_bind_default_fbo_as_draw() const;
    public:
      /// @}

      /// @{ -------------------------------------------------- FORWARD CONNECTIONS OF OTHER RENDERABLE
      void forward_signals(AbstractRenderable* r);

    protected:
      virtual void _forward_signals(AbstractRenderable*);
    public:
      /// @}

      /// @{ -------------------------------------------------- FORWARD SETTINGS TO OTHER RENDERARLE
      void forward_settings(AbstractRenderable* r);

    protected:
      virtual void _forward_settings(AbstractRenderable*);
    public:
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h);
      virtual void on_oit_enabled(bool b);
      virtual void on_animation_enabled(bool b);
      virtual void on_modelview_matrix_changed(bool b);
      virtual void on_new_modelview_matrix(const ColMat4<GLfloat>& m);
      virtual void on_new_projection_matrix(const ColMat4<GLfloat>& p);
      virtual void on_visible_changed(bool b);
      virtual void on_mouse_pos_changed(GLint x, GLint y);
      virtual void on_mouse_button_pressed(MouseButton_ btn);
      virtual void on_mouse_button_released(MouseButton_ btn);
      virtual void on_key_pressed(Key_ k);
      virtual void on_key_released(Key_ k);
      virtual void on_mouse_wheel_up();
      virtual void on_mouse_wheel_down();
      virtual void on_ssaa_factor_changed(GLint ssaa_factor);
      virtual void on_animation_time_changed(GLfloat t);
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw();
    protected:
      virtual void draw_impl()=0;
    public:
      /// @}
  }; // class AbstractRenderable
} // namespace bk::details

#endif //BKGL_ABSTRACTRENDERABLE_H


