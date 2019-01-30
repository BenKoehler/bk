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

#ifndef BKTOOLS_PROGRESS_H
#define BKTOOLS_PROGRESS_H

#include <string>
#include <string_view>

#include <bk/CopyablePIMPL>
#include <bkTools/lib/bkTools_export.h>

namespace bk
{
  // -------------------- forward declaration
  template<typename... Args> class Signal;
  // -------------------- forward declaration END

  class BKTOOLS_EXPORT Progress
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Progress;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      Progress();
      Progress(const self_type&) = delete;
      Progress(self_type&&) noexcept;
      Progress(double max, std::string_view description = "");
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Progress();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET UNIQUE ID
      [[nodiscard]] unsigned int id() const;
      /// @}

      /// @{ -------------------------------------------------- GET DESCRIPTION
      [[nodiscard]] const std::string& description() const;
      [[nodiscard]] Signal<std::string>& signal_description_changed();
      /// @}

      /// @{ -------------------------------------------------- GET MAX
      [[nodiscard]] double max() const;
      [[nodiscard]] Signal<double>& signal_max_changed();
      /// @}

      /// @{ -------------------------------------------------- GET CURRENT
      [[nodiscard]] double current() const;
      [[nodiscard]] Signal<double>& signal_current_changed();
      /// @}

      /// @{ -------------------------------------------------- IS FINISHED
      [[nodiscard]] bool finished() const;
      [[nodiscard]] Signal<unsigned int>& signal_finished();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET MAX
      void set_max(double m);
      /// @}

      /// @{ -------------------------------------------------- SET CURRENT
      void set_current(double c);
      /// @}

      /// @{ -------------------------------------------------- SET FINISHED
      void set_finished();
      /// @}

      /// @{ -------------------------------------------------- ENABLE SIGNALS
      //! if disabled, no signals will be sent
      void set_signals_enabled(bool enable);
      void enable_signals();
      void disable_signals();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INCREMENT
      void increment(double step);
      /// @}

      /// @{ -------------------------------------------------- CLEAR SLOTS
      void clear_slots();
      /// @}
  }; // class Progress
} // namespace bk

#endif //BKTOOLS_PROGRESS_H


