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

#include <bkGL/Camera.h>

#include <cmath>

#include <bk/Signal>
#include <bkMath/functions/radians_degree_conversion.h>
#include <bkMath/constants/pi.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class Camera::Impl
  {
    public:
      bk::Vec3<GLfloat> pos;
      bk::Vec3<GLfloat> look_at;
      bk::Vec3<GLfloat> up;
      // projection matrix
      bool projection_matrix_mode_perspective;
      // projection matrix perspective
      GLfloat beam_angle_rad;
      GLfloat aspect;
      // projection matrix orthogonal
      GLfloat xsize;
      GLfloat ysize;
      GLfloat znear;
      GLfloat zfar;
      bk::ColMat4<GLfloat> look_at_matrix;
      mutable bool look_at_matrix_is_up2date;
      bk::ColMat4<GLfloat> projection_matrix;
      mutable bool projection_matrix_is_up2date;
      bk::Signal<> s_projection_matrix_changed;
      bk::Signal<> s_lookat_matrix_changed;

      Impl()
          : pos(0, -2.5, 0), // in dicom patient coords, this is in front of the patient
            look_at(0, 0, 0),
            up(0, 0, 1),
            projection_matrix_mode_perspective(true),
            beam_angle_rad(bk::degrees_to_radians(30)),
            aspect(1),
            xsize(100),
            ysize(100),
            znear(0.5),
            zfar(10),
            look_at_matrix_is_up2date(false),
            projection_matrix_is_up2date(false)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() {s_projection_matrix_changed.disconnect_all();s_lookat_matrix_changed.disconnect_all();}
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Camera::Camera()
      : _pdata(std::make_unique<Impl>())
  {
      calc_look_at_matrix();
      calc_projection_matrix();
  }

  Camera::Camera(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Camera::~Camera() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET POSITION
  const bk::Vec3<GLfloat>& Camera::position() const
  { return _pdata->pos; }
  /// @}

  /// @{ -------------------------------------------------- GET LOOK AT VECTOR
  const bk::Vec3<GLfloat>& Camera::look_at() const
  { return _pdata->look_at; }
  /// @}

  /// @{ -------------------------------------------------- GET UP VECTOR
  const bk::Vec3<GLfloat>& Camera::up_vector() const
  { return _pdata->up; }
  /// @}

  /// @{ -------------------------------------------------- GET BEAM ANGLE
  GLfloat Camera::beam_angle_rad() const
  { return _pdata->beam_angle_rad; }
  /// @}

  /// @{ -------------------------------------------------- GET ASPECT RATIO
  GLfloat Camera::aspect_ratio() const
  { return _pdata->aspect; }
  /// @}

  /// @{ -------------------------------------------------- GET X/Y SIZE
  GLfloat Camera::xsize() const
  { return _pdata->xsize; }

  GLfloat Camera::ysize() const
  { return _pdata->ysize; }
  /// @}

  /// @{ -------------------------------------------------- GET ZNEAR/ZFAR
  GLfloat Camera::znear() const
  { return _pdata->znear; }

  GLfloat Camera::zfar() const
  { return _pdata->zfar; }
  /// @}

  /// @{ -------------------------------------------------- GET LOOK AT MATRIX
  const bk::ColMat4<GLfloat>& Camera::look_at_matrix() /*const*/
  {
      if (!_pdata->look_at_matrix_is_up2date)
      { calc_look_at_matrix(); }

      return _pdata->look_at_matrix;
  }

  const GLfloat* Camera::look_at_matrix_data_ptr() /*const*/
  { return &look_at_matrix()[0]; }
  /// @}

  /// @{ -------------------------------------------------- GET PROJECTION MATRIX MODE
  bool Camera::projection_matrix_mode_is_perspective() const
  { return _pdata->projection_matrix_mode_perspective; }

  bool Camera::projection_matrix_mode_is_orthogonal() const
  { return !_pdata->projection_matrix_mode_perspective; }
  /// @}

  /// @{ -------------------------------------------------- GET PROJECTION MATRIX
  const bk::ColMat4<GLfloat>& Camera::projection_matrix() /*const*/
  {
      if (!_pdata->projection_matrix_is_up2date)
      { calc_projection_matrix(); }

      return _pdata->projection_matrix;
  }

  const GLfloat* Camera::projection_matrix_data_ptr() /*const*/
  { return &projection_matrix()[0]; }
  /// @}

  /// @{ -------------------------------------------------- IS UP TO DATE
  bool Camera::look_at_matrix_is_up_to_date() const
  { return _pdata->look_at_matrix_is_up2date; }

  bool Camera::projection_matrix_is_up_to_date() const
  { return _pdata->projection_matrix_is_up2date; }

  bool Camera::is_up_to_date() const
  { return look_at_matrix_is_up_to_date() && projection_matrix_is_up_to_date(); }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<>& Camera::signal_projection_matrix_changed()
  { return _pdata->s_projection_matrix_changed; }

  const bk::Signal<>& Camera::signal_projection_matrix_changed() const
  { return _pdata->s_projection_matrix_changed; }

  bk::Signal<>& Camera::signal_lookat_matrix_changed()
  { return _pdata->s_lookat_matrix_changed; }

  const bk::Signal<>& Camera::signal_lookat_matrix_changed() const
  { return _pdata->s_lookat_matrix_changed; }

  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Camera::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET PROJECTION MATRIX MODE
  void Camera::set_projection_matrix_mode_perspective()
  {
      if (!_pdata->projection_matrix_mode_perspective)
      {
          _pdata->projection_matrix_mode_perspective = true;
          _pdata->projection_matrix_is_up2date = false;

          calc_projection_matrix_perspective();
      }
  }

  void Camera::set_projection_matrix_mode_orthogonal()
  {
      if (_pdata->projection_matrix_mode_perspective)
      {
          _pdata->projection_matrix_mode_perspective = false;
          _pdata->projection_matrix_is_up2date = false;

          calc_projection_matrix_orthogonal();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET POSITION
  void Camera::set_position(GLfloat x, GLfloat y, GLfloat z)
  {
      if (_pdata->pos[0] != x || _pdata->pos[1] != y || _pdata->pos[2] != z)
      {
          _pdata->pos.set(x, y, z);
          _pdata->look_at_matrix_is_up2date = false;
      }
  }

  void Camera::set_position(const bk::Vec3<GLfloat>& pos)
  { set_position(pos[0], pos[1], pos[2]); }
  /// @}

  /// @{ -------------------------------------------------- SET LOOK AT VECTOR
  void Camera::set_look_at_vector(GLfloat x, GLfloat y, GLfloat z)
  {
      if (_pdata->look_at[0] != x || _pdata->look_at[1] != y || _pdata->look_at[2] != z)
      {
          _pdata->look_at.set(x, y, z);
          _pdata->look_at_matrix_is_up2date = false;
      }
  }

  void Camera::set_look_at_vector(const bk::Vec3<GLfloat>& look_at)
  { set_look_at_vector(look_at[0], look_at[1], look_at[2]); }
  /// @}

  /// @{ -------------------------------------------------- SET UP VECTOR
  void Camera::set_up_vector(GLfloat x, GLfloat y, GLfloat z)
  {
      if (_pdata->up[0] != x || _pdata->up[1] != y || _pdata->up[2] != z)
      {
          _pdata->up.set(x, y, z);
          _pdata->look_at_matrix_is_up2date = false;
      }
  }

  void Camera::set_up_vector(const bk::Vec3<GLfloat>& up)
  { set_up_vector(up[0], up[1], up[2]); }
  /// @}

  /// @{ -------------------------------------------------- SET BEAM ANGLE
  void Camera::set_beam_angle_rad(GLfloat beam_angle_rad)
  {
      if (_pdata->beam_angle_rad != beam_angle_rad)
      {
          _pdata->beam_angle_rad = static_cast<GLfloat>(beam_angle_rad);
          _pdata->projection_matrix_is_up2date = false;
      }
  }

  void Camera::set_beam_angle_deg(GLfloat beam_angle_deg)
  {
      const GLfloat temp = bk::degrees_to_radians(static_cast<GLfloat>(beam_angle_deg));
      if (_pdata->beam_angle_rad != temp)
      {
          _pdata->beam_angle_rad = temp;
          _pdata->projection_matrix_is_up2date = false;
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET XY SIZE
  void Camera::set_x_size(GLfloat x)
  { _pdata->xsize = std::max(static_cast<GLfloat>(0.001), std::abs(x)); }

  void Camera::set_y_size(GLfloat y)
  { _pdata->ysize = std::max(static_cast<GLfloat>(0.001), std::abs(y)); }
  /// @}

  /// @{ -------------------------------------------------- SET ZNEAR / ZFAR
  void Camera::set_znear(GLfloat znear)
  {
      if (_pdata->znear != static_cast<GLfloat>(znear))
      {
          _pdata->znear = static_cast<GLfloat>(znear);
          _pdata->projection_matrix_is_up2date = false;
      }
  }

  void Camera::set_zfar(GLfloat zfar)
  {
      if (_pdata->zfar != static_cast<GLfloat>(zfar))
      {
          _pdata->zfar = static_cast<GLfloat>(zfar);
          _pdata->projection_matrix_is_up2date = false;
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET ASPECT RATIO
  void Camera::set_aspect_ratio(GLfloat aspect_ratio)
  {
      if (_pdata->aspect != static_cast<GLfloat>(aspect_ratio))
      {
          _pdata->aspect = static_cast<GLfloat>(aspect_ratio);
          _pdata->projection_matrix_is_up2date = false;
      }
  }

  void Camera::set_aspect_ratio(unsigned int width, unsigned int height)
  {
      assert(width != 0 && height != 0 && "invalid width and/or height");
      set_aspect_ratio(static_cast<GLfloat>(width) / static_cast<GLfloat>(height));
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  void Camera::init()
  {
      calc_look_at_matrix();
      calc_projection_matrix();
  }
  /// @}

  /// @{ -------------------------------------------------- CALC MATRICES
  void Camera::calc_look_at_matrix()
  {
      if (_pdata->look_at_matrix_is_up2date)
      { return; }

      const bk::Vec3<GLfloat>& f((_pdata->look_at - _pdata->pos).normalize());
      const bk::Vec3<GLfloat>& s(f.cross(_pdata->up).normalize());
      const bk::Vec3<GLfloat>& u(s.cross(f));

      _pdata->look_at_matrix.set_zero();
      _pdata->look_at_matrix[0] = s[0];
      _pdata->look_at_matrix[1] = u[0];
      _pdata->look_at_matrix[2] = -f[0];
      _pdata->look_at_matrix[4] = s[1];
      _pdata->look_at_matrix[5] = u[1];
      _pdata->look_at_matrix[6] = -f[1];
      _pdata->look_at_matrix[8] = s[2];
      _pdata->look_at_matrix[9] = u[2];
      _pdata->look_at_matrix[10] = -f[2];
      _pdata->look_at_matrix[15] = 1;

      _pdata->look_at_matrix_is_up2date = true;
      
      _pdata->s_lookat_matrix_changed.emit_signal();
  }

  void Camera::calc_projection_matrix()
  {
      if (_pdata->projection_matrix_is_up2date)
      { return; }

      _pdata->projection_matrix.set_zero();

      if (_pdata->projection_matrix_mode_perspective)
      { calc_projection_matrix_perspective(); }
      else
      { calc_projection_matrix_orthogonal(); }

      _pdata->projection_matrix_is_up2date = true;

      _pdata->s_projection_matrix_changed.emit_signal();
  }

  void Camera::calc_projection_matrix_perspective()
  {
      const GLfloat f = std::tan(static_cast<GLfloat>(0.5) * pi<GLfloat> - _pdata->beam_angle_rad);
      _pdata->projection_matrix[0] = f / _pdata->aspect;
      _pdata->projection_matrix[5] = f;
      _pdata->projection_matrix[10] = (_pdata->zfar + _pdata->znear) / (_pdata->znear - _pdata->zfar);
      _pdata->projection_matrix[11] = -1;
      _pdata->projection_matrix[14] = (2 * _pdata->zfar * _pdata->znear) / (_pdata->znear - _pdata->zfar);
  }

  void Camera::calc_projection_matrix_orthogonal()
  {
      _pdata->projection_matrix[0] = static_cast<GLfloat>(1) / _pdata->xsize;
      _pdata->projection_matrix[5] = static_cast<GLfloat>(1) / _pdata->ysize;
      _pdata->projection_matrix[10] = static_cast<GLfloat>(-2) / (_pdata->zfar - _pdata->znear);
      //_pdata->projection_matrix[10] = static_cast<GLfloat>(2) / (_zfar - _znear);
      _pdata->projection_matrix[14] = -(_pdata->zfar + _pdata->znear) / (_pdata->zfar - _pdata->znear);
      //_pdata->projection_matrix[14] = (_zfar + _znear) / (_zfar - _znear);
      _pdata->projection_matrix[15] = 1;
  }
  /// @}
} // namespace bk


