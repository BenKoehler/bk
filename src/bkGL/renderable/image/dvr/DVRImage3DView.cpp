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

#include <bkGL/renderable/image/dvr/DVRImage3DView.h>

#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture3D.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct DVRImage3DView::Impl
  {
      Texture3D image_tex;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : image_tex(gl)
          #endif
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  DVRImage3DView::DVRImage3DView()
      : base_type()
  #else

  DVRImage3DView::DVRImage3DView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  { /* do nothing */ }

  DVRImage3DView::DVRImage3DView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DVRImage3DView::~DVRImage3DView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET BUFFERS/TEXTURES
  Texture3D& DVRImage3DView::image_tex()
  { return _pdata->image_tex; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DVRImage3DView::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void DVRImage3DView::clear_shader_impl()
  { /*do nothing*/ }

  void DVRImage3DView::clear_buffers_and_textures_impl()
  { _pdata->image_tex.clear(); }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void DVRImage3DView::init_shader_impl()
  {
      using SL = details::ShaderLibrary::dvr;

      if (this->mode_is_default())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag(false)); }
      else if (this->mode_is_maximum_intensity_projection())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag_mip(false, false)); }
      else if (this->mode_is_minimum_intensity_projection())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag_mip(false, true)); }
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void DVRImage3DView::bind_image_textures()
  { _pdata->image_tex.bind(); /*bound to tex unit 2*/ }

  void DVRImage3DView::release_image_textures()
  { _pdata->image_tex.release(); }

  bool DVRImage3DView::is_up_to_date() const
  { return true; }
  /// @}
} // namespace bk


