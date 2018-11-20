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

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include <bkGL/renderable/VectorView.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/renderable/ColorBarView.h>
#include <bkGL/renderable/EColorScaleType.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/UBOVectorView.h>
#include <bk/Localization>
#include <bk/Color>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct VectorView::Impl
  {
      VBO vbo;
      VAO vao;
      details::UBOVectorView ubo;
      Shader shader_opaque;
      Shader shader_transparent;
      ColorBarView colorbarview;
      SSBO ssbo_colorbar;
      //
      std::vector<std::vector<std::pair<Vec3d, double>>> values;
      bool colorbar_enabled;
      bool color_by_attribute_enabled;
      bool color_transparency_enabled;
      GLfloat color_alpha_correction;
      GLfloat color_attrib_min;
      GLfloat color_attrib_max;
      GLfloat color_attrib_min_manual;
      GLfloat color_attrib_max_manual;
      bool scale_attrib_to_colorbar;
      GLint colorbar_num_colors;
      GLsizei sizeInd;
      GLfloat line_width;
      bool isl_enabled;
      GLfloat shininess;
      bool halo_enabled;
      GLfloat halo_width_in_percent;
      bk::ColorRGBA color;
      ColorScaleType colorscale_type;
      Vec3<GLfloat> center;
      //
      bool is_time_dependent;
      GLint num_times;
      GLfloat temporal_resolution;
      //

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl),
            ubo(gl),
            shader_opaque(gl),
            shader_transparent(gl),
            colorbarview(gl),
            ssbo_colorbar(gl),
            fbo_lineao_g(gl),
            fbo_lineao_mipmap0(gl),
            fbo_lineao_mipmap1(gl),
            tex_lineao_noise(gl),
            screenquad(gl),
            shader_lineao_gbuffer(gl),
            shader_lineao_mipmap0(gl),
            shader_lineao_mipmap1(gl),
            shader_lineao(gl),
          #endif
          colorbar_enabled(true),
          color_by_attribute_enabled(true),
          color_transparency_enabled(false),
          color_alpha_correction(0.33), // todo options
          color_attrib_min(0),
          color_attrib_max(0),
          color_attrib_min_manual(0),
          color_attrib_max_manual(0),
          scale_attrib_to_colorbar(true),
          colorbar_num_colors(0),
          sizeInd(0),
          line_width(0.25 /*mm*/), // todo options
          isl_enabled(true), // todo options
          //shininess(75), // todo options
          shininess(100), // todo options
          halo_enabled(true), // todo options
          halo_width_in_percent(0.25), // todo options
          color(bk::ColorRGBA::Yellow()), // todo options
          colorscale_type(ColorScaleType::Heat),
          center(0, 0, 0),
          is_time_dependent(false),
          num_times(0),
        temporal_resolution(0)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  VectorView::VectorView()
  #else

  LineView::LineView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->ibo.set_usage_STATIC_DRAW();
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ssbo_colorbar.set_usage_STATIC_DRAW();

      // done in init_buffers()
      //_pdata->vao.add_default_attribute_position_3xfloat();
      //_pdata->vao.add_default_attribute_normal_3xfloat();
      //_pdata->vao.add_default_attribute_scalar_1xfloat("attrib");

      _pdata->colorbarview.set_position_horizontal_left();
      _pdata->colorbarview.set_position_vertical(0);
      _pdata->colorbarview.set_value_precision(1);
  }

  VectorView::VectorView(VectorView&&) noexcept = default;

  VectorView::~VectorView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  GLfloat VectorView::line_width() const
  { return _pdata->line_width; }

  const ColorRGBA& VectorView::color() const
  { return _pdata->color; }

  bool VectorView::color_by_attribute_is_enabled() const
  { return _pdata->color_by_attribute_enabled; }

  GLfloat VectorView::color_attribute_min() const
  { return _pdata->color_attrib_min; }

  GLfloat VectorView::color_attribute_max() const
  { return _pdata->color_attrib_max; }

  ColorBarView& VectorView::colorbarview()
  { return _pdata->colorbarview; }

  const ColorBarView& VectorView::colorbarview() const
  { return _pdata->colorbarview; }

  GLfloat VectorView::shininess() const
  { return _pdata->shininess; }

  bool VectorView::isl_is_enabled() const
  { return _pdata->isl_enabled; }

  GLfloat VectorView::halo_width_in_percent() const
  { return _pdata->halo_width_in_percent; }

  bool VectorView::halo_is_enabled() const
  { return _pdata->halo_enabled; }

  bk::Vec3<GLfloat> VectorView::center() const
  { return _pdata->center; }

  bool VectorView::is_initialized() const
  { return _pdata->vao.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  VectorView& VectorView::operator=(const VectorView&) = default;
  VectorView& VectorView::operator=(VectorView&&) noexcept = default;

  void VectorView::set_line_width(GLfloat w)
  {
      assert(w > 0 && "invalid line width");
      _pdata->line_width = w;

      if (this->is_initialized())
      {
          _pdata->ubo.set_line_width(_pdata->line_width);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void VectorView::set_color(const ColorRGBA& c)
  { set_color(c.r(), c.g(), c.b()); }

  void VectorView::set_color(double r, double g, double b)
  {
      _pdata->color.set(r, g, b, 1);

      if (this->is_initialized())
      {
          _pdata->ubo.set_linecol_r(_pdata->color[0]);
          _pdata->ubo.set_linecol_g(_pdata->color[1]);
          _pdata->ubo.set_linecol_b(_pdata->color[2]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void VectorView::set_color_by_attribute_enabled(bool b)
  {
      if (_pdata->color_by_attribute_enabled != b)
      {
          _pdata->color_by_attribute_enabled = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_color_enabled(_pdata->color_by_attribute_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
              _pdata->ubo.release();

              init_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void VectorView::enable_color_by_attribute()
  { set_color_by_attribute_enabled(true); }

  void VectorView::disable_color_by_attribute()
  { set_color_by_attribute_enabled(false); }

  void VectorView::_set_colorbar(const ColorBarRGBA& colbar)
  { _set_colorbar_manual_num_colors(colbar, colbar.num_colors()); }

  void VectorView::_set_colorbar_manual_num_colors(const ColorBarRGBA& colbar, unsigned int nColors)
  {
      _pdata->ssbo_colorbar.clear();

      _pdata->colorbar_num_colors = nColors;

      std::vector<GLfloat> rgbaVals;
      rgbaVals.reserve(_pdata->colorbar_num_colors * 4);

      for (int i = 0; i < _pdata->colorbar_num_colors; ++i)
      {
          const bk::ColorRGBA& col = colbar[i];
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
              case ColorScaleType::Heat:_pdata->colorbarview.init_heat();
                  break;
              case ColorScaleType::Rainbow:_pdata->colorbarview.init_rainbow();
                  break;
              case ColorScaleType::BlueToRed:_pdata->colorbarview.init_blue_to_red();
                  break;
              case ColorScaleType::Magenta:_pdata->colorbarview.init_magenta();
                  break;
              case ColorScaleType::Cluster:_pdata->colorbarview.init_cluster();
                  break;
              case ColorScaleType::TrafficLight:_pdata->colorbarview.init_traffic_light();
                  break;
              case ColorScaleType::UniformYellow:_pdata->colorbarview.init_uniform_yellow();
                  break;
              case ColorScaleType::GreenToRed:_pdata->colorbarview.init_green_to_white_to_red();
                  break;
              case ColorScaleType::LightBlueToYellow:_pdata->colorbarview.init_light_blue_to_black_to_yellow();
                  break;
                  //case ColorScaleType::Gray: [[fallthrough]]
              default:
              { /* do nothing */ }
          }

          this->emit_signal_update_required();
      }
  }

  void VectorView::set_colorbar_heat()
  {
      _pdata->colorscale_type = ColorScaleType::Heat;
      _set_colorbar(bk::ColorBarRGBA::Heat());
  }

  void VectorView::set_colorbar_rainbow()
  {
      _pdata->colorscale_type = ColorScaleType::Rainbow;
      _set_colorbar(bk::ColorBarRGBA::Rainbow());
  }

  void VectorView::set_colorbar_blue_to_red()
  {
      _pdata->colorscale_type = ColorScaleType::BlueToRed;
      _set_colorbar(bk::ColorBarRGBA::Blue_White_Red());
  }

  void VectorView::set_colorbar_green_to_red()
  {
      _pdata->colorscale_type = ColorScaleType::GreenToRed;
      _set_colorbar(bk::ColorBarRGBA::Green_White_Red());
  }

  void VectorView::set_colorbar_magenta()
  {
      _pdata->colorscale_type = ColorScaleType::Magenta;
      _set_colorbar(bk::ColorBarRGBA::Magenta());
  }

  void VectorView::set_colorbar_uniform_yellow()
  {
      _pdata->colorscale_type = ColorScaleType::UniformYellow;
      _set_colorbar(bk::ColorBarRGBA::Uniform_Yellow());
  }

  void VectorView::set_colorbar_traffic_light()
  {
      _pdata->colorscale_type = ColorScaleType::TrafficLight;
      _set_colorbar(bk::ColorBarRGBA::Traffic_Light());
  }

  void VectorView::set_colorbar_cluster()
  {
      _pdata->colorscale_type = ColorScaleType::Cluster;
      _set_colorbar_manual_num_colors(bk::ColorBarRGBA::Cluster(), static_cast<int>(_pdata->color_attrib_max) + 1);
  }

  void VectorView::set_colorbar_light_blue_to_yellow()
  {
      _pdata->colorscale_type = ColorScaleType::LightBlueToYellow;
      _set_colorbar(bk::ColorBarRGBA::Light_Blue_Black_Yellow());
  }

  void VectorView::set_enable_colorbar(bool b)
  {
      if (b != _pdata->colorbar_enabled)
      {
          _pdata->colorbar_enabled = b;

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  void VectorView::set_color_attribute_min(GLfloat v)
  {
      _pdata->color_attrib_min = v;

      if (this->is_initialized())
      {
          _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max);
          this->emit_signal_update_required();
      }
  }

  void VectorView::set_color_attribute_max(GLfloat v)
  {
      _pdata->color_attrib_max = v;

      if (this->is_initialized())
      {
          _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max);
          this->emit_signal_update_required();
      }
  }

  void VectorView::set_color_attribute_clamp_min(GLfloat v)
  {
      _pdata->color_attrib_min_manual = v;

      if (this->is_initialized())
      {
          _pdata->ubo.set_min_value(_pdata->color_attrib_min_manual);
          _pdata->ubo.release();

          _pdata->colorbarview.set_clamp_value_range(_pdata->color_attrib_min_manual, _pdata->color_attrib_max_manual);

          this->emit_signal_update_required();
      }
  }

  void VectorView::set_color_attribute_clamp_max(GLfloat v)
  {
      _pdata->color_attrib_max_manual = v;

      if (this->is_initialized())
      {
          _pdata->ubo.set_max_value(_pdata->color_attrib_max_manual);
          _pdata->ubo.release();

          _pdata->colorbarview.set_clamp_value_range(_pdata->color_attrib_min_manual, _pdata->color_attrib_max_manual);

          this->emit_signal_update_required();
      }
  }

  void VectorView::set_attrib_scale_to_colorbar(bool b)
  {
      if (_pdata->scale_attrib_to_colorbar != b)
      {
          _pdata->scale_attrib_to_colorbar = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_scale_attrib_to_colorbar(_pdata->scale_attrib_to_colorbar ? static_cast<GLint>(1) : static_cast<GLint>(0));
              _pdata->ubo.release();

              this->emit_signal_update_required();
          }
      }
  }

  void VectorView::set_color_attribute_transparency_enabled(bool b)
  {
      if (b != _pdata->color_transparency_enabled)
      {
          _pdata->color_transparency_enabled = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_color_transparency_enabled(_pdata->color_transparency_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
              _pdata->ubo.release();

              if (_pdata->color_by_attribute_enabled)
              { this->emit_signal_update_required(); }
          }
      }
  }

  void VectorView::set_shininess(GLfloat shininess)
  {
      _pdata->shininess = std::max(static_cast<GLfloat>(0), shininess);

      if (this->is_initialized())
      {
          _pdata->ubo.set_shininess(_pdata->shininess);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void VectorView::set_isl_enabled(bool b)
  {
      if (b != _pdata->isl_enabled)
      {
          _pdata->isl_enabled = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_isl_enabled(_pdata->isl_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
              _pdata->ubo.release();
              this->emit_signal_update_required();
          }
      }
  }

  void VectorView::set_halo_width_in_percent(GLfloat p)
  {
      _pdata->halo_width_in_percent = std::clamp(p, static_cast<GLfloat>(0), static_cast<GLfloat>(1));
      if (this->is_initialized())
      {
          _pdata->ubo.set_halo_width_in_percent(_pdata->halo_width_in_percent);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void VectorView::set_halo_enabled(bool b)
  {
      if (b != _pdata->halo_enabled)
      {
          _pdata->halo_enabled = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_halo_enabled(_pdata->halo_enabled ? static_cast<GLfloat>(1) : static_cast<GLfloat>(0));
              _pdata->ubo.release();
              this->emit_signal_update_required();
          }
      }
  }

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void VectorView::clear_shader()
  {
      _pdata->shader_opaque.clear();
      _pdata->shader_transparent.clear();
  }

  void VectorView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
      _pdata->ubo.clear();
      _pdata->sizeInd = 0;
  }

  void VectorView::clear()
  {
      clear_buffers();
      clear_shader();

      _pdata->values.clear();

      _pdata->color_transparency_enabled = false;
      _pdata->ssbo_colorbar.clear();
      _pdata->color_attrib_min = 0;
      _pdata->color_attrib_max = 0;
      _pdata->colorbarview.clear();

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_enabled(_pdata->color_by_attribute_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
          _pdata->ubo.set_min_value(_pdata->color_attrib_min);
          _pdata->ubo.set_max_value(_pdata->color_attrib_max);
          _pdata->ubo.set_color_transparency_enabled(_pdata->color_transparency_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
          _pdata->ubo.release();
      }

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void VectorView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::vector_view;

      //const std::string vert = SL::vert(_pdata->lines_have_time_attribute, _pdata->lines_have_color_attribute, _pdata->color_by_attribute_enabled);
      //const std::string geom = SL::geom(_pdata->lines_have_time_attribute, this->animation_is_enabled(), _pdata->lines_have_color_attribute, _pdata->color_by_attribute_enabled);
      //const std::string frag_opaque = SL::frag_opaque(_pdata->lines_have_time_attribute, this->animation_is_enabled(), _pdata->lines_have_color_attribute, _pdata->color_by_attribute_enabled);
      //const std::string frag_transparent = SL::frag_transparent(_pdata->lines_have_time_attribute, this->animation_is_enabled(), _pdata->lines_have_color_attribute, _pdata->color_by_attribute_enabled, this->oit_is_available());
      //
      //_pdata->shader_opaque.init_from_sources(vert, frag_opaque, geom);
      //_pdata->shader_transparent.init_from_sources(vert, frag_transparent, geom);

      // todo
  }

  void VectorView::init_buffers(const std::vector<std::vector<std::tuple<Vec3d/*pos*/, Vec3d/*vec*/, double/*attrib*/>>>& vecs, double temporal_resolution, std::string_view color_attribute_name)
  {
      clear_buffers();
      _pdata->temporal_resolution = temporal_resolution;

      const unsigned int numVecs = vecs.size();
      if (numVecs == 0)
      { return; }

      _pdata->num_times = vecs[0].size();
      _pdata->is_time_dependent = _pdata->num_times > 1;

      const unsigned int floatsPerVertex = 7 * (_pdata->is_time_dependent ? 2 : 1); // posx posy posz vecx vecy vecz attrib

      _pdata->color_attrib_min_manual = _pdata->color_attrib_min;
      _pdata->color_attrib_max_manual = _pdata->color_attrib_max;

      /*
       * vbo
       */
      std::vector<GLfloat> vbodata(floatsPerVertex * numVecs);

      #pragma omp parallel for
      for (unsigned int i = 0; i < numVecs; ++i)
      {
          unsigned int off = floatsPerVertex * i;

          // pos t0 (3)
          for (unsigned int v = 0; v < 3; ++v)
          { vbodata[off++] = std::get<0>(vecs[i][0])[v]; }

          // vec t0 (3)
          for (unsigned int v = 0; v < 3; ++v)
          { vbodata[off++] = std::get<1>(vecs[i][0])[v]; }

          // attrib t0 (1)
          vbodata[off++] = static_cast<GLfloat>(std::get<2>(vecs[i][0]));

          if (_pdata->is_time_dependent)
          {
              // pos t1 (3)
              for (unsigned int v = 0; v < 3; ++v)
              { vbodata[off++] = std::get<0>(vecs[i][1])[v]; }

              // vec t1 (3)
              for (unsigned int v = 0; v < 3; ++v)
              { vbodata[off++] = std::get<1>(vecs[i][1])[v]; }

              // attrib t1 (1)
              vbodata[off++] = static_cast<GLfloat>(std::get<2>(vecs[i][1]));
          }

          for (unsigned int t = 0; t < _pdata->num_times; ++t)
          {
              #pragma omp critical(init_vectorview)
              {
                  _pdata->center += std::get<0>(vecs[i][t]);
                  _pdata->color_attrib_min_manual = std::min(_pdata->color_attrib_min_manual, static_cast<GLfloat>(std::get<2>(vecs[i][t])));
                  _pdata->color_attrib_max_manual = std::max(_pdata->color_attrib_max_manual, static_cast<GLfloat>(std::get<2>(vecs[i][t])));
              }
          }
      } // for i : numVecs

      _pdata->center /= numVecs * _pdata->num_times;
      _pdata->sizeInd = numVecs;

      /*
       * vao
       */
      if (_pdata->is_time_dependent)
      {
          _pdata->vao.add_default_attribute_position_3xfloat(); // pos t0
          _pdata->vao.add_default_attribute_normal_3xfloat(); // vec t0
          _pdata->vao.add_default_attribute_scalar_1xfloat("attrib_t0");
          _pdata->vao.add_default_attribute_position_3xfloat(); // pos t1
          _pdata->vao.add_default_attribute_normal_3xfloat(); // vec t1
          _pdata->vao.add_default_attribute_scalar_1xfloat("attrib_t1");
      }
      else
      {
          _pdata->vao.add_default_attribute_position_3xfloat();
          _pdata->vao.add_default_attribute_normal_3xfloat();
          _pdata->vao.add_default_attribute_scalar_1xfloat("attrib");
      }

      _pdata->vbo.init(vbodata);
      _pdata->vao.init(_pdata->vbo);

      /*
       * colorbar
       */
      if (_pdata->color_by_attribute_enabled)
      { set_colorbar_heat(); }

      _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max, false);
      _pdata->colorbarview.set_clamp_value_range(_pdata->color_attrib_min_manual, _pdata->color_attrib_max_manual);
      _pdata->colorbarview.set_title(___("Vectors: \"@0\"", color_attribute_name));
  }

  void VectorView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_line_width(_pdata->line_width);
      _pdata->ubo.set_col_r(_pdata->color[0]);
      _pdata->ubo.set_col_g(_pdata->color[1]);
      _pdata->ubo.set_col_b(_pdata->color[2]);
      _pdata->ubo.set_isl_enabled(_pdata->isl_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_lightcol_r(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_lightcol_g(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_lightcol_b(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_shininess(_pdata->shininess);
      _pdata->ubo.set_halo_enabled(_pdata->halo_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_halo_width_in_percent(_pdata->halo_width_in_percent);
      _pdata->ubo.set_color_enabled(_pdata->color_by_attribute_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_num_colors(_pdata->colorbar_num_colors);
      _pdata->ubo.set_min_value(_pdata->color_attrib_min);
      _pdata->ubo.set_max_value(_pdata->color_attrib_max);
      _pdata->ubo.set_color_transparency_enabled(_pdata->color_transparency_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_color_alpha_correction(_pdata->color_alpha_correction);
      _pdata->ubo.set_scale_attrib_to_colorbar(_pdata->scale_attrib_to_colorbar ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_num_times(_pdata->num_times);
      _pdata->ubo.set_temporal_resolution(_pdata->temporal_resolution);

      _pdata->ubo.release();
  }

  void VectorView::init(const std::vector<std::vector<std::tuple<Vec3d/*pos*/, Vec3d/*vec*/, double/*attrib*/>>>& vecs, double temporal_resolution, std::string_view color_attribute_name)
  {
      init_buffers(vecs, temporal_resolution, color_attribute_name);
      init_shader();
      init_ubo();

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
} // namespace bk