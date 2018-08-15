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

#ifndef BKGL_BUFFER_H
#define BKGL_BUFFER_H

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <bkGL/AbstractBindable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class RegisteredValue;
  // -------------------- forward declaration END

  class BKGL_EXPORT Buffer : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Buffer;
      using base_type = details::AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      Buffer();
      #else
      explicit Buffer(qt_gl_functions* gl);
      #endif
      Buffer(const self_type& other) = delete;
      Buffer(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~Buffer();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS BOUND TO BUFFER BASE
      [[nodiscard]] bool is_bound_to_buffer_base() const;
      /// @}

      /// @{ -------------------------------------------------- GET TARGET
      [[nodiscard]] GLenum target() const;
      /// @}

      /// @{ -------------------------------------------------- GET BUFFER TYPE
      [[nodiscard]] bool is_vertex_buffer_object() const;
      [[nodiscard]] bool is_index_buffer_object() const;
      [[nodiscard]] bool is_shader_storage_buffer_object() const;
      [[nodiscard]] bool is_uniform_buffer_object() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] GLuint size_in_bytes() const;
      /// @}

      /// @{ -------------------------------------------------- GET USAGE
      [[nodiscard]] GLenum usage() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET VBO/IBO/UBO/SSBO
      void set_default_config_vertex_buffer_object();
      void set_default_config_index_buffer_object();
      void set_default_config_shader_storage_buffer_object();
      void set_default_config_uniform_buffer_object();
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
    private:
      void set_size_in_bytes(GLuint s);
    public:
      /// @}

      /// @{ -------------------------------------------------- SET USAGE
      void set_usage(GLenum usg);
      void set_usage_STREAM_DRAW();
      void set_usage_STREAM_READ();
      void set_usage_STREAM_COPY();
      void set_usage_STATIC_DRAW();
      void set_usage_STATIC_READ();
      void set_usage_STATIC_COPY();
      void set_usage_DYNAMIC_DRAW();
      void set_usage_DYNAMIC_READ();
      void set_usage_DYNAMIC_COPY();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CLEAR
      void clear_impl() override;
      /// @}

      /// @{ -------------------------------------------------- (UN)BIND
      void bind_impl() override;
      void release_impl() override;
    public:
      [[maybe_unused]] bool bind_to_base(GLuint bufferBaseID);
      void release_from_base();
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init(const GLvoid* data = nullptr, GLuint size = 0);
      [[maybe_unused]] bool init(GLuint s);
      //! @brief init with container<T>
      template<template<typename K, typename Alloc = std::allocator<K>> typename Container, typename T>
      [[maybe_unused]] bool init(const Container<T>& data);
      template<template<typename K, typename Alloc = std::allocator<K>> typename Container, typename T>
      [[maybe_unused]] bool init(Container<T>&& data);
      //! @brief init with iterators
      template<typename It>
      [[maybe_unused]] bool init(It first, It last);
      /// @}

      /// @{ -------------------------------------------------- UPDATE
      void update_data(GLuint offset_in_bytes, GLuint size_in_bytes, const GLvoid* data);
      /// @}

      /// @{ -------------------------------------------------- (UN)MAP
      template<typename T = GLvoid>
      [[nodiscard]] T* map(GLenum access);
      template<typename T = GLvoid>
      [[nodiscard]] T* map_write_only();
      template<typename T = GLvoid>
      [[nodiscard]] T* map_read_only();
      template<typename T = GLvoid>
      [[nodiscard]] T* map_read_write();
      void unmap();
      void unmap_and_release();
      /// @}

      /// @{ -------------------------------------------------- WRITE VALUES
      [[maybe_unused]] bool write_values(GLuint offset, GLuint size, const GLvoid* data);
      /// @}
  }; // class buffer
} // namespace bk

#include <bkGL/buffer/Buffer.hpp>

#endif //BKGL_BUFFER_H


