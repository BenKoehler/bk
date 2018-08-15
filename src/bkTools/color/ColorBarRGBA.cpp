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

#include <bkTools/color/ColorBarRGBA.h>

namespace bk
{
  //====================================================================================================  CONSTRUCTORS & DESTRUCTOR
  /// @{ -------------------------------------------------- CTOR
  ColorBarRGBA::ColorBarRGBA() = default;
  ColorBarRGBA::ColorBarRGBA(const self_type&) = default;
  ColorBarRGBA::ColorBarRGBA(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ColorBarRGBA::~ColorBarRGBA() = default;
  /// @}

  //====================================================================================================  SETTER
  /// @{ -------------------------------------------------- OPERATOR =
  auto ColorBarRGBA::operator=(const self_type&) -> self_type& = default;
  auto ColorBarRGBA::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}
} // namespace bk


