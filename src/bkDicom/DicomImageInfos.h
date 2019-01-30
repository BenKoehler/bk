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

#pragma once

#ifndef BKDICOM_DICOMIMAGEINFOS_H
#define BKDICOM_DICOMIMAGEINFOS_H

#include <string>

#include <bkDicom/lib/bkDicom_export.h>
#include <bk/Matrix>

namespace bk
{
  class BKDICOM_EXPORT DicomImageInfos
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DicomImageInfos;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    public:
      int id_file_start;
      int id_file_end;
      int nDimensions;
      int Rows;
      int Columns;
      int Slices;
      int TemporalPositions; // also: CardiacNumberOfImages
      int NumberOfFrames;
      double RowSpacing;
      double ColSpacing;
      double SliceSpacing;
      double TemporalResolution;
      std::string PatientName;
      std::string PatientID;
      std::string PatientSex;
      int PatientAge;
      double PatientWeight;
      std::string PatientBirthDate;
      std::string SequenceName;
      std::string SequenceName_Private;
      std::string PatientPosition;
      std::string StudyDescription;
      std::string SeriesDescription;
      std::string SeriesInstanceUID;
      std::string StudyInstanceUID;
      std::string ProtocolName;
      std::string Modality;
      int SamplesPerPixel;
      int LargestImagePixelValue;
      int BitsAllocated;
      int BitsStored;
      int HighBit;
      std::string AcquisitionDate;
      std::string InstitutionName;
      Vec3d ImageOrientationPatientX;
      Vec3d ImageOrientationPatientY;
      Mat4d worldMatrix;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      DicomImageInfos();
      DicomImageInfos(const self_type&);
      DicomImageInfos(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DicomImageInfos();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(const std::string& filepath) const;
      void save(std::ofstream& file) const;
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(const std::string& filepath);
      void load(std::ifstream& file);
      /// @}
  }; // class DicomImageInfos
} // namespace bk

#endif //BKDICOM_DICOMIMAGEINFOS_H


