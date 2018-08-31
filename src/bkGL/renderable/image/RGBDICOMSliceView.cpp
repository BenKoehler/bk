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

    #include <algorithm>

    #include <bkGL/renderable/image/RGBDICOMSliceView.h>

    #include <bk/Dicom>
    #include <bk/Matrix>
    #include <bkGL/buffer/SSBO.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct RGBDICOMSliceView::Impl
  {
      image_type image;
      DicomDirImporter* dcm_importer;
      unsigned int dcm_image_r_id;
      unsigned int dcm_image_g_id;
      unsigned int dcm_image_b_id;
      Vec4<GLuint> size;

      Impl()
          : dcm_importer(nullptr),
            dcm_image_r_id(0),
            dcm_image_g_id(0),
            dcm_image_b_id(0)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  RGBDICOMSliceView::RGBDICOMSliceView()
      : base_type()
  #else

      RGBDICOMSliceView::RGBDICOMSliceView(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  { /* do nothing */ }

  RGBDICOMSliceView::RGBDICOMSliceView(self_type&&) noexcept = default;

  RGBDICOMSliceView::~RGBDICOMSliceView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  const DicomDirImporter* RGBDICOMSliceView::dicom_dir_importer() const
  { return _pdata->dcm_importer; }

  GLuint RGBDICOMSliceView::image_size(GLuint id) const
  { return id < 4 ? _pdata->size[id] : 1; }

  GLfloat RGBDICOMSliceView::image_scale(GLuint id) const
  { return id < static_cast<GLuint>(_pdata->image.geometry().num_dimensions()) ? _pdata->image.geometry().transformation().scale(id) : 1; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  bool RGBDICOMSliceView::set_images(unsigned int dcm_image_r_id, unsigned int dcm_image_g_id, unsigned int dcm_image_b_id)
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
      _pdata->image.set_size(info_r.Columns, info_r.Rows);
      _pdata->image.geometry().transformation().set_scale(info_r.ColSpacing, info_r.RowSpacing);
      _pdata->size.set(info_r.Columns, info_r.Rows, info_r.Slices > 1 ? info_r.Slices : 1, info_r.TemporalPositions > 1 ? info_r.TemporalPositions : 1);
      return _pdata->image.num_values() > 1;
  }

  auto RGBDICOMSliceView::operator=(self_type&& other) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void RGBDICOMSliceView::clear_image()
  {
      _pdata->image.set_size(1, 1);
      _pdata->image.geometry().transformation().set_scale(1, 1);
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

  void RGBDICOMSliceView::update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t)
  {
      if (_pdata->dcm_importer == nullptr)
      { return; }
      GLfloat* intensities = _ssbo_intensity().map_write_only<GLfloat>();
      if (intensities != nullptr)
      {
          //const auto& size = _pdata->image.size();
          const auto img_r = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_r_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          const auto img_g = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_g_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          const auto img_b = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_b_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);
          _intensitymax() = -std::numeric_limits<GLfloat>::max();
          _intensitymin() = std::numeric_limits<GLfloat>::max();
          GLuint cnt = 0;
          //for (GLuint y = 0; y < static_cast<GLuint>(_pdata->size[1]); ++y)
          for (int y = static_cast<int>(_pdata->size[1] - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_pdata->size[0]); ++x)
              {
                  const rgb_type val(img_r->operator()(x, y), img_g->operator()(x, y), img_b->operator()(x, y));
                  for (unsigned int k = 0; k < 3; ++k)
                  {
                      intensities[cnt++] = val[k];
                      _intensitymin() = std::min(intensitymin(), val[k]);
                      _intensitymax() = std::max(intensitymax(), val[k]);
                  }
                  _pdata->image(x, y) = val;
              }
          }
          _ssbo_intensity().unmap_and_release();
      }
  }

  auto RGBDICOMSliceView::rgb_at_xyzt_current() const -> rgb_type
  { return this->is_initialized() ? _pdata->image(xcurrent(), ycurrent()) : rgb_type(0, 0, 0); }
} // namespace bk

#endif // BK_LIB_GDCM_AVAILABLE


