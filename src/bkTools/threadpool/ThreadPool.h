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

#pragma once

#ifndef BKTOOLS_THREADPOOL_H
#define BKTOOLS_THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <bkTools/lib/bkTools_export.h>

namespace bk
{
  class BKTOOLS_EXPORT ThreadPool
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ThreadPool;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      const unsigned int N;
      std::vector<std::thread> _threads;
      std::queue<std::function<void()>> _tasks;
      std::atomic_int _tasks_left;
      std::mutex _queue_mutex;
      std::condition_variable _condition;
      bool _stop;

    public:
      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTORS
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCTORS
      ThreadPool(unsigned int n = 8);
      ThreadPool(const self_type&) = delete;
      ThreadPool(self_type&&) = delete;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~ThreadPool();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM THREADS
      [[nodiscard]] unsigned int num_threads() noexcept;
      /// @}

      /// @{ -------------------------------------------------- GET NUM REMAINING TASKS
      [[nodiscard]] unsigned int num_tasks_remaining();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = delete;
      [[maybe_unused]] self_type& operator=(self_type&&) = delete;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- ENQUEUE
      template<typename F, typename... Args>
      [[maybe_unused]] auto enqueue(F&& f, Args&& ... args)
      {
          using return_type = typename std::result_of_t<F(Args...)>;
          auto task = std::make_shared<std::packaged_task<return_type() >>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
          auto res = task->get_future();

          {
              std::unique_lock<std::mutex> lock(_queue_mutex);

              if (_stop)
              { throw std::runtime_error("enqueue on stopped ThreadPool"); }

              _tasks.emplace([task]()
                             { (*task)(); });

              ++_tasks_left;
          }
          _condition.notify_one();

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- WAIT FOR TASKS
      void wait_for_all_tasks_to_finish();
      /// @}
  }; // class ThreadPool
} // namespace bk

#endif //BKTOOLS_THREADPOOL_H


