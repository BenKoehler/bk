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

#ifdef BK_LIB_GDCM_AVAILABLE

    #include <bkGL/renderable/image/RGBDICOMFlowImageSliceView.h>

    #include <algorithm>
    #include <cmath>

    #include <bk/Dicom>
    #include <bk/Image>
    #include <bk/Matrix>
    #include <bkGL/buffer/SSBO.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct RGBDICOMFlowImageSliceView::Impl
  {
      image_type image;
      DicomDirImporter* dcm_importer;
      unsigned int  dcm_image_r_id;
      unsigned int  dcm_image_g_id;
      unsigned int  dcm_image_b_id;
      bool          flip_image_r;
      bool          flip_image_g;
      bool          flip_image_b;
      bool          abs_image_r;
      bool          abs_image_g;
      bool          abs_image_b;
      bool          col_image_r;
      bool          col_image_g;
      bool          col_image_b;
      GLfloat       lipv_r;
      GLfloat       lipv_g;
      GLfloat       lipv_b;
      Vec4 <GLuint> size;

      Impl()
          : dcm_importer(nullptr),
            dcm_image_r_id(0),
            dcm_image_g_id(0),
            dcm_image_b_id(0),
            flip_image_r(false),
            flip_image_g(false),
            flip_image_b(false),
            abs_image_r(true),
            abs_image_g(true),
            abs_image_b(true),
            col_image_r(true),
            col_image_g(true),
            col_image_b(true),
            lipv_r(0),
            lipv_g(0),
            lipv_b(0),
            size(0, 0, 0, 0)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  RGBDICOMFlowImageSliceView::RGBDICOMFlowImageSliceView()
      : base_type()
  #else

      RGBDICOMFlowImageSliceView::RGBDICOMFlowImageSliceView(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  { /* do nothing */ }

  RGBDICOMFlowImageSliceView::RGBDICOMFlowImageSliceView(self_type&&) noexcept = default;

  RGBDICOMFlowImageSliceView::~RGBDICOMFlowImageSliceView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  void RGBDICOMFlowImageSliceView::set_dicom_dir_importer(DicomDirImporter* d)
  { _pdata->dcm_importer = d; }

  const DicomDirImporter* RGBDICOMFlowImageSliceView::dicom_dir_importer() const
  { return _pdata->dcm_importer; }

  auto RGBDICOMFlowImageSliceView::image_size(GLuint id) const -> GLuint
  { return id < 4 ? _pdata->size[id] : 1; }

  auto RGBDICOMFlowImageSliceView::image_scale(GLuint id) const -> GLfloat
  { return id < static_cast<GLuint>(_pdata->image.geometry().num_dimensions()) ? _pdata->image.geometry().transformation().scale(id) : 1; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  bool RGBDICOMFlowImageSliceView::set_images(unsigned int dcm_image_r_id, unsigned int dcm_image_g_id, unsigned int dcm_image_b_id)
  {
      if (_pdata->dcm_importer == nullptr)
      { return false; }
      const unsigned int nImages = _pdata->dcm_importer->num_images();
      if (dcm_image_r_id >= nImages || dcm_image_g_id >= nImages || dcm_image_b_id >= nImages)
      { return false; }
      _pdata->dcm_image_r_id = dcm_image_r_id;
      _pdata->dcm_image_g_id = dcm_image_g_id;
      _pdata->dcm_image_b_id = dcm_image_b_id;
      const DicomImageInfos& info_r = _pdata->dcm_importer->image_infos(_pdata->dcm_image_r_id);
      const DicomImageInfos& info_g = _pdata->dcm_importer->image_infos(_pdata->dcm_image_g_id);
      const DicomImageInfos& info_b = _pdata->dcm_importer->image_infos(_pdata->dcm_image_b_id);
      if (info_r.Columns != info_g.Columns || info_r.Columns != info_b.Columns || info_g.Columns != info_b.Columns || info_r.Rows != info_g.Rows || info_r.Rows != info_b.Rows || info_g.Rows != info_b.Rows)
      { return false; }
      _pdata->lipv_r = info_r.LargestImagePixelValue;
      _pdata->lipv_g = info_g.LargestImagePixelValue;
      _pdata->lipv_b = info_b.LargestImagePixelValue;
      _pdata->image.set_size(info_r.Columns, info_r.Rows);
      _pdata->image.geometry().transformation().set_scale(info_r.ColSpacing, info_r.RowSpacing);
      _pdata->size.set(info_r.Columns, info_r.Rows, info_r.Slices > 1 ? info_r.Slices : 1, info_r.TemporalPositions > 1 ? info_r.TemporalPositions : 1);
      return _pdata->image.num_values() > 1;
  }

  void RGBDICOMFlowImageSliceView::set_flip_images(bool flip_image_r, bool flip_image_g, bool flip_image_b)
  {
      _pdata->flip_image_r = flip_image_r;
      _pdata->flip_image_g = flip_image_g;
      _pdata->flip_image_b = flip_image_b;
  }

  void RGBDICOMFlowImageSliceView::set_use_abs(bool abs_image_r, bool abs_image_g, bool abs_image_b)
  {
      _pdata->abs_image_r = abs_image_r;
      _pdata->abs_image_g = abs_image_g;
      _pdata->abs_image_b = abs_image_b;
  }

  void RGBDICOMFlowImageSliceView::set_use_for_coloring(bool col_image_r, bool col_image_g, bool col_image_b)
  {
      _pdata->col_image_r = col_image_r;
      _pdata->col_image_g = col_image_g;
      _pdata->col_image_b = col_image_b;
  }

  auto RGBDICOMFlowImageSliceView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void RGBDICOMFlowImageSliceView::clear_image()
  {
      _pdata->image.set_size(0, 0);
      _pdata->image.geometry().transformation().set_scale(1, 1);
      _xmax()         = 0;
      _ymax()         = 0;
      _zmax()         = 0;
      _tmax()         = 0;
      _xcurrent()     = 0;
      _ycurrent()     = 0;
      _zcurrent()     = 0;
      _tcurrent()     = 0;
      _intensitymax() = -std::numeric_limits<GLfloat>::max();
      _intensitymin() = std::numeric_limits<GLfloat>::max();
      rgb_current().set_constant(0);
  }

  void RGBDICOMFlowImageSliceView::update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t)
  {
      if (_pdata->dcm_importer == nullptr)
      { return; }
      GLfloat* intensities = _ssbo_intensity().map_write_only<GLfloat>();
      if (intensities != nullptr)
      {
          //const auto& size = _image.size();
          const auto img_r = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_r_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          const auto img_g = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_g_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          const auto img_b = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_b_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          _intensitymax() = -std::numeric_limits<GLfloat>::max();
          _intensitymin() = std::numeric_limits<GLfloat>::max();
          GLuint   cnt = 0;
          //for (GLuint y = 0; y < static_cast<GLuint>(_size[1]); ++y)
          for (int y   = static_cast<int>(_pdata->size[1] - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_pdata->size[0]); ++x)
              {
                  //rgb_type val(img_r->operator()(x, y) - (_pdata->lipv_r / 2), img_g->operator()(x, y) - (_pdata->lipv_g / 2), img_b->operator()(x, y) - (_pdata->lipv_b / 2));
                  rgb_type val(0, 0, 0);

                  if (_pdata->col_image_r)
                  {
                      val[0] = img_r->operator()(x, y) - (_pdata->lipv_r / 2);

                      if (_pdata->flip_image_r)
                      { val[0] *= -1; }

                      val[0] = _pdata->abs_image_r ? std::abs(val[0]) : std::max(val[0], static_cast<typename rgb_type::value_type>(0));
                  }

                  if (_pdata->col_image_g)
                  {
                      val[1] = img_g->operator()(x, y) - (_pdata->lipv_g / 2);

                      if (_pdata->flip_image_g)
                      { val[1] *= -1; }

                      val[1] = _pdata->abs_image_g ? std::abs(val[1]) : std::max(val[1], static_cast<typename rgb_type::value_type>(0));
                  }

                  if (_pdata->col_image_b)
                  {
                      val[2] = img_b->operator()(x, y) - (_pdata->lipv_b / 2);

                      if (_pdata->flip_image_b)
                      { val[2] *= -1; }

                      val[2] = _pdata->abs_image_b ? std::abs(val[2]) : std::max(val[2], static_cast<typename rgb_type::value_type>(0));
                  }

                  for (unsigned int k = 0; k < 3; ++k)
                  {
                      //intensities[cnt++] = std::abs(val[k]);
                      intensities[cnt++] = val[k];
                      _intensitymin() = std::min(intensitymin(), std::abs(val[k]));
                      _intensitymax() = std::max(intensitymax(), std::abs(val[k]));
                  }
                  _pdata->image(x, y) = val;
              }
          }
          _ssbo_intensity().unmap_and_release();
      }
  }

  auto RGBDICOMFlowImageSliceView::rgb_at_xyzt_current() const -> rgb_type
  { return this->is_initialized() ? _pdata->image(xcurrent(), ycurrent()) : rgb_type(0, 0, 0); }
} // namespace bk

#endif // BK_LIB_GDCM_AVAILABLE


