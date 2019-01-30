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

#include<bkGL/renderable/mesh/TriangularMesh3DView.h>

#include <algorithm>
#include <vector>

#include <bk/Color>
#include <bk/NDContainer>
#include <bk/StringUtils>

#include<bkGL/renderable/mesh/EMeshRenderMode.h>
#include<bkGL/buffer/IBO.h>
#include<bkGL/buffer/VBO.h>
#include<bkGL/buffer/UBO.h>
#include<bkGL/buffer/SSBO.h>
#include<bkGL/shader/Shader.h>
#include<bkGL/shader/ShaderLibrary.h>
#include<bkGL/vao/VAO.h>
#include<bkGL/renderable/ColorBarView.h>
#include<bkGL/renderable/EColorScaleType.h>
#include<bkGL/UBOPhong.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct TriangularMesh3DView::Impl
  {
      VBO vbo;
      IBO ibo;
      VAO vao;
      details::UBOPhong ubo;
      Shader shader;
      Shader shader2;
      ColorBarView colorbarview;
      // ------- color attribute
      SSBO ssbo_colorbar;
      bool color_enabled;
      bool colorbar_enabled;
      GLint colorbar_num_colors;
      GLfloat color_attrib_min;
      GLfloat color_attrib_max;
      NDVector<double> color_attrib_values;
      bool color_attrib_time_dependent;
      GLuint color_attrib_num_times;
      GLfloat color_attrib_temporal_resolution;
      unsigned int mesh_num_points;
      // time
      GLint oldt0;
      GLint oldt1;
      double current_time;
      // ------- color attribute END
      GLsizei sizeInd;
      GLfloat shininess;
      GLfloat ghosted_view_falloff;
      GLfloat ghosted_view_cutoff;
      GLfloat silhouette_width;
      ColorRGBA color;
      ColorScaleType colorscale_type;
      Vec3<GLfloat> center;
      details::MeshRenderMode_ mode;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else
      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            ibo(gl),
            vao(gl),
            ubo(gl),
            shader(gl),
            shader2(gl),
            colorbarview(gl),
            ssbo_colorbar(gl),
          #endif
          color_enabled(false),
          colorbar_enabled(true),
          colorbar_num_colors(0),
          color_attrib_min(0),
          color_attrib_max(0),
          color_attrib_time_dependent(false),
          color_attrib_num_times(1),
          color_attrib_temporal_resolution(1),
          mesh_num_points(0),
          oldt0(-1),
          oldt1(-1),
          current_time(0),
          sizeInd(0),
          shininess(50),
          ghosted_view_falloff(3.5),
          ghosted_view_cutoff(0.85),
          silhouette_width(0.3),
          color(ColorRGBA::Green()),
          colorscale_type(ColorScaleType::Rainbow),
          center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
          mode(details::MeshRenderMode_Solid)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  TriangularMesh3DView::TriangularMesh3DView()
      : base_type()
  #else

  TriangularMesh3DView::TriangularMesh3DView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ibo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_3xfloat();
      _pdata->vao.add_default_attribute_normal_3xfloat();

      _pdata->colorbarview.set_position_horizontal_right();
      _pdata->colorbarview.set_position_vertical(0);
      _pdata->colorbarview.set_value_precision(1);
  }

  TriangularMesh3DView::TriangularMesh3DView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  TriangularMesh3DView::~TriangularMesh3DView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET MODE
  bool TriangularMesh3DView::mode_is_solid_default() const
  { return _pdata->mode == details::MeshRenderMode_Solid; }

  bool TriangularMesh3DView::mode_is_wireframe() const
  { return _pdata->mode == details::MeshRenderMode_WireFrame; }

  bool TriangularMesh3DView::mode_is_front_face_culling_with_ghosted_view() const
  { return _pdata->mode == details::MeshRenderMode_FrontFaceCullingWithGhostedView; }

  bool TriangularMesh3DView::mode_is_silhouette() const
  { return _pdata->mode == details::MeshRenderMode_Silhouette; }
  /// @}

  /// @{ -------------------------------------------------- GET SHININESS
  GLfloat TriangularMesh3DView::shininess() const
  { return _pdata->shininess; }
  /// @}

  /// @{ -------------------------------------------------- GET GHOSTED VIEW PARAMS
  GLfloat TriangularMesh3DView::ghosted_view_cutoff() const
  { return _pdata->ghosted_view_cutoff; }

  GLfloat TriangularMesh3DView::ghosted_view_falloff() const
  { return _pdata->ghosted_view_falloff; }
  /// @}

  /// @{ -------------------------------------------------- GET SILHOUETTE WIDTH
  GLfloat TriangularMesh3DView::silhouette_width() const
  { return _pdata->silhouette_width; }
  /// @}

  /// @{ -------------------------------------------------- GET COLOR
  const ColorRGBA& TriangularMesh3DView::color() const
  { return _pdata->color; }
  /// @}

  /// @{ -------------------------------------------------- GET CENTER
  Vec3<GLfloat> TriangularMesh3DView::center() const
  { return _pdata->center; }
  /// @}

  /// @{ -------------------------------------------------- GET COLOR ATTRIBUTE
  bool TriangularMesh3DView::color_attribute_is_time_dependent() const
  { return _pdata->color_attrib_time_dependent; }

  GLfloat TriangularMesh3DView::color_attribute_min_value() const
  { return _pdata->color_attrib_min; }

  GLfloat TriangularMesh3DView::color_attribute_max_value() const
  { return _pdata->color_attrib_max; }

  ColorBarView& TriangularMesh3DView::colorbarview()
  { return _pdata->colorbarview; }

  const ColorBarView& TriangularMesh3DView::colorbarview() const
  { return _pdata->colorbarview; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool TriangularMesh3DView::is_initialized() const
  { return _pdata->vao.is_initialized(); }
  /// @}

  unsigned int TriangularMesh3DView::_floats_per_vertex() const
  { return _pdata->color_enabled ? (_pdata->color_attrib_time_dependent ? 8 : 7) : 6; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET RENDER MODE
  void TriangularMesh3DView::set_mode_solid_default()
  {
      if (_pdata->mode != details::MeshRenderMode_Solid)
      {
          _pdata->mode = details::MeshRenderMode_Solid;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void TriangularMesh3DView::set_mode_wireframe()
  {
      if (_pdata->mode != details::MeshRenderMode_WireFrame)
      {
          _pdata->mode = details::MeshRenderMode_WireFrame;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void TriangularMesh3DView::set_mode_front_face_culling_with_ghosted_view()
  {
      if (_pdata->mode != details::MeshRenderMode_FrontFaceCullingWithGhostedView)
      {
          _pdata->mode = details::MeshRenderMode_FrontFaceCullingWithGhostedView;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void TriangularMesh3DView::set_mode_silhouette()
  {
      if (_pdata->mode != details::MeshRenderMode_Silhouette)
      {
          _pdata->mode = details::MeshRenderMode_Silhouette;

          if (this->is_initialized())
          {
              init_shader();
              this->emit_signal_update_required();
          }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET SHININESS
  void TriangularMesh3DView::set_shininess(GLfloat shininess)
  {
      _pdata->shininess = std::max(static_cast<GLfloat>(0), shininess);

      if (this->is_initialized())
      {
          _pdata->ubo.set_shininess(_pdata->shininess);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET GHOSTED VIEW PARAMS
  void TriangularMesh3DView::set_ghosted_view_falloff(GLfloat falloff)
  {
      _pdata->ghosted_view_falloff = std::max(static_cast<GLfloat>(0), falloff);

      if (this->is_initialized())
      {
          _pdata->ubo.set_ghostFalloff(_pdata->ghosted_view_falloff);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DView::set_ghosted_view_cutoff(GLfloat cutoff)
  {
      _pdata->ghosted_view_cutoff = std::max(static_cast<GLfloat>(0), cutoff);

      if (this->is_initialized())
      {
          _pdata->ubo.set_ghostCutoff(_pdata->ghosted_view_cutoff);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET SILHOUETTE WIDTH
  void TriangularMesh3DView::set_silhouette_width(GLfloat w)
  {
      _pdata->silhouette_width = std::max(static_cast<GLfloat>(0), std::min(static_cast<GLfloat>(1), w));

      if (this->is_initialized())
      {
          _pdata->ubo.set_silhouette_width(_pdata->silhouette_width);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void TriangularMesh3DView::_update_ubo_color()
  {
      if (this->is_initialized())
      {
          _pdata->ubo.set_meshcol_r(_pdata->color[0]);
          _pdata->ubo.set_meshcol_g(_pdata->color[1]);
          _pdata->ubo.set_meshcol_b(_pdata->color[2]);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DView::set_color(const ColorRGBA& c)
  {
      _pdata->color.set(c);
      this->_update_ubo_color();
  }

  void TriangularMesh3DView::set_color(ColorRGBA&& c)
  {
      _pdata->color.set(std::move(c));
      this->_update_ubo_color();
  }

  void TriangularMesh3DView::set_color(double r, double g, double b, double a)
  {
      _pdata->color.set(r, g, b, a);
      this->_update_ubo_color();
  }
  /// @}

  /// @{ -------------------------------------------------- SET COLORBAR
  void TriangularMesh3DView::_set_colorbar(const ColorBarRGBA& colbar)
  {
      _pdata->ssbo_colorbar.clear();

      _pdata->colorbar_num_colors = colbar.num_colors();

      std::vector<GLfloat> rgbaVals;
      rgbaVals.reserve(_pdata->colorbar_num_colors * 4);

      for (int i = 0; i < _pdata->colorbar_num_colors; ++i)
      {
          const ColorRGBA& col = colbar[i];

          rgbaVals.push_back(col[0]);
          rgbaVals.push_back(col[1]);
          rgbaVals.push_back(col[2]);
          rgbaVals.push_back(col[3]);
      }

      _pdata->ssbo_colorbar.init(rgbaVals);

      if (this->is_initialized())
      {
          _pdata->ubo.set_num_colors(_pdata->colorbar_num_colors);
          _pdata->ubo.release();

          switch (_pdata->colorscale_type)
          {
              case ColorScaleType::Heat:
              {
                  _pdata->colorbarview.init_heat();
                  break;
              }
              case ColorScaleType::Rainbow:
              {
                  _pdata->colorbarview.init_rainbow();
                  break;
              }
              case ColorScaleType::BlueToRed:
              {
                  _pdata->colorbarview.init_blue_to_red();
                  break;
              }
              case ColorScaleType::Magenta:
              {
                  _pdata->colorbarview.init_magenta();
                  break;
              }
              case ColorScaleType::LightBlueToYellow:
              {
                  _pdata->colorbarview.init_light_blue_to_black_to_yellow();
                  break;
              }
              case ColorScaleType::GreenToRed:
              {
                  _pdata->colorbarview.init_green_to_white_to_red();
                  break;
              }
                  //case ColorScaleType::Gray: [[fallthrough]]
                  //case ColorScaleType::Cluster: [[fallthrough]]
                  //case ColorScaleType::TrafficLight: [[fallthrough]]
                  //case ColorScaleType::UniformYellow: [[fallthrough]]
              default:
              { /* do nothing */ }
          }

          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DView::set_colorbar_heat()
  {
      _pdata->colorscale_type = ColorScaleType::Heat;
      _set_colorbar(ColorBarRGBA::Heat());
  }

  void TriangularMesh3DView::set_colorbar_rainbow()
  {
      _pdata->colorscale_type = ColorScaleType::Rainbow;
      _set_colorbar(ColorBarRGBA::Rainbow());
  }

  void TriangularMesh3DView::set_colorbar_blue_to_red()
  {
      _pdata->colorscale_type = ColorScaleType::BlueToRed;
      _set_colorbar(ColorBarRGBA::Blue_White_Red());
  }

  void TriangularMesh3DView::set_colorbar_magenta()
  {
      _pdata->colorscale_type = ColorScaleType::Magenta;
      _set_colorbar(ColorBarRGBA::Magenta());
  }

  void TriangularMesh3DView::set_colorbar_light_blue_to_yellow()
  {
      _pdata->colorscale_type = ColorScaleType::LightBlueToYellow;
      _set_colorbar(ColorBarRGBA::Light_Blue_Black_Yellow());
  }

  void TriangularMesh3DView::set_colorbar_green_to_red()
  {
      _pdata->colorscale_type = ColorScaleType::GreenToRed;
      _set_colorbar(ColorBarRGBA::Green_White_Red());
  }

  void TriangularMesh3DView::set_colorbar_plasma()
  {
      _pdata->colorscale_type = ColorScaleType::Plasma;
      _set_colorbar(ColorBarRGBA::Plasma());
  }

  void TriangularMesh3DView::set_enable_colorbar(bool b)
  {
      if (_pdata->colorbar_enabled != b)
      {
          _pdata->colorbar_enabled = b;

          if (this->is_initialized())
          {this->emit_signal_update_required();}
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET ATTRIBUTE
  void TriangularMesh3DView::enable_time_dependent_attribute(GLuint numTimes, GLfloat temporalResolution)
  {
      _pdata->color_attrib_time_dependent = true;
      _pdata->color_attrib_num_times = numTimes;
      _pdata->color_attrib_temporal_resolution = temporalResolution;
  }

  void TriangularMesh3DView::disable_time_dependent_attribute()
  {
      _pdata->color_attrib_time_dependent = false;
      _pdata->color_attrib_num_times = 1;
      _pdata->color_attrib_temporal_resolution = 1;
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto TriangularMesh3DView::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void TriangularMesh3DView::clear_shader()
  {
      _pdata->shader.clear();
      _pdata->shader2.clear();
  }

  void TriangularMesh3DView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->ibo.clear();
      _pdata->vao.clear();
      _pdata->ubo.clear();
      _pdata->sizeInd = 0;

      _pdata->ssbo_colorbar.clear();
      _pdata->colorbar_num_colors = 0;
  }

  void TriangularMesh3DView::clear()
  {
      clear_shader();
      clear_buffers();
      _pdata->colorbarview.clear();

      _pdata->color_attrib_values.clear();
      _pdata->oldt0 = -1;
      _pdata->oldt1 = -1;

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void TriangularMesh3DView::init_mesh(const TriangularMesh3D& mesh, std::string_view color_attribute_name)
  {
      _pdata->mesh_num_points = mesh.geometry().num_points();
      const unsigned int num_triangles = mesh.topology().num_cells();
      if (_pdata->mesh_num_points == 0 || num_triangles == 0)
      { return; }

      _pdata->color_enabled = !color_attribute_name.empty() && mesh.point_attribute_map().has_attribute(color_attribute_name);

      _pdata->vao.clear_attributes();
      _pdata->vao.add_default_attribute_position_3xfloat();
      _pdata->vao.add_default_attribute_normal_3xfloat();

      if (_pdata->color_enabled)
      {
          _pdata->vao.add_default_attribute_scalar_1xfloat("attribute_t0");

          if (_pdata->color_attrib_time_dependent)
          {
              _pdata->vao.add_default_attribute_scalar_1xfloat("attribute_t1");

              const std::vector<MatXd>& attrib = mesh.template point_attribute_vector_of_type<MatXd>(color_attribute_name);

              _pdata->color_attrib_values.resize(_pdata->mesh_num_points, _pdata->color_attrib_num_times);

              // copy attribute values
              #pragma omp parallel for
              for (unsigned int pid = 0; pid < _pdata->mesh_num_points; ++pid)
              {
                  for (unsigned int tid = 0; tid < _pdata->color_attrib_num_times; ++tid)
                  { _pdata->color_attrib_values(pid, tid) = attrib[pid][tid]; } // for tid: num_times
              } // for pid: num_points

              const auto[itMin, itMax] = std::minmax_element(_pdata->color_attrib_values.begin(), _pdata->color_attrib_values.end());
              _pdata->color_attrib_min = *itMin;
              _pdata->color_attrib_max = *itMax;
          }
          else
          {
              const std::vector<double>& attrib = mesh.template point_attribute_vector_of_type<double>(color_attribute_name);
              const auto[itMin, itMax] = std::minmax_element(attrib.begin(), attrib.end());
              _pdata->color_attrib_min = *itMin;
              _pdata->color_attrib_max = *itMax;
          }

          std::stringstream s;
          s << "Surface: \"";
          s << color_attribute_name;
          s << "\"";
          _pdata->colorbarview.set_title(s.str());
      }

      const unsigned int floatsPerVertex = _floats_per_vertex();
      std::vector<GLfloat> vertices_normals(floatsPerVertex * _pdata->mesh_num_points);
      std::vector<GLuint> indices(3 * num_triangles);

      #pragma omp parallel sections
      {
          #pragma omp section
          {
              this->_pdata->center.set_zero();

              #pragma omp parallel for
              for (unsigned int i = 0; i < _pdata->mesh_num_points; ++i)
              {
                  const auto pt = mesh.geometry().point(i);
                  vertices_normals[floatsPerVertex * i + 0] = pt[0];
                  vertices_normals[floatsPerVertex * i + 1] = pt[1];
                  vertices_normals[floatsPerVertex * i + 2] = pt[2];

                  #pragma omp critical
                  { this->_pdata->center += pt; }

                  if (mesh.has_normals())
                  {
                      const auto nrml = mesh.normal_of_point(i);
                      vertices_normals[floatsPerVertex * i + 3] = nrml[0];
                      vertices_normals[floatsPerVertex * i + 4] = nrml[1];
                      vertices_normals[floatsPerVertex * i + 5] = nrml[2];
                  }
                  else
                  {
                      vertices_normals[floatsPerVertex * i + 3] = 0;
                      vertices_normals[floatsPerVertex * i + 4] = 0;
                      vertices_normals[floatsPerVertex * i + 5] = 1;
                  }

                  if (this->_pdata->color_enabled)
                  {
                      if (!_pdata->color_attrib_time_dependent)
                      {
                          const GLfloat attrib_value = static_cast<GLfloat>(mesh.point_attribute_value_of_type<double>(color_attribute_name, i));
                          vertices_normals[floatsPerVertex * i + 6] = attrib_value;

                          //#pragma omp critical
                          //{
                          //    _pdata->color_attrib_min = std::min(_pdata->color_attrib_min, attrib_value);
                          //    _pdata->color_attrib_max = std::max(_pdata->color_attrib_max, attrib_value);
                          //}
                      }
                      else
                      {
                          vertices_normals[floatsPerVertex * i + 6] = 0;
                          vertices_normals[floatsPerVertex * i + 7] = 0;
                      }
                  }
              }

              this->_pdata->center /= _pdata->mesh_num_points;
          } // #pragma omp section

          #pragma omp section
          {
              #pragma omp parallel for
              for (unsigned int i = 0; i < num_triangles; ++i)
              {
                  const auto& tri = mesh.topology().cell(i);
                  indices[3 * i + 0] = tri[0];
                  indices[3 * i + 1] = tri[1];
                  indices[3 * i + 2] = tri[2];
              }

              _pdata->sizeInd = 3 * num_triangles;
          } // #pragma omp section
      } // #pragma omp parallel sections

      _pdata->vbo.init(vertices_normals);
      _pdata->ibo.init(indices);
      _pdata->vao.init(_pdata->vbo, _pdata->ibo);

      if (_pdata->color_enabled)
      {
          _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max);
          set_colorbar_rainbow();

          if (_pdata->color_attrib_time_dependent)
          { update_attribute(); }
      }

      this->emit_signal_scene_changed();
  }

  void TriangularMesh3DView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary;

      switch (_pdata->mode)
      {
          case details::MeshRenderMode_Solid:
          {
              if (!_pdata->color_enabled)
              { _pdata->shader.init_from_sources(SL::mesh::phong::vert(), SL::mesh::phong::frag()); }
              else
              { _pdata->shader.init_from_sources(SL::mesh::phong::vert_color(_pdata->color_attrib_time_dependent), SL::mesh::phong::frag_color(_pdata->color_attrib_time_dependent)); }

              break;
          }
          case details::MeshRenderMode_WireFrame:
          {
              if (!_pdata->color_enabled)
              { _pdata->shader.init_from_sources(SL::mesh::wireframe::vert(), SL::mesh::wireframe::frag(), SL::mesh::wireframe::geom()); }
              else
              { _pdata->shader.init_from_sources(SL::mesh::wireframe::vert(), SL::mesh::wireframe::frag_color(_pdata->color_attrib_time_dependent), SL::mesh::wireframe::geom_color(_pdata->color_attrib_time_dependent)); }

              break;
          }
          case details::MeshRenderMode_FrontFaceCullingWithGhostedView:
          {
              if (!_pdata->color_enabled)
              { _pdata->shader.init_from_sources(SL::mesh::phong::vert(), SL::mesh::phong::frag()); }
              else
              { _pdata->shader.init_from_sources(SL::mesh::phong::vert_color(_pdata->color_attrib_time_dependent), SL::mesh::phong::frag_color(_pdata->color_attrib_time_dependent)); }

              if (!this->oit_is_available())
              {
                  if (!_pdata->color_enabled)
                  { _pdata->shader2.init_from_sources(SL::mesh::phong::vert(), SL::mesh::phong::frag_ghosted()); }
                  else
                  { _pdata->shader2.init_from_sources(SL::mesh::phong::vert_color(_pdata->color_attrib_time_dependent), SL::mesh::phong::frag_ghosted_color(_pdata->color_attrib_time_dependent)); }
              }
              else
              {
                  if (!_pdata->color_enabled)
                  { _pdata->shader2.init_from_sources(SL::mesh::phong::vert(), SL::mesh::phong::frag_ghosted_oit()); }
                  else
                  { _pdata->shader2.init_from_sources(SL::mesh::phong::vert_color(_pdata->color_attrib_time_dependent), SL::mesh::phong::frag_ghosted_color_oit(_pdata->color_attrib_time_dependent)); }
              }

              break;
          }
          case details::MeshRenderMode_Silhouette:
          {
              if (!_pdata->color_enabled)
              { _pdata->shader.init_from_sources(SL::mesh::silhouette::vert(), SL::mesh::silhouette::frag()); }
              else
              { _pdata->shader.init_from_sources(SL::mesh::silhouette::vert_color(_pdata->color_attrib_time_dependent), SL::mesh::silhouette::frag_color(_pdata->color_attrib_time_dependent)); }

              break;
          }
      }
  }

  void TriangularMesh3DView::init_ubo()
  {
      constexpr GLfloat lightCol = 0.8;

      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_meshcol_r(_pdata->color[0]);
      _pdata->ubo.set_meshcol_g(_pdata->color[1]);
      _pdata->ubo.set_meshcol_b(_pdata->color[2]);
      _pdata->ubo.set_lightcol_r(lightCol);
      _pdata->ubo.set_lightcol_g(lightCol);
      _pdata->ubo.set_lightcol_b(lightCol);
      _pdata->ubo.set_shininess(_pdata->shininess);
      _pdata->ubo.set_ghostFalloff(_pdata->ghosted_view_falloff);
      _pdata->ubo.set_ghostCutoff(_pdata->ghosted_view_cutoff);
      _pdata->ubo.set_silhouette_width(_pdata->silhouette_width);
      _pdata->ubo.set_color_enabled(_pdata->color_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_num_colors(_pdata->colorbar_num_colors);
      _pdata->ubo.set_min_attribute_value(_pdata->color_attrib_min);
      _pdata->ubo.set_max_attribute_value(_pdata->color_attrib_max);
      _pdata->ubo.set_temporal_resolution(_pdata->color_attrib_temporal_resolution);

      _pdata->ubo.release();
  }

  void TriangularMesh3DView::init(const TriangularMesh3D& mesh, std::string_view color_attribute_name)
  {
      _pdata->oldt0 = -1;
      _pdata->oldt1 = -1;

      init_mesh(mesh, color_attribute_name);
      init_shader();
      init_ubo();

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}

  /// @{ -------------------------------------------------- COLOR ATTRIBUTE
  void TriangularMesh3DView::set_color_attribute(const TriangularMesh3D& mesh, std::string_view color_attribute_name)
  {
      if (!this->is_initialized())
      { return; }

      //------------------------------------------------------------------------------------------------------
      // colors were not yet allocated in the vbo
      //------------------------------------------------------------------------------------------------------
      if (!_pdata->color_enabled)
      {
          init(mesh, color_attribute_name);
          return;
      }

      //------------------------------------------------------------------------------------------------------
      // invalid line set or non-existing attribute?
      //------------------------------------------------------------------------------------------------------
      if (mesh.geometry().num_points() == 0)
      { return; }

      _pdata->color_enabled = !color_attribute_name.empty() && mesh.point_attribute_map().has_attribute(color_attribute_name);

      if (!_pdata->color_enabled)
      {
          init(mesh);
          return;
      }

      //------------------------------------------------------------------------------------------------------
      // colors values were allocated in the vbo; map and overwrite
      //------------------------------------------------------------------------------------------------------
      GLfloat* vbodata = _pdata->vbo.map_read_write<GLfloat>();
      if (vbodata != nullptr)
      {
          _pdata->color_attrib_min = std::numeric_limits<GLfloat>::max();
          _pdata->color_attrib_max = -_pdata->color_attrib_min;

          constexpr const unsigned int floatsPerVertex = 7; // 3 vertex, 3 normal, 1 attrib;

          for (unsigned int k = 0; k < mesh.geometry().num_points(); ++k)
          {
              const GLfloat attrib_value = static_cast<GLfloat>(mesh.point_attribute_value_of_type<double>(color_attribute_name, k));

              vbodata[floatsPerVertex * k + 6] = attrib_value;
              _pdata->color_attrib_min = std::min(_pdata->color_attrib_min, attrib_value);
              _pdata->color_attrib_max = std::max(_pdata->color_attrib_max, attrib_value);
          } // for num_points

          _pdata->vbo.unmap_and_release();

          _pdata->ubo.set_min_attribute_value(_pdata->color_attrib_min);
          _pdata->ubo.set_max_attribute_value(_pdata->color_attrib_max);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DView::clear_color_attribute()
  {
      _pdata->color_enabled = false;
      _pdata->ssbo_colorbar.clear();
      _pdata->color_attrib_min = std::numeric_limits<GLfloat>::max();
      _pdata->color_attrib_max = std::numeric_limits<GLfloat>::lowest();

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_enabled(_pdata->color_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
          _pdata->ubo.set_min_attribute_value(_pdata->color_attrib_min);
          _pdata->ubo.set_max_attribute_value(_pdata->color_attrib_max);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void TriangularMesh3DView::set_colorbar_min_max_value(double vmin, double vmax)
  {
      _pdata->ubo.set_min_attribute_value(vmin);
      _pdata->ubo.set_max_attribute_value(vmax);
      _pdata->ubo.release();
  }

  void TriangularMesh3DView::update_attribute()
  {
      if (!this->_pdata->color_enabled || !_pdata->color_attrib_time_dependent)
      { return; }

      const unsigned int numTimes = _pdata->color_attrib_values.size(1);
      const GLuint t0 = std::min(static_cast<GLuint>(std::floor(_pdata->current_time / _pdata->color_attrib_temporal_resolution)), numTimes - 1);
      const GLuint t1 = (t0 + 1) % numTimes;

      if (static_cast<int>(t0) == _pdata->oldt0 && static_cast<int>(t1) == _pdata->oldt1) // up to date
      { return; }

      GLfloat* data = _pdata->vbo.map_write_only<GLfloat>();
      if (data != nullptr)
      {
          const unsigned int floatsPerVertex = _floats_per_vertex();

          #pragma omp parallel for
          for (unsigned int i = 0; i < _pdata->mesh_num_points; ++i)
          {
              data[floatsPerVertex * i + 6] = _pdata->color_attrib_values(i, t0);
              data[floatsPerVertex * i + 7] = _pdata->color_attrib_values(i, t1);
          }

          _pdata->vbo.unmap_and_release();

          _pdata->oldt0 = t0;
          _pdata->oldt1 = t1;

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void TriangularMesh3DView::on_resize(GLint w, GLint h)
  { _pdata->colorbarview.on_resize(w, h); }

  void TriangularMesh3DView::on_oit_enabled(bool b)
  {
      if (_pdata->mode == details::MeshRenderMode_FrontFaceCullingWithGhostedView)
      { init_shader(); }

      _pdata->colorbarview.on_oit_enabled(b);
  }

  void TriangularMesh3DView::on_animation_enabled(bool b)
  { _pdata->colorbarview.on_animation_enabled(b); }

  void TriangularMesh3DView::on_modelview_matrix_changed(bool b)
  { _pdata->colorbarview.on_modelview_matrix_changed(b); }

  void TriangularMesh3DView::on_new_modelview_matrix(const ColMat4<GLfloat>& m)
  { _pdata->colorbarview.on_new_modelview_matrix(m); }

  void TriangularMesh3DView::on_new_projection_matrix(const ColMat4<GLfloat>& p)
  { _pdata->colorbarview.on_new_projection_matrix(p); }

  void TriangularMesh3DView::on_visible_changed(bool b)
  { _pdata->colorbarview.on_visible_changed(b); }

  void TriangularMesh3DView::on_mouse_pos_changed(GLint x, GLint y)
  { _pdata->colorbarview.on_mouse_pos_changed(x, y); }

  void TriangularMesh3DView::on_mouse_button_pressed(MouseButton_ btn)
  { _pdata->colorbarview.on_mouse_button_pressed(btn); }

  void TriangularMesh3DView::on_mouse_button_released(MouseButton_ btn)
  { _pdata->colorbarview.on_mouse_button_released(btn); }

  void TriangularMesh3DView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void TriangularMesh3DView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void TriangularMesh3DView::on_mouse_wheel_up()
  { /* do nothing */ }

  void TriangularMesh3DView::on_mouse_wheel_down()
  { /* do nothing */ }

  void TriangularMesh3DView::on_ssaa_factor_changed(GLint ssaa_factor)
  { _pdata->colorbarview.on_ssaa_factor_changed(ssaa_factor); }

  void TriangularMesh3DView::on_animation_time_changed(GLfloat t)
  {
      _pdata->current_time = t;

      if (_pdata->color_attrib_time_dependent && this->is_visible())
      { update_attribute(); }

      _pdata->colorbarview.on_animation_time_changed(t);
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void TriangularMesh3DView::draw_opaque_impl()
  {
      // ubo 0 must be global ubo with modelview/projection matrices
      _pdata->ubo.bind_to_default_base();

      if (_pdata->color_enabled)
      { _pdata->ssbo_colorbar.bind_to_base(7); }

      if (_pdata->mode == details::MeshRenderMode_FrontFaceCullingWithGhostedView)
      {
          // first pass: back side
          BK_QT_GL glPushAttrib(GL_POLYGON_BIT);

          BK_QT_GL glEnable(GL_CULL_FACE);
          BK_QT_GL glFrontFace(GL_CCW);
          BK_QT_GL glCullFace(GL_FRONT);

          _pdata->vao.bind();
          _pdata->shader.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader.release();
          _pdata->vao.release();

          BK_QT_GL glPopAttrib();
      }
      else
      {
          BK_QT_GL glDepthFunc(GL_LEQUAL);

          _pdata->vao.bind();
          _pdata->shader.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader.release();
          _pdata->vao.release();

          BK_QT_GL glDepthFunc(GL_LESS);
      }

      if (_pdata->color_enabled)
      { _pdata->ssbo_colorbar.release_from_base(); }

      _pdata->ubo.release_from_base();

      //------------------------------------------------------------------------------------------------------
      // colorbar view
      //------------------------------------------------------------------------------------------------------
      if (_pdata->colorbar_enabled && _pdata->color_enabled)
      { _pdata->colorbarview.draw(); }
  }

  void TriangularMesh3DView::draw_transparent_impl()
  {
      if (_pdata->mode == details::MeshRenderMode_FrontFaceCullingWithGhostedView)
      {
          // ubo 0 must be global ubo with modelview/projection matrices
          _pdata->ubo.bind_to_default_base();

          if (_pdata->color_enabled)
          { _pdata->ssbo_colorbar.bind_to_base(7); }

          // second pass: front side
          BK_QT_GL glPushAttrib(GL_POLYGON_BIT);

          BK_QT_GL glEnable(GL_CULL_FACE);
          BK_QT_GL glFrontFace(GL_CCW);
          BK_QT_GL glCullFace(GL_BACK);

          if (this->oit_is_available())
          {
              BK_QT_GL glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
              BK_QT_GL glDepthMask(GL_FALSE);
          }

          _pdata->vao.bind();
          _pdata->shader2.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader2.release();
          _pdata->vao.release();

          if (this->oit_is_available())
          {
              BK_QT_GL glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
              BK_QT_GL glDepthMask(GL_TRUE);
          }

          BK_QT_GL glPopAttrib();

          if (_pdata->color_enabled)
          { _pdata->ssbo_colorbar.release_from_base(); }

          _pdata->ubo.release_from_base();
      }
  }
  /// @}
} // namespace bk