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

#include <bkGL/trackball/TrackBall.h>

#include <algorithm>
#include <cmath>

#include <bkGL/Mouse.h>
#include <bkGL/WindowGeometry.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class TrackBall::Impl
  {
    public:
      // screen
      WindowGeometry window;
      // trackball
      bk::Vec3<GLfloat> trackball_center;
      GLfloat trackball_radius;
      bool rotation_is_active;
      // mouse
      bk::Vec3<GLfloat> mouse_position_current;
      bk::Vec3<GLfloat> mouse_position_at_activation;
      // rotation
      Quaternion rotation_quaternion_current;
      Quaternion rotation_quaternion_when_finished;
      bk::Mat4<GLfloat> rotation_matrix;
      // helper
      unsigned int upVectorId;

      Impl()
          : window(0, 0),
            trackball_center(0, 0, 0),
            trackball_radius(1),
            rotation_is_active(false),
            mouse_position_current(0, 0, 0),
            mouse_position_at_activation(0, 0, 0),
            rotation_quaternion_current(1, 0, 0, 0),
            rotation_quaternion_when_finished(1, 0, 0, 0),
            upVectorId(1)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  TrackBall::TrackBall()
      : _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  TrackBall::TrackBall(const self_type& other)
      : _pdata(new Impl(*other._pdata.get()))
  { /* do nothing */ }

  TrackBall::TrackBall(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  TrackBall::~TrackBall() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET ROTATION MATRIX
  const GLfloat* TrackBall::rotation_matrix_ptr() const
  { return &_pdata->rotation_matrix[0]; }
  /// @}

  /// @{ -------------------------------------------------- IS DRAGGING
  bool TrackBall::is_active() const
  { return _pdata->rotation_is_active; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET WINDOW SIZE
  void TrackBall::set_window_size(GLfloat width, GLfloat height)
  { _pdata->window.set(width, height); }

  void TrackBall::set_window_size(const WindowGeometry& window)
  { _pdata->window = window; }
  /// @}

  /// @{ -------------------------------------------------- SET CENTER
  void TrackBall::set_center(const bk::Vec3<GLfloat>& center)
  { _pdata->trackball_center = center; }
  /// @}

  /// @{ -------------------------------------------------- SET RADIUS
  void TrackBall::set_radius(GLfloat r)
  { _pdata->trackball_radius = r; }
  /// @}

  /// @{ -------------------------------------------------- SET CURRENT POSITION
  void TrackBall::set_current_mouse_position(int x, int y)
  {
      if (_pdata->upVectorId == 0)
      { y = _pdata->window.height() - y; }
      else if (_pdata->upVectorId == 2)
      {
          x = _pdata->window.width() - x;
          y = _pdata->window.height() - y;
      }

      _pdata->mouse_position_current[0] = 2 * (static_cast<GLfloat>(x) / _pdata->window.width()) - 1;
      _pdata->mouse_position_current[1] = 2 * (static_cast<GLfloat>(_pdata->window.height() - y) / _pdata->window.height()) - 1;
      _pdata->mouse_position_current[2] = 0;

      _update_rotation_matrix();
  }

  void TrackBall::set_current_mouse_position(const Mouse& mouse)
  { set_current_mouse_position(mouse.x(), mouse.y()); }
  /// @}f

  /// @{ -------------------------------------------------- SET UPVECTOR ID
  void TrackBall::set_upvector_id(unsigned int i)
  { _pdata->upVectorId = std::min(i, 2U); }
  /// @}

  /// @{ -------------------------------------------------- SET ACTIVE
  void TrackBall::set_active()
  {
      _pdata->rotation_is_active = true;
      _pdata->mouse_position_at_activation = _pdata->mouse_position_current;
  }

  void TrackBall::set_inactive()
  {
      _pdata->rotation_is_active = false;
      _pdata->rotation_quaternion_when_finished = _pdata->rotation_quaternion_current;
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- MAP MOUSE TO SPHERE SURFACE
  bk::Vec3<GLfloat> TrackBall::_map_mouse_to_sphere_surface(const bk::Vec3<GLfloat>& mouse, const bk::Vec3<GLfloat>& center, GLfloat r)
  {
      bk::Vec3<GLfloat> res = (mouse - center) * (static_cast<GLfloat>(1) / r);

      const GLfloat mag = res.norm_squared();

      if (mag > static_cast<GLfloat>(1))
      {
          res.normalize_internal();
          res[2] = 0;
      }
      else
      { res[2] = std::sqrt(static_cast<GLfloat>(1) - mag); }

      return res;
  }
  /// @}

  /// @{ -------------------------------------------------- CALC ARC BETWEEN POINTS
  Quaternion TrackBall::_calc_arc_between_point(const bk::Vec3<GLfloat>& from, const bk::Vec3<GLfloat>& to)
  { return Quaternion(from[0] * to[0] + from[1] * to[1] + from[2] * to[2], from[1] * to[2] - from[2] * to[1], from[2] * to[0] - from[0] * to[2], from[0] * to[1] - from[1] * to[0]); }
  /// @}

  /// @{ -------------------------------------------------- QUATERNION TO ROTATION MATRIX
  void TrackBall::_quaternion_to_rotation_matrix(bk::Mat4<GLfloat>& m, const Quaternion& q) const
  {
      const GLfloat l = q.norm_squared();
      const GLfloat s = l > 0 ? (static_cast<GLfloat>(2) / l) : static_cast<GLfloat>(0);
      
      const GLfloat xs = q[1] * s;
      const GLfloat ys = q[2] * s;
      const GLfloat zs = q[3] * s;
      
      const GLfloat wx = q[0] * xs;
      const GLfloat wy = q[0] * ys;
      const GLfloat wz = q[0] * zs;
      
      const GLfloat xx = q[1] * xs;
      const GLfloat xy = q[1] * ys;
      const GLfloat xz = q[1] * zs;
      
      const GLfloat yy = q[2] * ys;
      const GLfloat yz = q[2] * zs;
      const GLfloat zz = q[3] * zs;

      m[0] = static_cast<GLfloat>(1) - (yy + zz);
      m[1] = xy + wz;
      m[2] = xz - wy;
      m[3] = 0;

      m[4] = xy - wz;
      m[5] = static_cast<GLfloat>(1) - (xx + zz);
      m[6] = yz + wx;
      m[7] = 0;

      m[8] = xz + wy;
      m[9] = yz - wx;
      m[10] = static_cast<GLfloat>(1) - (xx + yy);
      m[11] = 0;

      m[12] = 0;
      m[13] = 0;
      m[14] = 0;
      m[15] = 1;

      if (_pdata->upVectorId == 0)
      {
          m.swap_cols_internal(2, 0);
          m.swap_rows_internal(2, 0);

          m.swap_cols_internal(1, 0);
          m.swap_rows_internal(1, 0);
      }
      else if (_pdata->upVectorId == 2)
      {
          m.swap_cols_internal(1, 2);
          m.swap_rows_internal(1, 2);
      }
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE
  void TrackBall::_update_rotation_matrix()
  {
      const bk::Vec3<GLfloat> from = _map_mouse_to_sphere_surface(_pdata->mouse_position_at_activation, _pdata->trackball_center, _pdata->trackball_radius);
      const bk::Vec3<GLfloat> to = _map_mouse_to_sphere_surface(_pdata->mouse_position_current, _pdata->trackball_center, _pdata->trackball_radius);

      if (_pdata->rotation_is_active)
      { _pdata->rotation_quaternion_current = _calc_arc_between_point(from, to) * _pdata->rotation_quaternion_when_finished; }

      _quaternion_to_rotation_matrix(_pdata->rotation_matrix, _pdata->rotation_quaternion_current);
  }
  /// @}

  /// @{ -------------------------------------------------- RESET
  void TrackBall::reset_rotation()
  {
      _pdata->rotation_matrix.set_identity();

      _pdata->mouse_position_current.set_zero();
      _pdata->mouse_position_at_activation.set_zero();

      _pdata->rotation_quaternion_current.set(1, 0, 0, 0);
      _pdata->rotation_quaternion_when_finished.set(1, 0, 0, 0);
  }
  /// @}
} // namespace bk


