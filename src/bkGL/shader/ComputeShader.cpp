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

#include <bkGL/shader/ComputeShader.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <bk/StringUtils>
#include <bkTools/io/read_text_file.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  ComputeShader::ComputeShader()
      : base_type(),
        _compute_shader_id(0)
  { /* do nothing */ }

  #else

  ComputeShader::ComputeShader(bk::qt_gl_functions* gl)
      : base_type(gl),
        _compute_shader_id(0)
  { /* do nothing */ }

  #endif

  ComputeShader::ComputeShader(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ComputeShader::~ComputeShader()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- HELPERS
  bool ComputeShader::shader_compile_success(GLuint shader_id)
  {
      GLint success = 0;
      BK_QT_GL glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

      return static_cast<bool>(success);
  }

  std::string ComputeShader::shader_error_log(GLuint shader_id)
  {
      GLint infologLength = 0;
      GLsizei charsWritten = 0;
      BK_QT_GL glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infologLength);

      char* infoLog = new char[infologLength];
      BK_QT_GL glGetShaderInfoLog(shader_id, infologLength, &charsWritten, infoLog);

      std::string log(infoLog);
      delete[] infoLog;

      return log;
  }

  bool ComputeShader::program_compile_success(GLuint program_id)
  {
      GLint success = 0;
      BK_QT_GL glGetProgramiv(program_id, GL_LINK_STATUS, &success);

      return success == GL_TRUE;
  }

  std::string ComputeShader::program_error_log(GLuint program_id)
  {
      GLint infologLength = 0;
      GLsizei charsWritten = 0;
      BK_QT_GL glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infologLength);

      if (infologLength != 0)
      {
          char* infoLog = new char[infologLength];
          BK_QT_GL glGetProgramInfoLog(program_id, infologLength, &charsWritten, infoLog);

          std::string log(infoLog);
          delete[] infoLog;

          return log;
      }

      return "";
  }
  /// @}

  /// @{ -------------------------------------------------- GET IDS
  GLuint ComputeShader::compute_shader_id() const
  { return _compute_shader_id; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto ComputeShader::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- HELPERS: COMPILE SHADER (FROM SOURCE)
  std::pair<bool/*success*/, std::string/*log*/> ComputeShader::compile_shader(const std::string& source)
  {
      GLuint shader_id = 0;
      shader_id = BK_QT_GL glCreateShader(GL_COMPUTE_SHADER);
      _compute_shader_id = shader_id;

      const char* sourcePtr = source.c_str();
      BK_QT_GL glShaderSource(shader_id, 1, &sourcePtr, nullptr);
      BK_QT_GL glCompileShader(shader_id);

      std::pair<bool, std::string> p(shader_compile_success(shader_id), "");
      if (!p.first)
      {
          p.second = shader_error_log(shader_id);

          if (p.second.empty())
          { p.first = true; }
      }

      return p;
  }

  std::pair<bool/*success*/, std::string/*log*/> ComputeShader::compile_shader_from_file(const std::string& filename)
  { return compile_shader(bk::read_text_file(filename)); }
  /// @}

  /// @{ -------------------------------------------------- SET COMPUTE SHADER
  std::pair<bool, std::string> ComputeShader::set_compute_shader(const std::string& source)
  { return compile_shader(source); }

  std::pair<bool, std::string> ComputeShader::set_compute_shader_from_file(const std::string& filename)
  { return compile_shader_from_file(filename); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void ComputeShader::clear_compute_shader()
  {
      if (_compute_shader_id != 0)
      {
          BK_QT_GL glDeleteShader(_compute_shader_id);
          _compute_shader_id = 0;
      }
  }

  void ComputeShader::clear_program()
  {
      if (_id != 0)
      {
          BK_QT_GL glDeleteProgram(_id);
          _id = 0;
      }
  }

  void ComputeShader::clear_impl()
  {
      clear_compute_shader();
      clear_program();
  }
  /// @}

  /// @{ -------------------------------------------------- COMPILE PROGRAM
  std::pair<bool/*success*/, std::string/*error log*/> ComputeShader::compile_program()
  {
      clear_program();

      _id = BK_QT_GL glCreateProgram();

      if (_compute_shader_id != 0)
      { BK_QT_GL glAttachShader(_id, _compute_shader_id); }

      BK_QT_GL glLinkProgram(_id);

      std::pair<bool, std::string> p(program_compile_success(_id), "");
      if (!p.first)
      {
          p.second = program_error_log(_id);

          if (p.second.empty())
          { p.first = true; }
      }

      return p;
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool ComputeShader::init(const std::string& compShaderPath)
  {
      clear();

      if (compShaderPath.empty())
      { return false; }

      using info_type = std::pair<bool/*success*/, std::string/*log*/>;
      bool valid_shader = true;
      std::stringstream ss;

      /*
       * compile compute shader
       */
      std::string csp = string_utils::replace(compShaderPath, "\\", "/");
      const info_type success_vert = set_compute_shader_from_file(csp);

      if (!success_vert.first)
      {
          valid_shader = false;
          ss << string_utils::split(csp, "/").back() << ":" << std::endl;
          ss << success_vert.second << std::endl;
      }

      /*
       * compile program
       */
      const info_type success_prog = compile_program();
      if (!success_prog.first)
      {
          valid_shader = false;
          ss << "link error:" << std::endl;
          ss << success_prog.second << std::endl;
      }

      if (!valid_shader)
      {
          std::stringstream tempss;
          tempss << "shader" << "_" << string_utils::split(string_utils::split(csp, "/").back(), ".").front() << "_error_log.txt";
          std::ofstream logfile(tempss.str(), std::ios_base::out);
          logfile << ss.str();
          logfile.close();
      }

      return valid_shader;
  }
  /// @}

  /// @{ -------------------------------------------------- (UN)BIND
  void ComputeShader::bind_impl()
  { BK_QT_GL glUseProgram(_id); }

  void ComputeShader::release_impl()
  { BK_QT_GL glUseProgram(0); }
  /// @}
} // namespace bk