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

#include <bkGL/renderable/plot/AreaPlotView.h>
#include <bkGL/renderable/plot/PlotArea.h>

#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AreaPlotView::Impl
  {
      std::shared_ptr<PlotArea> area;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          : area(std::make_shared<PlotArea>())
          #else

      explicit Impl(qt_gl_functions* gl)
          : area(std::make_shared<PlotArea>(gl))
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

  AreaPlotView::AreaPlotView()
      : base_type()
  #else

  AreaPlotView::AreaPlotView(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  { this->add_datavectorview(_pdata->area); }

  AreaPlotView::AreaPlotView(AreaPlotView&&) noexcept = default;

  AreaPlotView::~AreaPlotView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  std::shared_ptr<PlotArea>& AreaPlotView::area()
  { return _pdata->area; }

  const std::shared_ptr<PlotArea>& AreaPlotView::area() const
  { return _pdata->area; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  AreaPlotView& AreaPlotView::operator=(AreaPlotView&&) noexcept = default;

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool AreaPlotView::init()
  {
      const bool success = base_type::init();

      if (!success)
      { clear(); }

      return success;
  }
} // namespace bk


