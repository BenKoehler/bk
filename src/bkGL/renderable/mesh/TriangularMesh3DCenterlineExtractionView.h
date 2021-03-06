/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BKGL_TRIANGULARMESH3DCENTERLINEEXTRACTIONVIEW_H
#define BKGL_TRIANGULARMESH3DCENTERLINEEXTRACTIONVIEW_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bk/Mesh>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // ------ forward declaration -------
  class ColorRGBA;
  // ------ forward declaration -------

  class BKGL_EXPORT TriangularMesh3DCenterlineExtractionView : public details::AbstractSceneRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TriangularMesh3DCenterlineExtractionView;
      using base_type = details::AbstractSceneRenderable;
    public:
      using color_type = ColorRGBA;

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

      TriangularMesh3DCenterlineExtractionView();
      #else
      TriangularMesh3DCenterlineExtractionView(bk::qt_gl_functions* gl);
      #endif
      TriangularMesh3DCenterlineExtractionView(const self_type&) = delete;
      TriangularMesh3DCenterlineExtractionView(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~TriangularMesh3DCenterlineExtractionView();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET MODE
      [[nodiscard]] bool mode_is_solid_default() const;
      [[nodiscard]] bool mode_is_front_face_culling_with_ghosted_view() const;
      /// @}

      /// @{ -------------------------------------------------- GET SHININESS
      [[nodiscard]] GLfloat shininess() const;
      /// @}

      /// @{ -------------------------------------------------- GET GHOSTED VIEW PARAMS
      [[nodiscard]] GLfloat ghosted_view_cutoff() const;
      [[nodiscard]] GLfloat ghosted_view_falloff() const;
      /// @}

      /// @{ -------------------------------------------------- GET CENTER
      [[nodiscard]] virtual Vec3<GLfloat> center() const override;
      /// @}

      /// @{ -------------------------------------------------- INDEX AT SCREEN POS
      [[nodiscard]] int index_at_screen_pos(GLint x, GLint y);
      /// @}

      /// @{ -------------------------------------------------- START/END POINTS
      [[nodiscard]] GLint start_point_id() const;
      [[nodiscard]] const std::vector<GLint>& end_point_ids() const;

      [[nodiscard]] bool has_start_point_id() const;
      [[nodiscard]] bool has_end_point_ids() const;
      /// @}

      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET RENDER MODE
      void set_mode_solid_default();
      void set_mode_front_face_culling_with_ghosted_view();
      /// @}

      /// @{ -------------------------------------------------- SET SHININESS
      void set_shininess(GLfloat shininess);
      /// @}

      /// @{ -------------------------------------------------- SET GHOSTED VIEW PARAMS
      void set_ghosted_view_falloff(GLfloat falloff);
      void set_ghosted_view_cutoff(GLfloat cutoff);
      /// @}

      /// @{ -------------------------------------------------- SET COLOR
      void set_color(GLfloat r, GLfloat g, GLfloat b);
      /// @}

      /// @{ -------------------------------------------------- SET START/END POINTS
      void set_current_point_id_as_start_point();
      void set_start_point_id(GLint id);

    private:
      void _remove_duplicates_from_end_point_ids();
    public:

      void add_current_point_id_as_end_point();
      void add_end_point_id(GLint id);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    private:
      void clear_shader();
      void clear_buffers();
    public:
      void clear_start_and_end_points();
      void clear();
      /// @}

      /// @{ -------------------------------------------------- INIT
    private:
      void init_mesh(const TriangularMesh3D& mesh);
      void init_selection_sphere(GLfloat r, GLint nTheta, GLint nPhi);
      void init_shader();
      void init_ubo();
      void init_fbo_picking();
    public:
      void init(const TriangularMesh3D& mesh, GLfloat selection_sphere_radius = 3, GLint selection_sphere_theta = 20, GLint selection_sphere_phi = 20);
      /// @}

      /// @{ -------------------------------------------------- EVENTS
      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      /// @}

      /// @{ -------------------------------------------------- DRAW
    protected:
      virtual void draw_opaque_impl() override;
      virtual void draw_transparent_impl() override;
    public:

      /// @}
  }; // class TriangularMesh3DCenterlineExtractionView
} // namespace bk

#endif //BKGL_TRIANGULARMESH3DCENTERLINEEXTRACTIONVIEW_H


