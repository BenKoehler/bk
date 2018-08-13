#pragma once

#ifndef BK_GRAYIMAGELIVETHRESHOLDSLICEVIEW_H
#define BK_GRAYIMAGELIVETHRESHOLDSLICEVIEW_H

#include <cstdint>
#include <memory>

#include <bkDataset/Image>

#include <bkGL/renderable/image/GrayImageSliceView.h>

namespace bk
{
  class BK_GL_EXPORT GrayImageLiveThresholdSliceView : public GrayImageSliceView
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

      std::unique_ptr<Impl> _pdata;

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
      GrayImageLiveThresholdSliceView(self_type&& other);
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
      auto operator=(self_type&& other) -> self_type&;
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


