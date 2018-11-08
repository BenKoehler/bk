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

#pragma once

#ifndef BKGL_TRACKBALL_H
#define BKGL_TRACKBALL_H

#include <bk/CopyablePIMPL>
#include <bkGL/trackball/Quaternion.h>
#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>
#include <bk/Matrix>

namespace bk
{
  // -------------------- forward declaration
  class Mouse;
  class WindowGeometry;
  // -------------------- forward declaration END

  class BKGL_EXPORT TrackBall
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TrackBall;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      TrackBall();
      TrackBall(const self_type& other);
      TrackBall(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TrackBall();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET ROTATION MATRIX
      //! matrix pointer that can be used for glMultMatrixf
      [[nodiscard]] const GLfloat* rotation_matrix_ptr() const;
      /// @}

      /// @{ -------------------------------------------------- GET ROTATION QUATERNION
      [[nodiscard]] const Quaternion& rotation_quaternion() const;
      /// @}

      /// @{ -------------------------------------------------- IS ACTIVE
      //! mouse is clicked and trackball is dragged
      [[nodiscard]] bool is_active() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET WINDOW SIZE
      void set_window_size(GLfloat width, GLfloat height);
      void set_window_size(const WindowGeometry& window);
      /// @}

      /// @{ -------------------------------------------------- SET CENTER
      void set_center(const Vec3<GLfloat>& center);
      /// @}

      /// @{ -------------------------------------------------- SET RADIUS
      void set_radius(GLfloat r);
      /// @}

      /// @{ -------------------------------------------------- SET CURRENT POSITION
      /*!
       * updates rotation matrix
       */
      void set_current_mouse_position(int x, int y);
      void set_current_mouse_position(const Mouse& mouse);
      /// @}

      /// @{ -------------------------------------------------- SET UPVECTOR ID
      //! workaround for dicom scenarios where the upvector is not the default y-axis
      void set_upvector_id(unsigned int i);
      /// @}

      /// @{ -------------------------------------------------- SET ACTIVE
      void set_active();
      void set_inactive();
      /// @}

      /// @{ -------------------------------------------------- SET CUSTOM ROTATION
      void set_custom_rotation(const Quaternion& q);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- MAP MOUSE TO SPHERE SURFACE
      [[nodiscard]] static Vec3<GLfloat> _map_mouse_to_sphere_surface(const Vec3<GLfloat>& mouse, const Vec3<GLfloat>& center, GLfloat r);
      /// @}

      /// @{ -------------------------------------------------- CALC ARC BETWEEN POINTS
      [[nodiscard]] static Quaternion _calc_arc_between_point(const Vec3<GLfloat>& from, const Vec3<GLfloat>& to);
      /// @}

      /// @{ -------------------------------------------------- QUATERNION TO ROTATION MATRIX
      /*!
       * intended for right-handed coordinate systems only
       */
      void _quaternion_to_rotation_matrix(Mat4<GLfloat>& m, const Quaternion& q) const;
      /// @}

      /// @{ -------------------------------------------------- UPDATE
      void _update_rotation_matrix();
      /// @}

    public:
      /// @{ -------------------------------------------------- RESET
      void reset_rotation();
      /// @}
  };
} // namespace bk

#endif //BKGL_TRACKBALL_H


