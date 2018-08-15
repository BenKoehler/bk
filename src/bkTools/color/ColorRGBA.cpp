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

#include <bkTools/color/ColorRGBA.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ColorRGBA::ColorRGBA() = default;
  ColorRGBA::ColorRGBA(const self_type&) = default;
  ColorRGBA::ColorRGBA(self_type&&) noexcept = default;

  ColorRGBA::ColorRGBA(value_type r, value_type g, value_type b, value_type a)
      : base_type(r, g, b, a)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ColorRGBA::~ColorRGBA()
  { /* do nothing */ }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET DEFAULT COLOR
  void ColorRGBA::set_default_red(bool keep_current_alpha)
  { this->set(1, 0, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_light_red(bool keep_current_alpha)
  { this->set(1, 0.5, 0.5, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_dark_red(bool keep_current_alpha)
  { this->set(0.5, 0, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_green(bool keep_current_alpha)
  { this->set(0, 1, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_light_green(bool keep_current_alpha)
  { this->set(0.5, 1, 0.5, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_dark_green(bool keep_current_alpha)
  { this->set(0, 0.5, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_blue(bool keep_current_alpha)
  { this->set(0, 0, 1, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_light_blue(bool keep_current_alpha)
  { this->set(0.5, 0.5, 1, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_dark_blue(bool keep_current_alpha)
  { this->set(0, 0, 0.5, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_magenta(bool keep_current_alpha)
  { this->set(0.75, 0.2, 1, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_pink(bool keep_current_alpha)
  { this->set(0.75, 0.2, 1, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_purple(bool keep_current_alpha)
  { this->set(0.5, 0, 0.6, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_yellow(bool keep_current_alpha)
  { this->set(1, 1, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_orange(bool keep_current_alpha)
  { this->set(1, 0.5, 0, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_white(bool keep_current_alpha)
  { this->set(1, 1, 1, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_gray(bool keep_current_alpha)
  { this->set(0.5, 0.5, 0.5, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_light_gray(bool keep_current_alpha)
  { this->set(0.75, 0.75, 0.75, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_dark_gray(bool keep_current_alpha)
  { this->set(0.3, 0.3, 0.3, keep_current_alpha ? (*this)[3] : 1); }

  void ColorRGBA::set_default_black(bool keep_current_alpha)
  { this->set(0, 0, 0, keep_current_alpha ? (*this)[3] : 1); }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto ColorRGBA::operator=(const self_type& other) -> self_type& = default;
  auto ColorRGBA::operator=(self_type&& other) noexcept -> self_type& = default;

  auto ColorRGBA::operator=(std::initializer_list<double> values) -> self_type&
  {
      base_type::operator=(values);
      return *this;
  }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET RGB COMPONENT
  auto ColorRGBA::r() -> value_type&
  { return (*this)[0]; }

  auto ColorRGBA::r() const -> value_type
  { return (*this)[0]; }

  auto ColorRGBA::g() -> value_type&
  { return (*this)[1]; }

  auto ColorRGBA::g() const -> value_type
  { return (*this)[1]; }

  auto ColorRGBA::b() -> value_type&
  { return (*this)[2]; }

  auto ColorRGBA::b() const -> value_type
  { return (*this)[2]; }
  /// @}

  //====================================================================================================
  //===== OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR +
  ColorRGBA operator+(const ColorRGBA& c, double x)
  {
      ColorRGBA res(c);
      res += x;
      return res;
  }

  ColorRGBA operator+(ColorRGBA&& c, double x)
  {
      c += x;
      return c;
  }

  ColorRGBA operator+(double x, const ColorRGBA& c)
  { return c + x; }

  ColorRGBA operator+(double x, ColorRGBA&& c)
  { return std::move(c) + x; }

  ColorRGBA operator+(const ColorRGBA& a, const ColorRGBA& b)
  {
      ColorRGBA res(a);
      res += b;
      return res;
  }

  ColorRGBA operator+(ColorRGBA&& a, const ColorRGBA& b)
  {
      a += b;
      return a;
  }

  ColorRGBA operator+(const ColorRGBA& a, ColorRGBA&& b)
  { return std::move(b) + a; }

  ColorRGBA operator+(ColorRGBA&& a, ColorRGBA&& b)
  {
      a += b;
      return a;
  }

  /// @}

  /// @{ -------------------------------------------------- OPERATOR -
  ColorRGBA operator-(const ColorRGBA& c, double x)
  {
      ColorRGBA res(c);
      res -= x;
      return res;
  }

  ColorRGBA operator-(ColorRGBA&& c, double x)
  {
      c -= x;
      return c;
  }

  ColorRGBA operator-(const ColorRGBA& a, const ColorRGBA& b)
  {
      ColorRGBA res(a);
      res -= b;
      return res;
  }

  ColorRGBA operator-(ColorRGBA&& a, const ColorRGBA& b)
  {
      a -= b;
      return a;
  }

  ColorRGBA operator-(const ColorRGBA& a, ColorRGBA&& b)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { b[i] = a[i] - b[i]; }

      return b;
  }

  ColorRGBA operator-(ColorRGBA&& a, ColorRGBA&& b)
  {
      a -= b;
      return a;
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR *
  ColorRGBA operator*(const ColorRGBA& c, double x)
  {
      ColorRGBA res(c);
      res *= x;
      return res;
  }

  ColorRGBA operator*(ColorRGBA&& c, double x)
  {
      c *= x;
      return c;
  }

  ColorRGBA operator*(double x, const ColorRGBA& c)
  { return c * x; }

  ColorRGBA operator*(double x, ColorRGBA&& c)
  { return std::move(c) * x; }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR /
  ColorRGBA operator/(const ColorRGBA& c, double x)
  {
      ColorRGBA res(c);
      res /= x;
      return res;
  }

  ColorRGBA operator/(ColorRGBA&& c, double x)
  {
      c /= x;
      return c;
  }
  /// @}

} // namespace bk


