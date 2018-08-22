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

#include <bkGL/renderable/image/GrayImagePlusSegmentationSliceView.h>

#include <algorithm>
#include <cmath>

#include <bk/Image>
#include <bk/ThreadPool>

#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkGL/buffer/SSBO.h>
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
  struct GrayImagePlusSegmentationSliceView::Impl
  {
      SSBO ssbo;
      Shader shader_seg_contour;
      bool slice_changed;
      segmentation_type seg;

      #ifndef BK_LIB_QT_AVAILABLE
      Impl() :
      #else

      explicit Impl(bk::qt_gl_functions* gl)
          : ssbo(gl),
            shader_seg_contour(gl),
      #endif
            slice_changed(false)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl& operator=(const Impl&) = delete;
  };

  #ifndef BK_LIB_QT_AVAILABLE

  GrayImagePlusSegmentationSliceView::GrayImagePlusSegmentationSliceView()
  : base_type()
  #else

  GrayImagePlusSegmentationSliceView::GrayImagePlusSegmentationSliceView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  { _pdata->ssbo.set_usage_STATIC_DRAW(); }

  GrayImagePlusSegmentationSliceView::GrayImagePlusSegmentationSliceView(self_type&&) noexcept = default;

  GrayImagePlusSegmentationSliceView::~GrayImagePlusSegmentationSliceView()
  { /* do nothing */ };

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  auto GrayImagePlusSegmentationSliceView::_seg() -> segmentation_type&
  { return _pdata->seg; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void GrayImagePlusSegmentationSliceView::set_slice_impl(unsigned int /*z*/)
  {
      _pdata->slice_changed = true;
      update_ssbo();
  }

  auto GrayImagePlusSegmentationSliceView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void GrayImagePlusSegmentationSliceView::init()
  {
      base_type::init();
      init_shader();
      init_shader_seg_contour();

      _pdata->seg.set_size(xmax() + 1, ymax() + 1, zmax() + 1);
  }

  bool GrayImagePlusSegmentationSliceView::init_shader_seg_contour()
  {
      using SL = details::ShaderLibrary::segmentation::contour;
      return _pdata->shader_seg_contour.init_from_sources(SL::vert(), SL::frag());
  }

  void GrayImagePlusSegmentationSliceView::init_ssbo()
  {
      clear_ssbo();

      if (_pdata->seg.num_values() <= 1)
      { return; }

      const unsigned int N = (xmax() + 1) * (ymax() + 1);
      std::vector<ssbo_GLfloat> ssbo(N, 0);
      _pdata->ssbo.init(ssbo.data(), N * sizeof(ssbo_GLfloat));

      _pdata->slice_changed = true;
      update_ssbo();
  }

  void GrayImagePlusSegmentationSliceView::update_ssbo()
  {
      if (_pdata->seg.num_values() <= 1 || !_pdata->slice_changed)
      { return; }

      bool do_update = false;
      ssbo_GLfloat* ssbodata = _pdata->ssbo.map_write_only<ssbo_GLfloat>();
      if (ssbodata != nullptr)
      {
          GLuint cnt = 0;

          //for (GLuint y = 0; y < static_cast<GLuint>(_size[1]); ++y)
          for (int y = static_cast<int>(_image.geometry().size(1) - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_image.geometry().size(0)); ++x)
              { ssbodata[cnt++] = _pdata->seg(x, y, zcurrent()) != 0 ? 1 : 0; }
          }

          _pdata->ssbo.unmap_and_release();
      }

      _pdata->slice_changed = false;

      if (do_update)
      { this->emit_signal_update_required(); }
  }

  void GrayImagePlusSegmentationSliceView::clear()
  {
      base_type::clear();
      clear_shader_seg_contour();
      clear_ssbo();

      _pdata->seg.set_size(1, 1, 1);
      for (auto& x: _pdata->seg)
      {x = 0;}
  }

  void GrayImagePlusSegmentationSliceView::clear_shader_seg_contour()
  { _pdata->shader_seg_contour.clear(); }

  void GrayImagePlusSegmentationSliceView::clear_ssbo()
  { _pdata->ssbo.clear(); }

  void GrayImagePlusSegmentationSliceView::draw_impl()
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
      _ssbo_intensity().release_from_base();
      BK_QT_GL glEnable(GL_BLEND);
      BK_QT_GL glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      _pdata->ssbo.bind_to_base(1);
      _pdata->shader_seg_contour.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
      _pdata->shader_seg_contour.release();
      _pdata->ssbo.release_from_base();
      _vao().release();
      _ubo().release_from_base();
      BK_QT_GL glPopAttrib();
      if (_show_tf())
      { _tf_view().draw(); }
  }
} // namespace bk


