#include <bkGL/renderable/image/GrayImageSliceView.h>
#include <bkGL/buffer/SSBO.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  GrayImageSliceView::GrayImageSliceView() : base_type()
  #else

  GrayImageSliceView::GrayImageSliceView(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  {
  }

  GrayImageSliceView::GrayImageSliceView(self_type&&) = default;

  GrayImageSliceView::~GrayImageSliceView()
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  GLuint GrayImageSliceView::image_size(GLuint id) const 
  { return id < static_cast<GLuint>(_image.geometry().num_dimensions()) ? _image.geometry().size(id) : 1; }

  GLfloat GrayImageSliceView::image_scale(GLuint id) const 
  { return id < static_cast<GLuint>(_image.geometry().num_dimensions()) ? _image.geometry().transformation().scale(id) : 1; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto GrayImageSliceView::operator=(self_type&&) -> self_type& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void GrayImageSliceView::clear_image()
  {
      _image.set_size(0, 0, 0, 0);
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
      intensitycurrent() = 0;
  }

  void GrayImageSliceView::update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t)
  {
      if (_image.num_values() <= 1)
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
                  const GLfloat val = static_cast<GLfloat>(_image(x, y, z, t));
                  intensities[cnt++] = val;

                  //_intensitymin = min(_intensitymin, val);
                  //_intensitymax = max(_intensitymax, val);
              }
          }
          _ssbo_intensity().unmap_and_release();
      }
  }

  GLfloat GrayImageSliceView::intensity_at_xyzt_current() const 
  { return this->is_initialized() && _image.num_values() > 1 ? _image(xcurrent(), ycurrent(), zcurrent(), tcurrent()) : 0; }
} // namespace bk


