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

#ifndef BK_SINGLELINEPLOTVIEW_H
#define BK_SINGLELINEPLOTVIEW_H

#include <memory>

#include "PlotBase.h"

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
      using self_type = SingleLinePlotView;
      using base_type = PlotBase;

    public:
      using value_type = GLfloat;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      std::unique_ptr<Impl> _pdata;

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
      SingleLinePlotView(const self_type& other) = delete;
      SingleLinePlotView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~SingleLinePlotView();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      [[nodiscard]] std::shared_ptr<PlotLine>& get_line();
      [[nodiscard]] const std::shared_ptr<PlotLine>& get_line() const;
      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      //====================================================================================================
      //===== GL
      //====================================================================================================
      [[maybe_unused]] bool init();
      void clear();
  }; // class SingleLinePlotView
} // namespace bk

#endif //BK_SINGLELINEPLOTVIEW_H


