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

#include <bkGL/renderable/image/texture/Texture2DView.h>

#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct Texture2DView::Impl
  {
      ScreenQuad screenquad;
      UBO ubo;
      Shader shader;
      GLuint tex_unit;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else
      explicit Impl(bk::qt_gl_functions* gl)
          : screenquad(gl),
            ubo(gl),
            shader(gl),
          #endif
          tex_unit(0)
      {}
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  Texture2DView::Texture2DView()
      : base_type()
  #else

  Texture2DView::Texture2DView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->ubo.set_usage_STATIC_DRAW();
      _pdata->ubo.register_value("tex_unit", _pdata->tex_unit);
  }

  Texture2DView::Texture2DView(self_type&&) noexcept = default;

  Texture2DView::~Texture2DView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  GLuint Texture2DView::texture_unit() const
  { return _pdata->tex_unit; }

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool Texture2DView::is_initialized() const
  { return _pdata->screenquad.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void Texture2DView::set_texture_unit(GLuint i)
  {
      if (_pdata->tex_unit != i)
      {
          _pdata->tex_unit = i;

          if (this->is_initialized())
          {
              _pdata->ubo.write_registered_value("tex_unit", &_pdata->tex_unit);
              _pdata->ubo.release();

              this->emit_signal_update_required();
          }
      }
  }

  auto Texture2DView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void Texture2DView::clear_shader()
  { _pdata->shader.clear(); }

  void Texture2DView::clear_buffers()
  {
      _pdata->screenquad.clear();
      _pdata->ubo.clear();
  }

  void Texture2DView::clear()
  {
      clear_shader();
      clear_buffers();
  }

  void Texture2DView::init_shader()
  {
      clear_shader();

      using SL = details::ShaderLibrary::render_texture;
      _pdata->shader.init_from_sources(SL::vert(), SL::frag(_pdata->tex_unit, false));
  }

  void Texture2DView::init_ubo()
  {
      _pdata->ubo.clear();
      _pdata->ubo.init_from_registered_values_size();
      _pdata->ubo.write_registered_value("tex_unit", &_pdata->tex_unit);
      _pdata->ubo.release();
  }

  void Texture2DView::init()
  {
      _pdata->screenquad.init();
      init_shader();
      init_ubo();
      this->emit_signal_update_required();
  }

  void Texture2DView::draw_impl()
  {
      _pdata->ubo.bind_to_base(1);

      _pdata->shader.bind();
      _pdata->screenquad.draw();
      _pdata->shader.release();

      _pdata->ubo.release_from_base();
  }
} // namespace bk


