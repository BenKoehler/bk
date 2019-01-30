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

#include <bkGL/gl_error.h>

#include <iostream>

namespace bk
{
  void gl_print_error(const std::string& msg)
  {
      switch (glGetError())
      {
          case GL_NO_ERROR: /*do nothing*/ /*std::cout << "GL_NO_ERROR" << std::endl;*/
              break;
          case GL_INVALID_VALUE: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_INVALID_VALUE" << std::endl;
              break;
          case GL_INVALID_ENUM: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_INVALID_ENUM" << std::endl;
              break;
          case GL_INVALID_OPERATION: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_INVALID_OPERATION" << std::endl;
              break;
          case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
              break;
          case GL_OUT_OF_MEMORY: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_OUT_OF_MEMORY" << std::endl;
              break;
          case GL_STACK_UNDERFLOW: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_STACK_UNDERFLOW" << std::endl;
              break;
          case GL_STACK_OVERFLOW: std::cout << (!msg.empty() ? (msg + " ") : "") << "GL_STACK_OVERFLOW" << std::endl;
              break;
      }
  }

  bool gl_has_error()
  { return glGetError() != GL_NO_ERROR; }

  void gl_clear_error()
  {
      while (gl_has_error())
      { /* do nothing */ }
  }

  #ifdef BK_DEBUG_GL
  void print_opengl_error(unsigned short i_sev, const std::string& s_source, std::string s_type, const GLchar* message)
  {        fprintf(stderr, "%u - %s|%s - %s\n", i_sev, s_source.c_str(), s_type.c_str(), message);  }

  void /*APIENTRY*/ openglCallbackFunction(GLenum source, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
  {
      std::string s_source;
      switch (source)
      {
          case GL_DEBUG_SOURCE_API: s_source = "API";
              break;
          case GL_DEBUG_SOURCE_WINDOW_SYSTEM: s_source = "WIN";
              break;
          case GL_DEBUG_SOURCE_SHADER_COMPILER: s_source = "CMP";
              break;
          case GL_DEBUG_SOURCE_THIRD_PARTY: s_source = "3RD";
              break;
          case GL_DEBUG_SOURCE_APPLICATION: s_source = "APP";
              break;
          default: s_source = "???";
              break;
      }

      std::string s_type;
      switch (type)
      {
          case GL_DEBUG_TYPE_ERROR: s_type = "ERROR";
              break;
          case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: s_type = "DEPR.";
              break;
          case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: s_type = "UNDEF";
              break;
          case GL_DEBUG_TYPE_PORTABILITY: s_type = "PORT ";
              break;
          case GL_DEBUG_TYPE_MARKER: s_type = "MARKR";
              break;
          case GL_DEBUG_TYPE_PUSH_GROUP: s_type = "GPUSH";
              break;
          case GL_DEBUG_TYPE_POP_GROUP: s_type = "GPOP ";
              break;
          case GL_DEBUG_TYPE_PERFORMANCE: s_type = "PERF ";
              break;
          default: s_type = "?????";
              break;
      }

      unsigned short i_sev;
      switch (severity)
      {
          case GL_DEBUG_SEVERITY_HIGH: i_sev = 4;
              break;
          case GL_DEBUG_SEVERITY_MEDIUM: i_sev = 3;
              break;
          case GL_DEBUG_SEVERITY_LOW: i_sev = 2;
              break;
          case GL_DEBUG_SEVERITY_NOTIFICATION: i_sev = 1;
              break;
          default: i_sev = 0;
              break;
      }

      if (i_sev > 1 && type != GL_DEBUG_TYPE_PERFORMANCE)
      {
          //fprintf(stderr, "%u - %s|%s - %s\n", i_sev, s_source.c_str(), s_type.c_str(), message);
          print_opengl_error(i_sev, s_source, s_type, message);
      }
  }
  #endif
} // namespace bk


