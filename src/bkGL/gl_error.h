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

#ifndef BKGL_PRINT_ERROR_H
#define BKGL_PRINT_ERROR_H

#include <string>

#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  void BKGL_EXPORT gl_print_error(const std::string& msg = "");
  [[nodiscard]] bool BKGL_EXPORT gl_has_error();
  void BKGL_EXPORT gl_clear_error();

  #ifdef BK_DEBUG_GL
  void /*APIENTRY*/ openglCallbackFunction(GLenum source, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/);
  #endif
} // namespace bk

#endif //BKGL_PRINT_ERROR_H


