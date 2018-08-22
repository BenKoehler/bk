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

#include <bkGL/renderable/line/LineView.h>

#include <algorithm>
#include <cassert>
#include <random>
#include <vector>

#include <bkGL/buffer/FBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/renderable/ColorBarView.h>
#include <bkGL/renderable/EColorScaleType.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture2D.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/UBOLine.h>
#include <bkMath/functions/equals_approx.h>
#include <bkTools/color/ColorRGBA.h>
#include <bkTools/color/ColorBarRGBA.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct LineView::Impl
  {
      VBO vbo;
      IBO ibo;
      VAO vao;
      details::UBOLine ubo;
      Shader shader_opaque;
      Shader shader_transparent;
      ColorBarView colorbarview;
      SSBO ssbo_colorbar;
      // ------- lineao
      FBO fbo_lineao_g;
      FBO fbo_lineao_mipmap0;
      FBO fbo_lineao_mipmap1;
      Texture2D tex_lineao_noise;
      ScreenQuad screenquad;
      Shader shader_lineao_gbuffer;
      Shader shader_lineao_mipmap0;
      Shader shader_lineao_mipmap1;
      Shader shader_lineao;
      GLuint window_width;
      GLuint window_height;
      // ------- lineao end
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
      GLfloat halo_depth_dependent_dmax;
      bk::ColorRGBA color;
      ColorScaleType colorscale_type;
      Vec3<GLfloat> center;
      GLfloat trail_length_in_ms;
      GLfloat trail_opaque_part_in_percent;
      // ------- lineao
      bool lineao_enabled;
      bool lineao_anisotropic;
      // ------- lineao end
      mutable bool lines_have_time_attribute;
      mutable bool lines_have_color_attribute;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            ibo(gl),
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
          window_width(1),
          window_height(1),
          color_by_attribute_enabled(true),
          color_transparency_enabled(false),
          //color_alpha_correction(0.25), // todo options
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
          halo_depth_dependent_dmax(0.01), // todo options
          color(bk::ColorRGBA::Yellow()), // todo options
          colorscale_type(ColorScaleType::Heat),
          center(0, 0, 0),
          trail_length_in_ms(25), // todo options
          trail_opaque_part_in_percent(0.3), // todo options
          lineao_enabled(false),
          lineao_anisotropic(true),
          lines_have_time_attribute(false),
          lines_have_color_attribute(false)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  LineView::LineView()
      : base_type()
  #else

  LineView::LineView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ibo.set_usage_STATIC_DRAW();
      _pdata->ssbo_colorbar.set_usage_STATIC_DRAW();

      // done in init_lines(); depends on color_and time enabled or not
      //_pdata->vao.add_default_attribute_position_3xfloat_plus_time_1xfloat();

      _pdata->colorbarview.set_position_horizontal_right();
      _pdata->colorbarview.set_position_vertical(0);
      _pdata->colorbarview.set_value_precision(1);
  }

  LineView::LineView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  LineView::~LineView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET LINE WIDTH
  GLfloat LineView::line_width() const
  { return _pdata->line_width; }
  /// @}

  /// @{ -------------------------------------------------- GET COLOR
  const bk::ColorRGBA& LineView::color() const
  { return _pdata->color; }
  /// @}

  /// @{ -------------------------------------------------- GET COLOR ATTRIBUTES
  bool LineView::color_by_attribute_is_enabled() const
  { return _pdata->color_by_attribute_enabled; }

  GLfloat LineView::color_attribute_min() const
  { return _pdata->color_attrib_min; }

  GLfloat LineView::color_attribute_max() const
  { return _pdata->color_attrib_max; }
  /// @}

  /// @{ -------------------------------------------------- GET ISL
  GLfloat LineView::shininess() const
  { return _pdata->shininess; }

  bool LineView::isl_is_enabled() const
  { return _pdata->isl_enabled; }
  /// @}

  /// @{ -------------------------------------------------- GET HALO
  GLfloat LineView::halo_width_in_percent() const
  { return _pdata->halo_width_in_percent; }

  GLfloat LineView::halo_depth_dependent_dmax() const
  { return _pdata->halo_depth_dependent_dmax; }

  bool LineView::halo_is_enabled() const
  { return _pdata->halo_enabled; }
  /// @}

  /// @{ -------------------------------------------------- GET LINEAO
  bool LineView::lineAO_is_enabled() const
  { return _pdata->lineao_enabled; }

  bool LineView::lineAO_is_anisotropic() const
  { return _pdata->lineao_anisotropic; }
  /// @}

  /// @{ -------------------------------------------------- GET CENTER
  bk::Vec3<GLfloat> LineView::center() const
  { return _pdata->center; }
  /// @}

  /// @{ -------------------------------------------------- GET PARTICLE TRAILS
  GLfloat LineView::trail_length_in_ms() const
  { return _pdata->trail_length_in_ms; }

  GLfloat LineView::trail_opaque_part_in_percent() const
  { return _pdata->trail_opaque_part_in_percent; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool LineView::is_initialized() const
  { return _pdata->vao.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto LineView::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET LINE WIDTH
  void LineView::set_line_width(GLfloat w)
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
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void LineView::set_color(const bk::ColorRGBA& c)
  { set_color(c.r(), c.g(), c.b()); }

  void LineView::set_color(double r, double g, double b)
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
  /// @}

  /// @{ -------------------------------------------------- SET COLOR BY ATTRIBUTE ENABLED
  void LineView::set_color_by_attribute_enabled(bool b)
  {
      if (_pdata->color_by_attribute_enabled != b)
      {
          _pdata->color_by_attribute_enabled = b;

          if (this->is_initialized())
          {
              init_shader();
              init_lineao_shader();
              this->emit_signal_update_required();
          }
      }
  }

  void LineView::enable_color_by_attribute()
  { set_color_by_attribute_enabled(true); }

  void LineView::disable_color_by_attribute()
  { set_color_by_attribute_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET COLORBAR
  void LineView::_set_colorbar(const bk::ColorBarRGBA& colbar)
  { _set_colorbar_manual_num_colors(colbar, colbar.num_colors()); }

  void LineView::_set_colorbar_manual_num_colors(const bk::ColorBarRGBA& colbar, unsigned int nColors)
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
              case ColorScaleType::Cluster:
              {
                  _pdata->colorbarview.init_cluster();
                  break;
              }
              case ColorScaleType::TrafficLight:
              {
                  _pdata->colorbarview.init_traffic_light();
                  break;
              }
              case ColorScaleType::UniformYellow:
              {
                  _pdata->colorbarview.init_uniform_yellow();
                  break;
              }
              //case ColorScaleType::Gray: [[fallthrough]]
              default:
              { /* do nothing */ }
          }

          this->emit_signal_update_required();
      }
  }

  void LineView::set_colorbar_heat()
  {
      _pdata->colorscale_type = ColorScaleType::Heat;
      _set_colorbar(bk::ColorBarRGBA::Heat());
  }

  void LineView::set_colorbar_rainbow()
  {
      _pdata->colorscale_type = ColorScaleType::Rainbow;
      _set_colorbar(bk::ColorBarRGBA::Rainbow());
  }

  void LineView::set_colorbar_blue_to_red()
  {
      _pdata->colorscale_type = ColorScaleType::BlueToRed;
      _set_colorbar(bk::ColorBarRGBA::Blue_To_Red());
  }

  void LineView::set_colorbar_magenta()
  {
      _pdata->colorscale_type = ColorScaleType::Magenta;
      _set_colorbar(bk::ColorBarRGBA::Magenta());
  }

  void LineView::set_colorbar_uniform_yellow()
  {
      _pdata->colorscale_type = ColorScaleType::UniformYellow;
      _set_colorbar(bk::ColorBarRGBA::Uniform_Yellow());
  }

  void LineView::set_colorbar_traffic_light()
  {
      _pdata->colorscale_type = ColorScaleType::TrafficLight;
      _set_colorbar(bk::ColorBarRGBA::Traffic_Light());
  }

  void LineView::set_colorbar_cluster()
  {
      _pdata->colorscale_type = ColorScaleType::Cluster;
      _set_colorbar_manual_num_colors(bk::ColorBarRGBA::Cluster(), static_cast<int>(_pdata->color_attrib_max) + 1);
  }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: SET COLOR ATTRIBUTE
  void LineView::_init_set_color_attribute(std::string_view color_attribute_name)
  { _pdata->lines_have_color_attribute = !color_attribute_name.empty(); }

  GLfloat* LineView::_map_vbo_read_write()
  { return _pdata->vbo.map_read_write<GLfloat>(); }

  void LineView::_reset_color_attribute_min_max()
  {
      _pdata->color_attrib_min = std::numeric_limits<GLfloat>::max();
      _pdata->color_attrib_max = -_pdata->color_attrib_min;

      _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max);
  }

  void LineView::_finalize_set_color_attribute()
  {
      _pdata->vbo.unmap_and_release();

      _pdata->color_attrib_min_manual = _pdata->color_attrib_min;
      _pdata->color_attrib_max_manual = _pdata->color_attrib_max;

      _pdata->ubo.set_min_value(_pdata->color_attrib_min);
      _pdata->ubo.set_max_value(_pdata->color_attrib_max);
      _pdata->ubo.release();
  }
  /// @}

  /// @{ -------------------------------------------------- SET COLOR ATTRIBUTES
  void LineView::set_color_attribute_min(GLfloat v)
  {
      _pdata->color_attrib_min_manual = v;

      if (this->is_initialized())
      {
          _pdata->ubo.set_min_value(_pdata->color_attrib_min_manual);
          _pdata->ubo.release();

          _pdata->colorbarview.set_value_range(v, _pdata->color_attrib_max);

          this->emit_signal_update_required();
      }
  }

  void LineView::set_color_attribute_max(GLfloat v)
  {
      _pdata->color_attrib_max_manual = v;

      if (this->is_initialized())
      {
          _pdata->ubo.set_max_value(_pdata->color_attrib_max_manual);
          _pdata->ubo.release();

          _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, v);

          this->emit_signal_update_required();
      }
  }

  void LineView::set_attrib_scale_to_colorbar(bool b)
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

  void LineView::set_color_attribute_transparency_enabled(bool b)
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
  /// @}

  /// @{ -------------------------------------------------- SET ISL
  void LineView::set_shininess(GLfloat shininess)
  {
      _pdata->shininess = std::max(static_cast<GLfloat>(0), shininess);
      if (this->is_initialized())
      {
          _pdata->ubo.set_shininess(_pdata->shininess);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void LineView::set_isl_enabled(bool b)
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
  /// @}

  /// @{ -------------------------------------------------- SET HALO
  void LineView::set_halo_width_in_percent(GLfloat p)
  {
      _pdata->halo_width_in_percent = std::clamp(p, static_cast<GLfloat>(0), static_cast<GLfloat>(1));
      if (this->is_initialized())
      {
          _pdata->ubo.set_halo_width_in_percent(_pdata->halo_width_in_percent);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void LineView::set_halo_depth_dependent_dmax(GLfloat d)
  {
      _pdata->halo_depth_dependent_dmax = d;
      if (this->is_initialized())
      {
          _pdata->ubo.set_halo_depth_dependent_dmax(_pdata->halo_depth_dependent_dmax);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void LineView::set_halo_enabled(bool b)
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
  /// @}

  /// @{ -------------------------------------------------- SET LINEAO
  void LineView::set_lineao_enabled(bool b)
  {
      if (b != _pdata->lineao_enabled)
      {
          _pdata->lineao_enabled = b;

          if (this->is_initialized())
          {
              _pdata->ubo.set_lineao_enabled(_pdata->lineao_enabled ? static_cast<GLfloat>(1) : static_cast<GLfloat>(0));
              _pdata->ubo.release();

              set_trail_opaque_part_in_percent(_pdata->lineao_enabled ? 1 : 0.3); // todo options

              init_lineao();

              this->emit_signal_update_required();
          }
      }
  }

  void LineView::set_lineao_anisotropic(bool b)
  {
      if (b != _pdata->lineao_anisotropic)
      {
          _pdata->lineao_anisotropic = b;

          if (this->is_initialized())
          {
              const GLint lineao_anisotropic = _pdata->lineao_anisotropic ? 1 : 0;
              _pdata->ubo.set_lineao_anisotropic(lineao_anisotropic);
              _pdata->ubo.release();

              this->emit_signal_update_required();
          }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET PARTICLE TRAILS
  void LineView::set_trail_length_in_ms(GLfloat ms)
  {
      _pdata->trail_length_in_ms = std::max(static_cast<GLfloat>(0), ms);

      if (this->is_initialized())
      {
          _pdata->ubo.set_trail_length_in_ms(_pdata->trail_length_in_ms);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }

  void LineView::set_trail_opaque_part_in_percent(GLfloat p)
  {
      _pdata->trail_opaque_part_in_percent = std::max(static_cast<GLfloat>(0), std::min(static_cast<GLfloat>(1), p));

      if (this->is_initialized())
      {
          _pdata->ubo.set_trail_opaque_part_in_percent(_pdata->trail_opaque_part_in_percent);
          _pdata->ubo.release();
          this->emit_signal_update_required();
      }
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void LineView::clear_color_attribute()
  {
      //_pdata->color_by_attribute_enabled = false;
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

          this->emit_signal_update_required();
      }
  }

  void LineView::clear_shader()
  {
      _pdata->shader_opaque.clear();
      _pdata->shader_transparent.clear();
  }

  void LineView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->ibo.clear();
      _pdata->vao.clear();
      _pdata->ubo.clear();
      _pdata->sizeInd = 0;
  }

  void LineView::clear()
  {
      clear_shader();
      clear_buffers();
      clear_color_attribute();
      clear_lineao_buffers_textures();
      clear_lineao_shader();

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void LineView::clear_lineao_buffers_textures()
  {
      _pdata->fbo_lineao_g.clear();
      _pdata->fbo_lineao_mipmap0.clear();
      _pdata->fbo_lineao_mipmap1.clear();
      _pdata->tex_lineao_noise.clear();
      _pdata->screenquad.clear();
  }

  void LineView::clear_lineao_shader()
  {
      _pdata->shader_lineao_gbuffer.clear();
      _pdata->shader_lineao.clear();
      _pdata->shader_lineao_mipmap0.clear();
      _pdata->shader_lineao_mipmap1.clear();
  }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: INIT LINES
  void LineView::_init_lines(std::string_view color_attribute_name)
  {
      clear_buffers();
      _pdata->lines_have_time_attribute = true;
      _pdata->lines_have_color_attribute = !color_attribute_name.empty();
      this->_pdata->center.set_zero();
      _pdata->vao.clear_attributes();
  }

  bool LineView::_line_has_time_attribute(const bk::Line<3>& line) const
  {
      const bool hasTime = line.point_attribute_map().has_attribute(bk::attribute_info::time());
      _pdata->lines_have_time_attribute &= hasTime;
      return _pdata->lines_have_time_attribute;
  }

  bool LineView::_line_has_attribute(const bk::Line<3>& line, std::string_view name) const
  {
      const bool hasAttrib = line.point_attribute_map().has_attribute(name);
      _pdata->lines_have_color_attribute &= hasAttrib;
      return _pdata->lines_have_color_attribute;
  }

  unsigned int LineView::_floats_per_vertex() const
  {
      unsigned int floatPerVertex = 3; // x y z

      if (_pdata->lines_have_time_attribute)
      { ++floatPerVertex; }

      if (_pdata->color_by_attribute_enabled || _pdata->lines_have_color_attribute)
      { ++floatPerVertex; }

      return floatPerVertex;
  }

  bool LineView::_lines_have_time_attribute() const
  { return _pdata->lines_have_time_attribute; }

  bool LineView::_lines_have_color_attribute() const
  { return _pdata->lines_have_color_attribute; }

  void LineView::_add_to_center(double x, double y, double z)
  {
      _pdata->center[0] += x;
      _pdata->center[1] += y;
      _pdata->center[2] += z;
  }

  void LineView::_update_attribute_min_max(GLfloat newAttribVal)
  {
      _pdata->color_attrib_min = std::min(_pdata->color_attrib_min, newAttribVal);
      _pdata->color_attrib_max = std::max(_pdata->color_attrib_max, newAttribVal);
  }

  void LineView::_vao_add_attributes()
  {
      if (!_pdata->lines_have_time_attribute)
      { _pdata->vao.add_default_attribute_position_3xfloat(); }
      else
      { _pdata->vao.add_default_attribute_position_3xfloat_plus_time_1xfloat(); }

      if (_pdata->color_by_attribute_enabled || _pdata->lines_have_color_attribute)
      {
          _pdata->color_attrib_min = std::numeric_limits<GLfloat>::max();
          _pdata->color_attrib_max = -_pdata->color_attrib_min;

          _pdata->vao.add_default_attribute_scalar_1xfloat("colorAttribute");
      }
  }

  void LineView::_init_line_buffers(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, unsigned int num_points_total)
  {
      _pdata->color_attrib_min_manual = _pdata->color_attrib_min;
      _pdata->color_attrib_max_manual = _pdata->color_attrib_max;

      _pdata->center /= num_points_total;
      _pdata->sizeInd = indices.size();

      _pdata->vbo.init(vertices);
      _pdata->ibo.init(indices);
      _pdata->vao.init(_pdata->vbo, _pdata->ibo);

      if (_pdata->color_by_attribute_enabled || _pdata->lines_have_color_attribute)
      { set_colorbar_heat(); }
  }

  void LineView::_init_colorbar(std::string_view color_attribute_name)
  {
      _pdata->colorbarview.set_value_range(_pdata->color_attrib_min, _pdata->color_attrib_max, false);
      _pdata->colorbarview.set_clamp_value_range(_pdata->color_attrib_min_manual, _pdata->color_attrib_max_manual);

      std::stringstream s;
      s << "Lines: \"";
      s << color_attribute_name;
      s << "\"";
      _pdata->colorbarview.set_title(s.str());
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void LineView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::lines;

      const bool useColor = _pdata->color_by_attribute_enabled || _pdata->lines_have_color_attribute;

      const std::string vert = SL::vert(_pdata->lines_have_time_attribute, useColor);
      const std::string geom = SL::geom(_pdata->lines_have_time_attribute, this->animation_is_enabled(), useColor);
      const std::string frag_opaque = SL::frag_opaque(_pdata->lines_have_time_attribute, this->animation_is_enabled(), useColor);
      const std::string frag_transparent = SL::frag_transparent(_pdata->lines_have_time_attribute, this->animation_is_enabled(), useColor, this->oit_is_available());

      _pdata->shader_opaque.init_from_sources(vert, frag_opaque, geom);
      _pdata->shader_transparent.init_from_sources(vert, frag_transparent, geom);
  }

  void LineView::init_lineao_shader()
  {
      clear_lineao_shader();

      if (!_pdata->lineao_enabled)
      { return; }

      using SL = details::ShaderLibrary::lines::lineAO;

      const bool useColor = _pdata->color_by_attribute_enabled || _pdata->lines_have_color_attribute;

      const std::string vert_gbuffer = SL::gbuffer::vert(_pdata->lines_have_time_attribute, useColor);
      const std::string geom_gbuffer = SL::gbuffer::geom(_pdata->lines_have_time_attribute, this->animation_is_enabled(), useColor);
      const std::string frag_gbuffer = SL::gbuffer::frag(_pdata->lines_have_time_attribute, this->animation_is_enabled(), useColor);
      _pdata->shader_lineao_gbuffer.init_from_sources(vert_gbuffer, frag_gbuffer, geom_gbuffer);

      _pdata->shader_lineao_mipmap0.init_from_sources(SL::mipmap::vert(), SL::mipmap::frag(true));
      _pdata->shader_lineao_mipmap1.init_from_sources(SL::mipmap::vert(), SL::mipmap::frag(false));

      _pdata->shader_lineao.init_from_sources(SL::vert(), SL::frag());
  }

  void LineView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_line_width(_pdata->line_width);
      _pdata->ubo.set_linecol_r(_pdata->color[0]);
      _pdata->ubo.set_linecol_g(_pdata->color[1]);
      _pdata->ubo.set_linecol_b(_pdata->color[2]);
      _pdata->ubo.set_isl_enabled(_pdata->isl_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_lightcol_r(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_lightcol_g(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_lightcol_b(static_cast<GLfloat>(0.5));
      _pdata->ubo.set_shininess(_pdata->shininess);
      _pdata->ubo.set_halo_enabled(_pdata->halo_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_halo_width_in_percent(_pdata->halo_width_in_percent);
      _pdata->ubo.set_halo_depth_dependent_dmax(_pdata->halo_depth_dependent_dmax);
      _pdata->ubo.set_trail_length_in_ms(_pdata->trail_length_in_ms);
      _pdata->ubo.set_trail_opaque_part_in_percent(_pdata->trail_opaque_part_in_percent);
      _pdata->ubo.set_color_enabled(_pdata->color_by_attribute_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_num_colors(_pdata->colorbar_num_colors);
      _pdata->ubo.set_min_value(_pdata->color_attrib_min);
      _pdata->ubo.set_max_value(_pdata->color_attrib_max);
      _pdata->ubo.set_color_transparency_enabled(_pdata->color_transparency_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_color_alpha_correction(_pdata->color_alpha_correction);
      _pdata->ubo.set_lineao_enabled(_pdata->lineao_enabled ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_lineao_anisotropic(_pdata->lineao_anisotropic ? static_cast<GLint>(1) : static_cast<GLint>(0));
      _pdata->ubo.set_scale_attrib_to_colorbar(_pdata->scale_attrib_to_colorbar ? static_cast<GLint>(1) : static_cast<GLint>(0));

      _pdata->ubo.release();
  }

  void LineView::init_lineao()
  {
      clear_lineao_buffers_textures();
      clear_lineao_shader();

      if (!_pdata->lineao_enabled)
      { return; }

      _pdata->screenquad.init();

      init_lineao_shader();

      //------------------------------------------------------------------------------------------------------
      // init FBO G-Buffer
      //------------------------------------------------------------------------------------------------------
      /*
       * Tangent Depth Map (rgb = tangent, a = depth)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_tangent_depth;
      #else
      Texture2D tex_g_tangent_depth(this->_gl);
      #endif
      tex_g_tangent_depth.set_width(_pdata->window_width);
      tex_g_tangent_depth.set_height(_pdata->window_height);
      tex_g_tangent_depth.set_default_config_rgba_tex();
      tex_g_tangent_depth.set_texture_unit_number(7);
      if (!tex_g_tangent_depth.init())
      { std::cerr << "LineView::init_lineao() - tex_g_tangent_depth.init() FAILED" << std::endl; }

      tex_g_tangent_depth.bind();
      tex_g_tangent_depth.set_interpolation_NEAREST();
      tex_g_tangent_depth.set_texture_coordinates_CLAMP();
      tex_g_tangent_depth.release();

      _pdata->fbo_lineao_g.set_texture_COLOR_ATTACHMENT(std::move(tex_g_tangent_depth), 0);

      /*
       * Pos Angle Map (rgb = pos, a = screen-space angle)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_pos_angle;
      #else
      Texture2D tex_g_pos_angle(this->_gl);
      #endif
      tex_g_pos_angle.set_width(_pdata->window_width);
      tex_g_pos_angle.set_height(_pdata->window_height);
      tex_g_pos_angle.set_default_config_rgba_tex();
      tex_g_pos_angle.set_texture_unit_number(8);
      if (!tex_g_pos_angle.init())
      { std::cerr << "LineView::init_lineao() - tex_g_pos_angle.init() FAILED" << std::endl; }

      tex_g_pos_angle.bind();
      tex_g_pos_angle.set_interpolation_NEAREST();
      tex_g_pos_angle.set_texture_coordinates_CLAMP();
      tex_g_pos_angle.release();

      _pdata->fbo_lineao_g.set_texture_COLOR_ATTACHMENT(std::move(tex_g_pos_angle), 1);

      /*
       * Color Map (rgb = color, a = zoom)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_color_zoom;
      #else
      Texture2D tex_g_color_zoom(this->_gl);
      #endif
      tex_g_color_zoom.set_width(_pdata->window_width);
      tex_g_color_zoom.set_height(_pdata->window_height);
      tex_g_color_zoom.set_default_config_rgba_tex();
      tex_g_color_zoom.set_texture_unit_number(9);
      if (!tex_g_color_zoom.init())
      { std::cerr << "LineView::init_lineao() - tex_g_color_zoom.init() FAILED" << std::endl; }

      tex_g_color_zoom.bind();
      tex_g_color_zoom.set_interpolation_NEAREST();
      tex_g_color_zoom.set_texture_coordinates_CLAMP();
      tex_g_color_zoom.release();

      _pdata->fbo_lineao_g.set_texture_COLOR_ATTACHMENT(std::move(tex_g_color_zoom), 2);

      /*
       * Depth
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_depth;
      #else
      Texture2D tex_g_depth(this->_gl);
      #endif
      tex_g_depth.set_width(_pdata->window_width);
      tex_g_depth.set_height(_pdata->window_height);
      tex_g_depth.set_default_config_depth_tex();
      tex_g_depth.set_texture_unit_number(10);
      if (!tex_g_depth.init())
      { std::cerr << "LineView::init_lineao() - tex_g_depth.init() FAILED" << std::endl; }

      tex_g_depth.bind();
      tex_g_depth.set_interpolation_NEAREST();
      tex_g_depth.set_texture_coordinates_CLAMP_TO_EDGE();
      tex_g_depth.release();

      _pdata->fbo_lineao_g.set_texture_DEPTH_ATTACHMENT(std::move(tex_g_depth));

      _pdata->fbo_lineao_g.init();

      //------------------------------------------------------------------------------------------------------
      // init FBO MipMap 0
      //------------------------------------------------------------------------------------------------------
      /*
       * Tangent Depth MipMap 0 (rgb = tangent, a = depth)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_tangent_depth_mipmap0;
      #else
      Texture2D tex_g_tangent_depth_mipmap0(this->_gl);
      #endif
      tex_g_tangent_depth_mipmap0.set_width(_pdata->window_width / 2);
      tex_g_tangent_depth_mipmap0.set_height(_pdata->window_height / 2);
      tex_g_tangent_depth_mipmap0.set_default_config_rgba_tex();
      tex_g_tangent_depth_mipmap0.set_texture_unit_number(11);
      if (!tex_g_tangent_depth_mipmap0.init())
      { std::cerr << "LineView::init_lineao() - tex_g_tangent_depth_mipmap0.init() FAILED" << std::endl; }

      tex_g_tangent_depth_mipmap0.bind();
      tex_g_tangent_depth_mipmap0.set_interpolation_NEAREST();
      tex_g_tangent_depth_mipmap0.set_texture_coordinates_CLAMP();
      tex_g_tangent_depth_mipmap0.release();

      _pdata->fbo_lineao_mipmap0.set_texture_COLOR_ATTACHMENT(std::move(tex_g_tangent_depth_mipmap0), 0);

      /*
       * Pos Angle MipMap 0 (rgb = pos, a = screen-space angle)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_pos_angle_mipmap0;
      #else
      Texture2D tex_g_pos_angle_mipmap0(this->_gl);
      #endif
      tex_g_pos_angle_mipmap0.set_width(_pdata->window_width / 2);
      tex_g_pos_angle_mipmap0.set_height(_pdata->window_height / 2);
      tex_g_pos_angle_mipmap0.set_default_config_rgba_tex();
      tex_g_pos_angle_mipmap0.set_texture_unit_number(12);
      if (!tex_g_pos_angle_mipmap0.init())
      { std::cerr << "LineView::init_lineao() - tex_g_pos_angle_mipmap0.init() FAILED" << std::endl; }

      tex_g_pos_angle_mipmap0.bind();
      tex_g_pos_angle_mipmap0.set_interpolation_NEAREST();
      tex_g_pos_angle_mipmap0.set_texture_coordinates_CLAMP();
      tex_g_pos_angle_mipmap0.release();

      _pdata->fbo_lineao_mipmap0.set_texture_COLOR_ATTACHMENT(std::move(tex_g_pos_angle_mipmap0), 1);

      _pdata->fbo_lineao_mipmap0.init();

      //------------------------------------------------------------------------------------------------------
      // init FBO MipMap 1
      //------------------------------------------------------------------------------------------------------
      /*
       * Tangent Depth MipMap 1 (rgb = tangent, a = depth)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_tangent_depth_mipmap1;
      #else
      Texture2D tex_g_tangent_depth_mipmap1(this->_gl);
      #endif
      tex_g_tangent_depth_mipmap1.set_width(_pdata->window_width / 4);
      tex_g_tangent_depth_mipmap1.set_height(_pdata->window_height / 4);
      tex_g_tangent_depth_mipmap1.set_default_config_rgba_tex();
      tex_g_tangent_depth_mipmap1.set_texture_unit_number(13);
      if (!tex_g_tangent_depth_mipmap1.init())
      { std::cerr << "LineView::init_lineao() - tex_g_tangent_depth_mipmap1.init() FAILED" << std::endl; }

      tex_g_tangent_depth_mipmap1.bind();
      tex_g_tangent_depth_mipmap1.set_interpolation_NEAREST();
      tex_g_tangent_depth_mipmap1.set_texture_coordinates_CLAMP();
      tex_g_tangent_depth_mipmap1.release();

      _pdata->fbo_lineao_mipmap1.set_texture_COLOR_ATTACHMENT(std::move(tex_g_tangent_depth_mipmap1), 0);

      /*
       * Pos Angle MipMap 1 (rgb = pos, a = screen-space angle)
       */
      #ifndef BK_LIB_QT_AVAILABLE
      Texture2D tex_g_pos_angle_mipmap1;
      #else
      Texture2D tex_g_pos_angle_mipmap1(this->_gl);
      #endif
      tex_g_pos_angle_mipmap1.set_width(_pdata->window_width / 4);
      tex_g_pos_angle_mipmap1.set_height(_pdata->window_height / 4);
      tex_g_pos_angle_mipmap1.set_default_config_rgba_tex();
      tex_g_pos_angle_mipmap1.set_texture_unit_number(14);
      if (!tex_g_pos_angle_mipmap1.init())
      { std::cerr << "LineView::init_lineao() - tex_g_pos_angle_mipmap1.init() FAILED" << std::endl; }

      tex_g_pos_angle_mipmap1.bind();
      tex_g_pos_angle_mipmap1.set_interpolation_NEAREST();
      tex_g_pos_angle_mipmap1.set_texture_coordinates_CLAMP();
      tex_g_pos_angle_mipmap1.release();

      _pdata->fbo_lineao_mipmap1.set_texture_COLOR_ATTACHMENT(std::move(tex_g_pos_angle_mipmap1), 1);

      _pdata->fbo_lineao_mipmap1.init();

      //------------------------------------------------------------------------------------------------------
      // init Noise Texture
      // - 3D values € [-1,+1]
      //------------------------------------------------------------------------------------------------------
      constexpr const GLuint noise_texture_size = 1024;
      constexpr const GLuint vectorDim = 3;
      std::vector<GLfloat> ssaoNoise(noise_texture_size * noise_texture_size * vectorDim, 0);

      std::random_device r;
      std::default_random_engine reng(r());
      std::uniform_real_distribution<GLfloat> dist(-1, +1);

      #pragma omp parallel for
      for (GLuint i = 0; i < noise_texture_size * noise_texture_size; ++i)
      {
          bk::Vec3<GLfloat> r;
          GLfloat len = 0;

          do
          {
              r.set_random_float(-1, +1);
              len = r.norm();
          } while (equals_approx(len, 0));

          r /= len;

          for (GLuint k = 0; k < vectorDim; ++k)
          { ssaoNoise[vectorDim * i + k] = r[k]; }
      }

      _pdata->tex_lineao_noise.set_default_config_rgb_tex();
      _pdata->tex_lineao_noise.set_width(noise_texture_size);
      _pdata->tex_lineao_noise.set_height(noise_texture_size);
      _pdata->tex_lineao_noise.set_texture_unit_number(15);
      _pdata->tex_lineao_noise.init(static_cast<GLvoid*>(ssaoNoise.data()));

      _pdata->tex_lineao_noise.bind();
      _pdata->tex_lineao_noise.set_interpolation_NEAREST();
      _pdata->tex_lineao_noise.set_texture_coordinates_REPEAT();
      _pdata->tex_lineao_noise.release();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void LineView::on_resize(GLint w, GLint h)
  {
      if (static_cast<GLuint>(w) != _pdata->window_width || static_cast<GLuint>(h) != _pdata->window_height)
      {
          _pdata->window_width = static_cast<GLuint>(w);
          _pdata->window_height = static_cast<GLuint>(h);

          init_lineao();
      }

      _pdata->colorbarview.on_resize(w, h);
  }

  void LineView::on_oit_enabled(bool b)
  {
      init_shader();
      _pdata->colorbarview.on_oit_enabled(b);
  }

  void LineView::on_animation_enabled(bool /*b*/)
  {
      init_shader();
      init_lineao_shader();
  }

  void LineView::on_modelview_changed(bool b)
  { _pdata->colorbarview.on_modelview_changed(b); }

  void LineView::on_visible_changed(bool b)
  { _pdata->colorbarview.on_visible_changed(b); }

  void LineView::on_mouse_pos_changed(GLint x, GLint y)
  { _pdata->colorbarview.on_mouse_pos_changed(x, y); }

  void LineView::on_mouse_button_pressed(MouseButton btn)
  { _pdata->colorbarview.on_mouse_button_pressed(btn); }

  void LineView::on_mouse_button_released(MouseButton btn)
  { _pdata->colorbarview.on_mouse_button_released(btn); }

  void LineView::on_ssaa_factor_changed(GLint ssaa_factor)
  { _pdata->colorbarview.on_ssaa_factor_changed(ssaa_factor); }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void LineView::draw_opaque_impl()
  {
      // ubo 0 must be global ubo with modelview/projection matrices
      _pdata->ubo.bind_to_default_base();

      if (_pdata->color_by_attribute_enabled)
      { _pdata->ssbo_colorbar.bind_to_base(7); }

      if (!_pdata->lineao_enabled)
      {
          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

          _pdata->vao.bind();

          // either:
          // - default animation
          // - stopped oit animation
          // - OIT first pass: opaque middle parts of temporally visible lines
          _pdata->shader_opaque.bind();
          BK_QT_GL glDrawElements(GL_LINE_STRIP_ADJACENCY, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_opaque.release();

          _pdata->vao.release();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
      }
      else
      {
          //------------------------------------------------------------------------------------------------------
          // Line AO
          //------------------------------------------------------------------------------------------------------
          /*
           * copy default depth buffer to fbo depth texture
           */
          this->emit_signal_bind_default_fbo_as_read();
          BK_QT_GL glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _pdata->fbo_lineao_g.id());
          BK_QT_GL glBlitFramebuffer(0, 0, _pdata->window_width, _pdata->window_height, 0, 0, _pdata->window_width, _pdata->window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
          BK_QT_GL glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
          BK_QT_GL glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

          /*
           * PASS 1: render gbuffer
           */
          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

          BK_QT_GL glPushAttrib(GL_COLOR_BUFFER_BIT); // causes API|ERROR - GL_INVALID_OPERATION error generated. Invalid draw bindings.
          BK_QT_GL glDisable(GL_BLEND);
          BK_QT_GL glDisable(GL_ALPHA_TEST);

          _pdata->fbo_lineao_g.bind();
          BK_QT_GL glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
          BK_QT_GL glClear(GL_COLOR_BUFFER_BIT);

          _pdata->vao.bind();
          _pdata->shader_lineao_gbuffer.bind();
          BK_QT_GL glDrawElements(GL_LINE_STRIP_ADJACENCY, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_lineao_gbuffer.release();
          _pdata->vao.release();

          //BK_QT_GL glPopAttrib();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);

          /*
           * generate MipMaps
           */
          _pdata->fbo_lineao_g.color_attachment(0)->bind(); // Tangent Depth Map (3D tangent, 1D linearized depth)
          _pdata->fbo_lineao_g.color_attachment(1)->bind(); // 3D Pos, 1D screen-space angle

          // level 1 (size/2)
          _pdata->fbo_lineao_mipmap0.bind();
          //BK_QT_GL glClear(GL_COLOR_BUFFER_BIT); // not necessary since every pixel is written to

          _pdata->shader_lineao_mipmap0.bind();
          _pdata->screenquad.draw();
          _pdata->shader_lineao_mipmap0.release();

          // level 2 (size/4)
          _pdata->fbo_lineao_mipmap1.bind();
          //BK_QT_GL glClear(GL_COLOR_BUFFER_BIT); // not necessary since every pixel is written to

          _pdata->shader_lineao_mipmap1.bind();
          _pdata->screenquad.draw();
          _pdata->shader_lineao_mipmap1.release();

          BK_QT_GL glPopAttrib(); // causes API|ERROR - GL_INVALID_OPERATION error generated. Invalid draw bindings.

          this->emit_signal_bind_default_fbo();

          /*
           * PASS 2: calc SSAO; multiply with color
           */
          _pdata->fbo_lineao_g.color_attachment(2)->bind(); // color map (3D color + 1D zoom)
          _pdata->fbo_lineao_mipmap0.color_attachment(0)->bind(); // Tangent Depth Mipmap 0
          _pdata->fbo_lineao_mipmap0.color_attachment(1)->bind(); // Pos Angle Mipmap 0
          _pdata->fbo_lineao_mipmap1.color_attachment(0)->bind(); // Tangent Depth Mipmap 1
          _pdata->fbo_lineao_mipmap1.color_attachment(1)->bind(); // Pos Angle Mipmap 1
          _pdata->tex_lineao_noise.bind(); // noise texture

          _pdata->shader_lineao.bind();
          _pdata->screenquad.draw();
          _pdata->shader_lineao.release();

          _pdata->fbo_lineao_g.color_attachment(0)->release(); // Tangent Depth Map (3D tangent, 1D linearized depth)
          _pdata->fbo_lineao_g.color_attachment(1)->release(); // 1D screen-space angle
          _pdata->fbo_lineao_g.color_attachment(2)->release(); // color map (3D color + 1D zoom)
          _pdata->fbo_lineao_mipmap0.color_attachment(0)->release(); // Tangent Depth Mipmap 0
          _pdata->fbo_lineao_mipmap0.color_attachment(1)->release(); // Pos Angle Mipmap 0
          _pdata->fbo_lineao_mipmap1.color_attachment(0)->release(); // Tangent Depth Mipmap 1
          _pdata->fbo_lineao_mipmap1.color_attachment(1)->release(); // Pos Angle Mipmap 1
          _pdata->tex_lineao_noise.release(); // noise texture

          /*
           * copy fbo depth texture to default depth buffer
           */
          this->emit_signal_bind_default_fbo_as_draw();
          BK_QT_GL glBindFramebuffer(GL_READ_FRAMEBUFFER, _pdata->fbo_lineao_g.id());
          BK_QT_GL glBlitFramebuffer(0, 0, _pdata->window_width, _pdata->window_height, 0, 0, _pdata->window_width, _pdata->window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
          BK_QT_GL glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
          BK_QT_GL glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

          this->emit_signal_bind_default_fbo();
      }

      if (_pdata->color_by_attribute_enabled)
      { _pdata->ssbo_colorbar.release_from_base(); }

      _pdata->ubo.release_from_base();

      //------------------------------------------------------------------------------------------------------
      // colorbar view
      //------------------------------------------------------------------------------------------------------
      if (_pdata->color_by_attribute_enabled && _pdata->lines_have_color_attribute)
      { _pdata->colorbarview.draw(); }
  }

  void LineView::draw_transparent_impl()
  {
      //if (this->is_initialized() && this->is_visible() && this->oit_is_available() && this->animation_is_enabled() && !_pdata->lineao_enabled)
      if (!this->animation_is_enabled() || _pdata->lineao_enabled)
      { return; }

      // ubo 0 must be global ubo with modelview/projection matrices
      _pdata->ubo.bind_to_default_base();

      if (_pdata->color_by_attribute_enabled)
      { _pdata->ssbo_colorbar.bind_to_base(7); }

      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->vao.bind();

      BK_QT_GL glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      BK_QT_GL glDepthMask(GL_FALSE);

      // OIT second pass: transparent outer parts of temporally visible lines
      _pdata->shader_transparent.bind();
      BK_QT_GL glDrawElements(GL_LINE_STRIP_ADJACENCY, _pdata->sizeInd, GL_UNSIGNED_INT, nullptr);
      _pdata->shader_transparent.release();

      BK_QT_GL glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      BK_QT_GL glDepthMask(GL_TRUE);

      _pdata->vao.release();

      if (_pdata->color_by_attribute_enabled)
      { _pdata->ssbo_colorbar.release_from_base(); }

      _pdata->ubo.release_from_base();

      BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
  }
  /// @}
} // namespace bk


