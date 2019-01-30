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

#ifndef BK_PLOTABSTRACTDATAVECTORVIEW_H
#define BK_PLOTABSTRACTDATAVECTORVIEW_H

#include <bkGL/renderable/AbstractRenderable.h>

namespace bk::details
{
  class BKGL_EXPORT PlotAbstractDataVectorView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = details::AbstractRenderable;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE
      PlotAbstractDataVectorView();
      #else
      explicit PlotAbstractDataVectorView(qt_gl_functions* gl);
      #endif
      PlotAbstractDataVectorView(const PlotAbstractDataVectorView&) = delete;
      PlotAbstractDataVectorView(PlotAbstractDataVectorView&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~PlotAbstractDataVectorView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] virtual GLfloat x_min() const = 0;
      [[nodiscard]] virtual GLfloat x_max() const = 0;
      [[nodiscard]] virtual GLfloat y_min() const = 0;
      [[nodiscard]] virtual GLfloat y_max() const = 0;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] PlotAbstractDataVectorView& operator=(const PlotAbstractDataVectorView&) = delete;
      [[maybe_unused]] PlotAbstractDataVectorView& operator=(PlotAbstractDataVectorView&&) noexcept;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      [[maybe_unused]] virtual bool init() = 0;
      virtual void clear() = 0;

  }; // class PlotAbstractDataVectorView
} // namespace bk::details

#endif //BK_PLOTABSTRACTDATAVECTORVIEW_H


