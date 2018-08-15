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

#include <bkGL/shader/Shader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <bk/StringUtils>
#include <bkTools/io/read_text_file.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  Shader::Shader()
      : base_type(),
        _vertex_shader_id(0),
        _fragment_shader_id(0),
        _geometry_shader_id(0)
  { /* do nothing */ }

  #else

  Shader::Shader(bk::qt_gl_functions* gl)
      : base_type(gl),
        _vertex_shader_id(0),
        _fragment_shader_id(0),
        _geometry_shader_id(0)
  { /* do nothing */ }

  #endif

  Shader::Shader(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Shader::~Shader()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- HELPERS
  bool Shader::shader_compile_success(GLuint shader_id)
  {
      GLint success = 0;
      BK_QT_GL glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

      return static_cast<bool>(success);
  }

  std::string Shader::shader_error_log(GLuint shader_id)
  {
      GLint logLength = 0;
      GLsizei charsWritten = 0;
      BK_QT_GL glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);

      std::string log;

      if (logLength != 0)
      {
          log.resize(logLength, ' ');
          BK_QT_GL glGetShaderInfoLog(shader_id, logLength, &charsWritten, log.data());
      }

      return log;
  }

  bool Shader::program_compile_success(GLuint program_id)
  {
      GLint success = 0;
      BK_QT_GL glGetProgramiv(program_id, GL_LINK_STATUS, &success);

      return success == GL_TRUE;
  }

  std::string Shader::program_error_log(GLuint program_id)
  {
      GLint logLength = 0;
      GLsizei charsWritten = 0;
      BK_QT_GL glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLength);

      std::string log;

      if (logLength != 0)
      {
          log.resize(logLength, ' ');
          BK_QT_GL glGetProgramInfoLog(program_id, logLength, &charsWritten, log.data());
      }

      return log;
  }
  /// @}

  /// @{ -------------------------------------------------- GET IDS
  GLuint Shader::vertex_shader_id() const
  { return _vertex_shader_id; }

  GLuint Shader::fragment_shader_id() const
  { return _fragment_shader_id; }

  GLuint Shader::geometry_shader_id() const
  { return _geometry_shader_id; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto Shader::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- HELPERS: COMPILE SHADER (FROM SOURCE)
  bool Shader::compile_shader(ShaderType type, std::string_view source)
  {
      GLuint shader_id = 0;

      if (type == ShaderType::Vertex)
      {
          shader_id = BK_QT_GL glCreateShader(GL_VERTEX_SHADER);
          _vertex_shader_id = shader_id;
      }
      else if (type == ShaderType::Fragment)
      {
          shader_id = BK_QT_GL glCreateShader(GL_FRAGMENT_SHADER);
          _fragment_shader_id = shader_id;
      }
      else if (type == ShaderType::Geometry)
      {
          shader_id = BK_QT_GL glCreateShader(GL_GEOMETRY_SHADER);
          _geometry_shader_id = shader_id;
      }

      const char* sourcePtr = source.data();
      BK_QT_GL glShaderSource(shader_id, 1, &sourcePtr, nullptr);
      BK_QT_GL glCompileShader(shader_id);

      const bool success = shader_compile_success(shader_id);

      if (!success)
      {
          const std::string typestring = type == ShaderType::Vertex ? "Vertex" : type == ShaderType::Fragment ? "Fragment" : "Geometry";
          std::cerr << typestring << " shader compilation failed!" << std::endl;
          std::cerr << shader_error_log(shader_id) << std::endl;
          std::cout << " ==================================================== source:" << std::endl;
          std::cout << source << std::endl;
          std::cout << " ====================================================" << std::endl;
      }

      return success;
  }

  bool Shader::compile_shader_from_file(ShaderType type, std::string_view filename)
  { return compile_shader(type, bk::read_text_file(filename)); }
  /// @}

  /// @{ -------------------------------------------------- SET VERTEX SHADER
  bool Shader::set_vertex_shader(std::string_view source)
  { return compile_shader(ShaderType::Vertex, source); }

  bool Shader::set_vertex_shader_from_file(std::string_view filename)
  { return compile_shader_from_file(ShaderType::Vertex, filename); }
  /// @}

  /// @{ -------------------------------------------------- SET FRAGMENT SHADER
  bool Shader::set_fragment_shader(std::string_view source)
  { return compile_shader(ShaderType::Fragment, source); }

  bool Shader::set_fragment_shader_from_file(std::string_view filename)
  { return compile_shader_from_file(ShaderType::Fragment, filename); }
  /// @}

  /// @{ -------------------------------------------------- SET GEOMETRY SHADER
  bool Shader::set_geometry_shader(std::string_view source)
  { return compile_shader(ShaderType::Geometry, source); }

  bool Shader::set_geometry_shader_from_file(std::string_view filename)
  { return compile_shader_from_file(ShaderType::Geometry, filename); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void Shader::clear_vertex_shader()
  {
      if (_vertex_shader_id != 0)
      {
          BK_QT_GL glDeleteShader(_vertex_shader_id);
          _vertex_shader_id = 0;
      }
  }

  void Shader::clear_fragment_shader()
  {
      if (_fragment_shader_id != 0)
      {
          BK_QT_GL glDeleteShader(_fragment_shader_id);
          _fragment_shader_id = 0;
      }
  }

  void Shader::clear_geometry_shader()
  {
      if (_geometry_shader_id != 0)
      {
          BK_QT_GL glDeleteShader(_geometry_shader_id);
          _geometry_shader_id = 0;
      }
  }

  void Shader::clear_program()
  {
      if (_id != 0)
      {
          BK_QT_GL glDeleteProgram(_id);
          _id = 0;
      }
  }

  void Shader::clear_impl()
  {
      clear_vertex_shader();
      clear_fragment_shader();
      clear_geometry_shader();
      clear_program();
  }
  /// @}

  /// @{ -------------------------------------------------- COMPILE PROGRAM
  bool Shader::compile_program()
  {
      clear_program();

      _id = BK_QT_GL glCreateProgram();

      if (_vertex_shader_id != 0)
      { BK_QT_GL glAttachShader(_id, _vertex_shader_id); }

      if (_fragment_shader_id != 0)
      { BK_QT_GL glAttachShader(_id, _fragment_shader_id); }

      if (_geometry_shader_id != 0)
      { BK_QT_GL glAttachShader(_id, _geometry_shader_id); }

      BK_QT_GL glLinkProgram(_id);

      const bool success = program_compile_success(_id);

      if (!success)
      {
          std::cerr << "Program compilation failed!" << std::endl;
          std::cerr << program_error_log(_id) << std::endl;
      }

      return success;
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool Shader::init_from_sources(std::string_view vertShaderSource, std::string_view fragShaderSource, std::string_view geomShaderSource)
  {
      clear();

      const bool successVert = set_vertex_shader(vertShaderSource);
      const bool successFrag = set_fragment_shader(fragShaderSource);
      bool successGeom = true;

      if (!geomShaderSource.empty())
      { successGeom = set_geometry_shader(geomShaderSource); }

      const bool successProgram = compile_program();

      if (!successVert)
      { std::cerr << "Shader::init_from_sources - VERTEX SHADER FAILED" << std::endl; }
     
      if (!successFrag)
      { std::cerr << "Shader::init_from_sources - FRAGMENT SHADER FAILED" << std::endl; }
   
      if (!successGeom)
      { std::cerr << "Shader::init_from_sources - GEOMETRY SHADER FAILED" << std::endl; }
    
      if (!successProgram)
      { std::cerr << "Shader::init_from_sources - PROGRAM FAILED" << std::endl; }

      return successVert && successFrag && successGeom && successProgram;
  }

  bool Shader::init(std::string_view vertShaderPath, std::string_view fragShaderPath, std::string_view geomShaderPath)
  {
      clear();

      const bool successVert = set_vertex_shader_from_file(vertShaderPath);
      const bool successFrag = set_fragment_shader_from_file(fragShaderPath);
      bool successGeom = true;
      
      if (!geomShaderPath.empty())
      { successGeom = set_geometry_shader_from_file(geomShaderPath); }

      const bool successProgram = compile_program();

      if (!successVert)
      { std::cerr << "Shader::init - VERTEX SHADER FAILED" << std::endl; }

      if (!successFrag)
      { std::cerr << "Shader::init - FRAGMENT SHADER FAILED" << std::endl; }

      if (!successGeom)
      { std::cerr << "Shader::init - GEOMETRY SHADER FAILED" << std::endl; }

      if (!successProgram)
      { std::cerr << "Shader::init - PROGRAM FAILED" << std::endl; }

      return successVert && successFrag && successGeom && successProgram;
  }
  /// @}

  /// @{ -------------------------------------------------- (UN)BIND
  void Shader::bind_impl()
  { BK_QT_GL glUseProgram(_id); }

  void Shader::release_impl()
  { BK_QT_GL glUseProgram(0); }
  /// @}
} // namespace bk


