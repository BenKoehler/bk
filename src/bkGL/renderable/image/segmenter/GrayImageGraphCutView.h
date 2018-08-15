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

#ifndef BK_GRAYIMAGEGRAPHCUTVIEW_H
#define BK_GRAYIMAGEGRAPHCUTVIEW_H

#include <cstdint>
#include <memory>

#include <bkGL/renderable/image/GrayImageSliceView.h> // cannot derive from DICOMSliceView, since the full image is needed at once for the graph cut
#include <bk/Image>
#include <bk/GraphCut>

namespace bk
{
  // ------ forward declaration -------
  template<typename...> class Signal;
  // ------ forward declaration -------

  class BKGL_EXPORT GrayImageGraphCutView : public GrayImageSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GrayImageGraphCutView;
      using base_type = GrayImageSliceView;

    public:
      using image_type = typename base_type::image_type;
      using segmentation_type = bk::RegularImage<unsigned char, 3>;

      using graph_cut_type = GraphCutParallel<3>;

      using ssbo_GLfloat = GLuint;
      static constexpr const ssbo_GLfloat SegmentationBit = 1;
      static constexpr const ssbo_GLfloat InsideBit = 2;
      static constexpr const ssbo_GLfloat OutsideBit = 4;

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

      GrayImageGraphCutView();
      #else
      GrayImageGraphCutView(bk::qt_gl_functions* gl);
      #endif
      GrayImageGraphCutView(const self_type& other) = delete;
      GrayImageGraphCutView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~GrayImageGraphCutView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET GRAPH CUT
      auto graph_cut() const -> const graph_cut_type*;
      /// @}

    private:
      auto _seg() -> segmentation_type&;
      bool& _seg_changed();
    public:

      /// @{ -------------------------------------------------- GET PENCILSIZE
      unsigned int pencil_size() const;
      /// @}

      /// @{ -------------------------------------------------- GET SEGMENTATION
      auto segmentation() const -> const segmentation_type&;
      /// @}

      /// @{ -------------------------------------------------- SIGNALS
      bk::Signal<>& signal_graph_cut_finished();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET SLICE
    protected:
      void set_slice_impl(unsigned int /*z*/) override;

    public:
      /// @}

      /// @{ -------------------------------------------------- SET GRAPH CUT
      void set_graph_cut(graph_cut_type* gc);
      /// @}

      /// @{ -------------------------------------------------- SET PENCILSIZE
      void set_pencil_size(unsigned int s);
      /// @}

      /// @{ -------------------------------------------------- SET DRAW MODE
      void set_draw_mode_inside();;
      void set_draw_mode_outside();
      void set_draw_mode_erase();
      /// @}

      /// @{ -------------------------------------------------- AUTO UPDATE SEGMENTATION
      void set_auto_update_segmentation(bool b);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      void set_inside_outside_from_graph_cut();
      template<typename Img3_>
      void set_segmentation(const Img3_& seg);

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- PROCESS ONSCREEN DRAWING
      void process_onscreen_drawing(GLuint screen_x, GLuint screen_y, bool mouse_was_released = true);
      /// @}

      /// @{ -------------------------------------------------- INIT
      virtual void init() override;
      /// @}

      /// @{ -------------------------------------------------- INIT SHADER
      bool init_shader_gc();
      bool init_shader_seg_contour();
      /// @}

      /// @{ -------------------------------------------------- INIT SSBO GC
      void init_ssbo_gc();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      virtual void clear() override;
      void clear_segmentation();
      /// @}

      /// @{ -------------------------------------------------- CLEAR SHADER
      void clear_shader_gc();
      void clear_shader_seg_contour();
      /// @}

      /// @{ -------------------------------------------------- CLEAR SSBO GC
      void clear_ssbo_gc();
      /// @}

      /// @{ -------------------------------------------------- UPDATE SEGMENTATION (RUN GC)
      void update_gc();
      /// @}

      /// @{ -------------------------------------------------- UPDATE SSBO GC
      void update_ssbo_gc();
      /// @}

      /// @{ -------------------------------------------------- DRAW
      virtual void draw_impl() override;
      /// @}

  }; // class GrayImageGraphCutView
} // namespace bk

#include <bkGL/renderable/image/segmenter/GrayImageGraphCutView.hpp>

#endif //BK_GRAYIMAGEGRAPHCUTVIEW_H


