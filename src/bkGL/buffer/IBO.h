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

#ifndef BKGL_IBO_H
#define BKGL_IBO_H

#include <bkGL/buffer/Buffer.h>

namespace bk
{
  class BKGL_EXPORT IBO : protected Buffer
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = IBO;
      using base_type = Buffer;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      IBO();
      #else
      IBO(qt_gl_functions* gl);
      #endif
      IBO(const self_type& other) = delete;
      IBO(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~IBO();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      using base_type::id;
      using base_type::is_bound;
      using base_type::is_bound_to_buffer_base;
      using base_type::target;
      using base_type::size_in_bytes;
      using base_type::usage;
      using base_type::is_initialized;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      using base_type::set_usage;
      using base_type::set_usage_STREAM_DRAW;
      using base_type::set_usage_STREAM_READ;
      using base_type::set_usage_STREAM_COPY;
      using base_type::set_usage_STATIC_DRAW;
      using base_type::set_usage_STATIC_READ;
      using base_type::set_usage_STATIC_COPY;
      using base_type::set_usage_DYNAMIC_DRAW;
      using base_type::set_usage_DYNAMIC_READ;
      using base_type::set_usage_DYNAMIC_COPY;

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      using base_type::clear;
      using base_type::init;
      using base_type::bind;
      using base_type::bind_to_base;
      using base_type::release;
      using base_type::release_from_base;
      using base_type::update_data;
      using base_type::write_values;
      using base_type::map_write_only;
      using base_type::map_read_only;
      using base_type::map_read_write;
      using base_type::unmap;
      using base_type::unmap_and_release;
  }; // class IBO
} // namespace bk

#endif //BKGL_IBO_H


