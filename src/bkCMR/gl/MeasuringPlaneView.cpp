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

#include <bkCMR/gl/MeasuringPlaneView.h>

#include <algorithm>
#include <limits>
#include <vector>

#include <bk/Color>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/texture/Texture3D.h>
#include <bkGL/buffer/SSBO.h>
#include <bkCMR/gl/ShaderLibrary_CMR.h>
#include <bkCMR/gl/UBOMeasuringPlane.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct MeasuringPlaneView::Impl
  {
      VBO vbo;
      VAO vao;
      UBOMeasuringPlane ubo;
      Shader shader;
      Texture3D tex;
      SSBO ssbo_colorbar;
      GLint colorbar_num_colors;
      ColorRGBA color;
      Vec3<GLfloat> center;
      bool no_plane_is_set;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl),
            ubo(gl),
            shader(gl),
            tex(gl),
            ssbo_colorbar(gl),
          #endif
          colorbar_num_colors(0),
          color(ColorRGBA::Yellow()),
          center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
          no_plane_is_set(true)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = default;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  MeasuringPlaneView::MeasuringPlaneView()
  #else

  MeasuringPlaneView::MeasuringPlaneView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_scalar_1xfloat("attrib");
  }

  MeasuringPlaneView::MeasuringPlaneView(MeasuringPlaneView&&) noexcept = default;
  MeasuringPlaneView::~MeasuringPlaneView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  const ColorRGBA& MeasuringPlaneView::color() const
  { return _pdata->color; }

  Vec3<GLfloat> MeasuringPlaneView::center() const
  { return _pdata->center; }

  bool MeasuringPlaneView::is_initialized() const
  { return _pdata->vao.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void MeasuringPlaneView::_update_ubo_color()
  {
      if (this->is_initialized())
      {
          _pdata->ubo.set_color_r(_pdata->color[0]);
          _pdata->ubo.set_color_g(_pdata->color[1]);
          _pdata->ubo.set_color_b(_pdata->color[2]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void MeasuringPlaneView::set_color(const ColorRGBA& c)
  { set_color(c.r(), c.g(), c.b(), c.alpha()); }

  void MeasuringPlaneView::set_color(double r, double g, double b, double a)
  {
      _pdata->color.set(r, g, b, a);
      this->_update_ubo_color();
  }

  void MeasuringPlaneView::_set_colorbar(const ColorBarRGBA& colbar)
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

          this->emit_signal_update_required();
      }
  }

  void MeasuringPlaneView::set_colorbar_heat()
  { _set_colorbar(ColorBarRGBA::Heat()); }

  void MeasuringPlaneView::set_colorbar_rainbow()
  { _set_colorbar(ColorBarRGBA::Rainbow()); }

  void MeasuringPlaneView::set_colorbar_blue_to_red()
  { _set_colorbar(ColorBarRGBA::Blue_To_Red()); }

  void MeasuringPlaneView::set_colorbar_magenta()
  { _set_colorbar(ColorBarRGBA::Magenta()); }

  void MeasuringPlaneView::set_colorbar_uniform_yellow()
  { _set_colorbar(ColorBarRGBA::Uniform_Yellow()); }

  void MeasuringPlaneView::set_colorbar_min_max_value(double vmin, double vmax)
  {
      _pdata->ubo.set_vmin(vmin);
      _pdata->ubo.set_vmax(vmax);
      _pdata->ubo.release();

      this->emit_signal_update_required();
  }

  MeasuringPlaneView& MeasuringPlaneView::operator=(MeasuringPlaneView&&) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void MeasuringPlaneView::clear_shader()
  { _pdata->shader.clear(); }

  void MeasuringPlaneView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
      _pdata->ubo.clear();
      _pdata->ssbo_colorbar.clear();
  }

  void MeasuringPlaneView::clear()
  {
      clear_shader();
      clear_buffers();
      _pdata->tex.clear();
      _pdata->no_plane_is_set = true;

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void MeasuringPlaneView::set_plane(const MeasuringPlane& plane)
  {
      const Vec3d& c = plane.geometry().transformation().center();
      const Vec3d& nx = plane.geometry().transformation().nx();
      const Vec3d& ny = plane.geometry().transformation().ny();
      const Vec3d& nz = plane.geometry().transformation().nz();
      const Vec3i& gs = plane.geometry().transformation().grid_size();
      const Vec3d& s = plane.geometry().transformation().scale();

      _pdata->ubo.set_center_x(c[0]);
      _pdata->ubo.set_center_y(c[1]);
      _pdata->ubo.set_center_z(c[2]);
      _pdata->ubo.set_nx_x(nx[0]);
      _pdata->ubo.set_nx_y(nx[1]);
      _pdata->ubo.set_nx_z(nx[2]);
      _pdata->ubo.set_ny_x(ny[0]);
      _pdata->ubo.set_ny_y(ny[1]);
      _pdata->ubo.set_ny_z(ny[2]);
      _pdata->ubo.set_nz_x(nz[0]);
      _pdata->ubo.set_nz_y(nz[1]);
      _pdata->ubo.set_nz_z(nz[2]);
      _pdata->ubo.set_scale_x(s[0]);
      _pdata->ubo.set_scale_y(s[1]);
      _pdata->ubo.set_grid_size_x(gs[0]);
      _pdata->ubo.set_grid_size_y(gs[1]);
      _pdata->ubo.set_values_initialized(0);
      _pdata->ubo.release();

      _pdata->no_plane_is_set = false;

      //_pdata->center = plane.geometry().transformation().center();
      //this->emit_signal_scene_changed();

      this->emit_signal_update_required();
  }

  void MeasuringPlaneView::set_color_attribute(const MeasuringPlane& plane, std::string_view color_attribute_name)
  {
      const Vec3i& gs = plane.geometry().transformation().grid_size();

      _pdata->tex.clear();
      _pdata->tex.set_default_config_r_tex();
      _pdata->tex.set_size(gs[0], gs[1], gs[2]);
      _pdata->tex.set_texture_unit_number(1);
      std::vector<GLfloat> texvals(gs[0] * gs[1] * gs[2]);
      GLfloat vmin = std::numeric_limits<GLfloat>::max();
      GLfloat vmax = -vmin;

      const bool hasAttrib = plane.point_attribute_map().has_attribute(color_attribute_name);

      #pragma omp parallel for reduction (min:vmin) reduction(max:vmax)
      for (unsigned int t = 0; t < static_cast<unsigned int>(gs[2]); ++t)
      {
          for (unsigned int y = 0; y < static_cast<unsigned int>(gs[1]); ++y)
          {
              for (unsigned int x = 0; x < static_cast<unsigned int>(gs[0]); ++x)
              {
                  const GLfloat v = hasAttrib ? plane.point_attribute_value_of_type<double>(color_attribute_name, x, y, t) : 0;
                  const unsigned int texoff = t * gs[1] * gs[0] + y * gs[0] + x;
                  texvals[texoff] = v;

                  //#pragma omp critical
                  //{
                  vmin = std::min(vmin, v);
                  vmax = std::max(vmax, v);
                  //}
              }
          }
      }

      _pdata->tex.init(static_cast<GLvoid*>(texvals.data()));

      _pdata->tex.bind();
      _pdata->tex.set_interpolation_LINEAR();
      _pdata->tex.set_texture_coordinates_CLAMP_TO_BORDER();
      _pdata->tex.release();

      _pdata->ubo.set_vmin(vmin);
      _pdata->ubo.set_vmax(vmax);
      _pdata->ubo.set_values_initialized(1);
      _pdata->ubo.release();

      this->emit_signal_update_required();
  }

  void MeasuringPlaneView::reset_plane()
  {
      constexpr const GLfloat ftemp = 0;
      constexpr const GLint itemp = 0;
      _pdata->ubo.set_center_x(ftemp);
      _pdata->ubo.set_center_y(ftemp);
      _pdata->ubo.set_center_z(ftemp);
      _pdata->ubo.set_nx_x(ftemp);
      _pdata->ubo.set_nx_y(ftemp);
      _pdata->ubo.set_nx_z(ftemp);
      _pdata->ubo.set_ny_x(ftemp);
      _pdata->ubo.set_ny_y(ftemp);
      _pdata->ubo.set_ny_z(ftemp);
      _pdata->ubo.set_nz_x(ftemp);
      _pdata->ubo.set_nz_y(ftemp);
      _pdata->ubo.set_nz_z(ftemp);
      _pdata->ubo.set_scale_x(ftemp);
      _pdata->ubo.set_scale_y(ftemp);
      _pdata->ubo.set_grid_size_x(itemp);
      _pdata->ubo.set_grid_size_y(itemp);
      _pdata->ubo.set_vmin(ftemp);
      _pdata->ubo.set_vmax(ftemp);
      _pdata->ubo.set_values_initialized(itemp);
      _pdata->ubo.release();

      _pdata->no_plane_is_set = true;

      //_pdata->center.set_zero();
      //this->emit_signal_scene_changed();

      this->emit_signal_update_required();
  }

  void MeasuringPlaneView::init_plane()
  {
      constexpr const GLfloat data[1] = {0};

      _pdata->vbo.init(data);
      _pdata->vao.init(_pdata->vbo);

      this->emit_signal_scene_changed();
  }

  void MeasuringPlaneView::init_shader()
  {
      clear_shader();

      using SL = ShaderLibrary_CMR::measuring_plane;
      _pdata->shader.init_from_sources(SL::vert(), SL::frag(), SL::geom());
  }

  void MeasuringPlaneView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_color_r(_pdata->color[0]);
      _pdata->ubo.set_color_g(_pdata->color[1]);
      _pdata->ubo.set_color_b(_pdata->color[2]);

      _pdata->ubo.release();
  }

  void MeasuringPlaneView::init(const MeasuringPlane& plane)
  {
      init();
      set_plane(plane);
  }

  void MeasuringPlaneView::init()
  {
      init_shader();
      init_ubo();
      init_plane();
  }

  void MeasuringPlaneView::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_oit_enabled(bool /*b*/)
  {
      init_shader();
      this->emit_signal_update_required();
  }

  void MeasuringPlaneView::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_modelview_changed(bool)
  { /* do nothing */ }

  void MeasuringPlaneView::on_visible_changed(bool)
  { /* do nothing */ }

  void MeasuringPlaneView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void MeasuringPlaneView::on_mouse_wheel_up()
  { /* do nothing */ }

  void MeasuringPlaneView::on_mouse_wheel_down()
  { /* do nothing */ }

  void MeasuringPlaneView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  void MeasuringPlaneView::draw_opaque_impl()
  {
      if (!_pdata->no_plane_is_set)
      {
          // ubo 0 must be global ubo with modelview/projection matrices
          _pdata->ubo.bind_to_default_base();
          _pdata->vao.bind();

          _pdata->tex.bind();
          _pdata->ssbo_colorbar.bind_to_base(7);

          _pdata->shader.bind();
          BK_QT_GL glDrawArrays(GL_POINTS, 0, 1);
          _pdata->shader.release();

          _pdata->tex.release();
          _pdata->ssbo_colorbar.release_from_base();

          _pdata->vao.release();
          _pdata->ubo.release_from_base();
      }
  }

  void MeasuringPlaneView::draw_transparent_impl()
  { /* do nothing */ }
} // namespace bk


