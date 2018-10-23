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

#ifndef BKGL_DVRIMAGEVIEW_H
#define BKGL_DVRIMAGEVIEW_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bk/Image>

#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class VBO;
  class IBO;
  class VAO;
  class Shader;
  class FBO;
  class Texture3D;
  class WindowingTransferFunction;
  class WindowingTransferFunctionView;
  class ScreenQuad;
  namespace details
  {
    class UBODVR;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT DVRImageView : public details::AbstractSceneRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DVRImageView;
      using base_type = details::AbstractSceneRenderable;
    public:

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
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE

      DVRImageView();
      #else
      DVRImageView(bk::qt_gl_functions* gl);
      #endif
      DVRImageView(const self_type& other) = delete;
      DVRImageView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~DVRImageView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET CENTER
      [[nodiscard]] virtual bk::Vec3<GLfloat> center() const override;
      /// @}

      /// @{ -------------------------------------------------- GET DVR SETTINGS
      GLuint num_ray_samples() const;
      bool mode_is_default() const;
      bool mode_is_maximum_intensity_projection() const;
      bool mode_is_minimum_intensity_projection() const;
      /// @}

      /// @{ -------------------------------------------------- HELPERS: GET MEMBERS
    protected:
      [[nodiscard]] bk::Vec3<GLfloat>& _center();
      [[nodiscard]] VBO& vbo_cuboid();
      [[nodiscard]] IBO& ibo_cuboid();
      [[nodiscard]] GLsizei& size_ind();
      [[nodiscard]] VAO& vao_cuboid();
      [[nodiscard]] GLsizei& sizeInd();
      [[nodiscard]] details::UBODVR& ubo();
      [[nodiscard]] FBO& fbo();
      [[nodiscard]] FBO& fbo_result();
      [[nodiscard]] Shader& shader_cuboid();
      [[nodiscard]] Shader& shader_dvr();
      [[nodiscard]] Shader& shader_screenquad_texture();
      [[nodiscard]] WindowingTransferFunction& tf();
      [[nodiscard]] WindowingTransferFunctionView& tf_view();
      [[nodiscard]] ScreenQuad& screenquad();
      [[nodiscard]] bool show_tf() const;
      [[nodiscard]] bk::Vec3<GLuint>& image_size();
      [[nodiscard]] bk::Vec3<GLfloat>& image_scale();
    public:
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET NUM RAY SAMPLES
      void set_num_ray_samples(GLuint n);
      /// @}

      /// @{ -------------------------------------------------- SET MODE
      void set_mode_default();
      void set_mode_maximum_intensity_projection();
      void set_mode_minimum_intensity_projection();
      /// @}

      /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
      void reset_transfer_function(bool tolerant = false);
      void transfer_function_shift_center_left(double percent);
      void transfer_function_shift_center_right(double percent);
      void transfer_function_increase_width(double percent);
      void transfer_function_decrease_width(double percent);
      /// @}

      /// @{ -------------------------------------------------- SHOW TRANSFER FUNCTION
      void show_transfer_function();
      void hide_transfer_function();
      /// @}

      /// @{ -------------------------------------------------- ENABLE/DISABLE TRANSFER FUNCTION INTERACTION
      void enable_transfer_function_right_click_interaction();
      void disable_transfer_function_right_click_interaction();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    protected:
      virtual void clear_shader_impl() = 0;
      virtual void clear_buffers_and_textures_impl() = 0;
      void clear_shader();
      void clear_buffers_and_textures();
    public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
    protected:
      virtual void init_shader_impl() = 0;
      void init_shader();
      void init_ubo();
      void init_fbo(GLuint window_width, GLuint window_height);
    public:
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_matrix_changed(bool) override;
      virtual void on_new_modelview_matrix(const ColMat4<GLfloat>& m) override;
      virtual void on_new_projection_matrix(const ColMat4<GLfloat>& p) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_mouse_button_released(MouseButton_ btn) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void bind_image_textures() = 0;
      virtual void release_image_textures() = 0;
      [[nodiscard]] virtual bool is_up_to_date() const = 0;
    public:
      virtual void draw_opaque_impl() override;
      virtual void draw_transparent_impl() override;
      /// @}
  }; // class DVRImageView
} // namespace bk

#endif //BKGL_DVRIMAGEVIEW_H


