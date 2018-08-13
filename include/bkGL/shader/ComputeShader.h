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

#ifndef BKGL_COMPUTESHADER_H
#define BKGL_COMPUTESHADER_H

#include <string>
#include <utility>

#include "../AbstractBindable.h"
#include "../lib/export.h"

namespace bk
{
  class BK_GL_EXPORT ComputeShader : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ComputeShader;
      using base_type = details::AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      GLuint _compute_shader_id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      ComputeShader();
      #else
      explicit ComputeShader(qt_gl_functions* gl);
      #endif
      ComputeShader(const self_type& other) = delete;
      ComputeShader(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~ComputeShader();
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
      [[nodiscard]] GLuint compute_shader_id() const;
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
      [[maybe_unused]] std::pair<bool/*success*/, std::string/*log*/> compile_shader(const std::string& source);
      [[maybe_unused]] std::pair<bool/*success*/, std::string/*log*/> compile_shader_from_file(const std::string& filename);
      /// @}

    public:
      /// @{ -------------------------------------------------- SET COMPUTE SHADER
      [[maybe_unused]] std::pair<bool, std::string> set_compute_shader(const std::string& source);
      [[maybe_unused]] std::pair<bool, std::string> set_compute_shader_from_file(const std::string& filename);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear_compute_shader();
      void clear_program();
    protected:
      void clear_impl() override;
    public:
      /// @}

      /// @{ -------------------------------------------------- COMPILE PROGRAM
      [[maybe_unused]] std::pair<bool/*success*/, std::string/*error log*/> compile_program();
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init(const std::string& compShaderPath);
      /// @}

      /// @{ -------------------------------------------------- (UN)BIND
    protected:
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class ComputeShader
} // namespace bk

#endif //BKGL_COMPUTESHADER_H


