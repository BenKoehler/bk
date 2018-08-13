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

#ifndef BKDATASET_DICOMTRANSFORMATIONBASE_H
#define BKDATASET_DICOMTRANSFORMATIONBASE_H

#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../bkMath/Matrix"
#include "../../bkTypeTraits/has_index_operator.h"

#include "../lib/export.h"
#include "EDicomImageType.h"

namespace bk
{
  class BK_DATASET_EXPORT DicomTransformation
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DicomTransformation;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      DicomImageType _image_type;
      //
      Mat5d _world_matrix;
      Mat5d _inverse_world_matrix;
      // extracted information
      Vec4d _scale;
      Vec4d _translation;
      Mat4d _rot_shear_matrix;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      DicomTransformation();
      DicomTransformation(const self_type&);
      DicomTransformation(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DicomTransformation();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DICOM IMAGE TYPE
      [[nodiscard]] DicomImageType dicom_image_type() const;
      [[nodiscard]] bool dicom_image_type_is_2d() const;
      [[nodiscard]] bool dicom_image_type_is_2dt() const;
      [[nodiscard]] bool dicom_image_type_is_3d() const;
      [[nodiscard]] bool dicom_image_type_is_3dt() const;
      /// @}

      /// @{ -------------------------------------------------- GET (INVERSE) WORLD MATRIX WITH TIME
      [[nodiscard]] const Mat5d& world_matrix_with_time() const;
      [[nodiscard]] const Mat5d& inverse_world_matrix_with_time() const;
      /// @}

      /// @{ -------------------------------------------------- GET (INVERSE) WORLD MATRIX
      [[nodiscard]] Mat4d world_matrix() const;
      [[nodiscard]] Mat4d inverse_world_matrix() const;
      /// @}

      /// @{ -------------------------------------------------- GET SCALE
      [[nodiscard]] const Vec4d& scale() const;
      [[nodiscard]] double scale(unsigned int dimid) const;
      /// @}

      /// @{ -------------------------------------------------- GET TRANSLATION
      [[nodiscard]] const Vec4d& translation() const;
      [[nodiscard]] double translation(unsigned int dimid) const;
      /// @}

      /// @{ -------------------------------------------------- GET ROTATION/SHEAR MATRIX
      [[nodiscard]] const Mat4d& rotation_shear_matrix() const;
      /// @}

      /// @{ -------------------------------------------------- GET TEMPORAL RESOLUTION
      [[nodiscard]] double temporal_resolution() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
    protected:
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
    public:
      /// @}

      /// @{ -------------------------------------------------- SET DICOM IMAGE TYPE
      void set_dicom_image_type(DicomImageType type);
      void set_dicom_image_type_2d();
      void set_dicom_image_type_2dt();
      void set_dicom_image_type_3d();
      void set_dicom_image_type_3dt();
      /// @}

      /// @{ -------------------------------------------------- SET WORLD MATRIX
      void set_world_matrix(const Mat5d& w);
      void set_world_matrix(const Mat4d& w, double temporal_resolution = 1.0);
      /// @}

      /// @{ -------------------------------------------------- SET TEMPORAL RESOLUTION
      void set_temporal_resolution(double dt);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- EXTRACT INFORMATION
    private:
      void _extract_transformation_components();
      /// @}

      /// @{ -------------------------------------------------- HELPER: HOMOGENEOUS COORDINATES
    private:
      [[nodiscard]] ColVec5d _to_homogeneous_coordinates(const Vec4d& vec) const;
      void _from_homogeneous_coordinates(const ColVec5d& v, Vec4d& vec) const;
      /// @}

      /// @{ -------------------------------------------------- TO WORLD COORDINATES
    protected:
      void to_world_coordinates_impl(Vec4d& vec) const;
    public:

      [[nodiscard]] MatXd to_world_coordinates(double x0, double x1, double x2 = 0.0, double x3 = 0.0) const;

      template<typename TIteratableClass>
      [[nodiscard]] MatXd to_world_coordinates(const TIteratableClass& vec) const
      {
          std::array<double, 4> temp{{0, 0, 0, 0}};
          auto ittemp = temp.begin();

          for (auto it = std::begin(vec); it != std::end(vec); ++it, ++ittemp)
          { *ittemp = *it; }

          return to_world_coordinates(temp[0], temp[1], temp[2], temp[3]);
      }

      template<typename T>
      [[nodiscard]] MatXd to_world_coordinates(std::initializer_list<T> ilist) const
      {
          std::array<double, 4> temp{{0, 0, 0, 0}};
          auto ittemp = temp.begin();

          for (auto it = std::begin(ilist); it != std::end(ilist); ++it, ++ittemp)
          { *ittemp = *it; }

          return to_world_coordinates(temp[0], temp[1], temp[2], temp[3]);
      }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
    protected:
      void to_object_coordinates_impl(Vec4d& vec) const;
    public:

      [[nodiscard]] MatXd to_object_coordinates(double x0, double x1, double x2, double x3 = 0.0) const;

      template<typename T>
      [[nodiscard]] MatXd to_object_coordinates(const T& vec) const
      {
          std::array<double, 4> temp{{0, 0, 0, 0}};
          auto ittemp = temp.begin();

          for (auto it = std::begin(vec); it != std::end(vec); ++it, ++ittemp)
          { *ittemp = *it; }

          return to_object_coordinates(temp[0], temp[1], temp[2], temp[3]);
      }

      template<typename T>
      [[nodiscard]] MatXd to_object_coordinates(std::initializer_list<T> ilist) const
      {
          std::array<double, 4> temp{{0, 0, 0, 0}};
          auto ittemp = temp.begin();

          for (auto it = std::begin(ilist); it != std::end(ilist); ++it, ++ittemp)
          { *ittemp = *it; }

          return to_object_coordinates(temp[0], temp[1], temp[2], temp[3]);
      }
      /// @}
  }; // class DicomTransformation
} // namespace bk

#endif //BKDATASET_DICOMTRANSFORMATIONBASE_H
