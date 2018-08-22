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
    public:
      using value_type = GLfloat;
      using size_type = GLuint;
      using color_type = ColorRGBA;

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
      [[nodiscard]] auto shininess() const -> value_type;
      /// @}

      /// @{ -------------------------------------------------- GET GHOSTED VIEW PARAMS
      [[nodiscard]] auto ghosted_view_cutoff() const -> value_type;
      [[nodiscard]] auto ghosted_view_falloff() const -> value_type;
      /// @}

      /// @{ -------------------------------------------------- GET SILHOUETTE WIDTH
      [[nodiscard]] auto silhouette_width() const -> value_type;
      /// @}

      /// @{ -------------------------------------------------- GET COLOR
      [[nodiscard]] const color_type& color() const;
      /// @}

      /// @{ -------------------------------------------------- GET CENTER
      [[nodiscard]] virtual auto center() const -> Vec3<value_type> override;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const override;
      /// @}

      /// @{ -------------------------------------------------- GET INTERNALS
    private:
      [[nodiscard]] IBO& ibo();
      [[nodiscard]] VAO& vao();
      [[nodiscard]] VBO& vbo();
      [[nodiscard]] Shader& shader();
      [[nodiscard]] SSBO& ssbo();
      [[nodiscard]] details::UBOPhong& ubo();
      [[nodiscard]] GLsizei sizeInd() const;
    public:
      /// @}

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
      void set_shininess(value_type shininess);
      /// @}

      /// @{ -------------------------------------------------- SET GHOSTED VIEW PARAMS
      void set_ghosted_view_falloff(value_type falloff);
      void set_ghosted_view_cutoff(value_type cutoff);
      /// @}

      /// @{ -------------------------------------------------- SET SILHOUETTE WIDTH
      void set_silhouette_width(value_type w);
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
  private:
      void _update_ubo_color();
  public:
      void set_color(const color_type& c);
      void set_color(color_type&& c);
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
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_changed(bool) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton btn) override;
      virtual void on_mouse_button_released(MouseButton btn) override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
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


