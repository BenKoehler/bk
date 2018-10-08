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
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : line(std::make_shared<PlotLine>(gl))
          #endif
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE
  SingleLinePlotView::SingleLinePlotView()
    : base_type(),
      _pdata(std::make_unique<Impl>())
  #else

  SingleLinePlotView::SingleLinePlotView(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  { this->add_datavectorview(_pdata->line); }

  SingleLinePlotView::SingleLinePlotView(self_type&&) = default;

  SingleLinePlotView::~SingleLinePlotView()
  { /*do nothing*/ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  std::shared_ptr<PlotLine>& SingleLinePlotView::get_line()
  { return _pdata->line; }

  const std::shared_ptr<PlotLine>& SingleLinePlotView::get_line() const
  { return _pdata->line; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto SingleLinePlotView::operator=(self_type&& other) -> self_type& = default;

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool SingleLinePlotView::init()
  {
      //clear();

      const bool success = base_type::init();
      this->set_initialized(success);

      if (!success)
      { clear(); }

      return success;
  }

  void SingleLinePlotView::clear()
  {
      base_type::clear();
      this->set_initialized(false);
  }
} // namespace bk


