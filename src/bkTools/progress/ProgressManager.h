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

#ifndef BKTOOLS_PROGRESSMANAGER_H
#define BKTOOLS_PROGRESSMANAGER_H

#include <memory>
#include <string>

#include <bkTools/lib/bkTools_export.h>

namespace bk
{
  // -------------------- forward declaration
  template<typename... Args> class Signal;
  class Progress;
  // -------------------- forward declaration END

  class BKTOOLS_EXPORT ProgressManager
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ProgressManager;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ProgressManager();
      ProgressManager(const self_type&) = delete;
      ProgressManager(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ProgressManager();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM TASKS
    private:
      [[nodiscard]] unsigned int _num_tasks() const;
    public:
      [[nodiscard]] unsigned int num_tasks() const;
      /// @}

      /// @{ -------------------------------------------------- HAS TASKS
    private:
      [[nodiscard]] bool _has_tasks() const;
    public:
      [[nodiscard]] bool has_tasks() const;
      /// @}

      /// @{ -------------------------------------------------- GET TASK
      [[nodiscard]] Progress* task(unsigned int id);
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] const Signal<unsigned int /*id*/, double /*max*/, double /*current*/, std::string_view /*description*/>& signal_task_added() const;
      [[nodiscard]] Signal<unsigned int /*id*/, double /*max*/, double /*current*/, std::string_view /*description*/>& signal_task_added();

      [[nodiscard]] const Signal<unsigned int>& signal_task_finished() const;
      [[nodiscard]] Signal<unsigned int>& signal_task_finished();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- ADD TASK
      /// @param p task to add (move)
      /// @return reference to the task
      [[nodiscard]] Progress& add_task(Progress&& p);
      [[nodiscard]] Progress& add_task();
      /// @}

      /// @{ -------------------------------------------------- EMPLACE TASK
      [[nodiscard]] Progress& emplace_task(double max = 100, const std::string& description = "");
      /// @}

      /// @{ -------------------------------------------------- REMOVE TASK
      void remove_task(unsigned int id);
      /// @}

      /// @{ -------------------------------------------------- CLEAR OLD TASKS
    private:
      void _clear_old_tasks();
      void clear_old_tasks();
    public:
      /// @}
  }; // class ProgressManager
} // namespace bk

#endif //BKTOOLS_PROGRESSMANAGER_H