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

#include <bkGL/renderable/background/AbstractBackground.h>

#include <iostream>
#include <sstream>
#include <string>

#include <bkGL/buffer/VBO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/vao/VAO.h>

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AbstractBackground::Impl
  {
      VBO vbo;
      VAO vao;
      Shader shader;

          #ifdef BK_LIB_QT_AVAILABLE

      Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl),
            shader(gl)
      { /* do nothing */ }

          #endif
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  AbstractBackground::AbstractBackground()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  AbstractBackground::AbstractBackground(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();

      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->vao.add_default_attribute_colorRGBA_4xfloat();
  }

  AbstractBackground::AbstractBackground(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractBackground::~AbstractBackground() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET VBO
  VBO& AbstractBackground::vbo()
  { return _pdata->vbo; }

  const VBO& AbstractBackground::vbo() const
  { return _pdata->vbo; }
  /// @}

  /// @{ -------------------------------------------------- GET VAO
  VAO& AbstractBackground::vao()
  { return _pdata->vao; }

  const VAO& AbstractBackground::vao() const
  { return _pdata->vao; }
  /// @}

  /// @{ -------------------------------------------------- GET SHADER
  Shader& AbstractBackground::shader()
  { return _pdata->shader; }

  const Shader& AbstractBackground::shader() const
  { return _pdata->shader; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool AbstractBackground::is_initialized() const
  { return _pdata->vbo.is_initialized() && _pdata->vao.is_initialized() && _pdata->shader.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractBackground::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void AbstractBackground::clear_shader()
  { _pdata->shader.clear(); }

  void AbstractBackground::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }

  void AbstractBackground::clear()
  {
      clear_shader();
      clear_buffers();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool AbstractBackground::init_shader()
  { return _pdata->shader.init_from_sources(ShaderLibrary::background::vert(), ShaderLibrary::background::frag()); }

  void AbstractBackground::init()
  {
      init_shader();
      init_vbo_vao();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void AbstractBackground::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void AbstractBackground::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void AbstractBackground::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void AbstractBackground::on_modelview_changed(bool)
  { /* do nothing */ }

  void AbstractBackground::on_visible_changed(bool)
  { /* do nothing */ }

  void AbstractBackground::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void AbstractBackground::on_mouse_button_pressed(MouseButton /*btn*/)
  { /* do nothing */ }

  void AbstractBackground::on_mouse_button_released(MouseButton /*btn*/)
  { /* do nothing */ }

  void AbstractBackground::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void AbstractBackground::draw_impl()
  {
      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);

      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glEnable(GL_BLEND);

      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      _pdata->shader.release();
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();
  }
  /// @}

} // namespace bk::details


