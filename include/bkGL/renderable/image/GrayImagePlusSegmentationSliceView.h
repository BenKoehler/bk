#pragma once

#ifndef BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H
#define BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H

#include <cstdint>
#include <memory>

#include <bkDataset/Image>

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

#include "GrayImagePlusSegmentationSliceView.hpp"

#endif //BK_GRAYIMAGEPLUSSEGMENTATIONSLICEVIEW_H


