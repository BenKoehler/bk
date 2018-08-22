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

#include <bkGL/renderable/transfer_function/WindowingTransferFunctionView.h>

#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/vao/VAO.h>
#include <bkTools/color/WindowingTransferFunction.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct WindowingTransferFunctionView::Impl
  {
      WindowingTransferFunction* tf;
      VBO vbo;
      IBO ibo;
      VAO vao;
      Shader shader;
          #ifndef BK_LIB_QT_AVAILABLE

      Impl(WindowingTransferFunction* tf_ = nullptr)
          : tf(tf_)
          #else

      Impl(WindowingTransferFunction* tf_, bk::qt_gl_functions* gl_)
          : tf(tf_),
            vbo(gl_),
            ibo(gl_),
            vao(gl_),
            shader(gl_)
          #endif
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  WindowingTransferFunctionView::WindowingTransferFunctionView(WindowingTransferFunction* tf)
      : base_type(),
        _pdata(tf)
  #else

  WindowingTransferFunctionView::WindowingTransferFunctionView(WindowingTransferFunction* tf, bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(tf, gl)
  #endif
  {
      _pdata->vao.add_default_attribute_position_2xfloat();
      _pdata->vbo.set_usage_STATIC_DRAW();
      _pdata->ibo.set_usage_STATIC_DRAW();
  }

  WindowingTransferFunctionView::WindowingTransferFunctionView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  WindowingTransferFunctionView::~WindowingTransferFunctionView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TRANSFER FUNCTION
  const WindowingTransferFunction* WindowingTransferFunctionView::transfer_function() const
  { return _pdata->tf; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool WindowingTransferFunctionView::is_initialized() const
  { return _pdata->vao.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto WindowingTransferFunctionView::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
  void WindowingTransferFunctionView::set_transfer_function(WindowingTransferFunction* tf)
  { _pdata->tf = tf; }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  // @{ -------------------------------------------------- CLEAR
  void WindowingTransferFunctionView::clear_shader()
  { _pdata->shader.clear(); }

  void WindowingTransferFunctionView::clear_buffers()
  {
      _pdata->vbo.clear();
      _pdata->ibo.clear();
      _pdata->vao.clear();
  }

  void WindowingTransferFunctionView::clear()
  {
      clear_shader();
      clear_buffers();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool WindowingTransferFunctionView::init_shader()
  {
      using SL = details::ShaderLibrary::transfer_function;
      return _pdata->shader.init_from_sources(SL::vert(), SL::frag());
  }

  void WindowingTransferFunctionView::init(WindowingTransferFunction* tf)
  {
      if (tf != nullptr)
      { _pdata->tf = tf; }

      init_shader();

      constexpr const GLfloat pos[16] = {/*tf 0-3*/ -1, -1, 0, -1, 1, 1, 1, 1, /*center rect 0-3*/ 0, 0, 0, 0, 0, 0, 0, 0};
      _pdata->vbo.init(pos, 16 * sizeof(GLfloat));
      update_tf();

      constexpr const GLuint ind[10] = {0, 1, 2, 3, std::numeric_limits<GLuint>::max(), 4, 5, 6, 7, 4};
      _pdata->ibo.init(ind, 10 * sizeof(GLuint));
      _pdata->vao.init(_pdata->vbo, _pdata->ibo);
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE VBO
  void WindowingTransferFunctionView::update_tf()
  {
      if (_pdata->tf == nullptr)
      { return; }

      static constexpr const GLfloat crect_size = static_cast<GLfloat>(0.01 * 2); // 1 percent of width

      GLfloat* pos = _pdata->vbo.map_write_only<GLfloat>();
      if (pos != nullptr)
      {
          const GLfloat irange = _pdata->tf->intensity_range();
          pos[2] = -1 + 2 * (_pdata->tf->center() - _pdata->tf->intensity_min() - _pdata->tf->width()) / irange;
          pos[4] = -1 + 2 * (_pdata->tf->center() - _pdata->tf->intensity_min() + _pdata->tf->width()) / irange;
          const GLfloat temp = -1 + 2 * (_pdata->tf->center() - _pdata->tf->intensity_min()) / irange;
          pos[8] = pos[14] = temp - crect_size;
          pos[9] = pos[11] = 0 - crect_size;
          pos[10] = pos[12] = temp + crect_size;
          pos[13] = pos[15] = 0 + crect_size;
      }

      _pdata->vbo.unmap_and_release();
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void WindowingTransferFunctionView::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_modelview_changed(bool)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_visible_changed(bool)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_mouse_button_pressed(MouseButton /*btn*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_mouse_button_released(MouseButton /*btn*/)
  { /* do nothing */ }

  void WindowingTransferFunctionView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void WindowingTransferFunctionView::draw_impl()
  {
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glPushAttrib(GL_DEPTH_BUFFER_BIT);
      BK_QT_GL glDisable(GL_DEPTH_TEST);
      BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
      BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

      _pdata->vao.bind();
      _pdata->shader.bind();
      BK_QT_GL glDrawElements(GL_LINE_STRIP, 10, GL_UNSIGNED_INT, nullptr);
      _pdata->shader.release();
      _pdata->vao.release();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPopMatrix();

      BK_QT_GL glPopAttrib();
      BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
  }
  /// @}
} // namespace bk


