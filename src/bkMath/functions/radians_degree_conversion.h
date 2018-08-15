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

#ifndef BKMATH_RADIANS_DEGREES_CONVERSION_H
#define BKMATH_RADIANS_DEGREES_CONVERSION_H

namespace bk
{
  /// @{ -------------------------------------------------- RADIANS TO DEGREES
  constexpr double radians_to_degrees(double rad)
  {
      //return 180.0 * rad / 3.14159265358979323846264338327950288419716939937510;
      return 57.2957795130823208768 * rad;
  }
  /// @}

  /// @{ -------------------------------------------------- DEGREES TO RADIANS
  constexpr double degrees_to_radians(double deg)
  {
      //return 3.14159265358979323846264338327950288419716939937510 * deg / 180.0;
      return 0.01745329251994329577 * deg;
  }
  /// @}
} // namespace bk

#endif //BKMATH_RADIANS_DEGREES_CONVERSION_H
