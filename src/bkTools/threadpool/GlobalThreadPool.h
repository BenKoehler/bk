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

#ifndef BKTOOLS_GLOBALTHREADPOOL_H
#define BKTOOLS_GLOBALTHREADPOOL_H

#include <bk/Singleton>
#include <bkTools/threadpool/ThreadPool.h>
#include <bkTools/lib/bkTools_export.h>

#define bk_threadpool bk::GlobalThreadPool::get_instance()

namespace bk
{
  class BKTOOLS_EXPORT GlobalThreadPool : public bk::Singleton<ThreadPool>
  {
      using self_type = GlobalThreadPool;

      GlobalThreadPool() = delete;
      GlobalThreadPool(const self_type&) = delete;
      GlobalThreadPool(self_type&&) = delete;

      ~GlobalThreadPool() = delete;

      self_type& operator=(const self_type&) = delete;
      self_type& operator=(self_type&&) noexcept = delete;
  }; // class GlobalThreadPool

} // namespace bk

#endif //BKPROGRESS_GLOBALTHREADPOOL_H
