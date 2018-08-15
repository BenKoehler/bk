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

#include <bkGL/renderable/mesh/TriangularMesh3DCenterlineExtractionView.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include <bkTools/color/ColorRGBA.h>
#include <bkMath/constants/pi.h>

#include <bkGL/renderable/mesh/TriangularMesh3DView.h>
#include <bkGL/renderable/mesh/EMeshRenderMode.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/UBOPhong.h>
#include <bkGL/UBOSelectionSphere.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture2D.h>

namespace bk
{
  struct TriangularMesh3DCenterlineExtractionView::Impl
  {
      TriangularMesh3DView meshView;
      VBO vbo_selection_sphere;
      IBO ibo_selection_sphere;
      VAO vao_selection_sphere;
      details::UBOSelectionSphere ubo_selection_sphere;
      Shader shader_picking_ids;
      Shader shader_selection_sphere;
      FBO fbo_picking;
      GLsizei sizeInd_selection_sphere;
      Vec3<value_type> center;
      GLint screen_w;
      GLint screen_h;
      GLint mouse_x;
      GLint mouse_y;
      GLint ssaa_factor;
      GLint selected_point_id;
      std::vector<Vec3<value_type>> mesh_vertices;
      GLint start_point_id;
      std::vector<GLint> end_point_ids;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : meshView(gl),
            vbo_selection_sphere(gl),
            ibo_selection_sphere(gl),
            vao_selection_sphere(gl),
            ubo_selection_sphere(gl),
            shader_picking_ids(gl),
            shader_selection_sphere(gl),
            fbo_picking(gl),
          #endif
          sizeInd_selection_sphere(0),
          center(MatrixFactory::Zero_Vec_3D<value_type>()),
          screen_w(0),
          screen_h(0),
          mouse_x(0),
          mouse_y(0),
          ssaa_factor(1),
          selected_point_id(-1),
          start_point_id(-1)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  TriangularMesh3DCenterlineExtractionView::TriangularMesh3DCenterlineExtractionView()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  TriangularMesh3DCenterlineExtractionView::TriangularMesh3DCenterlineExtractionView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo_selection_sphere.set_usage_STATIC_DRAW();
      _pdata->ibo_selection_sphere.set_usage_STATIC_DRAW();
      _pdata->vao_selection_sphere.add_default_attribute_position_3xfloat();

      _pdata->meshView.forward_signals(this);
  }

  TriangularMesh3DCenterlineExtractionView::TriangularMesh3DCenterlineExtractionView(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  TriangularMesh3DCenterlineExtractionView::~TriangularMesh3DCenterlineExtractionView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET MODE
  bool TriangularMesh3DCenterlineExtractionView::mode_is_solid_default() const
  { return _pdata->meshView.mode_is_solid_default(); }

  bool TriangularMesh3DCenterlineExtractionView::mode_is_front_face_culling_with_ghosted_view() const
  { return _pdata->meshView.mode_is_front_face_culling_with_ghosted_view(); }
  /// @}

  /// @{ -------------------------------------------------- GET SHININESS
  auto TriangularMesh3DCenterlineExtractionView::shininess() const -> value_type
  { return _pdata->meshView.shininess(); }
  /// @}

  /// @{ -------------------------------------------------- GET GHOSTED VIEW PARAMS
  auto TriangularMesh3DCenterlineExtractionView::ghosted_view_cutoff() const -> value_type
  { return _pdata->meshView.ghosted_view_cutoff(); }

  auto TriangularMesh3DCenterlineExtractionView::ghosted_view_falloff() const -> value_type
  { return _pdata->meshView.ghosted_view_falloff(); }
  /// @}

  /// @{ -------------------------------------------------- GET CENTER
  auto TriangularMesh3DCenterlineExtractionView::center() const -> Vec3<value_type>
  { return _pdata->center; }
  /// @}

  /// @{ -------------------------------------------------- INDEX AT SCREEN POS
  int TriangularMesh3DCenterlineExtractionView::index_at_screen_pos(GLint x, GLint y) const
  {
      _pdata->fbo_picking.bind();

      value_type data[3] = {-1, -1, -1};

      BK_QT_GL glReadBuffer(GL_COLOR_ATTACHMENT0);
      BK_QT_GL glReadPixels(x * _pdata->ssaa_factor, _pdata->screen_h - y * _pdata->ssaa_factor, 1, 1, GL_RGB, GL_FLOAT, data);

      this->emit_signal_bind_default_fbo();

      return static_cast<int>(std::round(data[0]));
  }
  /// @}

  /// @{ -------------------------------------------------- START/END POINTS
  GLint TriangularMesh3DCenterlineExtractionView::start_point_id() const
  { return _pdata->start_point_id; }

  const std::vector<GLint>& TriangularMesh3DCenterlineExtractionView::end_point_ids() const
  { return _pdata->end_point_ids; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool TriangularMesh3DCenterlineExtractionView::is_initialized() const
  { return _pdata->meshView.is_initialized() && _pdata->vbo_selection_sphere.is_initialized() && _pdata->ibo_selection_sphere.is_initialized() && _pdata->ubo_selection_sphere.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET RENDER MODE
  void TriangularMesh3DCenterlineExtractionView::set_mode_solid_default()
  {
      if (!mode_is_solid_default())
      {
          _pdata->meshView.set_mode_solid_default();

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void TriangularMesh3DCenterlineExtractionView::set_mode_front_face_culling_with_ghosted_view()
  {
      if (!mode_is_front_face_culling_with_ghosted_view())
      {
          _pdata->meshView.set_mode_front_face_culling_with_ghosted_view();

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET SHININESS
  void TriangularMesh3DCenterlineExtractionView::set_shininess(value_type shininess)
  { _pdata->meshView.set_shininess(shininess); }
  /// @}

  /// @{ -------------------------------------------------- SET GHOSTED VIEW PARAMS
  void TriangularMesh3DCenterlineExtractionView::set_ghosted_view_falloff(value_type falloff)
  { _pdata->meshView.set_ghosted_view_falloff(falloff); }

  void TriangularMesh3DCenterlineExtractionView::set_ghosted_view_cutoff(value_type cutoff)
  { _pdata->meshView.set_ghosted_view_falloff(cutoff); }
  /// @}

  /// @{ -------------------------------------------------- SET START/END POINTS
  void TriangularMesh3DCenterlineExtractionView::set_current_point_id_as_start_point()
  {
      if (_pdata->selected_point_id >= 0 && _pdata->selected_point_id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->start_point_id = _pdata->selected_point_id;
          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DCenterlineExtractionView::set_start_point_id(GLint id)
  {
      if (id >= 0 && id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->start_point_id = id;
          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DCenterlineExtractionView::_remove_duplicates_from_end_point_ids()
  { _pdata->end_point_ids.erase(std::unique(_pdata->end_point_ids.begin(), _pdata->end_point_ids.end()), _pdata->end_point_ids.end()); }

  void TriangularMesh3DCenterlineExtractionView::add_current_point_id_as_end_point()
  {
      if (_pdata->selected_point_id >= 0 && _pdata->selected_point_id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->end_point_ids.push_back(_pdata->selected_point_id);
          _remove_duplicates_from_end_point_ids();

          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DCenterlineExtractionView::add_end_point_id(GLint id)
  {
      if (id >= 0 && id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->end_point_ids.push_back(id);
          _remove_duplicates_from_end_point_ids();

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto TriangularMesh3DCenterlineExtractionView::operator=(self_type&& other) -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void TriangularMesh3DCenterlineExtractionView::clear_shader()
  {
      _pdata->shader_picking_ids.clear();
      _pdata->shader_selection_sphere.clear();
  }

  void TriangularMesh3DCenterlineExtractionView::clear_buffers()
  {
      _pdata->vbo_selection_sphere.clear();
      _pdata->ibo_selection_sphere.clear();
      _pdata->vao_selection_sphere.clear();
      _pdata->ubo_selection_sphere.clear();
      _pdata->sizeInd_selection_sphere = 0;

      _pdata->fbo_picking.clear();
  }

  void TriangularMesh3DCenterlineExtractionView::clear_start_and_end_points()
  {
      _pdata->start_point_id = -1;
      _pdata->end_point_ids.clear();
      this->emit_signal_update_required();
  }

  void TriangularMesh3DCenterlineExtractionView::clear()
  {
      _pdata->meshView.clear();

      clear_shader();
      clear_buffers();
      clear_start_and_end_points();

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void TriangularMesh3DCenterlineExtractionView::init_mesh(const TriangularMesh3D& mesh)
  {
      _pdata->meshView.init(mesh);
      _pdata->meshView.set_color(0.5, 0.5, 0.5, 1);

      const unsigned int nPoints = mesh.geometry().num_points();
      _pdata->mesh_vertices.resize(nPoints);

      #pragma omp parallel for
      for (unsigned int i = 0; i < nPoints; ++i)
      { _pdata->mesh_vertices[i] = mesh.geometry().point(i); }
  }

  void TriangularMesh3DCenterlineExtractionView::init_selection_sphere(value_type r, GLint nTheta, GLint nPhi)
  {
      const value_type dtheta = pi<value_type> / (nTheta - 1);
      const value_type dphi = 2 * pi<value_type> / (nPhi - 1);

      std::vector<value_type> vertices(nTheta * nPhi * 3);

      #pragma omp parallel for
      for (unsigned int t = 0; t < static_cast<unsigned int>(nTheta); ++t)
      {
          for (unsigned int p = 0; p < static_cast<unsigned int>(nPhi); ++p)
          {
              const unsigned int off = t * nPhi * 3 + p * 3;
              const value_type theta = t * dtheta;
              const value_type phi = p * dphi;
              const value_type sintheta = std::sin(theta);
              vertices[off + 0] = r * sintheta * std::cos(phi);
              vertices[off + 1] = r * sintheta * std::sin(phi);
              vertices[off + 2] = r * std::cos(theta);
          }
      }

      std::vector<size_type> indices;
      indices.reserve((nTheta - 1) * nPhi + (nTheta - 1));

      for (unsigned int t = 0; t < static_cast<unsigned int>(nTheta - 1); ++t)
      {
          for (unsigned int p = 0; p < static_cast<unsigned int>(nPhi); ++p)
          {
              const unsigned int off = t * nPhi + p;

              indices.push_back(off);
              indices.push_back(off + nPhi);
          }

          indices.push_back(std::numeric_limits<size_type>::max());
      }

      _pdata->sizeInd_selection_sphere = indices.size();

      _pdata->vbo_selection_sphere.init(vertices);
      _pdata->ibo_selection_sphere.init(indices);
      _pdata->vao_selection_sphere.init(_pdata->vbo_selection_sphere, _pdata->ibo_selection_sphere);
  }

  void TriangularMesh3DCenterlineExtractionView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary;

      _pdata->shader_picking_ids.init_from_sources(SL::mesh::picking::vert(), SL::mesh::picking::frag());
      _pdata->shader_selection_sphere.init_from_sources(SL::mesh::picking::vert_current_selection(), SL::mesh::picking::frag_current_selection(), SL::mesh::picking::geom_current_selection());
  }

  void TriangularMesh3DCenterlineExtractionView::init_ubo_selection_sphere()
  {
      _pdata->ubo_selection_sphere.clear();
      _pdata->ubo_selection_sphere.init_from_registered_values_size();
      _pdata->ubo_selection_sphere.release();
  }

  void TriangularMesh3DCenterlineExtractionView::init_fbo_picking()
  {
      _pdata->fbo_picking.clear();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D indices_tex;
      #else
      Texture2D indices_tex(this->_gl);
      #endif

      indices_tex.set_width(_pdata->screen_w);
      indices_tex.set_height(_pdata->screen_h);
      indices_tex.set_default_config_rgb_tex();
      indices_tex.set_texture_unit_number(1);
      indices_tex.init();

      indices_tex.bind();
      indices_tex.set_interpolation_NEAREST();
      indices_tex.set_texture_coordinates_CLAMP_TO_BORDER(-1, -1, -1, -1);
      indices_tex.release();

      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D depth_tex;
      #else
      Texture2D depth_tex(this->_gl);
      #endif
      depth_tex.set_width(_pdata->screen_w);
      depth_tex.set_height(_pdata->screen_h);
      depth_tex.set_default_config_depth_tex();
      depth_tex.set_texture_unit_number(2);
      depth_tex.init();

      depth_tex.bind();
      depth_tex.set_interpolation_LINEAR();
      depth_tex.set_texture_coordinates_CLAMP();
      depth_tex.release();

      _pdata->fbo_picking.set_texture_COLOR_ATTACHMENT(std::move(indices_tex), 0);
      _pdata->fbo_picking.set_texture_DEPTH_ATTACHMENT(std::move(depth_tex));
      _pdata->fbo_picking.init();
  }

  void TriangularMesh3DCenterlineExtractionView::init(const TriangularMesh3D& mesh, GLint w, GLint h, value_type selection_sphere_radius, GLint selection_sphere_theta, GLint selection_sphere_phi)
  {
      _pdata->screen_w = w;
      _pdata->screen_h = h;

      init_mesh(mesh);
      init_selection_sphere(selection_sphere_radius, selection_sphere_theta, selection_sphere_phi);
      init_shader();
      init_ubo_selection_sphere();
      init_fbo_picking();
      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void TriangularMesh3DCenterlineExtractionView::on_resize(GLint w, GLint h)
  {
      _pdata->screen_w = w;
      _pdata->screen_h = h;

      init_fbo_picking();
      this->emit_signal_update_required();
  }

  void TriangularMesh3DCenterlineExtractionView::on_oit_enabled(bool b)
  { _pdata->meshView.on_oit_enabled(b); }

  void TriangularMesh3DCenterlineExtractionView::on_animation_enabled(bool b)
  { _pdata->meshView.on_animation_enabled(b); }

  void TriangularMesh3DCenterlineExtractionView::on_modelview_changed(bool b)
  { _pdata->meshView.on_modelview_changed(b); }

  void TriangularMesh3DCenterlineExtractionView::on_visible_changed(bool b)
  { _pdata->meshView.on_visible_changed(b); }

  void TriangularMesh3DCenterlineExtractionView::on_mouse_pos_changed(GLint x, GLint y)
  {
      _pdata->mouse_x = x;
      _pdata->mouse_y = y;

      _pdata->selected_point_id = index_at_screen_pos(_pdata->mouse_x, _pdata->mouse_y);

      this->emit_signal_update_required();
  }

  void TriangularMesh3DCenterlineExtractionView::on_mouse_button_pressed(MouseButton btn)
  {
      if (btn == MouseButton::Left)
      { set_current_point_id_as_start_point(); }
      else if (btn == MouseButton::Right)
      { add_current_point_id_as_end_point(); }
  }

  void TriangularMesh3DCenterlineExtractionView::on_mouse_button_released(MouseButton /*btn*/)
  { /* do nothing */ }

  void TriangularMesh3DCenterlineExtractionView::on_ssaa_factor_changed(GLint ssaa_factor)
  { _pdata->ssaa_factor = ssaa_factor; }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void TriangularMesh3DCenterlineExtractionView::draw_opaque_impl()
  {
      // ubo 0 must be global ubo with modelview/projection matrices

      _pdata->meshView.draw_opaque();

      /*
       * draw picking ids
       */
      _pdata->meshView.ubo().bind_to_default_base();

      _pdata->fbo_picking.bind();
      BK_QT_GL glClearColor(-1, -1, -1, 0);
      BK_QT_GL glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      BK_QT_GL glClearColor(0, 0, 0, 0);

      _pdata->meshView.vao().bind();
      _pdata->shader_picking_ids.bind();
      BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->meshView.sizeInd(), GL_UNSIGNED_INT, nullptr);
      _pdata->shader_picking_ids.release();
      _pdata->meshView.vao().release();

      _pdata->fbo_picking.release();

      this->emit_signal_bind_default_fbo();

      const bool hasSelections = (_pdata->selected_point_id >= 0 && _pdata->selected_point_id < static_cast<int>(_pdata->mesh_vertices.size())) || (_pdata->start_point_id >= 0 && _pdata->start_point_id < static_cast<int>(_pdata->mesh_vertices.size())) || !_pdata->end_point_ids.empty();

      if (hasSelections)
      {
          _pdata->ubo_selection_sphere.bind_to_default_base();

          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<size_type>::max());

          _pdata->vao_selection_sphere.bind();
          _pdata->shader_selection_sphere.bind();
      }

      /*
       * draw current picking position
       */
      if (_pdata->selected_point_id >= 0 && _pdata->selected_point_id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->ubo_selection_sphere.set_center_x(_pdata->mesh_vertices[_pdata->selected_point_id][0]);
          _pdata->ubo_selection_sphere.set_center_y(_pdata->mesh_vertices[_pdata->selected_point_id][1]);
          _pdata->ubo_selection_sphere.set_center_z(_pdata->mesh_vertices[_pdata->selected_point_id][2]);
          _pdata->ubo_selection_sphere.set_color_r(static_cast<GLfloat>(0.25));
          _pdata->ubo_selection_sphere.set_color_g(static_cast<GLfloat>(0.5));
          _pdata->ubo_selection_sphere.set_color_b(static_cast<GLfloat>(1)); // light blue
          _pdata->ubo_selection_sphere.release();

          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd_selection_sphere, GL_UNSIGNED_INT, nullptr);
      }

      if (_pdata->start_point_id >= 0 && _pdata->start_point_id < static_cast<int>(_pdata->mesh_vertices.size()))
      {
          _pdata->ubo_selection_sphere.set_center_x(_pdata->mesh_vertices[_pdata->selected_point_id][0]);
          _pdata->ubo_selection_sphere.set_center_y(_pdata->mesh_vertices[_pdata->selected_point_id][1]);
          _pdata->ubo_selection_sphere.set_center_z(_pdata->mesh_vertices[_pdata->selected_point_id][2]);
          _pdata->ubo_selection_sphere.set_color_r(static_cast<GLfloat>(0));
          _pdata->ubo_selection_sphere.set_color_g(static_cast<GLfloat>(1));
          _pdata->ubo_selection_sphere.set_color_b(static_cast<GLfloat>(0)); // green
          _pdata->ubo_selection_sphere.release();

          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd_selection_sphere, GL_UNSIGNED_INT, nullptr);
      }

      for (auto id: _pdata->end_point_ids)
      {
          _pdata->ubo_selection_sphere.set_center_x(_pdata->mesh_vertices[id][0]);
          _pdata->ubo_selection_sphere.set_center_y(_pdata->mesh_vertices[id][1]);
          _pdata->ubo_selection_sphere.set_center_z(_pdata->mesh_vertices[id][2]);
          _pdata->ubo_selection_sphere.set_color_r(static_cast<GLfloat>(1));
          _pdata->ubo_selection_sphere.set_color_g(static_cast<GLfloat>(0));
          _pdata->ubo_selection_sphere.set_color_b(static_cast<GLfloat>(0)); // red
          _pdata->ubo_selection_sphere.release();

          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd_selection_sphere, GL_UNSIGNED_INT, nullptr);
      }

      if (hasSelections)
      {
          _pdata->ubo_selection_sphere.release_from_base();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);

          _pdata->shader_selection_sphere.release();
          _pdata->vao_selection_sphere.release();
      }

      _pdata->meshView.ubo().release_from_base();
  }

  void TriangularMesh3DCenterlineExtractionView::draw_transparent_impl()
  { _pdata->meshView.draw_transparent(); }
  /// @}
} // namespace bk


