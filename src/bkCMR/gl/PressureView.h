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

#ifndef BK_PRESSUREVIEW_H
#define BK_PRESSUREVIEW_H

#include <memory>

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bk/Image>
#include <bkGL/renderable/AbstractSceneRenderable.h>

namespace bk
{
  // -------------------- forward declaration
  class VBO;

  class IBO;

  class VAO;

  class Shader;

  class UBO;

  class FBO;

  class ScreenQuad;

  class Texture3D;
  // -------------------- forward declaration END

  inline namespace cmr
  {
    class PressureView : public bk::details::AbstractSceneRenderable
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using base_type = bk::details::AbstractSceneRenderable;
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
        /// @{ -------------------------------------------------- CONSTRUCTORS
        #ifndef BK_LIB_QT_AVAILABLE
        PressureView();
        #else
        explicit PressureView(bk::qt_gl_functions* gl);
        #endif
        PressureView(const PressureView&) = delete;
        PressureView(PressureView&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~PressureView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- DVR SETTINGS
        [[nodiscard]] GLuint num_ray_samples() const;
        /// @}

        /// @{ -------------------------------------------------- SCALE VAL
        [[nodiscard]] GLfloat scale_val() const;
        /// @}

        /// @{ -------------------------------------------------- REFERENCE VAL
        [[nodiscard]] GLfloat reference_val() const;
        /// @}

        /// @{ -------------------------------------------------- BUFFERS/TEXTURES
      protected:
        [[nodiscard]] VBO& vbo_mesh();
        [[nodiscard]] IBO& ibo_mesh();
        [[nodiscard]] GLsizei& sizeInd_mesh();
        [[nodiscard]] VAO& vao_mesh();
        [[nodiscard]] VBO& vbo_cuboid();
        [[nodiscard]] IBO& ibo_cuboid();
        [[nodiscard]] GLsizei& sizeInd_cuboid();
        [[nodiscard]] VAO& vao_cuboid();
        [[nodiscard]] UBO& ubo();
        [[nodiscard]] FBO& fbo_front();
        [[nodiscard]] FBO& fbo_back();
        [[nodiscard]] Shader& shader_dvr();
        [[nodiscard]] Texture3D& image_tex(unsigned int i);
        [[nodiscard]] RegularImage<double, 4>& img();
        [[nodiscard]] GLfloat& minVal();
        [[nodiscard]] GLfloat& maxVal();
      public:

        [[nodiscard]] GLfloat min_value() const;
        [[nodiscard]] GLfloat max_value() const;
        /// @}

        [[nodiscard]] bool is_up_to_date() const;

        /// @{ -------------------------------------------------- GET CENTER
        [[nodiscard]] virtual Vec3<GLfloat> center() const override;
        /// @}

        /// @{ -------------------------------------------------- IS INITIALIZED
        [[nodiscard]] virtual bool is_initialized() const override;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- SET NUM RAY SAMPLES
        void set_num_ray_samples(GLuint n);
        /// @}

        /// @{ -------------------------------------------------- SCALE VAL
        void set_scale_val(GLfloat s);
        /// @}

        /// @{ -------------------------------------------------- REFERENCE VAL
        void set_reference_val(GLfloat r);
        /// @}

        /// @{ -------------------------------------------------- SET STATIC MODE
        void set_static_mode(bool b);
        /// @}

        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] PressureView& operator=(const PressureView&) = delete;
        [[maybe_unused]] PressureView& operator=(PressureView&&) noexcept;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- CLEAR
        void clear_shader();
        void clear_buffers_and_textures();
        void clear();
        /// @}

        /// @{ -------------------------------------------------- INIT
        void init_shader();
        void init_ubo();
        void init_fbo(GLuint window_width, GLuint window_height);
        template<typename Mesh3D_, typename Img4D_, typename Seg3D_>
        void init(const Mesh3D_& mesh, const Img4D_& pressureMap, const Seg3D_& vesselSeg, GLuint window_width, GLuint window_height);
        /// @}

        /// @{ -------------------------------------------------- EVENTS
        virtual void on_resize(GLint w, GLint h) override;
        virtual void on_oit_enabled(bool b) override;
        virtual void on_animation_enabled(bool b) override;
        virtual void on_modelview_changed(bool) override;
        virtual void on_visible_changed(bool b) override;
        virtual void on_mouse_pos_changed(GLint x, GLint y) override;
        virtual void on_mouse_button_pressed(MouseButton_ btn) override;
        virtual void on_mouse_button_released(MouseButton_ btn) override;
        virtual void on_key_pressed(Key_ k) override;
        virtual void on_key_released(Key_ k) override;
        virtual void on_mouse_wheel_up() override;
        virtual void on_mouse_wheel_down() override;
        virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
        void on_animation_time_changed(double d);
        /// @}

        /// @{ -------------------------------------------------- DRAW
      protected:
        virtual void draw_opaque_impl() override;
        virtual void draw_transparent_impl() override;
      public:
        /// @}
    }; // class PressureView
  } // inline namespace cmr
} // namespace bk

#include "PressureView.hpp"

#endif //BK_// PRESSUREVIEW_H
