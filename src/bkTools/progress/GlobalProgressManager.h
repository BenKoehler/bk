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

#ifndef BKTOOLS_GLOBALPROGRESSMANAGER_H
#define BKTOOLS_GLOBALPROGRESSMANAGER_H

#include <bkTools/lib/bkTools_export.h>
#include <bkTools/progress/ProgressManager.h>
#include <bk/Singleton>

#define bk_progress bk::GlobalProgressManager::get_instance()

namespace bk
{
  class BKTOOLS_EXPORT GlobalProgressManager : public bk::Singleton<ProgressManager>
  {
      using self_type = GlobalProgressManager;

      GlobalProgressManager() = delete;
      GlobalProgressManager(const self_type&) = delete;
      GlobalProgressManager(self_type&&) = delete;

      ~GlobalProgressManager() = delete;

      self_type& operator=(const self_type&) = delete;
      self_type& operator=(self_type&&) noexcept = delete;
  }; // class GlobalProgressManager

} // namespace bk

#endif //BKPROGRESS_GLOBALPROGRESSMANAGER_H
