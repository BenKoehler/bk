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

#include "tools/color/ColorRGBA.h"
#include "gl/buffer/IBO.h"
#include "gl/buffer/UBO.h"
#include "gl/buffer/VBO.h"
#include "gl/vao/VAO.h"
#include "gl/Shader.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotMarker::Impl
  {
      VAO vao;
      VBO vbo;
      UBO ubo;
      Shader shader;
      value_type dataValue;
      value_type xmin;
      value_type xmax;
      value_type ymin;
      value_type ymax;
      color_type color;
      value_type lineWidth;
      details::PlotMarkerOrientation orientation;

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
            orientation(details::PlotMarkerOrientation::Vertical)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = delete;
      Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR 
  //====================================================================================================

  #ifndef BK_LIB_QT_AVAILABLE
  PlotMarker::PlotMarker()
      : base_type(),
      _pdata(std::make_unique<Impl>())
  #else

  PlotMarker::PlotMarker(qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();

      _pdata->ubo.set_usage_STATIC_DRAW();
      _pdata->ubo.register_value_of_size("lineWidth", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorr", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorg", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colorb", sizeof(value_type));
      _pdata->ubo.register_value_of_size("colora", sizeof(value_type));
  }

  PlotMarker::PlotMarker(self_type&&) = default;

  PlotMarker::~PlotMarker()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER 
  //====================================================================================================
  auto PlotMarker::get_color() const -> const color_type&
  { return _pdata->color; }

  auto PlotMarker::get_line_width() const -> value_type
  { return _pdata->lineWidth; }

  auto PlotMarker::get_data_value() const -> value_type
  { return _pdata->dataValue; }

  const details::PlotMarkerOrientation& PlotMarker::get_orientation() const
  { return _pdata->orientation; }

  bool PlotMarker::orientation_is_horizontal() const
  { return _pdata->orientation == details::PlotMarkerOrientation::Horizontal; }

  bool PlotMarker::orientation_is_vertical() const
  { return _pdata->orientation == details::PlotMarkerOrientation::Vertical; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto PlotMarker::operator=(self_type&& other) -> self_type& = default;

  void PlotMarker::set_color(const color_type& col)
  { set_color(col[0], col[1], col[2], col[3]); }

  void PlotMarker::set_color(value_type r, value_type g, value_type b, value_type a)
  { _pdata->color.set(r, g, b, a); }

  void PlotMarker::set_line_width(value_type w)
  {
      _pdata->lineWidth = std::max(w, static_cast<value_type>(0));

      if (this->is_initialized())
      {
          _pdata->ubo.write_registered_value("lineWidth", &_pdata->lineWidth);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_data_value(value_type x)
  {
      _pdata->dataValue = x;

      if (this->is_initialized())
      {
          value_type* buf = _pdata->vbo.map_write_only<value_type>();
          if (buf != nullptr)
          {
              //const unsigned int ids[2] = (orientation_is_horizontal()) ? {1,3} : {0,2};
              const unsigned int ids[2] = {orientation_is_horizontal() ? 1U : 0U, orientation_is_horizontal() ? 0U : 2U};

              for (unsigned int i: ids)
              { buf[i] = _pdata->dataValue; }

              /*if (orientation_is_horizontal())
              {
                  buf[1] = _pdata->dataValue;
                  buf[3] = _pdata->dataValue;
              }
              else // if (orientation_is_vertical())
              {
                  buf[0] = _pdata->dataValue;
                  buf[2] = _pdata->dataValue;
              }*/

              _pdata->vbo.unmap();
          }

          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_x_min(value_type xmin)
  { _pdata->xmin = xmin; }

  void PlotMarker::set_x_max(value_type xmax)
  { _pdata->xmax = xmax; }

  void PlotMarker::set_y_min(value_type ymin)
  { _pdata->ymin = ymin; }

  void PlotMarker::set_y_max(value_type ymax)
  { _pdata->ymax = ymax; }

  void PlotMarker::set_orientation(details::PlotMarkerOrientation orientation)
  {
      _pdata->orientation = orientation;

      if (this->is_initialized())
      {
          init_vbo_vao();
          this->emit_signal_update_required();
      }
  }

  void PlotMarker::set_orientation_horizontal()
  { set_orientation(details::PlotMarkerOrientation::Horizontal); }

  void PlotMarker::set_orientation_vertical()
  { set_orientation(details::PlotMarkerOrientation::Vertical); }

  //====================================================================================================
  //===== GL
  //====================================================================================================
  bool PlotMarker::init_shader()
  { return _pdata->shader.init("shader/plot-new/plotmarker.vert", "shader/plot-new/plotmarker.frag", "shader/plot-new/plotmarker.geom"); }

  bool PlotMarker::init_vbo_vao()
  {
      // axis
      std::vector<value_type> vert;
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

      _pdata->ubo.write_registered_value("lineWidth", &_pdata->lineWidth);
      value_type ftemp = _pdata->color[0];
      _pdata->ubo.write_registered_value("colorr", &ftemp);
      ftemp = _pdata->color[1];
      _pdata->ubo.write_registered_value("colorg", &ftemp);
      ftemp = _pdata->color[2];
      _pdata->ubo.write_registered_value("colorb", &ftemp);
      ftemp = _pdata->color[3];
      _pdata->ubo.write_registered_value("colora", &ftemp);
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
      {
          this->set_initialized(true);
          return true;
      }
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

      this->set_initialized(false);
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

  void PlotMarker::draw_impl()
  {
      _pdata->ubo.bind_to_base(2);

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


