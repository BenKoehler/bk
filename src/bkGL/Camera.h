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

#ifndef BKGL_CAMERA_H
#define BKGL_CAMERA_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  template<typename...> class Signal;

  namespace details
  {
    class AbstractRenderable;
    class AbstractSceneRenderable;
  } // namespace details
  // -------------------- forward declaration END

  class BKGL_EXPORT Camera
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Camera;

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
      Camera();
      Camera(const self_type&) = delete;
      Camera(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~Camera();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET POSITION
      [[nodiscard]] const bk::Vec3<GLfloat>& position() const;
      /// @}

      /// @{ -------------------------------------------------- GET LOOK AT VECTOR
      [[nodiscard]] const bk::Vec3<GLfloat>& look_at() const;
      /// @}

      /// @{ -------------------------------------------------- GET UP VECTOR
      [[nodiscard]] const bk::Vec3<GLfloat>& up_vector() const;
      /// @}

      /// @{ -------------------------------------------------- GET BEAM ANGLE
      //! beam angle in radians
      [[nodiscard]] GLfloat beam_angle_rad() const;
      /// @}

      /// @{ -------------------------------------------------- GET ASPECT RATIO
      [[nodiscard]] GLfloat aspect_ratio() const;
      /// @}

      /// @{ -------------------------------------------------- GET X/Y SIZE
      //! parameters for orthogonal projection matrix
      [[nodiscard]] GLfloat xsize() const;
      [[nodiscard]] GLfloat ysize() const;
      /// @}

      /// @{ -------------------------------------------------- GET ZNEAR/ZFAR
      [[nodiscard]] GLfloat znear() const;
      [[nodiscard]] GLfloat zfar() const;
      /// @}

      /// @{ -------------------------------------------------- GET LOOK AT MATRIX
      [[nodiscard]] const bk::ColMat4<GLfloat>& look_at_matrix() /*const*/  ;
      [[nodiscard]] const GLfloat* look_at_matrix_data_ptr() /*const*/  ;
      /// @}

      /// @{ -------------------------------------------------- GET PROJECTION MATRIX MODE
      [[nodiscard]] bool projection_matrix_mode_is_perspective() const;
      [[nodiscard]] bool projection_matrix_mode_is_orthogonal() const;
      /// @}

      /// @{ -------------------------------------------------- GET PROJECTION MATRIX
      [[nodiscard]] const bk::ColMat4<GLfloat>& projection_matrix() /*const*/  ;
      [[nodiscard]] const GLfloat* projection_matrix_data_ptr() /*const*/  ;
      /// @}

      /// @{ -------------------------------------------------- IS UP TO DATE
      [[nodiscard]] bool look_at_matrix_is_up_to_date() const;
      [[nodiscard]] bool projection_matrix_is_up_to_date() const;
      [[nodiscard]] bool is_up_to_date() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] bk::Signal<>& signal_projection_matrix_changed();
      [[nodiscard]] const bk::Signal<>& signal_projection_matrix_changed() const;

      [[nodiscard]] bk::Signal<const ColMat4<GLfloat>&>& signal_new_projection_matrix();
      [[nodiscard]] const bk::Signal<const ColMat4<GLfloat>&>& signal_new_projection_matrix() const;

      [[nodiscard]] bk::Signal<>& signal_lookat_matrix_changed();
      [[nodiscard]] const bk::Signal<>& signal_lookat_matrix_changed() const;

      [[nodiscard]] bk::Signal<const ColMat4<GLfloat>&>& signal_new_lookat_matrix();
      [[nodiscard]] const bk::Signal<const ColMat4<GLfloat>&>& signal_new_lookat_matrix() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[nodiscard]] auto operator=(const self_type&) -> self_type& = delete;
      [[nodiscard]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET PROJECTION MATRIX MODE
      void set_projection_matrix_mode_perspective();
      void set_projection_matrix_mode_orthogonal();
      /// @}

      /// @{ -------------------------------------------------- SET POSITION
      void set_position(GLfloat x, GLfloat y, GLfloat z);
      void set_position(const bk::Vec3<GLfloat>& pos);
      /// @}

      /// @{ -------------------------------------------------- SET LOOK AT VECTOR
      void set_look_at_vector(GLfloat x, GLfloat y, GLfloat z);
      void set_look_at_vector(const bk::Vec3<GLfloat>& look_at);
      /// @}

      /// @{ -------------------------------------------------- SET UP VECTOR
      void set_up_vector(GLfloat x, GLfloat y, GLfloat z);
      void set_up_vector(const bk::Vec3<GLfloat>& up);
      /// @}

      /// @{ -------------------------------------------------- SET BEAM ANGLE
      void set_beam_angle_rad(GLfloat beam_angle_rad);
      void set_beam_angle_deg(GLfloat beam_angle_deg);
      /// @}

      /// @{ -------------------------------------------------- SET XY SIZE
      void set_x_size(GLfloat x);
      void set_y_size(GLfloat y);
      /// @}

      /// @{ -------------------------------------------------- SET ZNEAR / ZFAR
      void set_znear(GLfloat znear);
      void set_zfar(GLfloat zfar);
      /// @}

      /// @{ -------------------------------------------------- SET ASPECT RATIO
      void set_aspect_ratio(GLfloat aspect_ratio);
      void set_aspect_ratio(unsigned int width, unsigned int height);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init();
      /// @}

      /// @{ -------------------------------------------------- CONNECT SIGNALS
      void connect_signals(std::shared_ptr<details::AbstractRenderable>& r);
      void connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r);
      /// @}

      /// @{ -------------------------------------------------- CALC MATRICES
      void calc_look_at_matrix();
      void calc_projection_matrix();
    private:
      void calc_projection_matrix_perspective();
      void calc_projection_matrix_orthogonal();
    public:
      /// @}
  }; // class Camera
} // namespace bk

#endif //BKGL_CAMERA_H


