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

#ifndef BK_CPIMPL_H
#define BK_CPIMPL_H

#include <memory>
#include <utility>

namespace bk
{
  template<typename T> class cpimpl
  {
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::unique_ptr<T> _p;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      cpimpl()
          : _p(std::make_unique<T>())
      { /* do nothing */ }

      cpimpl(const cpimpl& other)
          : _p(std::make_unique<T>(*other._p.get()))
      { /* do nothing */ }

      cpimpl(cpimpl&&) noexcept = default;

      template<typename... Args>
      cpimpl(Args&&... args)
          : _p(std::make_unique<T>(std::forward<Args>(args)...))
      { /* do nothing */ }

      ~cpimpl() = default;

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] T* operator->()
      { return _p.get(); }

      [[nodiscard]] const T* operator->() const
      { return _p.get(); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      [[maybe_unused]] cpimpl& operator=(const cpimpl& other)
      {
          _p.reset(nullptr);
          _p = std::make_unique<T>(*other._p.get());
          return *this;
      }

      [[maybe_unused]] cpimpl& operator=(cpimpl&&) noexcept = default;
  }; // class cpimpl
} // namespace bk

#endif //BK_CPIMPL_H
