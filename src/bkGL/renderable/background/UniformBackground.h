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

#ifndef BKGL_UNIFORMBACKGROUND_H
#define BKGL_UNIFORMBACKGROUND_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/background/AbstractBackground.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  class BKGL_EXPORT UniformBackground : public details::AbstractBackground
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UniformBackground;
      using base_type = details::AbstractBackground;
    public:
      using color_type = ColorRGBA;

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
      UniformBackground();
      #else
      UniformBackground(bk::qt_gl_functions* gl);
      #endif
      UniformBackground(const self_type&) = delete;
      UniformBackground(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~UniformBackground();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] auto color() const -> const color_type&;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
      void set_color(const color_type& col);
      void set_color(color_type&& col);
      void set_color(double r, double g, double b, double a = 1);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init_vbo_vao() override;
      /// @}

      /// @{ -------------------------------------------------- UPDATE
    protected:
      void update_colors() override;
    public:
      /// @}
  }; // class UniformBackground
} // namespace bk

#endif //BKGL_UNIFORMBACKGROUND_H


