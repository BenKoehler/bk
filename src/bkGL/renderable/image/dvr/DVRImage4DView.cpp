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

#include "../../../../../include/bkGL/renderable/image/dvr/DVRImage4DView.h"
#include "../../../../../include/bkGL/shader/Shader.h"
#include "../../../../../include/bkGL/shader/ShaderLibrary.h"
#include "../../../../../include/bkGL/texture/Texture3D.h"
#include "../../../../../include/bkGL/UBODVR.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct DVRImage4DView::Impl
  {
      RegularImage<double, 4> img;
      Texture3D image_tex[2];
      bool time_changed;
      GLuint oldt0;
      GLuint oldt1;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : image_tex{Texture3D(gl), Texture3D(gl)},
            time_changed(true),
            oldt0(0),
            oldt1(0)
          #endif
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  DVRImage4DView::DVRImage4DView()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else

  DVRImage4DView::DVRImage4DView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  { /* do nothing */ }

  DVRImage4DView::DVRImage4DView(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DVRImage4DView::~DVRImage4DView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET BUFFERS/TEXTURES
  Texture3D& DVRImage4DView::image_tex(unsigned int i)
  { return _pdata->image_tex[i]; }

  RegularImage<double, 4>& DVRImage4DView::img()
  { return _pdata->img; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DVRImage4DView::operator=(self_type&&) -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void DVRImage4DView::clear_shader_impl()
  { /*do nothing*/ }

  void DVRImage4DView::clear_buffers_and_textures_impl()
  {
      _pdata->image_tex[0].clear();
      _pdata->image_tex[1].clear();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  void DVRImage4DView::init_shader_impl()
  {
      using SL = details::ShaderLibrary::dvr;

      if (this->mode_is_default())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag(true)); }
      else if (this->mode_is_maximum_intensity_projection())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag_mip(true, false)); }
      else if (this->mode_is_minimum_intensity_projection())
      { shader_dvr().init_from_sources(SL::vert(), SL::frag_mip(true, true)); }
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void DVRImage4DView::on_animation_time_changed(double d)
  {
      const auto& image_size = _pdata->img.geometry().size();
      const auto& image_scale = _pdata->img.geometry().transformation().scale();
      const GLuint t0 = std::min(static_cast<GLuint>(std::floor(d / image_scale[3])), image_size[3] - 1);
      const GLuint t1 = (t0 + 1) % image_size[3];

      _pdata->time_changed = true;

      if (t0 == _pdata->oldt0 && t1 == _pdata->oldt1) // up to date
      { return; }

      const unsigned int N = image_size[0] * image_size[1] * image_size[2];
      std::vector<GLfloat> texVals0(N, 0);
      std::vector<GLfloat> texVals1(N, 0);

      #pragma omp parallel for
      for (unsigned int z = 0; z < image_size[2]; ++z)
      {
          unsigned int cnt0 = z * image_size[1] * image_size[0];
          unsigned int cnt1 = cnt0;

          for (unsigned int y = 0; y < image_size[1]; ++y)
          {
              for (unsigned int x = 0; x < image_size[0]; ++x)
              {
                  texVals0[cnt0++] = static_cast<GLfloat>(_pdata->img(x, y, z, t0));
                  texVals1[cnt1++] = static_cast<GLfloat>(_pdata->img(x, y, z, t1));
              }
          }
      }

      image_tex(0).init(static_cast<GLvoid*>(texVals0.data()));
      image_tex(1).init(static_cast<GLvoid*>(texVals1.data()));

      this->ubo().set_current_t0(t0);
      this->ubo().release();

      _pdata->oldt0 = t0;
      _pdata->oldt1 = t1;
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void DVRImage4DView::bind_image_textures()
  {
      _pdata->image_tex[0].bind();
      _pdata->image_tex[1].bind();
  }

  void DVRImage4DView::release_image_textures()
  {
      _pdata->image_tex[0].release();
      _pdata->image_tex[1].release();
  }

  bool DVRImage4DView::is_up_to_date() const
  {
      const bool b = _pdata->time_changed;
      _pdata->time_changed = false;
      return !b;
  }
  /// @}
} // namespace bk


