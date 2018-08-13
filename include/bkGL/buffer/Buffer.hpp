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

#ifndef BKGL_BUFFER_IMPL_H
#define BKGL_BUFFER_IMPL_H

#include <iterator>
#include <vector>

#include "Buffer.h"

namespace bk
{
  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  template<template<typename K, typename Alloc = std::allocator<K>> typename Container, typename T>
  bool Buffer::init(const Container<T>& data)
  {
      set_size_in_bytes(data.size() * sizeof(T));
      return init(static_cast<const GLvoid*>(data.data()));
  }

  template<template<typename K, typename Alloc = std::allocator<K>> typename Container, typename T>
  bool Buffer::init(Container<T>&& data)
  {
      set_size_in_bytes(data.size() * sizeof(T));
      return init(static_cast<const GLvoid*>(data.data()));
  }

  template<typename It>
  bool Buffer::init(It first, It last)
  {
      using it_value_type = typename std::iterator_traits<It>::value_type;
      return init(std::vector<it_value_type>(first, last));
  }
  /// @}

  /// @{ -------------------------------------------------- (UN)MAP
  template<typename T>
  T* Buffer::map(GLenum access)
  {
      if (!is_bound())
      { bind(); }

      return static_cast<T*>(BK_QT_GL glMapBuffer(target(), access));
  }

  template<typename T>
  T* Buffer::map_write_only()
  { return map<T>(GL_WRITE_ONLY); }

  template<typename T>
  T* Buffer::map_read_only()
  { return map<T>(GL_READ_ONLY); }

  template<typename T>
  T* Buffer::map_read_write()
  { return map<T>(GL_READ_WRITE); }
  /// @}
} // namespace bk

#endif // BKGL_BUFFER_IMPL_H


