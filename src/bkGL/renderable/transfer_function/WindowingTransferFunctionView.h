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

#ifndef BKGL_WINDOWINGTRANSFERFUNCTIONVIEW_H
#define BKGL_WINDOWINGTRANSFERFUNCTIONVIEW_H

#include <memory>

#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // ------ forward declaration -------
  class WindowingTransferFunction;
  // ------ forward declaration -------

  class BKGL_EXPORT WindowingTransferFunctionView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = WindowingTransferFunctionView;
      using base_type = details::AbstractRenderable;

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
      #ifndef BK_LIB_QT_AVAILABLE

      WindowingTransferFunctionView(WindowingTransferFunction* tf = nullptr);
      #else
      WindowingTransferFunctionView(WindowingTransferFunction* tf, bk::qt_gl_functions* gl);
      #endif
      WindowingTransferFunctionView(const self_type& other) = delete;
      WindowingTransferFunctionView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~WindowingTransferFunctionView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TRANSFER FUNCTION
      [[nodiscard]] const WindowingTransferFunction* transfer_function() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
      void set_transfer_function(WindowingTransferFunction* tf);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_buffers();
  public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
    private:
      bool init_shader();
    public:
      void init(WindowingTransferFunction* tf = nullptr);
      /// @}

      /// @{ -------------------------------------------------- UPDATE VBO
      void update_tf();
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_changed(bool) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton btn) override;
      virtual void on_mouse_button_released(MouseButton btn) override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_impl() final;
    public:
      /// @}
  }; // class WindowingTransferFunctionView
} // namespace bk

#endif //BKGL_WINDOWINGTRANSFERFUNCTIONVIEW_H


