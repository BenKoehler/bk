/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BKTOOLS_COLORBARRGBA_H
#define BKTOOLS_COLORBARRGBA_H

#include <bkTools/color/ColorRGBA.h>

#include <bkTools/color/ColorBar.h>
#include <bkTools/lib/bkTools_export.h>

namespace bk
{
  class BKTOOLS_EXPORT ColorBarRGBA : public ColorBar<ColorRGBA>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ColorBarRGBA;
      using base_type = ColorBar<ColorRGBA>;

    public:
      using color_type = ColorRGBA;


      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ColorBarRGBA();
      ColorBarRGBA(const self_type&);
      ColorBarRGBA(self_type&&) noexcept;

      template<typename... Colors_>
      ColorBarRGBA(Colors_&& ... colors)
          : base_type(std::forward<Colors_>(colors)...)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ColorBarRGBA();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}
  }; // class ColorBarRGBA
} // namespace bk

#endif //BKTOOLS_COLORBARRGBA_H


