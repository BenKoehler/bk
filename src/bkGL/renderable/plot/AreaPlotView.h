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

#ifndef BK_AREAPLOTVIEW_H
#define BK_AREAPLOTVIEW_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/plot/PlotBase.h>

namespace bk
{
  // -------------------- forward declaration
  class PlotArea;
  // -------------------- forward declaration END

  class BKGL_EXPORT AreaPlotView : public PlotBase
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
      AreaPlotView();
      #else
      explicit AreaPlotView(qt_gl_functions* gl);
      #endif
      AreaPlotView(const AreaPlotView&) = delete;
      AreaPlotView(AreaPlotView&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~AreaPlotView();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] std::shared_ptr<PlotArea>& area();
      [[nodiscard]] const std::shared_ptr<PlotArea>& area() const;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] AreaPlotView& operator=(const AreaPlotView&) = delete;
      [[maybe_unused]] AreaPlotView& operator=(AreaPlotView&&) noexcept;
      /// @}

      //====================================================================================================
      //===== GL
      //====================================================================================================
      [[maybe_unused]] bool init();
  }; // class AreaPlotView
} // namespace bk

#endif //BK_AREAPLOTVIEW_H


