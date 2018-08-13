#pragma once

#ifndef BK_TEXTURE2DVIEW_H
#define BK_TEXTURE2DVIEW_H

#include <memory>

#include "../../AbstractRenderable.h"

namespace bk
{
  class Texture2DView : public details::AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Texture2DView;
      using base_type = details::AbstractRenderable;

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
      /// @{ -------------------------------------------------- CONSTRUCTORS
      #ifndef BK_LIB_QT_AVAILABLE

      Texture2DView();
      #else
      Texture2DView(bk::qt_gl_functions* gl);
      #endif
      Texture2DView(const self_type& other) = delete;
      Texture2DView(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      virtual ~Texture2DView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TEXTURE UNIT
      GLuint texture_unit() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}
      
      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET TEXTURE UNIT
      void set_texture_unit(GLuint i);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      self_type& operator=(const self_type& other) = delete;
      auto operator=(self_type&& other) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear_shader();
      void clear_buffers();
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init_shader();
      void init_ubo();
      void init();
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) = 0;
      virtual void on_oit_enabled(bool b) = 0;
      virtual void on_animation_enabled(bool b) = 0;
      virtual void on_modelview_changed(bool b) = 0;
      virtual void on_visible_changed(bool b) = 0;
      virtual void on_mouse_pos_changed(GLint x, GLint y) = 0;
      virtual void on_mouse_button_pressed(MouseButton btn) = 0;
      virtual void on_mouse_button_released(MouseButton btn) = 0;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) = 0;
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw_impl() override;
      /// @}
  }; // class Texture2DView
} // namespace bk

#endif //BK_TEXTURE2DVIEW_H


