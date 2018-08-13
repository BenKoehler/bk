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

#include <cinttypes>
#include <fstream>

#include "../../include/bkTools/StringUtils"

#include "bkDicom/DicomImageInfos.h"

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DicomImageInfos::DicomImageInfos()
      : id_file_start(0),
        id_file_end(0),
        nDimensions(0),
        Rows(0),
        Columns(0),
        Slices(0),
        TemporalPositions(0),
        NumberOfFrames(0),
        RowSpacing(0),
        ColSpacing(0),
        SliceSpacing(0),
        TemporalResolution(0),
        PatientName(""),
        PatientID(""),
        PatientSex(""),
        PatientAge(0),
        PatientWeight(0),
        PatientBirthDate(""),
        SequenceName(""),
        SequenceName_Private(""),
        PatientPosition(""),
        StudyDescription(""),
        SeriesDescription(""),
        SeriesInstanceUID(""),
        StudyInstanceUID(""),
        ProtocolName(""),
        Modality(""),
        SamplesPerPixel(0),
        LargestImagePixelValue(0),
        BitsAllocated(0),
        BitsStored(0),
        HighBit(-1),
        AcquisitionDate(""),
        InstitutionName(""),
        ImageOrientationPatientX(MatrixFactory::Zero_Vec_3D<double>()),
        ImageOrientationPatientY(MatrixFactory::Zero_Vec_3D<double>()),
        worldMatrix(MatrixFactory::Zero_Mat_4D<double>())
  { /* do nothing */ }

  DicomImageInfos::DicomImageInfos(const self_type&) = default;
  DicomImageInfos::DicomImageInfos(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DicomImageInfos::~DicomImageInfos() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DicomImageInfos::operator=(const self_type&) -> self_type& = default;
  auto DicomImageInfos::operator=(self_type&&) -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- SAVE
  bool DicomImageInfos::save(const std::string& filepath) const
  {
      std::ofstream file(filepath, std::ios_base::binary | std::ios_base::out);

      if (file.good())
      {
          save(file);
          file.close();
          return true;
      }

      return false;
  }

  void DicomImageInfos::save(std::ofstream& file) const
  {
      using size_type = std::uint16_t;

      file.write(reinterpret_cast<const char*>(&id_file_start), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&id_file_end), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&nDimensions), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&Rows), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&Columns), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&Slices), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&TemporalPositions), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&NumberOfFrames), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&RowSpacing), sizeof(double));
      file.write(reinterpret_cast<const char*>(&ColSpacing), sizeof(double));
      file.write(reinterpret_cast<const char*>(&SliceSpacing), sizeof(double));
      file.write(reinterpret_cast<const char*>(&TemporalResolution), sizeof(double));
      string_utils::write_string_to_binary_file(PatientName, file);
      string_utils::write_string_to_binary_file(PatientID, file);
      string_utils::write_string_to_binary_file(PatientSex, file);
      file.write(reinterpret_cast<const char*>(&PatientAge), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&PatientWeight), sizeof(double));
      string_utils::write_string_to_binary_file(PatientBirthDate, file);
      string_utils::write_string_to_binary_file(SequenceName, file);
      string_utils::write_string_to_binary_file(SequenceName_Private, file);
      string_utils::write_string_to_binary_file(PatientPosition, file);
      string_utils::write_string_to_binary_file(StudyDescription, file);
      string_utils::write_string_to_binary_file(SeriesDescription, file);
      string_utils::write_string_to_binary_file(SeriesInstanceUID, file);
      string_utils::write_string_to_binary_file(StudyInstanceUID, file);
      string_utils::write_string_to_binary_file(ProtocolName, file);
      string_utils::write_string_to_binary_file(Modality, file);
      file.write(reinterpret_cast<const char*>(&SamplesPerPixel), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&LargestImagePixelValue), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&BitsAllocated), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&BitsStored), sizeof(size_type));
      file.write(reinterpret_cast<const char*>(&HighBit), sizeof(size_type));
      string_utils::write_string_to_binary_file(AcquisitionDate, file);
      string_utils::write_string_to_binary_file(InstitutionName, file);
      for (unsigned int i = 0; i < 3; ++i)
      { file.write(reinterpret_cast<const char*>(&ImageOrientationPatientX[i]), sizeof(double)); }
      for (unsigned int i = 0; i < 3; ++i)
      { file.write(reinterpret_cast<const char*>(&ImageOrientationPatientY[i]), sizeof(double)); }
      for (unsigned int i = 0; i < 16; ++i)
      { file.write(reinterpret_cast<const char*>(&worldMatrix[i]), sizeof(double)); }
  }
  /// @}

  /// @{ -------------------------------------------------- LOAD
  bool DicomImageInfos::load(const std::string& filepath)
  {
      std::ifstream file(filepath, std::ios_base::binary | std::ios_base::in);

      if (file.good())
      {
          load(file);
          file.close();
          return true;
      }

      return false;
  }

  void DicomImageInfos::load(std::ifstream& file)
  {
      using size_type = std::uint16_t;

      file.read(reinterpret_cast<char*>(&id_file_start), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&id_file_end), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&nDimensions), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&Rows), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&Columns), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&Slices), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&TemporalPositions), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&NumberOfFrames), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&RowSpacing), sizeof(double));
      file.read(reinterpret_cast<char*>(&ColSpacing), sizeof(double));
      file.read(reinterpret_cast<char*>(&SliceSpacing), sizeof(double));
      file.read(reinterpret_cast<char*>(&TemporalResolution), sizeof(double));
      PatientName = string_utils::read_string_from_binary_file(file);
      PatientID = string_utils::read_string_from_binary_file(file);
      PatientSex = string_utils::read_string_from_binary_file(file);
      file.read(reinterpret_cast<char*>(&PatientAge), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&PatientWeight), sizeof(double));
      PatientBirthDate = string_utils::read_string_from_binary_file(file);
      SequenceName = string_utils::read_string_from_binary_file(file);
      SequenceName_Private = string_utils::read_string_from_binary_file(file);
      PatientPosition = string_utils::read_string_from_binary_file(file);
      StudyDescription = string_utils::read_string_from_binary_file(file);
      SeriesDescription = string_utils::read_string_from_binary_file(file);
      SeriesInstanceUID = string_utils::read_string_from_binary_file(file);
      StudyInstanceUID = string_utils::read_string_from_binary_file(file);
      ProtocolName = string_utils::read_string_from_binary_file(file);
      Modality = string_utils::read_string_from_binary_file(file);
      file.read(reinterpret_cast<char*>(&SamplesPerPixel), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&LargestImagePixelValue), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&BitsAllocated), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&BitsStored), sizeof(size_type));
      file.read(reinterpret_cast<char*>(&HighBit), sizeof(size_type));
      AcquisitionDate = string_utils::read_string_from_binary_file(file);
      InstitutionName = string_utils::read_string_from_binary_file(file);
      for (unsigned int i = 0; i < 3; ++i)
      { file.read(reinterpret_cast<char*>(&ImageOrientationPatientX[i]), sizeof(double)); }
      for (unsigned int i = 0; i < 3; ++i)
      { file.read(reinterpret_cast<char*>(&ImageOrientationPatientY[i]), sizeof(double)); }
      for (unsigned int i = 0; i < 16; ++i)
      { file.read(reinterpret_cast<char*>(&worldMatrix[i]), sizeof(double)); }
  }
  /// @}
} // namespace bk


