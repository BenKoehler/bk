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

#ifndef BKTOOLS_COLOR_RGBA_H
#define BKTOOLS_COLOR_RGBA_H

#include <bkTools/color/AbstractColor.h>
#include <bkTools/color/ColorFactory.h>
#include <bkTools/lib/bkTools_export.h>

namespace bk
{
  class BKTOOLS_EXPORT ColorRGBA
      : public details::AbstractColor,
        public details::ColorFactory<ColorRGBA>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ColorRGBA;
      using base_type = details::AbstractColor;

    public:
      using value_type = details::AbstractColor::value_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ColorRGBA();
      ColorRGBA(const self_type&);
      ColorRGBA(self_type&&);
      ColorRGBA(value_type r, value_type g, value_type b, value_type a = 1);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~ColorRGBA();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET DEFAULT COLOR
      virtual void set_default_red(bool keep_current_alpha = false) override;
      virtual void set_default_light_red(bool keep_current_alpha = false) override;
      virtual void set_default_dark_red(bool keep_current_alpha = false) override;
      virtual void set_default_green(bool keep_current_alpha = false) override;
      virtual void set_default_light_green(bool keep_current_alpha = false) override;
      virtual void set_default_dark_green(bool keep_current_alpha = false) override;
      virtual void set_default_blue(bool keep_current_alpha = false) override;
      virtual void set_default_light_blue(bool keep_current_alpha = false) override;
      virtual void set_default_dark_blue(bool keep_current_alpha = false) override;
      virtual void set_default_magenta(bool keep_current_alpha = false) override;
      virtual void set_default_pink(bool keep_current_alpha = false) override;
      virtual void set_default_purple(bool keep_current_alpha = false) override;
      virtual void set_default_yellow(bool keep_current_alpha = false) override;
      virtual void set_default_orange(bool keep_current_alpha = false) override;
      virtual void set_default_white(bool keep_current_alpha = false) override;
      virtual void set_default_gray(bool keep_current_alpha = false) override;
      virtual void set_default_light_gray(bool keep_current_alpha = false) override;
      virtual void set_default_dark_gray(bool keep_current_alpha = false) override;
      virtual void set_default_black(bool keep_current_alpha = false) override;
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      [[maybe_unused]] auto operator=(std::initializer_list<double> values) -> self_type&;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET RGB COMPONENT
      [[nodiscard]] auto r() -> value_type&;
      [[nodiscard]] auto r() const -> value_type;

      [[nodiscard]] auto g() -> value_type&;
      [[nodiscard]] auto g() const -> value_type;

      [[nodiscard]] auto b() -> value_type&;
      [[nodiscard]] auto b() const -> value_type;
      /// @}

  }; // class ColorRGBA

  //====================================================================================================
  //===== OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR +
  [[nodiscard]] ColorRGBA operator+(const ColorRGBA& c, double x);
  [[nodiscard]] ColorRGBA operator+(ColorRGBA&& c, double x);
  [[nodiscard]] ColorRGBA operator+(double x, const ColorRGBA& c);
  [[nodiscard]] ColorRGBA operator+(double x, ColorRGBA&& c);
  [[nodiscard]] ColorRGBA operator+(const ColorRGBA& a, const ColorRGBA& b);
  [[nodiscard]] ColorRGBA operator+(ColorRGBA&& a, const ColorRGBA& b);
  [[nodiscard]] ColorRGBA operator+(const ColorRGBA& a, ColorRGBA&& b);
  [[nodiscard]] ColorRGBA operator+(ColorRGBA&& a, ColorRGBA&& b);
  /// @}

  /// @{ -------------------------------------------------- OPERATOR -
  [[nodiscard]] ColorRGBA operator-(const ColorRGBA& c, double x);
  [[nodiscard]] ColorRGBA operator-(ColorRGBA&& c, double x);
  [[nodiscard]] ColorRGBA operator-(const ColorRGBA& a, const ColorRGBA& b);
  [[nodiscard]] ColorRGBA operator-(ColorRGBA&& a, const ColorRGBA& b);
  [[nodiscard]] ColorRGBA operator-(const ColorRGBA& a, ColorRGBA&& b);
  [[nodiscard]] ColorRGBA operator-(ColorRGBA&& a, ColorRGBA&& b);
  /// @}

  /// @{ -------------------------------------------------- OPERATOR *
  [[nodiscard]] ColorRGBA operator*(const ColorRGBA& c, double x);
  [[nodiscard]] ColorRGBA operator*(ColorRGBA&& c, double x);
  [[nodiscard]] ColorRGBA operator*(double x, const ColorRGBA& c);
  [[nodiscard]] ColorRGBA operator*(double x, ColorRGBA&& c);
  /// @}

  /// @{ -------------------------------------------------- OPERATOR /
  [[nodiscard]] ColorRGBA operator/(const ColorRGBA& c, double x);
  [[nodiscard]] ColorRGBA operator/(ColorRGBA&& c, double x);
  /// @}

} // namespace bk

#endif //BKTOOLS_COLOR_RGBA_H