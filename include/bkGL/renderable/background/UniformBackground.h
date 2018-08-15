#pragma once

#ifndef BKGL_UNIFORMBACKGROUND_H
#define BKGL_UNIFORMBACKGROUND_H

#include <memory>

#include "AbstractBackground.h"

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;
  // -------------------- forward declaration END

  class BKGL_EXPORT UniformBackground : public details::AbstractBackground
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UniformBackground;
      using base_type = details::AbstractBackground;
    public:
      using color_type = ColorRGBA;

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
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UniformBackground();
      #else
      UniformBackground(bk::qt_gl_functions* gl);
      #endif
      UniformBackground(const self_type&) = delete;
      UniformBackground(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~UniformBackground();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] auto color() const -> const color_type&;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
      void set_color(const color_type& col);
      void set_color(color_type&& col);
      void set_color(double r, double g, double b, double a = 1);
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
  }; // class UniformBackground
} // namespace bk

#endif //BKGL_UNIFORMBACKGROUND_H


