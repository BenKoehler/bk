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

#ifndef BKGL_UBO_H
#define BKGL_UBO_H

#include <bk/CopyablePIMPL>
#include <bkGL/buffer/Buffer.h>
#include <bkGL/shader/glsl_helpers.h>
#include <bkGL/gl_type_traits.h>

namespace bk
{
  // -------------------- forward declaration
  class RegisteredValue;
  // -------------------- forward declaration END

  class BKGL_EXPORT UBO : protected Buffer
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBO;
      using base_type = Buffer;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBO();
      #else
      UBO(qt_gl_functions* gl);
      #endif
      UBO(const self_type& other) = delete;
      UBO(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBO();
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

      /// @{ -------------------------------------------------- GET NUM REGISTERED VALUES
      [[nodiscard]] unsigned int num_registered_values() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] unsigned int registered_values_size_in_bytes() const;
      /// @}

      /// @{ -------------------------------------------------- GET REGISTERED VALUES ITERATORS
      [[nodiscard]] auto registered_values_begin();
      [[nodiscard]] auto registered_values_begin() const;
      [[nodiscard]] auto registered_values_end();
      [[nodiscard]] auto registered_values_end() const;
      /// @}

      /// @{ -------------------------------------------------- REGISTERED VALUES
      [[nodiscard]] std::vector<RegisteredValue> registered_values() const;
      /// @}

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

      /// @{ -------------------------------------------------- REGISTER VALUE
    private:
      void _register_value(std::string_view name, GLenum type);
    public:
      void register_value(std::string_view name, bool x);
      void register_value(std::string_view name, std::uint8_t x);
      void register_value(std::string_view name, std::uint16_t x);
      void register_value(std::string_view name, std::uint32_t x);
      void register_value(std::string_view name, std::uint64_t x);
      void register_value(std::string_view name, std::int8_t x);
      void register_value(std::string_view name, std::int16_t x);
      void register_value(std::string_view name, std::int32_t x);
      void register_value(std::string_view name, std::int64_t x);
      void register_value(std::string_view name, float x);
      void register_value(std::string_view name, double x);

      void register_GL_INT(std::string_view name);
      void register_GL_UNSIGNED_INT(std::string_view name);
      void register_GL_FLOAT(std::string_view name);
      void register_GL_DOUBLE(std::string_view name);
      void register_GL_INT_VEC2(std::string_view name);
      void register_GL_INT_VEC3(std::string_view name);
      void register_GL_INT_VEC4(std::string_view name);
      void register_GL_UNSIGNED_INT_VEC2(std::string_view name);
      void register_GL_UNSIGNED_INT_VEC3(std::string_view name);
      void register_GL_UNSIGNED_INT_VEC4(std::string_view name);
      void register_GL_FLOAT_VEC2(std::string_view name);
      void register_GL_FLOAT_VEC3(std::string_view name);
      void register_GL_FLOAT_VEC4(std::string_view name);
      void register_GL_DOUBLE_VEC2(std::string_view name);
      void register_GL_DOUBLE_VEC3(std::string_view name);
      void register_GL_DOUBLE_VEC4(std::string_view name);
      void register_GL_FLOAT_MAT2(std::string_view name);
      void register_GL_FLOAT_MAT3(std::string_view name);
      void register_GL_FLOAT_MAT4(std::string_view name);
      void register_GL_DOUBLE_MAT2(std::string_view name);
      void register_GL_DOUBLE_MAT3(std::string_view name);
      void register_GL_DOUBLE_MAT4(std::string_view name);
      void register_GL_FLOAT_MAT2x3(std::string_view name);
      void register_GL_DOUBLE_MAT2x3(std::string_view name);
      void register_GL_FLOAT_MAT2x4(std::string_view name);
      void register_GL_DOUBLE_MAT2x4(std::string_view name);
      void register_GL_FLOAT_MAT3x2(std::string_view name);
      void register_GL_DOUBLE_MAT3x2(std::string_view name);
      void register_GL_FLOAT_MAT3x4(std::string_view name);
      void register_GL_DOUBLE_MAT3x4(std::string_view name);
      void register_GL_FLOAT_MAT4x2(std::string_view name);
      void register_GL_DOUBLE_MAT4x2(std::string_view name);
      void register_GL_FLOAT_MAT4x3(std::string_view name);
      void register_GL_DOUBLE_MAT4x3(std::string_view name);
      /// @}

      /// @{ -------------------------------------------------- UNREGISTER VALUE
      void unregister_value(std::string_view name);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      using base_type::clear;
      using base_type::bind;
      using base_type::bind_to_base;
      using base_type::release;
      using base_type::release_from_base;
      using base_type::write_values;
      using base_type::map_write_only;
      using base_type::map_read_only;
      using base_type::map_read_write;
      using base_type::unmap;
      using base_type::unmap_and_release;

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init_from_registered_values_size();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      void clear_registered_values();
      /// @}

      /// @{ -------------------------------------------------- WRITE REGISTERED VALUES
      [[maybe_unused]] bool write_registered_value(const std::string& name, const GLvoid* data);
      /// @}
  }; // class UBO
} // namespace bk

//====================================================================================================
//===== MACROS FOR SPECIALIZATIONS
//====================================================================================================
#define BK_UBO_SPECIALIZATION_DECLARE(NAME,TYPE) \
\
    using NAME##_value_type = bk::gl_underlying_type_t< TYPE > ;\
\
    [[nodiscard]] static constexpr const char* name_##NAME () noexcept\
    { return #NAME; }\
\
    [[nodiscard]] static constexpr unsigned int numel_##NAME () noexcept\
    { return bk::gl_type_traits< TYPE >::numel ;}\
\
    [[nodiscard]] static constexpr unsigned int bytes_##NAME () noexcept\
    { return bk::gl_type_traits< TYPE >::size_in_bytes; }\
\
    [[nodiscard]] static constexpr const char* typename_##NAME##_glsl () noexcept\
    { return bk::details::glenum_to_glsl_type_name( TYPE ) ;}\
\
    void set_##NAME ( NAME##_value_type val);\
    void set_##NAME (const NAME##_value_type* val);



#define BK_UBO_SPECIALIZATION_DEFINE(CLASSNAME,NAME,TYPE) \
\
    void CLASSNAME ::set_##NAME ( NAME##_value_type val)\
    {\
        if (this->is_initialized())\
        { \
            bk::gl_underlying_cast_type_t< TYPE > temp = static_cast<bk::gl_underlying_cast_type_t< TYPE >>(val);\
            this->write_registered_value( name_##NAME (), &temp); \
        }\
    }\
\
    void CLASSNAME ::set_##NAME(const NAME##_value_type* val)\
    {\
        if (this->is_initialized())\
        { this->write_registered_value( name_##NAME (), val); }\
    }\


#define BK_UBO_SPECIALIZATION_REGISTER(NAME, TYPE) \
    this->register_##TYPE ( #NAME );

#endif //BKGL_UBO_H


