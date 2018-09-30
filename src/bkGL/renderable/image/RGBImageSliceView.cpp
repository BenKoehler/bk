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

#include <bkGL/renderable/image/RGBImageSliceView.h>

#include <algorithm>
#include <limits>

#include <bkGL/buffer/SSBO.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  RGBImageSliceView::RGBImageSliceView() : base_type()
  #else

  RGBImageSliceView::RGBImageSliceView(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  {
  }

  RGBImageSliceView::RGBImageSliceView(self_type&&) noexcept = default;

  RGBImageSliceView::~RGBImageSliceView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  auto RGBImageSliceView::image_size(GLuint id) const -> GLuint
  { return id < static_cast<GLuint>(_image.geometry().num_dimensions()) ? _image.geometry().size(id) : 1; }

  auto RGBImageSliceView::image_scale(GLuint id) const -> GLfloat
  { return id < static_cast<GLuint>(_image.geometry().num_dimensions()) ? _image.geometry().size(id) : 1; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto RGBImageSliceView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void RGBImageSliceView::clear_image()
  {
      _image.set_size(1, 1, 1, 1);
      _image.geometry().transformation().set_scale(1, 1, 1, 1);
      _xmax() = 0;
      _ymax() = 0;
      _zmax() = 0;
      _tmax() = 0;
      _xcurrent() = 0;
      _ycurrent() = 0;
      _zcurrent() = 0;
      _tcurrent() = 0;
      _intensitymax() = -std::numeric_limits<GLfloat>::max();
      _intensitymin() = std::numeric_limits<GLfloat>::max();
      rgb_current().set_constant(0);
  }

  void RGBImageSliceView::update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t)
  {
      if (_image.num_values() == 0)
      { return; }
      GLfloat* intensities = _ssbo_intensity().map_write_only<GLfloat>();
      if (intensities != nullptr)
      {
          //_intensitymax = -std::numeric_limits<GLfloat>::max();
          //_intensitymin = std::numeric_limits<GLfloat>::max();

          GLuint cnt = 0;
          for (int y = static_cast<int>(_image.geometry().size(1) - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_image.geometry().size(0)); ++x)
              {
                  const rgb_type val(_image(x, y, z, t));
                  for (unsigned int k = 0; k < 3; ++k)
                  { intensities[cnt++] = val[k]; }

                  //_intensitymin = std::min(_intensitymin, val);
                  //_intensitymax = std::max(_intensitymax, val);
              }
          }
          _ssbo_intensity().unmap_and_release();
      }
  }

  auto RGBImageSliceView::rgb_at_xyzt_current() const -> rgb_type
  { return this->is_initialized() ? _image(xcurrent(), ycurrent(), zcurrent(), tcurrent()) : rgb_type(0, 0, 0); }
} // namespace bk


