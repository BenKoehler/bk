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

#ifndef __BK_MATRIX_TYPES_H__
#define __BK_MATRIX_TYPES_H__

#include "Matrix.h"

namespace bk
{
  //====================================================================================================
  //===== STATIC SIZE TYPES
  //====================================================================================================
  /// Matrix
  template<int Rows, int Cols, typename Value_> using ColMat = Matrix<Value_, Rows, Cols, ColMajor>;
  template<int Rows, int Cols, typename Value_> using RowMat = Matrix<Value_, Rows, Cols, RowMajor>;
  template<int Rows, int Cols, typename Value_> using Mat = ColMat<Rows, Cols, Value_>;
  #define MAKE_STATIC_MATRIX_TYPES(N)\
            /* ColMat types */\
            template<typename Value_> using ColMat##N = ColMat<N, N, Value_>;\
            using ColMat##N##ui = ColMat##N <unsigned int>;\
            using ColMat##N##i = ColMat##N <int>;\
            using ColMat##N##f = ColMat##N <float>;\
            using ColMat##N##d = ColMat##N <double>;\
            /* RowMat types */\
            template<typename Value_> using RowMat##N = RowMat<N, N, Value_>;\
            using RowMat##N##ui = RowMat##N <unsigned int>;\
            using RowMat##N##i = RowMat##N <int>;\
            using RowMat##N##f = RowMat##N <float>;\
            using RowMat##N##d = RowMat##N <double>;\
            /* Mat types */\
            template<typename Value_> using Mat##N = ColMat<N, N, Value_>;\
            using Mat##N##ui = Mat##N <unsigned int>;\
            using Mat##N##i = Mat##N <int>;\
            using Mat##N##f = Mat##N <float>;\
            using Mat##N##d = Mat##N <double>;

  MAKE_STATIC_MATRIX_TYPES(2)
  MAKE_STATIC_MATRIX_TYPES(3)
  MAKE_STATIC_MATRIX_TYPES(4)
  MAKE_STATIC_MATRIX_TYPES(5)
  #undef MAKE_STATIC_MATRIX_TYPES

  /// Vector
  template<int N, typename Value_> using ColVec = Matrix<Value_, N, 1, ColMajor>;
  template<int N, typename Value_> using RowVec = Matrix<Value_, 1, N, RowMajor>;
  template<int N, typename Value_> using Vec = ColVec<N, Value_>;
  #define MAKE_STATIC_VECTOR_TYPES(N)\
            /*ColVec types*/\
            template<typename Value_> using ColVec##N = ColVec<N, Value_>;\
            using ColVec##N##ui = ColVec##N <unsigned int>;\
            using ColVec##N##i = ColVec##N <int>;\
            using ColVec##N##f = ColVec##N <float>;\
            using ColVec##N##d = ColVec##N <double>;\
            /*RowVec types*/\
            template<typename Value_> using RowVec##N = RowVec<N, Value_>;\
            using RowVec##N##ui = RowVec##N <unsigned int>;\
            using RowVec##N##i = RowVec##N <int>;\
            using RowVec##N##f = RowVec##N <float>;\
            using RowVec##N##d = RowVec##N <double>;\
            /*Vec types*/\
            template<typename Value_> using Vec##N = Vec<N, Value_>;\
            using Vec##N##ui = Vec##N <unsigned int>;\
            using Vec##N##i = Vec##N <int>;\
            using Vec##N##f = Vec##N <float>;\
            using Vec##N##d = Vec##N <double>;

  MAKE_STATIC_VECTOR_TYPES(2)
  MAKE_STATIC_VECTOR_TYPES(3)
  MAKE_STATIC_VECTOR_TYPES(4)
  MAKE_STATIC_VECTOR_TYPES(5)
  #undef MAKE_STATIC_VECTOR_TYPES

  //====================================================================================================
  //===== DYNAMIC SIZE TYPES
  //====================================================================================================
  template<typename Value_> using MatX = Matrix<Value_>;
  #define MAKE_DYNAMIC_MATRIX_TYPES(NAME)\
  using NAME##Xui = NAME##X<unsigned int>;\
  using NAME##Xi = NAME##X<int>;\
  using NAME##Xf = NAME##X<float>;\
  using NAME##Xd = NAME##X<double>;

  MAKE_DYNAMIC_MATRIX_TYPES(Mat)

  #undef MAKE_DYNAMIC_MATRIX_TYPES
} // namespace bk

#endif // __BK_MATRIX_TYPES_H__



