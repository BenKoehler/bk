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

#include <bkGL/renderable/image/segmenter/GrayImageGraphCutView.h>

#include <algorithm>
#include <cmath>

#include <bk/Image>
#include <bk/Signal>
#include <bk/ThreadPool>
#include <bkMath/functions/list_grid_id_conversion.h>

#include <bkGL/buffer/SSBO.h>
#include <bkGL/UBOSliceView.h>
#include <bkGL/Mouse.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/renderable/image/segmenter/GraphCutDrawMode.h>
#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Progress>
    #include <bk/Localization>

#endif

namespace bk
{
  //====================================================================================================
  //===== ENUMS
  //====================================================================================================
  enum GraphCutInteractionMode_ : int
  {
      GraphCutInteractionMode_TransferFunction = 1, GraphCutInteractionMode_GraphCut = 2
  };

  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct GrayImageGraphCutView::Impl
  {
      SSBO ssbo_gc;
      Shader shader_gc;
      Shader shader_seg_contour;
      graph_cut_type* gc;
      unsigned int pencil_size;
      details::GraphCutDrawMode draw_mode;
      bool auto_update_segmentation;
      bool inout_changed;
      bool gc_is_running;
      bool seg_changed;
      bool slice_changed;
      segmentation_type seg;
      segmentation_type in;
      segmentation_type out;
      bk::Signal<> s_gc_finished;
      bk::Signal<> s_input_changed;
      GraphCutInteractionMode_ interaction_mode;
      bool show_inout_overlay;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : ssbo_gc(gl),
            shader_gc(gl),
            shader_seg_contour(gl),
          #endif
          gc(nullptr),
          pencil_size(4),
          draw_mode(details::GraphCutDrawMode::Inside),
          auto_update_segmentation(false),
          inout_changed(false),
          gc_is_running(false),
          seg_changed(false),
          slice_changed(false),
          interaction_mode(GraphCutInteractionMode_GraphCut),
          show_inout_overlay(true)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl& operator=(const Impl&) = delete;
  };

  #ifndef BK_LIB_QT_AVAILABLE

  GrayImageGraphCutView::GrayImageGraphCutView()
      : base_type()
  #else

  GrayImageGraphCutView::GrayImageGraphCutView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->ssbo_gc.set_usage_DYNAMIC_DRAW();
      _pdata->s_gc_finished.connect_member([&]()
                                           {
                                               //this->update_ssbo_gc();

                                               _pdata->seg_changed = true;

                                               if (this->_pdata->inout_changed && _pdata->auto_update_segmentation)
                                               { this->update_gc(); }
                                           });
  }

  GrayImageGraphCutView::GrayImageGraphCutView(self_type&&) noexcept = default;

  GrayImageGraphCutView::~GrayImageGraphCutView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  auto GrayImageGraphCutView::graph_cut() const -> const graph_cut_type*
  { return _pdata->gc; }

  auto GrayImageGraphCutView::_seg() -> segmentation_type&
  { return _pdata->seg; }

  bool& GrayImageGraphCutView::_seg_changed()
  { return _pdata->seg_changed; }

  unsigned int GrayImageGraphCutView::pencil_size() const
  { return _pdata->pencil_size; }

  auto GrayImageGraphCutView::segmentation() const -> const segmentation_type&
  { return _pdata->seg; }

  /// @{ -------------------------------------------------- GET IN OUT IMAGES
  auto GrayImageGraphCutView::inside() const -> const segmentation_type&
  { return _pdata->in; }

  auto GrayImageGraphCutView::outside() const -> const segmentation_type&
  { return _pdata->out; }
  /// @}

  bk::Signal<>& GrayImageGraphCutView::signal_graph_cut_finished()
  { return _pdata->s_gc_finished; }

  bk::Signal<>& GrayImageGraphCutView::signal_input_changed()
  { return _pdata->s_input_changed; }

  bool GrayImageGraphCutView::interaction_mode_is_graph_cut() const
  { return _pdata->interaction_mode == GraphCutInteractionMode_GraphCut; }

  bool GrayImageGraphCutView::interaction_mode_is_transfer_function() const
  { return _pdata->interaction_mode == GraphCutInteractionMode_TransferFunction; }

  bool GrayImageGraphCutView::input_changed() const
  { return _pdata->inout_changed; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void GrayImageGraphCutView::set_slice_impl(unsigned int /*z*/)
  {
      _pdata->slice_changed = true;
      update_ssbo_gc();
  }

  void GrayImageGraphCutView::set_graph_cut(graph_cut_type* gc)
  { _pdata->gc = gc; }

  void GrayImageGraphCutView::set_pencil_size(unsigned int s)
  { _pdata->pencil_size = std::max(static_cast<unsigned int>(1), s); }

  void GrayImageGraphCutView::set_draw_mode_inside()
  { _pdata->draw_mode = details::GraphCutDrawMode::Inside; }

  void GrayImageGraphCutView::set_draw_mode_outside()
  { _pdata->draw_mode = details::GraphCutDrawMode::Outside; }

  void GrayImageGraphCutView::set_draw_mode_erase()
  { _pdata->draw_mode = details::GraphCutDrawMode::Erase; }

  void GrayImageGraphCutView::set_auto_update_segmentation(bool b)
  {
      const bool previous_config = _pdata->auto_update_segmentation;
      _pdata->auto_update_segmentation = b;

      //bool do_update = false;
      if (!previous_config && b && _pdata->gc != nullptr && _pdata->inout_changed)
      {
          //_gc->run();
          //do_update = true;

          update_gc();
      }

      //_auto_update_segmentation = b;
      //
      //if (do_update)
      //{
      //    _s_update_required.emit_signal();
      //}
  }

  auto GrayImageGraphCutView::operator=(self_type&& other) noexcept -> self_type& = default;

  void GrayImageGraphCutView::set_inside_outside_from_graph_cut(bool resetSegmentation)
  {
      #pragma omp parallel sections
      {
          #pragma omp section
          {
              for (auto& x: _pdata->in)
              { x = 0; }

              for (auto it = _pdata->gc->nodes_connected_to_source().begin(); it != _pdata->gc->nodes_connected_to_source().end(); ++it)
              { _pdata->in(*it) = 1; }
          }
          #pragma omp section
          {
              for (auto& x: _pdata->out)
              { x = 0; }

              for (auto it = _pdata->gc->nodes_connected_to_sink().begin(); it != _pdata->gc->nodes_connected_to_sink().end(); ++it)
              { _pdata->out(*it) = 1; }
          }
          #pragma omp section
          {
              if (resetSegmentation)
              {
                  for (auto& x: _pdata->seg)
                  { x = 0; }
              }
          }
      }

      _pdata->inout_changed = true;

      update_ssbo_gc();
      if (_pdata->auto_update_segmentation)
      {
          update_gc(); // runs in thread
      }
  }

  void GrayImageGraphCutView::set_interaction_mode_graph_cut()
  { _pdata->interaction_mode = GraphCutInteractionMode_GraphCut; }

  void GrayImageGraphCutView::set_interaction_mode_transfer_function()
  { _pdata->interaction_mode = GraphCutInteractionMode_TransferFunction; }

  void GrayImageGraphCutView::set_show_inout_overlay(bool show)
  {
      if (_pdata->show_inout_overlay != show)
      {
          _pdata->show_inout_overlay = show;

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void GrayImageGraphCutView::process_onscreen_drawing(GLuint screen_x, GLuint screen_y, bool mouse_was_released)
  {
      if (_pdata->gc == nullptr || _image.num_values() <= 1)
      { return; }

      GLuint img_x;
      GLuint img_y;
      mouseXY_to_imageXY(screen_x, screen_y, img_x, img_y);

      //const int radius_ceiled = static_cast<int>(std::ceil(_pdata->pencil_size));
      //for (int dy = -radius_ceiled; dy <= radius_ceiled; ++dy)
      //{
      //    for (int dx = -radius_ceiled; dx <= radius_ceiled; ++dx)
      //    {
      //const int rmin = -static_cast<int>(_pdata->pencil_size/2);
      //const int rmax = -rmin + (_pdata->pencil_size % 2 ? 1 : 0);
      const int rmin = -static_cast<int>(_pdata->pencil_size / 2);
      const int rmax = -rmin + (_pdata->pencil_size % 2 ? 0 : -1);
      for (int dy = rmin; dy <= rmax; ++dy)
      {
          for (int dx = rmin; dx <= rmax; ++dx)
          {
              const GLuint idx = static_cast<GLuint>(std::clamp(static_cast<int>(img_x) + dx, 0, static_cast<int>(xmax())));
              const GLuint idy = static_cast<GLuint>(std::clamp(static_cast<int>(img_y) + dy, 0, static_cast<int>(ymax())));
              //const GLuint lid = grid_to_list_id(_image.size(), idx, idy, zcurrent(), tcurrent());

              switch (_pdata->draw_mode)
              {
                  case details::GraphCutDrawMode::Inside:
                  {
                      _pdata->in(idx, idy, zcurrent()) = 1;
                      _pdata->out(idx, idy, zcurrent()) = 0;
                      _pdata->inout_changed = true;
                      break;
                  }
                  case details::GraphCutDrawMode::Outside:
                  {
                      _pdata->in(idx, idy, zcurrent()) = 0;
                      _pdata->out(idx, idy, zcurrent()) = 1;
                      _pdata->inout_changed = true;
                      break;
                  }
                  case details::GraphCutDrawMode::Erase:
                  {
                      _pdata->in(idx, idy, zcurrent()) = 0;
                      _pdata->out(idx, idy, zcurrent()) = 0;
                      _pdata->inout_changed = true;
                      break;
                  }
              }
          }
      }

      update_ssbo_gc();

      if (mouse_was_released)
      {
          _pdata->s_input_changed.emit_signal();

          if (_pdata->auto_update_segmentation)
          {
              update_gc(); // runs in thread
          }
      }
  }

  void GrayImageGraphCutView::init()
  {
      base_type::init();
      init_shader_gc();
      init_shader_seg_contour();
      init_ssbo_gc();

      //_seg_in_out.set_size(xmax() + 1, ymax() + 1, zmax() + 1);
      _pdata->seg.set_size(xmax() + 1, ymax() + 1, zmax() + 1);
      _pdata->in.set_size(xmax() + 1, ymax() + 1, zmax() + 1);
      _pdata->out.set_size(xmax() + 1, ymax() + 1, zmax() + 1);
  }

  bool GrayImageGraphCutView::init_shader_gc()
  {
      using SL = details::ShaderLibrary::segmentation::inout_overlay;
      return _pdata->shader_gc.init_from_sources(SL::vert(), SL::frag());
  }

  bool GrayImageGraphCutView::init_shader_seg_contour()
  {
      using SL = details::ShaderLibrary::segmentation::contour;
      return _pdata->shader_seg_contour.init_from_sources(SL::vert(), SL::frag());
  }

  void GrayImageGraphCutView::init_ssbo_gc()
  {
      clear_ssbo_gc();

      const unsigned int N = (xmax() + 1) * (ymax() + 1);
      std::vector<GLuint> zero(N, 0);

      _pdata->ssbo_gc.init(zero.data(), N * sizeof(GLuint));
  }

  void GrayImageGraphCutView::clear()
  {
      base_type::clear();
      clear_shader_gc();
      clear_shader_seg_contour();
      clear_ssbo_gc();

      _pdata->seg.set_size(1, 1, 1);
      _pdata->in.set_size(1, 1, 1);
      _pdata->out.set_size(1, 1, 1);
      _pdata->seg[0] = 0;
      _pdata->in[0] = 0;
      _pdata->out[0] = 0;
  }

  void GrayImageGraphCutView::clear_segmentation()
  {
      #pragma omp parallel sections
      {
          #pragma omp section
          {
              for (auto& x: _pdata->seg)
              { x = 0; }
          }
          #pragma omp section
          {
              for (auto& x: _pdata->in)
              { x = 0; }
          }
          #pragma omp section
          {
              for (auto& x: _pdata->out)
              { x = 0; }
          }
      };

      _pdata->inout_changed = true;
      _pdata->seg_changed = true;
      _pdata->gc_is_running = false;

      if (_image.num_values() <= 1)
      { return; }

      GLuint* inout = _pdata->ssbo_gc.map_write_only<GLuint>();
      if (inout != nullptr)
      {
          unsigned int cnt = 0;
          for (int y = static_cast<int>(_image.geometry().size(1) - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_image.geometry().size(0)); ++x)
              { inout[cnt++] = 0; }
          }
          _pdata->ssbo_gc.unmap_and_release();
      }

      this->emit_signal_update_required();
  }

  void GrayImageGraphCutView::clear_shader_gc()
  { _pdata->shader_gc.clear(); }

  void GrayImageGraphCutView::clear_shader_seg_contour()
  { _pdata->shader_seg_contour.clear(); }

  void GrayImageGraphCutView::clear_ssbo_gc()
  { _pdata->ssbo_gc.clear(); }

  void GrayImageGraphCutView::update_gc(double narrow_band_width_percent, unsigned int narrow_band_min_pixel_width)
  {
      if (!_pdata->gc_is_running && _pdata->gc != nullptr)
      {
          _pdata->gc_is_running = true;
          _pdata->inout_changed = false;

          #pragma omp parallel
          {
              #pragma omp single nowait
              {
                  #ifdef BK_EMIT_PROGRESS
                  bk::Progress& prog_input = bk_progress.emplace_task(2, ___("Setting graph cut input"));
                  #endif

                  _pdata->gc->clear_source_nodes();
                  _pdata->gc->clear_sink_nodes();

                  #ifdef BK_EMIT_PROGRESS
                  prog_input.increment(1);
                  #endif

                  for (GLuint i = 0; i < static_cast<GLuint>(_pdata->seg.num_values()); ++i)
                  {
                      if (_pdata->in[i] != 0)
                      {
                          const auto gid = bk::list_to_grid_id(_pdata->in.size(), i);
                          _pdata->gc->add_source_node(gid[0], gid[1], gid[2]);
                      }
                      else if (_pdata->out[i] != 0)
                      {
                          const auto gid = bk::list_to_grid_id(_pdata->in.size(), i);
                          _pdata->gc->add_sink_node(gid[0], gid[1], gid[2]);
                      }
                  }

                  if (narrow_band_width_percent < 1)
                  { _pdata->gc->narrow_band_sink_ids(narrow_band_width_percent, narrow_band_min_pixel_width); }

                  #ifdef BK_EMIT_PROGRESS
                  prog_input.set_finished();
                  #endif

                  _pdata->gc->run();

                  /*
                  * - write segmentation
                  * - enforce that regions drawn as inside/outside are 1/0 in segmentation
                  */
                  #pragma omp parallel for
                  for (GLuint i = 0; i < static_cast<GLuint>(_pdata->seg.num_values()); ++i)
                  {
                      if (_pdata->in[i] != 0)
                      { _pdata->seg[i] = 1; }
                      else if (_pdata->out[i] != 0)
                      { _pdata->seg[i] = 0; }
                      else
                      {
                          const auto gid = bk::list_to_grid_id(_pdata->in.size(), i);
                          _pdata->seg[i] = _pdata->gc->is_in_segmentation(gid[0], gid[1], gid[2]) ? 1 : 0;
                      }
                  }

                  #ifdef BK_EMIT_PROGRESS
                  bk::Progress& prog_postpro = bk_progress.emplace_task(6, ___("Post-processing graph cut result"));
                  #endif

                  /*
                   * perform 3x3x3 closing and opening
                   */
                  bk::MorphologicalClosingAndOpeningImageFilter f_morph(3, 3);
                  _pdata->seg = _pdata->seg.filter(f_morph);

                  #ifdef BK_EMIT_PROGRESS
                  prog_postpro.increment(2);
                  #endif

                  _pdata->seg = bk::ConnectedComponentAnalysisKeepLargestRegionImageFilter::apply(_pdata->seg);

                  #ifdef BK_EMIT_PROGRESS
                  prog_postpro.increment(1);
                  #endif

                  _pdata->seg = bk::FillHolesInSegmentationFilter::apply(_pdata->seg);

                  #ifdef BK_EMIT_PROGRESS
                  prog_postpro.increment(1);
                  #endif

                  /*
                   * - enforce that regions drawn as inside/outside are 1/0 in segmentation (also after filtering)
                   */
                  #pragma omp parallel for
                  for (GLuint i = 0; i < static_cast<GLuint>(_pdata->seg.num_values()); ++i)
                  {
                      if (_pdata->in[i] != 0)
                      { _pdata->seg[i] = 1; }
                      else if (_pdata->out[i] != 0)
                      { _pdata->seg[i] = 0; }
                      //else
                      //{
                      //    const auto gid = bk::list_to_grid_id(_pdata->in.size(), i);
                      //    _pdata->seg[i] = _pdata->gc->is_in_segmentation(gid[0], gid[1], gid[2]) ? 1 : 0;
                      //}
                  }

                  #ifdef BK_EMIT_PROGRESS
                  prog_postpro.set_finished();
                  #endif

                  _pdata->gc_is_running = false;
                  _pdata->s_gc_finished.emit_signal();
              }
          }
      }
  }

  void GrayImageGraphCutView::update_ssbo_gc()
  {
      if (_pdata->gc == nullptr || _image.num_values() <= 1 || (!_pdata->inout_changed && !_pdata->slice_changed && !_pdata->seg_changed))
      { return; }

      bool do_update = false;
      GLuint* inout = _pdata->ssbo_gc.map_write_only<GLuint>();
      if (inout != nullptr)
      {
          do_update = true;
          GLuint cnt = 0;

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(1 + _image.geometry().size(1) * _image.geometry().size(0));
          #endif

          const bool isRunning = _pdata->gc_is_running; // racing fix

          //for (GLuint y = 0; y < static_cast<GLuint>(_size[1]); ++y)
          for (int y = static_cast<int>(_image.geometry().size(1) - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_image.geometry().size(0)); ++x)
              {
                  GLuint b = 0;

                  if (_pdata->in(x, y, zcurrent()) != 0)
                  { b |= InsideBit; }

                  if (_pdata->out(x, y, zcurrent()) != 0)
                  { b |= OutsideBit; }

                  //if (_pdata->seg(x, y, zcurrent()) != 0)
                  if (!isRunning && _pdata->seg(x, y, zcurrent()) != 0) // racing fix
                  { b |= SegmentationBit; }

                  inout[cnt++] = b;
              }

              #ifdef BK_EMIT_PROGRESS
              prog.increment(_image.geometry().size(0));
              #endif
          }

          _pdata->ssbo_gc.unmap_and_release();

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif
      }

      _pdata->seg_changed = false;
      _pdata->slice_changed = false;

      if (do_update)
      { this->emit_signal_update_required(); }
  }

  bool GrayImageGraphCutView::on_mouse_pos_changed_impl(GLint x, GLint y)
  {
      if (interaction_mode_is_graph_cut())
      {
          determine_currentIntensity(x, y);

          const bool process_drawing = _mouse().middle_button_is_pressed() || _mouse().right_button_is_pressed() || _mouse().left_button_is_pressed();

          if (process_drawing)
          { process_onscreen_drawing(x, y, false /*mouse was released*/); }

          //this->doneCurrent();
      }

      return !interaction_mode_is_graph_cut();
  }

  bool GrayImageGraphCutView::on_mouse_button_pressed_impl(MouseButton_ btn)
  {
      bool process_drawing = false;

      if (btn == MouseButton_Left)
      {
          if (interaction_mode_is_graph_cut())
          {
              set_draw_mode_inside();
              process_drawing = true;
          }
      }
      else if (btn == MouseButton_Middle)
      {
          if (interaction_mode_is_graph_cut())
          {
              set_draw_mode_erase();
              process_drawing = true;
          }
      }
      else if (btn == MouseButton_Right)
      {
          if (interaction_mode_is_transfer_function())
          { show_transfer_function(); }
          else
          {
              set_draw_mode_outside();
              process_drawing = true;
          }
      }

      if (process_drawing)
      { process_onscreen_drawing(_mouse().x(), _mouse().y(), false /*mouse was released*/); }

      return !interaction_mode_is_graph_cut();
  }

  bool GrayImageGraphCutView::on_mouse_button_released_impl(MouseButton_ btn)
  {
      if (btn == MouseButton_Right)
      {
          if (interaction_mode_is_transfer_function())
          { hide_transfer_function(); }
      }

      if (interaction_mode_is_graph_cut())
      { process_onscreen_drawing(_mouse().x(), _mouse().y(), true /*mouse was released*/); }

      return !interaction_mode_is_graph_cut();
  }

  void GrayImageGraphCutView::draw_impl()
  {
      _ubo().bind_to_default_base();

      BK_QT_GL glPushAttrib(GL_DEPTH_TEST | GL_PRIMITIVE_RESTART | GL_BLEND);
      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _vao().bind();

      _ssbo_intensity().bind_to_base(2);
      _shader().bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
      _shader().release();
      _ssbo_intensity().release_from_base();

      BK_QT_GL glEnable(GL_BLEND);
      BK_QT_GL glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      _pdata->ssbo_gc.bind_to_base(2);

      if (_pdata->show_inout_overlay)
      {
          _pdata->shader_gc.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
          _pdata->shader_gc.release();
      }

      _pdata->shader_seg_contour.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _sizeInd(), GL_UNSIGNED_INT, nullptr);
      _pdata->shader_seg_contour.release();
      _pdata->ssbo_gc.release_from_base();

      _vao().release();

      _ubo().release_from_base();

      BK_QT_GL glPopAttrib();

      if (_show_tf())
      { _tf_view().draw(); }
  }
} // namespace bk


