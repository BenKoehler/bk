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

#ifndef BKTOOLS_COLORFACTORY_H
#define BKTOOLS_COLORFACTORY_H

namespace bk::details
{
  template<typename ColorType> class ColorFactory
  {
    public:
      [[nodiscard]] static ColorType Red()
      {
          ColorType c;
          c.set_default_red();
          return c;
      }

      [[nodiscard]] static ColorType Light_Red()
      {
          ColorType c;
          c.set_default_light_red();
          return c;
      }

      [[nodiscard]] static ColorType Dark_Red()
      {
          ColorType c;
          c.set_default_dark_red();
          return c;
      }

      [[nodiscard]] static ColorType Green()
      {
          ColorType c;
          c.set_default_green();
          return c;
      }

      [[nodiscard]] static ColorType Light_Green()
      {
          ColorType c;
          c.set_default_light_green();
          return c;
      }

      [[nodiscard]] static ColorType Dark_Green()
      {
          ColorType c;
          c.set_default_dark_green();
          return c;
      }

      [[nodiscard]] static ColorType Blue()
      {
          ColorType c;
          c.set_default_blue();
          return c;
      }

      [[nodiscard]] static ColorType Light_Blue()
      {
          ColorType c;
          c.set_default_light_blue();
          return c;
      }

      [[nodiscard]] static ColorType Dark_Blue()
      {
          ColorType c;
          c.set_default_dark_blue();
          return c;
      }

      [[nodiscard]] static ColorType Magenta()
      {
          ColorType c;
          c.set_default_magenta();
          return c;
      }

      [[nodiscard]] static ColorType Pink()
      {
          ColorType c;
          c.set_default_pink();
          return c;
      }

      [[nodiscard]] static ColorType Purple()
      {
          ColorType c;
          c.set_default_purple();
          return c;
      }

      [[nodiscard]] static ColorType Yellow()
      {
          ColorType c;
          c.set_default_yellow();
          return c;
      }

      [[nodiscard]] static ColorType Orange()
      {
          ColorType c;
          c.set_default_orange();
          return c;
      }

      [[nodiscard]] static ColorType White()
      {
          ColorType c;
          c.set_default_white();
          return c;
      }

      [[nodiscard]] static ColorType Gray()
      {
          ColorType c;
          c.set_default_gray();
          return c;
      }

      [[nodiscard]] static ColorType Light_Gray()
      {
          ColorType c;
          c.set_default_light_gray();
          return c;
      }

      [[nodiscard]] static ColorType Dark_Gray()
      {
          ColorType c;
          c.set_default_dark_gray();
          return c;
      }

      [[nodiscard]] static ColorType Black()
      {
          ColorType c;
          c.set_default_black();
          return c;
      }
  }; // class ColorFactory
} // namespace bk::details



#endif //BKTOOLS_COLORFACTORY_H


