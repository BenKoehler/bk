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

#include <bkGL/renderable/image/GrayImageLiveThresholdSliceView.h>

#include <algorithm>
#include <cmath>

#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkGL/UBOSliceView.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct GrayImageLiveThresholdSliceView::Impl
  {
      Shader shader_live_threshold_overlay;
      bool slice_changed;
      GLfloat threshold;

          #ifndef BK_LIB_QT_AVAILABLE
      Impl() :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : shader_live_threshold_overlay(gl),
          #endif
            slice_changed(false),
            threshold(0.5)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl& operator=(const Impl&) = delete;
  };

  #ifndef BK_LIB_QT_AVAILABLE

  GrayImageLiveThresholdSliceView::GrayImageLiveThresholdSliceView()
  : base_type(),
  _pdata(std::make_unique<Impl>())
  #else

  GrayImageLiveThresholdSliceView::GrayImageLiveThresholdSliceView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  { /* do nothing */ }

  GrayImageLiveThresholdSliceView::GrayImageLiveThresholdSliceView(self_type&&) noexcept = default;

  GrayImageLiveThresholdSliceView::~GrayImageLiveThresholdSliceView()
  { /* do nothing */ };

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  auto GrayImageLiveThresholdSliceView::threshold() const -> GLfloat
  { return _pdata->threshold; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void GrayImageLiveThresholdSliceView::set_threshold(GLfloat threshold)
  {
      _pdata->threshold = threshold;

      if (this->is_initialized())
      {
          _ubo().set_threshold(_pdata->threshold);
          _ubo().release();

          this->emit_signal_update_required();
      }
  }

  void GrayImageLiveThresholdSliceView::set_slice_impl(unsigned int /*z*/)
  { _pdata->slice_changed = true; }

  auto GrayImageLiveThresholdSliceView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void GrayImageLiveThresholdSliceView::init()
  {
      base_type::init();
      init_shader();
      init_shader_live_threshold_overlay();
  }

  bool GrayImageLiveThresholdSliceView::init_shader_live_threshold_overlay()
  {
      using SL = details::ShaderLibrary::segmentation::live_threshold;
      return _pdata->shader_live_threshold_overlay.init_from_sources(SL::vert(), SL::frag());
  }

  void GrayImageLiveThresholdSliceView::clear()
  {
      base_type::clear();
      clear_shader_live_threshold_overlay();
  }

  void GrayImageLiveThresholdSliceView::clear_shader_live_threshold_overlay()
  { _pdata->shader_live_threshold_overlay.clear(); }

  void GrayImageLiveThresholdSliceView::draw_impl()
  {
      BK_QT_GL glPushAttrib(GL_DEPTH_TEST | GL_PRIMITIVE_RESTART | GL_BLEND);

      _ubo().bind_to_default_base();

      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _vao().bind();
      _ssbo_intensity().bind_to_base(1);

      _shader().bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
      _shader().release();

      BK_QT_GL glEnable(GL_BLEND);
      BK_QT_GL glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      _pdata->shader_live_threshold_overlay.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
      _pdata->shader_live_threshold_overlay.release();

      _ssbo_intensity().release_from_base();
      _vao().release();

      _ubo().release_from_base();

      BK_QT_GL glPopAttrib();

      if (_show_tf())
      { _tf_view().draw(); }
  }
} // namespace bk


