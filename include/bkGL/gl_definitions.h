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

#ifndef BK_GL_DEFINITIONS_H
#define BK_GL_DEFINITIONS_H

//====================================================================================================
//===== DEFINITIONS
//====================================================================================================
#ifndef BK_LIB_QT_AVAILABLE

    #include <GL/glew.h>

    #define BK_QT_GL
    #define BK_QT_GL_STATIC static

#else // ifdef BK_LIB_QT_AVAILABLE

    #ifdef BK_DEBUG_GL
        #include <QtGui/QOpenGLFunctions_4_5_Compatibility>
namespace bk
{
  using qt_gl_functions = QOpenGLFunctions_4_5_Compatibility;
} // namespace bk
    #else

        #include <QtGui/QOpenGLFunctions_4_3_Compatibility>

namespace bk
{
  using qt_gl_functions = QOpenGLFunctions_4_3_Compatibility;
} // namespace bk
    #endif

    #define BK_QT_GL _gl->
    #define BK_QT_GL_STATIC
#endif

#include <GL/gl.h>
#include "gl_error.h"

#endif //BK_GL_DEFINITIONS_H


