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

#ifndef BKGL_ABSTRACTBACKGROUND_H
#define BKGL_ABSTRACTBACKGROUND_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>

// -------------------- forward declaration
namespace bk
{
  class VBO;
  class VAO;
  class Shader;
} // namespace bk
// -------------------- forward declaration END

namespace bk::details
{
  class BKGL_EXPORT AbstractBackground : public AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractBackground;
      using base_type = AbstractRenderable;

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
      AbstractBackground();
      #else
      AbstractBackground(bk::qt_gl_functions* gl);
      #endif
      AbstractBackground(const self_type& other) = delete;
      AbstractBackground(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractBackground();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- GET VBO
      [[nodiscard]] VBO& vbo();
      [[nodiscard]] const VBO& vbo() const;
      /// @}

      /// @{ -------------------------------------------------- GET VAO
      [[nodiscard]] VAO& vao();
      [[nodiscard]] const VAO& vao() const;
      /// @}

      /// @{ -------------------------------------------------- GET SHADER
      [[nodiscard]] Shader& shader();
      [[nodiscard]] const Shader& shader() const;
      /// @}
    public:

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    protected:
      void clear_shader();
      void clear_buffers();
    public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
    protected:
      [[maybe_unused]] bool init_shader();
      virtual void init_vbo_vao() = 0;
    public:
      void init();
      /// @}

      /// @{ -------------------------------------------------- UPDATE
    protected:
      virtual void update_colors() = 0;
    public:
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_impl() override;
    public:
      /// @}
  }; // class AbstractBackground
} // namespace bk::details

#endif //BKGL_ABSTRACTBACKGROUND_H


