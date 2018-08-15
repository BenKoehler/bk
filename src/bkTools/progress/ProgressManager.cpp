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

#include <bkTools/progress/ProgressManager.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <vector>

#include <bkTools/progress/Progress.h>
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct ProgressManager::Impl
  {
      using task_container_type = std::map<unsigned int, Progress>;

      task_container_type tasks;
      mutable std::mutex tasks_mutex;
      Signal<unsigned int, double, double, std::string_view> s_task_added; // id, max, current, description
      Signal<unsigned int> s_task_finished; // id
      std::vector<unsigned int> tasks_to_remove;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ProgressManager::ProgressManager()
      : _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  ProgressManager::ProgressManager(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ProgressManager::~ProgressManager() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM TASKS
  unsigned int ProgressManager::_num_tasks() const
  {
      const unsigned int R = _pdata->tasks_to_remove.size();
      const unsigned int N = _pdata->tasks.size();
      return std::max(N, R) - R;
  }

  unsigned int ProgressManager::num_tasks() const
  {
      std::unique_lock<std::mutex> lock(_pdata->tasks_mutex);
      return _num_tasks();
  }
  /// @}

  /// @{ -------------------------------------------------- HAS TASKS
  bool ProgressManager::_has_tasks() const
  { return _num_tasks() != 0; }

  bool ProgressManager::has_tasks() const
  { return num_tasks() != 0; }
  /// @}

  /// @{ -------------------------------------------------- GET TASK
  Progress* ProgressManager::task(unsigned int id)
  {
      Impl::task_container_type::iterator it;

      {
          std::unique_lock<std::mutex> lock(_pdata->tasks_mutex);
          it = _pdata->tasks.find(id);
      }

      return (it != _pdata->tasks.end()) ? &it->second : nullptr;
  }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  const Signal<unsigned int /*id*/, double /*max*/, double /*current*/, std::string_view /*description*/>& ProgressManager::signal_task_added() const
  { return _pdata->s_task_added; }

  Signal<unsigned int /*id*/, double /*max*/, double /*current*/, std::string_view /*description*/>& ProgressManager::signal_task_added()
  { return _pdata->s_task_added; }

  const Signal<unsigned int>& ProgressManager::signal_task_finished() const
  { return _pdata->s_task_finished; }

  Signal<unsigned int>& ProgressManager::signal_task_finished()
  { return _pdata->s_task_finished; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto ProgressManager::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- ADD TASK
  Progress& ProgressManager::add_task(Progress&& p)
  {
      Impl::task_container_type::iterator it;

      {
          std::unique_lock<std::mutex> lock(_pdata->tasks_mutex);
          _clear_old_tasks();
          it = _pdata->tasks.insert(std::make_pair(p.id(), std::move(p))).first;
      }

      it->second.signal_finished().connect([&](unsigned int id)
                                           { remove_task(id); });

      _pdata->s_task_added.emit_signal(it->first, it->second.max(), it->second.current(), it->second.description());

      return it->second;
  }

  Progress& ProgressManager::add_task()
  { return add_task(Progress()); }
  /// @}

  /// @{ -------------------------------------------------- EMPLACE TASK
  Progress& ProgressManager::emplace_task(double max, const std::string& description)
  { return add_task(Progress(max, description)); }
  /// @}

  /// @{ -------------------------------------------------- REMOVE TASK
  void ProgressManager::remove_task(unsigned int id)
  {
      {
          std::unique_lock<std::mutex> lock(_pdata->tasks_mutex);
          _pdata->tasks_to_remove.push_back(id);
      }

      _pdata->s_task_finished.emit_signal(id);
  }
  /// @}

  /// @{ -------------------------------------------------- CLEAR OLD TASKS
  void ProgressManager::_clear_old_tasks()
  {
      for (auto it = _pdata->tasks_to_remove.begin(); it != _pdata->tasks_to_remove.end(); ++it)
      { _pdata->tasks.erase(*it); }

      _pdata->tasks_to_remove.clear();
  }

  void ProgressManager::clear_old_tasks()
  {
      std::unique_lock<std::mutex> lock(_pdata->tasks_mutex);
      _clear_old_tasks();
  }
  /// @}
} // namespace bk


