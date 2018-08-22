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

#ifndef BK_GRAYIMAGELIVETHRESHOLDSLICEVIEW_H
#define BK_GRAYIMAGELIVETHRESHOLDSLICEVIEW_H

#include <cstdint>

#include <bk/CopyablePIMPL>
#include <bk/Image>

#include <bkGL/renderable/image/GrayImageSliceView.h>

namespace bk
{
  class BKGL_EXPORT GrayImageLiveThresholdSliceView : public GrayImageSliceView
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GrayImageLiveThresholdSliceView;
      using base_type = GrayImageSliceView;

    public:
      using image_type = typename base_type::image_type;

      using ssbo_GLfloat = GLuint;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      GrayImageLiveThresholdSliceView();
      #else
      GrayImageLiveThresholdSliceView(bk::qt_gl_functions* gl);
      #endif
      GrayImageLiveThresholdSliceView(const self_type& other) = delete;
      GrayImageLiveThresholdSliceView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~GrayImageLiveThresholdSliceView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET THRESHOLD
      GLfloat threshold() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET THRESHOLD
      void set_threshold(GLfloat threshold);
      /// @}

      /// @{ -------------------------------------------------- SET SLICE
    protected:
      void set_slice_impl(unsigned int /*z*/) override;

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      virtual void init() override;
      /// @}

      /// @{ -------------------------------------------------- INIT SHADER
      bool init_shader_live_threshold_overlay();
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      virtual void clear() override;
      /// @}

      /// @{ -------------------------------------------------- CLEAR SHADER
      void clear_shader_live_threshold_overlay();
      /// @}

      /// @{ -------------------------------------------------- DRAW
      virtual void draw_impl() override;
      /// @}

  }; // class GrayImageLiveThresholdSliceView
} // namespace bk

#endif //BK_GRAYIMAGELIVETHRESHOLDSLICEVIEW_H


