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

#include <bkGL/renderable/ScreenQuad.h>

#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct ScreenQuad::Impl
  {
      VBO vbo;
      VAO vao;
          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          #else
      Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl)
          #endif
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  ScreenQuad::ScreenQuad()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  ScreenQuad::ScreenQuad(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  {
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->vao.add_default_attribute_texture_coordinates_2xfloat();
  }

  ScreenQuad::ScreenQuad(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ScreenQuad::~ScreenQuad() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS INITIALIZED
  bool ScreenQuad::is_initialized() const
  { return _pdata->vbo.is_initialized() && _pdata->vao.is_initialized(); }
  /// @}

  /// @{ -------------------------------------------------- GET BUFFER
  const VBO& ScreenQuad::vbo() const
  { return _pdata->vbo; }

  const VAO& ScreenQuad::vao() const
  { return _pdata->vao; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto ScreenQuad::operator=(self_type&&) -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void ScreenQuad::clear()
  {
      _pdata->vbo.clear();
      _pdata->vao.clear();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void ScreenQuad::init()
  {
      /* vertex ordering:

           2 ------ 3
           |\       |
           |  \     |
           |    \   |
           |      \ |
           0 ------ 1
       */

      constexpr unsigned int N = 16;
      constexpr GLfloat vertices_texCoords_interleaved[N] = {
          // vert0
          -1, -1,
          // texCoord0
          0, 0,
          // vert1
          1, -1,
          // texCoord1
          1, 0,
          // vert2
          -1, 1,
          // texCoord2
          0, 1,
          // vert3
          1, 1,
          // texCoord3
          1, 1};

      _pdata->vbo.init(vertices_texCoords_interleaved, N * sizeof(GLfloat));
      _pdata->vao.init(_pdata->vbo);
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void ScreenQuad::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void ScreenQuad::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void ScreenQuad::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void ScreenQuad::on_modelview_changed(bool)
  { /* do nothing */ }

  void ScreenQuad::on_visible_changed(bool)
  { /* do nothing */ }

  void ScreenQuad::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void ScreenQuad::on_mouse_button_pressed(MouseButton /*btn*/)
  { /* do nothing */ }

  void ScreenQuad::on_mouse_button_released(MouseButton /*btn*/)
  { /* do nothing */ }

  void ScreenQuad::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void ScreenQuad::draw_impl()
  {
      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);

      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      _pdata->vao.bind();
      BK_QT_GL glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();
  }
  /// @}
} // namespace bk


