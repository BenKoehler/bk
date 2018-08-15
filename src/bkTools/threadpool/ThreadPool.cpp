/*
 * This ThreadPool implementation is based on the implementation
 * by Jakob Progsch and Václav Zeman, Copyright (c) 2012
 * (see ZLIB LICENSE below)
 *
 * https://github.com/progschj/ThreadPool
 *
 * ------------------------------------------------------
 *
 * Copyright (c) 2012 Jakob Progsch, Václav Zeman
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */

#include <bkTools/threadpool/ThreadPool.h>

#include <algorithm>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTORS
  //====================================================================================================
  /// @{ -------------------------------------------------- CONSTRUCTORS
  ThreadPool::ThreadPool(unsigned int n)
      : N(std::max(n, 1U)),
        _tasks_left(0),
        _stop(false)
  {
      for (unsigned int i = 0; i < N; ++i)
      {
          _threads.emplace_back([this]
                                {
                                    for (;;)
                                    {
                                        std::function<void()> task;

                                        {
                                            std::unique_lock<std::mutex> lock(_queue_mutex);

                                            _condition.wait(lock, [this]
                                            { return _stop || !_tasks.empty(); });

                                            if (_stop && _tasks.empty())
                                            { return; }

                                            task = std::move(_tasks.front());
                                            _tasks.pop();
                                        }

                                        task();
                                        --_tasks_left;
                                    }
                                });
      }
  }
  /// @}

  /// @{ -------------------------------------------------- DESTRUCTOR
  ThreadPool::~ThreadPool()
  {
      {
          std::unique_lock<std::mutex> lock(_queue_mutex);
          _stop = true;
      }
      _condition.notify_all();

      for (auto& t : _threads)
      {
          if (t.joinable())
          { t.join(); }
      }
  }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM THREADS
  unsigned int ThreadPool::num_threads() noexcept
  { return N; }
  /// @}

  /// @{ -------------------------------------------------- GET NUM REMAINING TASKS
  unsigned int ThreadPool::num_tasks_remaining()
  { return _tasks_left.load(); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- WAIT FOR TASKS
  void ThreadPool::wait_for_all_tasks_to_finish()
  {
      while (_tasks_left != 0)
      { /* wait for task completions */ }
  }
  /// @}
} // namespace bk


