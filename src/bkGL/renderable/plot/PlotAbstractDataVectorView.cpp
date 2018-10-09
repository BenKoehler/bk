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

#include <bkGL/renderable/plot/PlotAbstractDataVectorView.h>

#include <bk/Signal>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  PlotAbstractDataVectorView::PlotAbstractDataVectorView()
      : base_type()
  #else

  PlotAbstractDataVectorView::PlotAbstractDataVectorView(qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /*do nothing*/ }

  PlotAbstractDataVectorView::PlotAbstractDataVectorView(PlotAbstractDataVectorView&&) noexcept = default;

  PlotAbstractDataVectorView::~PlotAbstractDataVectorView() = default;

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotAbstractDataVectorView& PlotAbstractDataVectorView::operator=(PlotAbstractDataVectorView&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void PlotAbstractDataVectorView::on_resize(GLint /*w*/, GLint /*h*/)
  { /*do nothing*/ }

  void PlotAbstractDataVectorView::on_oit_enabled(bool /*b*/)
  { /*do nothing*/ }

  void PlotAbstractDataVectorView::on_animation_enabled(bool /*b*/)
  { /*do nothing*/ }

  void PlotAbstractDataVectorView::on_modelview_changed(bool /*b*/)
  { /*do nothing*/ }

  void PlotAbstractDataVectorView::on_visible_changed(bool /*b*/)
  { /*do nothing*/ }

  void PlotAbstractDataVectorView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_mouse_wheel_up()
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_mouse_wheel_down()
  { /* do nothing */ }

  void PlotAbstractDataVectorView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  //void PlotAbstractDataVectorView::draw_impl()
  //{ /*do nothing*/ }
} // namespace bk::details


