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

#ifndef BKTOOLS_CLOCK_H
#define BKTOOLS_CLOCK_H

#include <chrono>
#include <ostream>
#include <tuple>

#include "../lib/bkTools_export.h"

namespace bk
{
  class BKTOOLS_EXPORT Clock
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Clock;
    public:
      using clock_type = std::chrono::high_resolution_clock;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      std::chrono::system_clock::time_point _start;
      std::chrono::system_clock::time_point _stop;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      Clock();
      Clock(const self_type&);
      Clock(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~Clock();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TIME IN [UNIT]
      [[nodiscard]] unsigned int time_in_nano_sec() const;
      [[nodiscard]] unsigned int time_in_micro_sec() const;
      [[nodiscard]] unsigned int time_in_milli_sec() const;
      [[nodiscard]] unsigned int time_in_sec() const;
      [[nodiscard]] unsigned int time_in_min() const;
      [[nodiscard]] unsigned int time_in_h() const;
      [[nodiscard]] std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> time_in_h_min_s_ms() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: TIME DIFFERENCE
    private:
      [[nodiscard]] std::chrono::system_clock::duration _time_diff() const;
    public:
      /// @}

      /// @{ -------------------------------------------------- START / STOP
      void start();
      void stop();
      /// @}
  }; // class Clock

  //====================================================================================================
  //===== EXTERNAL OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR <<
  [[maybe_unused]] std::ostream& operator<<(std::ostream& o, const bk::Clock& c);
  /// @}
} // namespace bk

#endif //BKTOOLS_CLOCK_H


