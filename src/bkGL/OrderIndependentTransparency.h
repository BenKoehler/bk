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

#ifndef BKGL_ORDERINDEPENDENTTRANSPARENCY_H
#define BKGL_ORDERINDEPENDENTTRANSPARENCY_H

#include <memory>
#include <string>

#include <bkGL/AbstractObject.h>
#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class ScreenQuad;
  class Shader;
  class SSBO;
  template<typename...> class Signal;

  namespace details
  {
    class AbstractRenderable;
    class AbstractSceneRenderable;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT OrderIndependentTransparency : public details::AbstractObject
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = OrderIndependentTransparency;
      using base_type = details::AbstractObject;
    public:

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;
      std::unique_ptr <Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      OrderIndependentTransparency();
      #else
      OrderIndependentTransparency(bk::qt_gl_functions* gl);
      #endif
      OrderIndependentTransparency(const self_type&) = delete;
      OrderIndependentTransparency(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~OrderIndependentTransparency();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS ENABLED
      [[nodiscard]] bool is_enabled() const;
      /// @}

      /// @{ -------------------------------------------------- GET WINDOW SIZE
      [[nodiscard]] unsigned int window_width() const;
      [[nodiscard]] unsigned int window_height() const;
      /// @}

      /// @{ -------------------------------------------------- GET MAX FRAGMENTS PER PIXEL
      [[nodiscard]] unsigned int max_fragments_per_pixel() const;
      /// @}

      /// @{ -------------------------------------------------- GET BUFFER
      [[nodiscard]] const SSBO& ssbo_fragments() const;
      [[nodiscard]] const SSBO& ssbo_linked_list() const;
      /// @}

      /// @{ -------------------------------------------------- GET DEFAULT BUFFER BASE IDS
      [[nodiscard]] static constexpr GLuint buffer_base_atomic_counter() noexcept
      { return 4U; }

      [[nodiscard]] static constexpr GLuint buffer_base_ssbo_fragments() noexcept
      { return 5U; }

      [[nodiscard]] static constexpr GLuint buffer_base_ssbo_linkedlist() noexcept
      { return 6U; }
      /// @}

      /// @{ -------------------------------------------------- GET DEFAULT BUFFER NAMES
      [[nodiscard]] static constexpr const char* buffer_name_atomic_counter() noexcept
      { return "OIT_Counter"; }

      [[nodiscard]] static constexpr const char* buffer_name_ssbo_fragments() noexcept
      { return "OIT_Fragments"; }

      [[nodiscard]] static constexpr const char* buffer_name_ssbo_linkedlist() noexcept
      { return "OIT_LinkedList"; }
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<bool>& signal_enabled_changed();
      const bk::Signal<bool>& signal_enabled_changed() const;
      /// @}

      /// @{ -------------------------------------------------- GET SHADER TEXT
      [[nodiscard]] static constexpr GLfloat minimum_valid_opacity() noexcept
      { return 0.025; }

      [[nodiscard]] static std::string shader_definition_text();
      [[nodiscard]] static std::string shader_main_text();
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

      /// @{ -------------------------------------------------- SET ENABLED
      void set_enabled(bool b);
      void enable();
      void disable();
      /// @}

      /// @{ -------------------------------------------------- SET WINDOW SIZE
      void set_window_width(unsigned int width);
      void set_window_height(unsigned int height);
      void set_window_size(unsigned int width, unsigned int height);
      /// @}

      /// @{ -------------------------------------------------- SET MAX FRAGMENTS PER PIXEL
      void set_max_fragments_per_pixel(unsigned int n);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init();
      void init_if_enabled();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      void clear();
      /// @}

      /// @{ -------------------------------------------------- CONNECT SIGNALS
      void connect_signals(std::shared_ptr<details::AbstractRenderable>& r);
      void connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r);
      /// @}

      /// @{ -------------------------------------------------- GL BLEND FUNCTION
      void blendFunc();
      /// @}

      /// @{ -------------------------------------------------- ON RESIZE
      void on_resize(unsigned int width, unsigned int height);
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw_init();
      void draw_finish();
      /// @}
  }; // class OrderIndependentTransparency
} // namespace bk

#endif //BKGL_ORDERINDEPENDENTTRANSPARENCY_H
