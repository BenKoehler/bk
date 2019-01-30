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

#ifndef BK_MULTILINEPLOTVIEW_H
#define BK_MULTILINEPLOTVIEW_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/plot/PlotBase.h>

namespace bk
{
  // -------------------- forward declaration
  class PlotLine;
  // -------------------- forward declaration END

  class BKGL_EXPORT MultiLinePlotView : public PlotBase
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
      explicit MultiLinePlotView();
      #else
      explicit MultiLinePlotView(qt_gl_functions* gl);
      #endif
      MultiLinePlotView(const MultiLinePlotView&) = delete;
      MultiLinePlotView(MultiLinePlotView&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~MultiLinePlotView();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] unsigned int num_lines() const;
      [[nodiscard]] std::shared_ptr<PlotLine>& line(unsigned int i);
      [[nodiscard]] const std::shared_ptr<PlotLine>& line(unsigned int i) const;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] MultiLinePlotView& operator=(const MultiLinePlotView&) = delete;
      [[maybe_unused]] MultiLinePlotView& operator=(MultiLinePlotView&&) noexcept;
      /// @}

      [[maybe_unused]] std::shared_ptr<PlotLine>& add_line();
      [[maybe_unused]] bool remove_line(unsigned int i);
      void clear_lines();
      
      //====================================================================================================
      //===== GL
      //====================================================================================================
      [[maybe_unused]] bool init();
      void clear();
  }; // class MultiLinePlotView
} // namespace bk

#endif //BK_MULTILINEPLOTVIEW_H


