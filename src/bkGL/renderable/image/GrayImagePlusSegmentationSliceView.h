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

#ifndef BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H
#define BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H

#include <cstdint>
#include <memory>

#include <bk/Image>

#include <bkGL/renderable/image/GrayImageSliceView.h>

namespace bk
{
  class BKGL_EXPORT GrayImagePlusSegmentationSliceView : public GrayImageSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GrayImagePlusSegmentationSliceView;
      using base_type = GrayImageSliceView;

    public:
      using image_type = typename base_type::image_type;
      using segmentation_type = bk::RegularImage<unsigned char, 3>;

      using ssbo_GLfloat = GLuint;

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

      GrayImagePlusSegmentationSliceView();
      #else
      GrayImagePlusSegmentationSliceView(bk::qt_gl_functions* gl);
      #endif
      GrayImagePlusSegmentationSliceView(const self_type& other) = delete;
      GrayImagePlusSegmentationSliceView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~GrayImagePlusSegmentationSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      auto _seg() -> segmentation_type&;
    public:

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET SEGMENTATION
      template<typename Segmentation_>
      void set_segmentation(const Segmentation_& seg);
      /// @}

      /// @{ -------------------------------------------------- SET SLICE
    protected:
      void set_slice_impl(unsigned int /*z*/) override;

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      virtual void init() override;
      /// @}

      /// @{ -------------------------------------------------- INIT SHADER
      bool init_shader_seg_contour();
      /// @}

      /// @{ -------------------------------------------------- INIT SSBO
      void init_ssbo();
      void update_ssbo();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      virtual void clear() override;
      /// @}

      /// @{ -------------------------------------------------- CLEAR SHADER
      void clear_shader_seg_contour();
      /// @}

      /// @{ -------------------------------------------------- CLEAR SSBO
      void clear_ssbo();
      /// @}

      /// @{ -------------------------------------------------- DRAW
      virtual void draw_impl() override;
      /// @}

  }; // class GrayImagePlusSegmentationSliceView
} // namespace bk

#include <bkGL/renderable/image/GrayImagePlusSegmentationSliceView.hpp>

#endif //BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H


