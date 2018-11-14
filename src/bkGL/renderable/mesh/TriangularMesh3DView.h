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

#ifndef BKGL_TRIANGULARMESH3DVIEW_H
#define BKGL_TRIANGULARMESH3DVIEW_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bk/Mesh>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // ------ forward declaration -------
  class ColorRGBA;

  class ColorBarRGBA;

  class ColorBarView;

  class IBO;

  class VAO;

  class VBO;

  class Shader;

  class SSBO;

  namespace details
  {
    class UBOPhong;
  } // namespace details
  // ------ forward declaration -------

  class BKGL_EXPORT TriangularMesh3DView : public details::AbstractSceneRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TriangularMesh3DView;
      using base_type = details::AbstractSceneRenderable;

      friend class TriangularMesh3DCenterlineExtractionView;

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
      TriangularMesh3DView();
      #else
      TriangularMesh3DView(bk::qt_gl_functions* gl);
      #endif
      TriangularMesh3DView(const self_type& other) = delete;
      TriangularMesh3DView(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~TriangularMesh3DView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET MODE
      [[nodiscard]] bool mode_is_solid_default() const;
      [[nodiscard]] bool mode_is_wireframe() const;
      [[nodiscard]] bool mode_is_front_face_culling_with_ghosted_view() const;
      [[nodiscard]] bool mode_is_silhouette() const;
      /// @}

      /// @{ -------------------------------------------------- GET SHININESS
      [[nodiscard]] GLfloat shininess() const;
      /// @}

      /// @{ -------------------------------------------------- GET GHOSTED VIEW PARAMS
      [[nodiscard]] GLfloat ghosted_view_cutoff() const;
      [[nodiscard]] GLfloat ghosted_view_falloff() const;
      /// @}

      /// @{ -------------------------------------------------- GET SILHOUETTE WIDTH
      [[nodiscard]] GLfloat silhouette_width() const;
      /// @}

      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] const ColorRGBA& color() const;
      /// @}

      /// @{ -------------------------------------------------- GET CENTER
      [[nodiscard]] virtual Vec3<GLfloat> center() const override;
      /// @}

      /// @{ -------------------------------------------------- GET COLOR ATTRIBUTE
      [[nodiscard]] bool color_attribute_is_time_dependent() const;

      [[nodiscard]] GLfloat color_attribute_min_value() const;
      [[nodiscard]] GLfloat color_attribute_max_value() const;

      [[nodiscard]] ColorBarView& colorbarview();
      [[nodiscard]] const ColorBarView& colorbarview() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

    private:
      [[nodiscard]] unsigned int _floats_per_vertex() const;
    public:

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET RENDER MODE
      void set_mode_solid_default();
      void set_mode_wireframe();
      void set_mode_front_face_culling_with_ghosted_view();
      void set_mode_silhouette();
      /// @}

      /// @{ -------------------------------------------------- SET SHININESS
      void set_shininess(GLfloat shininess);
      /// @}

      /// @{ -------------------------------------------------- SET GHOSTED VIEW PARAMS
      void set_ghosted_view_falloff(GLfloat falloff);
      void set_ghosted_view_cutoff(GLfloat cutoff);
      /// @}

      /// @{ -------------------------------------------------- SET SILHOUETTE WIDTH
      void set_silhouette_width(GLfloat w);
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
    private:
      void _update_ubo_color();
    public:
      void set_color(const ColorRGBA& c);
      void set_color(ColorRGBA&& c);
      void set_color(double r, double g, double b, double a = 1);
      /// @}

      /// @{ -------------------------------------------------- SET COLORBAR
    private:
      void _set_colorbar(const ColorBarRGBA& colbar);
    public:
      void set_colorbar_heat();
      void set_colorbar_rainbow();
      void set_colorbar_blue_to_red();
      void set_colorbar_magenta();
      void set_colorbar_light_blue_to_yellow();
      void set_colorbar_green_to_red();

      void set_enable_colorbar(bool b);
      /// @}

      /// @{ -------------------------------------------------- SET ATTRIBUTE
      void enable_time_dependent_attribute(GLuint numTimes, GLfloat temporalResolution);
      void disable_time_dependent_attribute();
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_buffers();
    public:
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
      void init_mesh(const TriangularMesh3D& mesh, std::string_view color_attribute_name = "");
      void init_shader();
      void init_ubo();
      void init(const TriangularMesh3D& mesh, std::string_view color_attribute_name = "");
      /// @}

      /// @{ -------------------------------------------------- COLOR ATTRIBUTE
      void set_color_attribute(const TriangularMesh3D& mesh, std::string_view color_attribute_name = "");
      void clear_color_attribute();
      void set_colorbar_min_max_value(double vmin, double vmax);

    private:
      void update_attribute();
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
      virtual void on_key_pressed(Key_ k) override;
      virtual void on_key_released(Key_ k) override;
      virtual void on_mouse_wheel_up() override;
      virtual void on_mouse_wheel_down() override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      virtual void on_animation_time_changed(GLfloat t) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_opaque_impl() override;
      virtual void draw_transparent_impl() override;
    public:
      /// @}
  }; // class TriangularMesh3DView

} // namespace bk

#endif //BKGL_TRIANGULARMESH3DVIEW_H


