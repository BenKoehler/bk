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

#include "PlotMarker.h"

#include <algorithm>

#include <bkTools/color/ColorRGBA.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/UBOPlotLine.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotMarker::Impl
  {
      VAO vao;
      VBO vbo;
      details::UBOPlotLine ubo;
      Shader shader;
      GLfloat dataValue;
      GLfloat xmin;
      GLfloat xmax;
      GLfloat ymin;
      GLfloat ymax;
      ColorRGBA color;
      GLfloat lineWidth;
      PlotMarkerOrientation_ orientation;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(qt_gl_functions* gl)
          : vao(gl),
            vbo(gl),
            ubo(gl),
            shader(gl),
          #endif
          dataValue(0),
          xmin(0),
          xmax(0),
          ymin(0),
          ymax(0),
          color(0, 0, 0, 1),
          lineWidth(2),
          orientation(PlotMarkerOrientation_Vertical)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR 
  //====================================================================================================

  #ifndef BK_LIB_QT_AVAILABLE

  PlotMarker::PlotMarker()
      : base_type()
  #else

  PlotMarker::PlotMarker(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();
  }

  PlotMarker::PlotMarker(PlotMarker&&) noexcept = default;

  PlotMarker::~PlotMarker() = default;

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  const ColorRGBA& PlotMarker::color() const
  { return _pdata->color; }

  GLfloat PlotMarker::line_width() const
  { return _pdata->lineWidth; }

  GLfloat PlotMarker::data_value() const
  { return _pdata->dataValue; }

  const PlotMarkerOrientation_& PlotMarker::orientation() const
  { return _pdata->orientation; }

  bool PlotMarker::orientation_is_horizontal() const
  { return _pdata->orientation == PlotMarkerOrientation_Horizontal; }

  bool PlotMarker::orientation_is_vertical() const
  { return _pdata->orientation == PlotMarkerOrientation_Vertical; }

  bool PlotMarker::is_initialized() const
  { return _pdata->vao.is_initialized(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotMarker& PlotMarker::operator=(PlotMarker&&) noexcept = default;

  void PlotMarker::set_color(const ColorRGBA& col)
  { set_color(col[0], col[1], col[2], col[3]); }

  void PlotMarker::set_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  { _pdata->color.set(r, g, b, a); }

  void PlotMarker::set_line_width(GLfloat w)
  {
      _pdata->lineWidth = std::max(w, static_cast<GLfloat>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.set_line_width(_pdata->lineWidth);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_data_value(GLfloat x)
  {
      _pdata->dataValue = x;

      if (this->is_initialized())
      {
          GLfloat* buf = _pdata->vbo.map_write_only<GLfloat>();
          if (buf != nullptr)
          {
              //const unsigned int ids[2] = orientation_is_horizontal() ? {1U,3U} : {0U,2U};

              //const unsigned int ids[2] = {//
              //    orientation_is_horizontal() ? 1U : 0U,//
              //    orientation_is_horizontal() ? 3U : 2U};
              //
              //for (unsigned int i: ids)
              //{ buf[i] = _pdata->dataValue; }

              if (orientation_is_horizontal())
              {
                  buf[1] = _pdata->dataValue;
                  buf[3] = _pdata->dataValue;
              }
              else // if (orientation_is_vertical())
              {
                  buf[0] = _pdata->dataValue;
                  buf[2] = _pdata->dataValue;
              }

              _pdata->vbo.unmap_and_release();
          }

          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_x_min(GLfloat xmin)
  { _pdata->xmin = xmin; }

  void PlotMarker::set_x_max(GLfloat xmax)
  { _pdata->xmax = xmax; }

  void PlotMarker::set_y_min(GLfloat ymin)
  { _pdata->ymin = ymin; }

  void PlotMarker::set_y_max(GLfloat ymax)
  { _pdata->ymax = ymax; }

  void PlotMarker::set_orientation(PlotMarkerOrientation_ orientation)
  {
      _pdata->orientation = orientation;

      if (this->is_initialized())
      {
          init_vbo_vao();
          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_orientation_horizontal()
  { set_orientation(PlotMarkerOrientation_Horizontal); }

  void PlotMarker::set_orientation_vertical()
  { set_orientation(PlotMarkerOrientation_Vertical); }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotMarker::init_shader()
  {
      using SL = details::ShaderLibrary::plot::marker;
      return _pdata->shader.init_from_sources(SL::vert(), SL::frag(), SL::geom());
  }

  bool PlotMarker::init_vbo_vao()
  {
      // axis
      std::vector<GLfloat> vert;
      vert.reserve(4);

      if (orientation_is_horizontal())
      {
          vert.push_back(_pdata->xmin);
          vert.push_back(_pdata->dataValue);

          vert.push_back(_pdata->xmax);
          vert.push_back(_pdata->dataValue);
      }
      else // if (orientation_is_vertical())
      {
          vert.push_back(_pdata->dataValue);
          vert.push_back(_pdata->ymin);

          vert.push_back(_pdata->dataValue);
          vert.push_back(_pdata->ymax);
      }

      _pdata->vbo.init(vert);
      _pdata->vao.init(_pdata->vbo);

      return true;
  }

  bool PlotMarker::init_ubo()
  {
      if (!_pdata->ubo.init_from_registered_values_size())
      { return false; }

      _pdata->ubo.set_line_width(_pdata->lineWidth);
      _pdata->ubo.set_color_r(_pdata->color[0]);
      _pdata->ubo.set_color_g(_pdata->color[1]);
      _pdata->ubo.set_color_b(_pdata->color[2]);
      _pdata->ubo.set_color_a(_pdata->color[3]);
      _pdata->ubo.release();

      return true;
  }

  bool PlotMarker::init()
  {
      if (!(init_shader() && init_ubo() && init_vbo_vao()))
      {
          clear();
          return false;
      }
      else
      { return true; }
  }

  void PlotMarker::clear_shader()
  { _pdata->shader.clear(); }

  void PlotMarker::clear_vbo_vao()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }

  void PlotMarker::clear_ubo()
  { _pdata->ubo.clear(); }

  void PlotMarker::clear()
  {
      clear_shader();
      clear_vbo_vao();
      clear_ubo();
  }

  void PlotMarker::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void PlotMarker::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void PlotMarker::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void PlotMarker::on_modelview_changed(bool /*b*/)
  { /* do nothing */ }

  void PlotMarker::on_visible_changed(bool /*b*/)
  { /* do nothing */ }

  void PlotMarker::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void PlotMarker::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotMarker::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void PlotMarker::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void PlotMarker::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void PlotMarker::on_mouse_wheel_up()
  { /* do nothing */ }

  void PlotMarker::on_mouse_wheel_down()
  { /* do nothing */ }

  void PlotMarker::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  void PlotMarker::draw_impl()
  {
      _pdata->ubo.bind_to_default_base();

      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_BLEND);

      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawArrays(GL_LINES, 0, 2);
      _pdata->shader.release();
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


