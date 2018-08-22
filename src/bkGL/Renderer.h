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

#ifndef BKGL_RENDERER_H
#define BKGL_RENDERER_H

#include <bk/CopyablePIMPL>
#include <bk/Algorithm>
#include <bk/Image>
#include <bk/Matrix>
#include <bk/Signal>
#include <bkGL/AbstractObject.h>
#include <bkGL/EMouseButton.h>
#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/renderable/AbstractSceneRenderable.h>

namespace bk
{
  // ------ forward declaration -------
  class GradientBackground;
  class WindowGeometry;
  class Mouse;
  class Camera;
  class OrderIndependentTransparency;
  class Interactor;
  class Animator;
  class SuperSampler;

  namespace details
  {
    class UBOGlobal;
  } // namespace details
  // ------ forward declaration -------

  class BKGL_EXPORT Renderer : public details::AbstractObject
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Renderer;
      using base_type = details::AbstractObject;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE

      Renderer();
      #else
      Renderer(qt_gl_functions* gl);
      #endif
      Renderer(const self_type&) = delete;
      Renderer(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Renderer();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET WINDOW SIZE
      [[nodiscard]] WindowGeometry& window();
      [[nodiscard]] const WindowGeometry& window() const;
      [[nodiscard]] unsigned int width() const;
      [[nodiscard]] unsigned int height() const;
      /// @}

      /// @{ -------------------------------------------------- GET BACKGROUND
      [[nodiscard]] GradientBackground& background();
      [[nodiscard]] const GradientBackground& background() const;
      /// @}

      /// @{ -------------------------------------------------- GET CAMERA
      [[nodiscard]] Camera& camera();
      [[nodiscard]] const Camera& camera() const;

      [[nodiscard]] bk::Vec3<GLfloat> center_translation() const;
      /// @}

      /// @{ -------------------------------------------------- GET MATRICES
      //[[nodiscard]] const bk::ColMat4<GLfloat>& modelview_matrix() const;
      //[[nodiscard]] const bk::ColMat4<GLfloat>& projection_matrix() const;
      /// @}

      /// @{ -------------------------------------------------- GET OIT
      [[nodiscard]] OrderIndependentTransparency& oit();
      [[nodiscard]] const OrderIndependentTransparency& oit() const;
      /// @}

      /// @{ -------------------------------------------------- GET DEFAULT FBO ID
      [[nodiscard]] GLuint default_fbo_id() const;
      /// @}

      /// @{ -------------------------------------------------- GET INTERACTOR
      [[nodiscard]] Interactor& interactor();
      [[nodiscard]] const Interactor& interactor() const;
      /// @}

      /// @{ -------------------------------------------------- GET ANIMATOR
      [[nodiscard]] Animator& animator();
      [[nodiscard]] const Animator& animator() const;
      [[nodiscard]] bk::Signal<GLfloat>& signal_animation_current_time_changed();
      [[nodiscard]] const bk::Signal<GLfloat>& signal_animation_current_time_changed() const;
      /// @}

      /// @{ -------------------------------------------------- GET SUPERSAMPLER
      [[nodiscard]] SuperSampler& supersampler();
      [[nodiscard]] const SuperSampler& supersampler() const;
      /// @}
      
      /// @{ -------------------------------------------------- GET RENDER SETTINGS
      [[nodiscard]] unsigned int num_scene_renderables() const;
      [[nodiscard]] unsigned int num_renderables() const;
      [[nodiscard]] unsigned int num_all_renderables() const;
      /// @}

      /// @{ -------------------------------------------------- GET UBO
      [[nodiscard]] const details::UBOGlobal& ubo_global() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<>& signal_update_required();
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET MODELVIEW CHANGED
      void set_modelview_matrix_changed(bool b);
      /// @}

      /// @{ -------------------------------------------------- REMOVE RENDERABLES
      void clear_scene_renderables();
      void clear_renderables();
      /// @}

      /// @{ -------------------------------------------------- ADD RENDERABLE
      void add_scene_renderable(std::shared_ptr<details::AbstractSceneRenderable>&& r);
      void add_renderable(std::shared_ptr<details::AbstractRenderable>&& r);
      /// @}

      /// @{ -------------------------------------------------- SET DEFAULT FBO ID
      void set_default_fbo_id(GLuint id);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      void emit_signal_update_required();

      /// @{ -------------------------------------------------- CONNECT SIGNALS
      void connect_signals_scene_renderable(std::shared_ptr<details::AbstractSceneRenderable>& r);
      void connect_signals_renderable(std::shared_ptr<details::AbstractRenderable>& r);
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init();
      /// @}

      void update_centers();

      /// @{ -------------------------------------------------- MEDIA
      std::unique_ptr<CartesianImage<Vec<3, double>, 2>> render_screenshot(int ssaafac = -1, int sizex = -1, int sizey = -1);
      void render_video(const double fps, const double length_in_s, int ssaafac = -1, int sizex = -1, int sizey = -1);
      Signal<std::string, std::shared_ptr<CartesianImage<Vec<3, double>, 2>>>& signal_video_rendering_new_frame();

      #ifdef BK_LIB_PNG_AVAILABLE
      void save_screenshot(const std::string& path, int ssaafac = -1, int sizex = -1, int sizey = -1);
      void save_video(const std::string& path, const double fps, const double length_in_s, int ssaafac = -1, int sizex = -1, int sizey = -1);
      #endif

      /// @}

      //====================================================================================================
      //===== GL FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- PAINT
      void draw();
      /// @}

      /// @{ -------------------------------------------------- MOUSE MOVE
      void mouse_move(GLint x, GLint y);
      /// @}

      /// @{ -------------------------------------------------- MOUSE BUTTON PRESSED
      void mouse_pressed(MouseButton btn);
      /// @}

      /// @{ -------------------------------------------------- MOUSE BUTTON RELEASED
      void mouse_released(MouseButton btn);
      /// @}

      /// @{ -------------------------------------------------- RESIZE
      void resize(GLint w, GLint h);
      /// @}

      /// @{ -------------------------------------------------- DEFAULT FBO
    private:
      void _bind_default_fbo();
      void _bind_default_fbo_as_read();
      void _bind_default_fbo_as_draw();
    public:
      void bind_default_fbo();
      void bind_default_fbo_as_read();
      void bind_default_fbo_as_draw();
      /// @}

  }; // class Renderer

} // namespace bk

#endif //BKGL_RENDERER_H


