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

#include <bkDataset/transformation/DicomTransformation.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DicomTransformation::DicomTransformation()
      : _image_type(DicomImageType::TwoDimensional),
        _world_matrix(MatrixFactory::Identity(_world_matrix)),
        _inverse_world_matrix(_world_matrix),
        _scale(MatrixFactory::One(_scale)),
        _translation(MatrixFactory::Zero(_translation)),
        _rot_shear_matrix(MatrixFactory::Identity(_rot_shear_matrix))
  { /* do nothing */ }

  DicomTransformation::DicomTransformation(const self_type&) = default;
  DicomTransformation::DicomTransformation(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DicomTransformation::~DicomTransformation() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET DICOM IMAGE TYPE
  DicomImageType DicomTransformation::dicom_image_type() const
  { return _image_type; }

  bool DicomTransformation::dicom_image_type_is_2d() const
  { return _image_type == DicomImageType::TwoDimensional; }

  bool DicomTransformation::dicom_image_type_is_2dt() const
  { return _image_type == DicomImageType::TwoDimensionalPlusTime; }

  bool DicomTransformation::dicom_image_type_is_3d() const
  { return _image_type == DicomImageType::ThreeDimensional; }

  bool DicomTransformation::dicom_image_type_is_3dt() const
  { return _image_type == DicomImageType::ThreeDimensionalPlusTime; }
  /// @}

  /// @{ -------------------------------------------------- GET (INVERSE) WORLD MATRIX WITH TIME
  const Mat5d& DicomTransformation::world_matrix_with_time() const
  { return _world_matrix; }

  const Mat5d& DicomTransformation::inverse_world_matrix_with_time() const
  { return _inverse_world_matrix; }
  /// @}

  /// @{ -------------------------------------------------- GET (INVERSE) WORLD MATRIX
  Mat4d DicomTransformation::world_matrix() const
  {
      Mat4d w;

      // copy 3x3 minor
      for (unsigned int r = 0; r < 3; ++r)
      {
          for (unsigned int c = 0; c < 3; ++c)
          { w(r, c) = _world_matrix(r, c); }
      }

      // copy 3 elements of bottom row
      for (unsigned int c = 0; c < 3; ++c)
      { w(3, c) = _world_matrix(4, c); }

      // copy 3 elements of rightmost col
      for (unsigned int r = 0; r < 3; ++r)
      { w(r, 3) = _world_matrix(r, 4); }

      // copy bottom left element
      w(3, 3) = _world_matrix(4, 4);

      return w;
  }

  Mat4d DicomTransformation::inverse_world_matrix() const
  { return world_matrix().pseudo_inverse(); }
  /// @}

  /// @{ -------------------------------------------------- GET SCALE
  const Vec4d& DicomTransformation::scale() const
  { return _scale; }

  double DicomTransformation::scale(unsigned int dimid) const
  { return _scale[dimid]; }
  /// @}

  /// @{ -------------------------------------------------- GET TRANSLATION
  const Vec4d& DicomTransformation::translation() const
  { return _translation; }

  double DicomTransformation::translation(unsigned int dimid) const
  { return _translation[dimid]; }
  /// @}

  /// @{ -------------------------------------------------- GET ROTATION/SHEAR MATRIX
  const Mat4d& DicomTransformation::rotation_shear_matrix() const
  { return _rot_shear_matrix; }
  /// @}

  /// @{ -------------------------------------------------- GET TEMPORAL RESOLUTION
  double DicomTransformation::temporal_resolution() const
  { return _scale[3]; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DicomTransformation::operator=(const self_type&) -> self_type& = default;
  auto DicomTransformation::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET DICOM IMAGE TYPE
  void DicomTransformation::set_dicom_image_type(DicomImageType type)
  { _image_type = type; }

  void DicomTransformation::set_dicom_image_type_2d()
  { set_dicom_image_type(DicomImageType::TwoDimensional); }

  void DicomTransformation::set_dicom_image_type_2dt()
  { set_dicom_image_type(DicomImageType::TwoDimensionalPlusTime); }

  void DicomTransformation::set_dicom_image_type_3d()
  { set_dicom_image_type(DicomImageType::ThreeDimensional); }

  void DicomTransformation::set_dicom_image_type_3dt()
  { set_dicom_image_type(DicomImageType::ThreeDimensionalPlusTime); }
  /// @}

  /// @{ -------------------------------------------------- SET WORLD MATRIX
  void DicomTransformation::set_world_matrix(const Mat5d& w)
  {
      _world_matrix = w;
      _inverse_world_matrix = _world_matrix.pseudo_inverse();

      _extract_transformation_components();
  }

  void DicomTransformation::set_world_matrix(const Mat4d& w, double temporal_resolution)
  {
      Mat5d wt;

      // copy 3x3 minor
      for (unsigned int r = 0; r < 3; ++r)
      {
          for (unsigned int c = 0; c < 3; ++c)
          { wt(r, c) = w(r, c); }
      }

      // copy 3 elements of bottom row and rightmost col
      for (unsigned int i = 0; i < 3; ++i)
      {
          wt(4, i) = w(3, i);
          wt(i, 4) = w(i, 3);
      }

      // set temporal resolution
      wt(3, 3) = temporal_resolution;

      // copy bottom right element
      wt(4, 4) = w(3, 3);

      set_world_matrix(wt);
  }
  /// @}

  /// @{ -------------------------------------------------- SET TEMPORAL RESOLUTION
  void DicomTransformation::set_temporal_resolution(double dt)
  {
      Mat5d w = world_matrix_with_time();
      w(3, 3) = dt;
      set_world_matrix(w);
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- EXTRACT INFORMATION
  void DicomTransformation::_extract_transformation_components()
  {
      // https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

      /* given the following world matrix:
       *       a b c d
       *       e d g h
       *       i j k l
       *       0 0 0 1
       */

      /*
       * - the translation vector is [d, h, l]
       */
      for (unsigned int i = 0; i < 4; ++i)
      { _translation[i] = _world_matrix(i, 4); }

      /*
       * - the scale vector [s0, s1, s2] is (||[a, e, i]||, ||[b, f, j]||, ||[c, g, k]||)
       *
       * - the rotation+shear matrix is the upper 3x3 of:
       *       a/s0  b/s1  c/s2  0
       *       e/s0  f/s1  g/s2  0
       *       h/s0  i/s1  j/s2  0
       *        0     0     0    1
       */
      for (unsigned int colid = 0; colid < 4; ++colid)
      {
          _scale[colid] = _world_matrix.col(colid).norm();

          for (unsigned int rowid = 0; rowid < 4; ++rowid)
          { _rot_shear_matrix(rowid, colid) = _world_matrix(rowid, colid) / _scale[colid]; }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- HELPER: HOMOGENEOUS COORDINATES
  ColVec5d DicomTransformation::_to_homogeneous_coordinates(const Vec4d& vec) const
  {
      ColVec5d v(0, 0, 0, 0, 1);

      for (unsigned int i = 0; i < 4; ++i)
      { v[i] = vec[i]; }

      return v;
  }

  void DicomTransformation::_from_homogeneous_coordinates(const ColVec5d& v, Vec4d& vec) const
  {
      for (unsigned int i = 0; i < 4; ++i)
      { vec[i] = v[i] / v[4]; }
  }
  /// @}

  /// @{ -------------------------------------------------- TO WORLD COORDINATES
  void DicomTransformation::to_world_coordinates_impl(Vec4d& vec) const
  {
      ColVec5d v = _to_homogeneous_coordinates(vec);
      v = _world_matrix * v;
      _from_homogeneous_coordinates(v, vec);
  }

  MatXd DicomTransformation::to_world_coordinates(double x0, double x1, double x2, double x3) const
  {
      Vec4d temp(x0, x1, 0, 0);

      if (dicom_image_type_is_2dt())
      { temp[3] = x2; }
      else if (dicom_image_type_is_3d())
      { temp[2] = x2; }
      else if (dicom_image_type_is_3dt())
      {
          temp[2] = x2;
          temp[3] = x3;
      }

      to_world_coordinates_impl(temp);

      MatXd res;

      switch (_image_type)
      {
          case DicomImageType::TwoDimensional: [[fallthrough]]
          case DicomImageType::ThreeDimensional:
          {
              res.set_size(3, 1);
              break;
          }

          case DicomImageType::TwoDimensionalPlusTime: [[fallthrough]]
          case DicomImageType::ThreeDimensionalPlusTime:
          {
              res.set_size(4, 1);
              break;
          }
      }

      for (unsigned int i = 0; i < res.num_elements(); ++i)
      { res[i] = temp[i]; }

      return res;
  }

  /// @}

  /// @{ -------------------------------------------------- TO OBJECT COORDINATES
  void DicomTransformation::to_object_coordinates_impl(Vec4d& vec) const
  {
      ColVec5d v = _to_homogeneous_coordinates(vec);
      v = _inverse_world_matrix * v;
      _from_homogeneous_coordinates(v, vec);
  }

  MatXd DicomTransformation::to_object_coordinates(double x0, double x1, double x2, double x3) const
  {
      Vec4d temp(x0, x1, x2, x3);
      to_object_coordinates_impl(temp);

      MatXd res;

      switch (_image_type)
      {
          case DicomImageType::TwoDimensional:
          {
              res.set_size(2, 1);
              break;
          }

          case DicomImageType::ThreeDimensional: [[fallthrough]]
          case DicomImageType::TwoDimensionalPlusTime:
          {
              res.set_size(3, 1);
              break;
          }

          case DicomImageType::ThreeDimensionalPlusTime:
          {
              res.set_size(4, 1);
              break;
          }
      }

      res[0] = temp[0];
      res[1] = temp[1];

      if (dicom_image_type_is_2dt())
      { res[2] = temp[3]; }
      else if (dicom_image_type_is_3d())
      { res[2] = temp[2]; }
      else if (dicom_image_type_is_3dt())
      {
          res[2] = temp[2];
          res[3] = temp[3];
      }

      return res;
  }
  /// @}
} // namespace bk