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

#ifndef BKGL_SUPERSAMPLER_H
#define BKGL_SUPERSAMPLER_H

#include <memory>

#include "gl_definitions.h"
#include "lib/export.h"
#include "AbstractObject.h"

namespace bk
{
  // -------------------- forward declaration
  class ScreenQuad;
  class WindowGeometry;
  template<typename...> class Signal;

  namespace details
  {
    class AbstractRenderable;
    class AbstractSceneRenderable;
  } // namespace details
  // -------------------- forward declaration END

  class BK_GL_EXPORT SuperSampler : public details::AbstractObject
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SuperSampler;
      using base_type = details::AbstractObject;
    public:

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      SuperSampler();
      #else
      SuperSampler(bk::qt_gl_functions* gl);
      #endif
      SuperSampler(const self_type&) = delete;
      SuperSampler(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SuperSampler();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET SSAA FACTOR
      [[nodiscard]] GLuint factor() const;
      /// @}

      /// @{ -------------------------------------------------- GET WINDOW SIZE
      [[nodiscard]] WindowGeometry& window();
      [[nodiscard]] const WindowGeometry& window() const;
      [[nodiscard]] GLuint width() const;
      [[nodiscard]] GLuint height() const;
      /// @}

      /// @{ -------------------------------------------------- GET WINDOW SIZE UPSAMPLED
      //! the factor is multiplied to each window dimension 
      [[nodiscard]]  WindowGeometry window_upsampled() const;
      [[nodiscard]] GLuint width_upsampled() const;
      [[nodiscard]] GLuint height_upsampled() const;
      /// @}
      
      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<GLuint>& signal_factor_changed();
      [[nodiscard]] const bk::Signal<GLuint>& signal_factor_changed() const;

      [[nodiscard]] bk::Signal<GLuint, GLuint>& signal_window_size_changed();
      [[nodiscard]] const bk::Signal<GLuint, GLuint>& signal_window_size_changed() const;
      /// @}

      /// @{ -------------------------------------------------- GET TEXTURE UNIT NUMBERS
      [[nodiscard]] static constexpr GLuint texture_unit_number_color_tex() noexcept
      { return 1U; }

      [[nodiscard]] static constexpr GLuint texture_unit_number_depth_tex() noexcept
      { return 2U; }
      /// @}

      /// @{ -------------------------------------------------- GET ID OF FBO
      [[nodiscard]] GLuint fbo_id() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET FACTOR
      //! each dimension size is multiplied by this factor
      /*!
       * @param x values between 1 and 4 are allowed 
       */
      void set_factor(GLuint x);
      /// @}

      /// @{ -------------------------------------------------- SET WINDOW SIZE
      void set_window_size(const WindowGeometry& window);
      void set_window_size(GLuint w, GLuint h);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- EMIT SIGNALS
    private:
      void emit_signal_window_size_changed() const;
    public:
      /// @}
      
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

      /// @{ -------------------------------------------------- BIND/RELEASE FBO
      void bind_fbo();
      void release_fbo();
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw_init();
      void draw_finish();
      /// @}
  }; // class SuperSampler
} // namespace bk

#endif //BKGL_SUPERSAMPLER_H
