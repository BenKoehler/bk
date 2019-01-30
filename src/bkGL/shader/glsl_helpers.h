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

#ifndef BKGL_GLSL_HELPERS_H
#define BKGL_GLSL_HELPERS_H

#include <bkGL/gl_definitions.h>
#include <bkGL/shader/EShaderType.h>

namespace bk::details
{
  [[nodiscard]] static constexpr const char* glenum_to_glsl_type_name(GLenum t)
  {
      switch (t)
      {
          default: [[fallthrough]];
          case GL_SHORT: [[fallthrough]];
          case GL_UNSIGNED_SHORT: [[fallthrough]];
          case GL_INT: return "int";
          case GL_BYTE: [[fallthrough]];
          case GL_UNSIGNED_BYTE: return "bool";
          case GL_UNSIGNED_INT: return "uint";
          case GL_FLOAT: return "float";
          case GL_DOUBLE: return "double";

          case GL_INT_VEC2: return "ivec2";
          case GL_INT_VEC3: return "ivec3";
          case GL_INT_VEC4: return "ivec4";
          case GL_UNSIGNED_INT_VEC2: return "uvec2";
          case GL_UNSIGNED_INT_VEC3: return "uvec3";
          case GL_UNSIGNED_INT_VEC4: return "uvec4";
          case GL_FLOAT_VEC2: return "vec2";
          case GL_FLOAT_VEC3: return "vec3";
          case GL_FLOAT_VEC4: return "vec4";
          case GL_DOUBLE_VEC2: return "dvec2";
          case GL_DOUBLE_VEC3: return "dvec3";
          case GL_DOUBLE_VEC4: return "dvec4";

          case GL_SAMPLER_1D: return "sampler1D";
          case GL_SAMPLER_2D: return "sampler2D";
          case GL_SAMPLER_3D: return "sampler3D";
          case GL_INT_SAMPLER_1D: return "isampler1D";
          case GL_INT_SAMPLER_2D: return "isampler2D";
          case GL_INT_SAMPLER_3D: return "isampler3D";
          case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D";
          case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D";
          case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D";

          case GL_FLOAT_MAT2: return "mat2";
          case GL_FLOAT_MAT3: return "mat3";
          case GL_FLOAT_MAT4: return "mat4";

          case GL_DOUBLE_MAT2: return "dmat2";
          case GL_DOUBLE_MAT3: return "dmat3";
          case GL_DOUBLE_MAT4: return "dmat4";

          case GL_FLOAT_MAT2x3: return "mat2x3";
          case GL_DOUBLE_MAT2x3: return "dmat2x3";

          case GL_FLOAT_MAT2x4: return "mat2x4";
          case GL_DOUBLE_MAT2x4: return "dmat2x4";

          case GL_FLOAT_MAT3x2: return "mat3x2";
          case GL_DOUBLE_MAT3x2: return "dmat3x2";

          case GL_FLOAT_MAT3x4: return "mat3x4";
          case GL_DOUBLE_MAT3x4: return "dmat3x4";

          case GL_FLOAT_MAT4x2: return "mat4x2";
          case GL_DOUBLE_MAT4x2: return "dmat4x2";

          case GL_FLOAT_MAT4x3: return "mat4x3";
          case GL_DOUBLE_MAT4x3: return "dmat4x3";
      }
  }
} // namespace bk::details

#endif //BKGL_GLSL_HELPERS_H
