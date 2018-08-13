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

#ifndef BKGL_GL_TYPE_TRAITS_H
#define BKGL_GL_TYPE_TRAITS_H

#include "gl_definitions.h"

namespace bk
{
  template<GLenum T> struct gl_type_traits {using underlying_type = GLint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 1; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_INT; static constexpr GLenum glenum = GL_INT;};
  template<> struct gl_type_traits<GL_INT> {using underlying_type = GLint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 1; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_INT; static constexpr GLenum glenum = GL_INT;};
  template<> struct gl_type_traits<GL_INT_VEC2> {using underlying_type = GLint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 2; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_INT; static constexpr GLenum glenum = GL_INT_VEC2;};
  template<> struct gl_type_traits<GL_INT_VEC3> {using underlying_type = GLint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 3; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_INT; static constexpr GLenum glenum = GL_INT_VEC3;};
  template<> struct gl_type_traits<GL_INT_VEC4> {using underlying_type = GLint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_INT; static constexpr GLenum glenum = GL_INT_VEC4;};
  template<> struct gl_type_traits<GL_UNSIGNED_INT> {using underlying_type = GLuint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 1; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_UNSIGNED_INT; static constexpr GLenum glenum = GL_UNSIGNED_INT;};
  template<> struct gl_type_traits<GL_UNSIGNED_INT_VEC2> {using underlying_type = GLuint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 2; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_UNSIGNED_INT; static constexpr GLenum glenum = GL_UNSIGNED_INT_VEC2;};
  template<> struct gl_type_traits<GL_UNSIGNED_INT_VEC3> {using underlying_type = GLuint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 3; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_UNSIGNED_INT; static constexpr GLenum glenum = GL_UNSIGNED_INT_VEC3;};
  template<> struct gl_type_traits<GL_UNSIGNED_INT_VEC4> {using underlying_type = GLuint; using underlying_cast_type = GLint; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_UNSIGNED_INT; static constexpr GLenum glenum = GL_UNSIGNED_INT_VEC4;};
  template<> struct gl_type_traits<GL_FLOAT> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 1; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT;};
  template<> struct gl_type_traits<GL_FLOAT_VEC2> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 2; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_VEC2;};
  template<> struct gl_type_traits<GL_FLOAT_VEC3> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 3; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_VEC3;};
  template<> struct gl_type_traits<GL_FLOAT_VEC4> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_VEC4;};
  template<> struct gl_type_traits<GL_FLOAT_MAT2> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT2;};
  template<> struct gl_type_traits<GL_FLOAT_MAT2x3> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 6; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT2x3;};
  template<> struct gl_type_traits<GL_FLOAT_MAT2x4> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 8; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT2x4;};
  template<> struct gl_type_traits<GL_FLOAT_MAT3> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 9; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT3;};
  template<> struct gl_type_traits<GL_FLOAT_MAT3x2> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 6; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT3x2;};
  template<> struct gl_type_traits<GL_FLOAT_MAT3x4> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 12; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT3x4;};
  template<> struct gl_type_traits<GL_FLOAT_MAT4> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 16; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT4;};
  template<> struct gl_type_traits<GL_FLOAT_MAT4x2> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 8; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT4x2;};
  template<> struct gl_type_traits<GL_FLOAT_MAT4x3> {using underlying_type = GLfloat; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 12; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_FLOAT; static constexpr GLenum glenum = GL_FLOAT_MAT4x3;};
  template<> struct gl_type_traits<GL_DOUBLE> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 1; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE;};
  template<> struct gl_type_traits<GL_DOUBLE_VEC2> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 2; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_VEC2;};
  template<> struct gl_type_traits<GL_DOUBLE_VEC3> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 3; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_VEC3;};
  template<> struct gl_type_traits<GL_DOUBLE_VEC4> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_VEC4;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT2> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 4; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT2;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT2x3> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 6; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT2x3;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT2x4> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 8; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT2x4;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT3> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 9; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT3;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT3x2> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 6; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT3x2;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT3x4> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 12; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT3x4;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT4> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 16; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT4;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT4x2> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 8; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT4x2;};
  template<> struct gl_type_traits<GL_DOUBLE_MAT4x3> {using underlying_type = GLdouble; using underlying_cast_type = GLfloat; static constexpr unsigned int numel = 12; static constexpr unsigned int size_in_bytes = numel*sizeof(underlying_type); static constexpr GLenum underlying_glenum = GL_DOUBLE; static constexpr GLenum glenum = GL_DOUBLE_MAT4x3;};

  template<GLenum T> using gl_underlying_type_t = typename gl_type_traits<T>::underlying_type;
  template<GLenum T> using gl_underlying_cast_type_t = typename gl_type_traits<T>::underlying_cast_type;


  [[nodiscard]] constexpr GLenum gl_underlying_glenum(GLenum t)
  {
      switch (t)
      {
          default: return t;

          case GL_INT_VEC2: [[fallthrough]];
          case GL_INT_VEC3: [[fallthrough]];
          case GL_INT_VEC4: [[fallthrough]];
          case GL_INT_SAMPLER_1D: [[fallthrough]];
          case GL_INT_SAMPLER_2D: [[fallthrough]];
          case GL_INT_SAMPLER_3D: return GL_INT;

          case GL_UNSIGNED_INT_VEC2: [[fallthrough]];
          case GL_UNSIGNED_INT_VEC3: [[fallthrough]];
          case GL_UNSIGNED_INT_VEC4: [[fallthrough]];
          case GL_UNSIGNED_INT_SAMPLER_1D: [[fallthrough]];
          case GL_UNSIGNED_INT_SAMPLER_2D: [[fallthrough]];
          case GL_UNSIGNED_INT_SAMPLER_3D: return GL_UNSIGNED_INT;

          case GL_FLOAT_VEC2: [[fallthrough]];
          case GL_FLOAT_VEC3: [[fallthrough]];
          case GL_FLOAT_VEC4: [[fallthrough]];
          case GL_FLOAT_MAT2: [[fallthrough]];
          case GL_FLOAT_MAT2x3: [[fallthrough]];
          case GL_FLOAT_MAT2x4: [[fallthrough]];
          case GL_FLOAT_MAT3: [[fallthrough]];
          case GL_FLOAT_MAT3x2: [[fallthrough]];
          case GL_FLOAT_MAT3x4: [[fallthrough]];
          case GL_FLOAT_MAT4: [[fallthrough]];
          case GL_FLOAT_MAT4x2: [[fallthrough]];
          case GL_FLOAT_MAT4x3: [[fallthrough]];
          case GL_SAMPLER_1D: [[fallthrough]];
          case GL_SAMPLER_2D: [[fallthrough]];
          case GL_SAMPLER_3D: return GL_FLOAT;

          case GL_DOUBLE_VEC2: [[fallthrough]];
          case GL_DOUBLE_VEC3: [[fallthrough]];
          case GL_DOUBLE_VEC4: [[fallthrough]];
          case GL_DOUBLE_MAT2: [[fallthrough]];
          case GL_DOUBLE_MAT2x3: [[fallthrough]];
          case GL_DOUBLE_MAT2x4: [[fallthrough]];
          case GL_DOUBLE_MAT3: [[fallthrough]];
          case GL_DOUBLE_MAT3x2: [[fallthrough]];
          case GL_DOUBLE_MAT3x4: [[fallthrough]];
          case GL_DOUBLE_MAT4: [[fallthrough]];
          case GL_DOUBLE_MAT4x2: [[fallthrough]];
          case GL_DOUBLE_MAT4x3: return GL_DOUBLE;
      }
  }

  [[nodiscard]] constexpr unsigned int gl_numel(GLenum t)
  {
      switch (t)
      {
          default: return 0U;

          case GL_BYTE: [[fallthrough]];
          case GL_UNSIGNED_BYTE: [[fallthrough]];
          case GL_SHORT: [[fallthrough]];
          case GL_UNSIGNED_SHORT: [[fallthrough]];
          case GL_INT: [[fallthrough]];
          case GL_UNSIGNED_INT: [[fallthrough]];
          case GL_FLOAT: [[fallthrough]];
          case GL_DOUBLE: return 1U;

          case GL_INT_VEC2: [[fallthrough]];
          case GL_UNSIGNED_INT_VEC2: [[fallthrough]];
          case GL_FLOAT_VEC2: [[fallthrough]];
          case GL_DOUBLE_VEC2: return 2U;

          case GL_INT_VEC3: [[fallthrough]];
          case GL_UNSIGNED_INT_VEC3: [[fallthrough]];
          case GL_FLOAT_VEC3: [[fallthrough]];
          case GL_DOUBLE_VEC3: return 3U;

          case GL_INT_VEC4: [[fallthrough]];
          case GL_UNSIGNED_INT_VEC4: [[fallthrough]];
          case GL_FLOAT_VEC4: [[fallthrough]];
          case GL_DOUBLE_VEC4: [[fallthrough]];
          case GL_FLOAT_MAT2: [[fallthrough]];
          case GL_DOUBLE_MAT2: return 4U;

          case GL_FLOAT_MAT2x3: [[fallthrough]];
          case GL_FLOAT_MAT3x2: [[fallthrough]];
          case GL_DOUBLE_MAT2x3: [[fallthrough]];
          case GL_DOUBLE_MAT3x2: return 6U;

          case GL_FLOAT_MAT2x4: [[fallthrough]];
          case GL_FLOAT_MAT4x2: [[fallthrough]];
          case GL_DOUBLE_MAT2x4: [[fallthrough]];
          case GL_DOUBLE_MAT4x2: return 8U;

          case GL_FLOAT_MAT3: [[fallthrough]];
          case GL_DOUBLE_MAT3: return 9U;

          case GL_FLOAT_MAT3x4: [[fallthrough]];
          case GL_FLOAT_MAT4x3: [[fallthrough]];
          case GL_DOUBLE_MAT3x4: [[fallthrough]];
          case GL_DOUBLE_MAT4x3: return 12U;

          case GL_FLOAT_MAT4: [[fallthrough]];
          case GL_DOUBLE_MAT4: return 16U;
      }
  }

  [[nodiscard]] constexpr unsigned int gl_size_in_bytes(GLenum t)
  {
      switch (t)
      {
          case GL_BYTE: return sizeof(GLbyte);
          case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
          case GL_SHORT: return sizeof(GLshort);
          case GL_UNSIGNED_SHORT: return sizeof(GLushort);
          case GL_INT: return sizeof(GLint);
          case GL_UNSIGNED_INT: return sizeof(GLuint);
          case GL_FLOAT: return sizeof(GLfloat);
          case GL_DOUBLE: return sizeof(GLdouble);
          default: return gl_numel(t) * gl_size_in_bytes(gl_underlying_glenum(t));
      }
  }

  [[nodiscard]] constexpr const char* glenum_to_string(GLenum t)
  {
      switch (t)
      {
          case GL_BYTE: return "GL_BYTE";
          case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
          case GL_SHORT: return "GL_SHORT";
          case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
          case GL_INT: return "GL_INT";
          case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
          case GL_FLOAT: return "GL_FLOAT";
          case GL_DOUBLE: return "GL_DOUBLE";
          case GL_INT_VEC2: return "GL_INT_VEC2";
          case GL_INT_VEC3: return "GL_INT_VEC3";
          case GL_INT_VEC4: return "GL_INT_VEC4";
          case GL_UNSIGNED_INT_VEC2: return "GL_UNSIGNED_INT_VEC2";
          case GL_UNSIGNED_INT_VEC3: return "GL_UNSIGNED_INT_VEC3";
          case GL_UNSIGNED_INT_VEC4: return "GL_UNSIGNED_INT_VEC4";
          case GL_FLOAT_VEC2: return "GL_FLOAT_VEC2";
          case GL_FLOAT_VEC3: return "GL_FLOAT_VEC3";
          case GL_FLOAT_VEC4: return "GL_FLOAT_VEC4";
          case GL_DOUBLE_VEC2: return "GL_DOUBLE_VEC2";
          case GL_DOUBLE_VEC3: return "GL_DOUBLE_VEC3";
          case GL_DOUBLE_VEC4: return "GL_DOUBLE_VEC4";
          case GL_FLOAT_MAT2: return "GL_FLOAT_MAT2";
          case GL_FLOAT_MAT3: return "GL_FLOAT_MAT3";
          case GL_FLOAT_MAT4: return "GL_FLOAT_MAT4";
          case GL_DOUBLE_MAT2: return "GL_DOUBLE_MAT2";
          case GL_DOUBLE_MAT3: return "GL_DOUBLE_MAT3";
          case GL_DOUBLE_MAT4: return "GL_DOUBLE_MAT4";
          case GL_FLOAT_MAT2x3: return "GL_FLOAT_MAT2x3";
          case GL_FLOAT_MAT3x2: return "GL_FLOAT_MAT3x2";
          case GL_FLOAT_MAT2x4: return "GL_FLOAT_MAT2x4";
          case GL_FLOAT_MAT4x2: return "GL_FLOAT_MAT4x2";
          case GL_FLOAT_MAT3x4: return "GL_FLOAT_MAT3x4";
          case GL_FLOAT_MAT4x3: return "GL_FLOAT_MAT4x3";
          case GL_DOUBLE_MAT2x3: return "GL_DOUBLE_MAT2x3";
          case GL_DOUBLE_MAT3x2: return "GL_DOUBLE_MAT3x2";
          case GL_DOUBLE_MAT2x4: return "GL_DOUBLE_MAT2x4";
          case GL_DOUBLE_MAT4x2: return "GL_DOUBLE_MAT4x2";
          case GL_DOUBLE_MAT3x4: return "GL_DOUBLE_MAT3x4";
          case GL_DOUBLE_MAT4x3: return "GL_DOUBLE_MAT4x3";
          case GL_INT_SAMPLER_1D: return "GL_INT_SAMPLER_1D";
          case GL_INT_SAMPLER_2D: return "GL_INT_SAMPLER_2D";
          case GL_INT_SAMPLER_3D: return "GL_INT_SAMPLER_3D";
          case GL_UNSIGNED_INT_SAMPLER_1D: return "GL_UNSIGNED_INT_SAMPLER_1D";
          case GL_UNSIGNED_INT_SAMPLER_2D: return "GL_UNSIGNED_INT_SAMPLER_2D";
          case GL_UNSIGNED_INT_SAMPLER_3D: return "GL_UNSIGNED_INT_SAMPLER_3D";
          case GL_SAMPLER_1D: return "GL_SAMPLER_1D";
          case GL_SAMPLER_2D: return "GL_SAMPLER_2D";
          case GL_SAMPLER_3D: return "GL_SAMPLER_3D";
          default: return "UNKNOWN TYPE";
      }
  }
} // namespace bk

#endif //BKGL_GL_TYPE_TRAITS_H


