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

#include <bkGL/renderable/plot/MultiLinePlotView.h>

#include <vector>

#include <bk/Signal>

#include <bkGL/renderable/plot/PlotLine.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct MultiLinePlotView::Impl
  {
      std::vector<std::shared_ptr<PlotLine>> lines;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  MultiLinePlotView::MultiLinePlotView()
      : base_type()
  #else

  MultiLinePlotView::MultiLinePlotView(qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /*do nothing*/ }

  MultiLinePlotView::MultiLinePlotView(MultiLinePlotView&&) noexcept = default;

  MultiLinePlotView::~MultiLinePlotView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  unsigned int MultiLinePlotView::num_lines() const
  { return _pdata->lines.size(); }

  std::shared_ptr<PlotLine>& MultiLinePlotView::line(unsigned int i)
  { return _pdata->lines[i]; }

  const std::shared_ptr<PlotLine>& MultiLinePlotView::line(unsigned int i) const
  { return _pdata->lines[i]; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  MultiLinePlotView& MultiLinePlotView::operator=(MultiLinePlotView&&) noexcept = default;

  std::shared_ptr<PlotLine>& MultiLinePlotView::add_line()
  {
      std::shared_ptr<PlotLine>& l = _pdata->lines.emplace_back(std::make_shared<PlotLine>(
      #ifdef BK_LIB_QT_AVAILABLE
          this->_gl
      #endif
      ));

      this->add_datavectorview(l);

      //if (this->is_initialized())
      //{ init(); }

      return l;
  }

  bool MultiLinePlotView::remove_line(unsigned int i)
  {
      if (i < num_lines())
      {
          auto it = _pdata->lines.begin() + i;
          this->remove_datavectorview(*it);
          _pdata->lines.erase(it);
          return true;
      }
      else
      { return false; }
  }

  void MultiLinePlotView::clear_lines()
  {
      for (auto& l: _pdata->lines)
      { this->remove_datavectorview(l); }

      _pdata->lines.clear();

      //if (this->is_initialized())
      //{ init(); }
  }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool MultiLinePlotView::init()
  {
      //clear();

      const bool success = base_type::init();

      if (!success)
      { clear(); }

      return success;
  }

  void MultiLinePlotView::clear()
  { base_type::clear(); }
} // namespace bk


