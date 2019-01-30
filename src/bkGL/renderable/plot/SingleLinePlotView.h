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

#ifndef BK_SINGLELINEPLOTVIEW_H
#define BK_SINGLELINEPLOTVIEW_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/plot/PlotBase.h>

namespace bk
{
  // -------------------- forward declaration
  class PlotLine;
  // -------------------- forward declaration END

  class BKGL_EXPORT SingleLinePlotView : public PlotBase
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using base_type = PlotBase;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE
      SingleLinePlotView();
      #else
      explicit SingleLinePlotView(qt_gl_functions* gl);
      #endif
      SingleLinePlotView(const SingleLinePlotView&) = delete;
      SingleLinePlotView(SingleLinePlotView&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~SingleLinePlotView();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] std::shared_ptr<PlotLine>& line();
      [[nodiscard]] const std::shared_ptr<PlotLine>& line() const;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] SingleLinePlotView& operator=(const SingleLinePlotView&) = delete;
      [[maybe_unused]] SingleLinePlotView& operator=(SingleLinePlotView&&) noexcept;
      /// @}

      //====================================================================================================
      //===== GL
      //====================================================================================================
      [[maybe_unused]] bool init();
  }; // class SingleLinePlotView
} // namespace bk

#endif //BK_SINGLELINEPLOTVIEW_H


