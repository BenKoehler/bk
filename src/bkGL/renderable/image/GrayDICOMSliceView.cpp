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

#ifdef BK_LIB_GDCM_AVAILABLE

    #include <bkGL/renderable/image/GrayDICOMSliceView.h>

    #include <algorithm>
    #include <memory>

    #include <bk/Dicom>
    #include <bk/Matrix>
    #include <bkGL/buffer/SSBO.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct GrayDICOMSliceView::Impl
  {
      image_type image;
      DicomDirImporter* dcm_importer;
      unsigned int dcm_image_id;
      Vec4<GLuint> size;

      Impl()
          : dcm_importer(nullptr),
            dcm_image_id(0)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  GrayDICOMSliceView::GrayDICOMSliceView()
      : base_type()
  #else

  GrayDICOMSliceView::GrayDICOMSliceView(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  { /* do nothing */ }

  GrayDICOMSliceView::GrayDICOMSliceView(self_type&&) noexcept = default;

  GrayDICOMSliceView::~GrayDICOMSliceView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  const DicomDirImporter* GrayDICOMSliceView::dicom_dir_importer() const
  { return _pdata->dcm_importer; }

  GLuint GrayDICOMSliceView::image_size(GLuint id) const
  { return id < 4 ? _pdata->size[id] : 1; }

  GLfloat GrayDICOMSliceView::image_scale(GLuint id) const
  { return id < static_cast<GLuint>(_pdata->image.geometry().num_dimensions()) ? _pdata->image.geometry().transformation().scale(id) : 1; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void GrayDICOMSliceView::set_dicom_dir_importer(DicomDirImporter* d)
  { _pdata->dcm_importer = d; }

  bool GrayDICOMSliceView::set_image(unsigned int dcm_image_id)
  {
      if (_pdata->dcm_importer == nullptr)
      { return false; }

      const unsigned int nImages = _pdata->dcm_importer->num_images();

      if (dcm_image_id >= nImages)
      { return false; }

      _pdata->dcm_image_id = dcm_image_id;
      const DicomImageInfos& info = _pdata->dcm_importer->image_infos(_pdata->dcm_image_id);
      _pdata->image.set_size(info.Columns, info.Rows);
      _pdata->image.geometry().transformation().set_scale(info.ColSpacing, info.RowSpacing);
      _pdata->size.set(info.Columns, info.Rows, info.Slices > 1 ? info.Slices : 1, info.TemporalPositions > 1 ? info.TemporalPositions : 1);

      return _pdata->image.num_values() > 1;
  }

  auto GrayDICOMSliceView::operator=(self_type&&) noexcept -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void GrayDICOMSliceView::clear_image()
  {
      _pdata->image.set_size(1, 1);
      _pdata->image.geometry().transformation().set_scale(1, 1);
      _xmax()            = 0;
      _ymax()            = 0;
      _zmax()            = 0;
      _tmax()            = 0;
      _xcurrent()        = 0;
      _ycurrent()        = 0;
      _zcurrent()        = 0;
      _tcurrent()        = 0;
      _intensitymax()    = -std::numeric_limits<GLfloat>::max();
      _intensitymin()    = std::numeric_limits<GLfloat>::max();
      intensitycurrent() = 0;
  }

  void GrayDICOMSliceView::update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t)
  {
      if (_pdata->dcm_importer == nullptr)
      { return; }

      if (GLfloat* intensities = _ssbo_intensity().map_write_only<GLfloat>(); intensities != nullptr)
      {
          const std::unique_ptr<DicomImage<double, -1>> img = _pdata->dcm_importer->read_image_block(_pdata->dcm_image_id, 0, _pdata->size[0] - 1, 0, _pdata->size[1] - 1, z, z, t, t);

          _intensitymax() = -std::numeric_limits<GLfloat>::max();
          _intensitymin() = std::numeric_limits<GLfloat>::max();

          GLuint cnt = 0;

          //for (GLuint y = 0; y < static_cast<GLuint>(_size[1]); ++y)
          for (int y = static_cast<int>(_pdata->size[1] - 1); y >= 0; --y) // y is inverted, because GL coord system starts top left and image coord system starts bottom left
          {
              for (GLuint x = 0; x < static_cast<GLuint>(_pdata->size[0]); ++x)
              {
                  const GLfloat val = static_cast<GLfloat>(img->operator()(x, y));
                  intensities[cnt++] = val;
                  _pdata->image(x, y) = val;
                  _intensitymin()     = std::min(intensitymin(), val);
                  _intensitymax()     = std::max(intensitymax(), val);
              }
          }

          _ssbo_intensity().unmap_and_release();
      }
  }

  auto GrayDICOMSliceView::intensity_at_xyzt_current() const -> GLfloat
  { return this->is_initialized() ? _pdata->image(xcurrent(), ycurrent()) : 0; }
} // namespace bk

#endif // BK_LIB_GDCM_AVAILABLE
