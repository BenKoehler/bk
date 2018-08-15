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

#ifndef BKGL_SHADER_H
#define BKGL_SHADER_H

#include <string>
#include <string_view>
#include <utility>

#include "EShaderType.h"
#include "../AbstractBindable.h"
#include "../lib/bkGL_export.h"

namespace bk
{
  class BKGL_EXPORT Shader : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Shader;
      using base_type = details::AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      GLuint _vertex_shader_id;
      GLuint _fragment_shader_id;
      GLuint _geometry_shader_id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      Shader();
      #else
      explicit Shader(bk::qt_gl_functions* gl);
      #endif
      Shader(const self_type& other) = delete;
      Shader(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~Shader();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- HELPERS
      [[nodiscard]] BK_QT_GL_STATIC bool shader_compile_success(GLuint shader_id);
      [[nodiscard]] BK_QT_GL_STATIC std::string shader_error_log(GLuint shader_id);
      [[nodiscard]] BK_QT_GL_STATIC bool program_compile_success(GLuint program_id);
      [[nodiscard]] BK_QT_GL_STATIC std::string program_error_log(GLuint program_id);
      /// @}

    public:
      /// @{ -------------------------------------------------- GET IDS
      [[nodiscard]] GLuint vertex_shader_id() const;
      [[nodiscard]] GLuint fragment_shader_id() const;
      [[nodiscard]] GLuint geometry_shader_id() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

    private:
      /// @{ -------------------------------------------------- HELPERS: COMPILE SHADER (FROM SOURCE)
      [[maybe_unused]] bool compile_shader(ShaderType type, std::string_view source);
      [[maybe_unused]] bool compile_shader_from_file(ShaderType type, std::string_view filename);
      /// @}

    public:
      /// @{ -------------------------------------------------- SET VERTEX SHADER
      [[maybe_unused]] bool set_vertex_shader(std::string_view source);
      [[maybe_unused]] bool set_vertex_shader_from_file(std::string_view filename);
      /// @}

      /// @{ -------------------------------------------------- SET FRAGMENT SHADER
      [[maybe_unused]] bool set_fragment_shader(std::string_view source);
      [[maybe_unused]] bool set_fragment_shader_from_file(std::string_view filename);
      /// @}

      /// @{ -------------------------------------------------- SET GEOMETRY SHADER
      [[maybe_unused]] bool set_geometry_shader(std::string_view source);
      [[maybe_unused]] bool set_geometry_shader_from_file(std::string_view filename);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear_vertex_shader();
      void clear_fragment_shader();
      void clear_geometry_shader();
      void clear_program();
    protected:
      void clear_impl() override;
    public:
      /// @}

      /// @{ -------------------------------------------------- COMPILE PROGRAM
      [[maybe_unused]] bool compile_program();
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init_from_sources(std::string_view vertShaderSource, std::string_view fragShaderSource, std::string_view geomShaderSource = "");
      [[maybe_unused]] bool init(std::string_view vertShaderPath, std::string_view fragShaderPath, std::string_view geomShaderPath = "");
      /// @}

      /// @{ -------------------------------------------------- (UN)BIND
    protected:
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class Shader
} // namespace bk

#endif //BKGL_SHADER_H


