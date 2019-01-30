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

#include "SingleLinePlotView.h"
#include "PlotLine.h"
#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct SingleLinePlotView::Impl
  {
      std::shared_ptr<PlotLine> line;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          : line(std::make_shared<PlotLine>())
          #else

      explicit Impl(qt_gl_functions* gl)
          : line(std::make_shared<PlotLine>(gl))
          #endif
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  SingleLinePlotView::SingleLinePlotView()
      : base_type()
  #else

  SingleLinePlotView::SingleLinePlotView(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  { this->add_datavectorview(_pdata->line); }

  SingleLinePlotView::SingleLinePlotView(SingleLinePlotView&&) noexcept = default;

  SingleLinePlotView::~SingleLinePlotView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  std::shared_ptr<PlotLine>& SingleLinePlotView::line()
  { return _pdata->line; }

  const std::shared_ptr<PlotLine>& SingleLinePlotView::line() const
  { return _pdata->line; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  SingleLinePlotView& SingleLinePlotView::operator=(SingleLinePlotView&&) noexcept = default;

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool SingleLinePlotView::init()
  {
      const bool success = base_type::init();

      if (!success)
      { clear(); }

      return success;
  }
} // namespace bk


