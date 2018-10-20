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

#ifndef BKGL_SCREENQUAD_H
#define BKGL_SCREENQUAD_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk
{
  // -------------------- forward declaration
  class VBO;
  class VAO;
  // -------------------- forward declaration END

  class BKGL_EXPORT ScreenQuad : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ScreenQuad;
      using base_type = details::AbstractRenderable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      ScreenQuad();
      #else
      ScreenQuad(qt_gl_functions* gl);
      #endif
      ScreenQuad(const self_type&) = delete;
      ScreenQuad(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~ScreenQuad();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      /// @{ -------------------------------------------------- GET BUFFER
      [[nodiscard]] const VBO& vbo() const;
      [[nodiscard]] const VAO& vao() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
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

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_impl() override;
    public:
      /// @}
  }; // class ScreenQuad
} // namespace bk

#endif //BKGL_SCREENQUAD_H


