#pragma once

#ifndef BKGL_GRADIENTBACKGROUND_H
#define BKGL_GRADIENTBACKGROUND_H

#include <memory>

#include "AbstractBackground.h"

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  class BKGL_EXPORT GradientBackground : public details::AbstractBackground
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GradientBackground;
      using base_type = details::AbstractBackground;
    public:
      using color_type = ColorRGBA;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;
      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      GradientBackground();
      #else
      GradientBackground(bk::qt_gl_functions* gl);
      #endif
      GradientBackground(const self_type&) = delete;
      GradientBackground(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~GradientBackground();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] auto color_bottom_or_left() const -> const color_type&;
      [[nodiscard]] auto color_top_or_right() const -> const color_type&;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
      void set_color_bottom_or_left(const color_type& col);
      void set_color_bottom_or_left(color_type&& col);
      void set_color_bottom_or_left(double r, double g, double b, double a = 1);
      void set_color_top_or_right(const color_type& col);
      void set_color_top_or_right(color_type&& col);
      void set_color_top_or_right(double r, double g, double b, double a = 1);
      /// @}

      /// @{ -------------------------------------------------- SET VERTICAL / HORIZONTAL
      void set_vertical();
      void set_horizontal();
      /// @}

      /// @{ -------------------------------------------------- SET DEFAULT
      void set_default_bk();
      void set_default_gray_vertical();
      void set_default_transparent();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init_vbo_vao() override;
      /// @}

      /// @{ -------------------------------------------------- UPDATE
    protected:
      void update_colors() override;
    public:
      /// @}
  }; // class GradientBackground
} // namespace bk

#endif //BKGL_GRADIENTBACKGROUND_H


