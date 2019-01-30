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

#include <bkDicom/DicomFileInfos.h>

#include <cinttypes>

#include <bk/StringUtils>

namespace bk
{
  //====================================================================================================
  //===== CTOR & DTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DicomFileInfos::DicomFileInfos()
      : filename(""),
        InstanceNumber(0),
        StudyInstanceUID(""),
        SeriesInstanceUID(""),
        SequenceName(""),
        ProtocolName(""),
        SliceLocation(0),
        AcquisitionTime(0),
        ImagePositionPatient(0, 0, 0),
        StudyDescription(""),
        SeriesDescription(""),
        ImageType("")
  { /* do nothing */ }

  DicomFileInfos::DicomFileInfos(const self_type&) = default;
  DicomFileInfos::DicomFileInfos(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DicomFileInfos::~DicomFileInfos() = default;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto DicomFileInfos::operator=(const self_type&) -> self_type& = default;
  auto DicomFileInfos::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- SAVE
  bool DicomFileInfos::save(std::string_view filepath) const
  {
      std::ofstream file(filepath.data(), std::ios_base::binary | std::ios_base::out);

      if (file.good())
      {
          save(file);
          file.close();
          return true;
      }

      return false;
  }

  void DicomFileInfos::save(std::ofstream& file) const
  {
      string_utils::write_string_to_binary_file(filename, file);
      file.write(reinterpret_cast<const char*>(&InstanceNumber), sizeof(std::uint32_t));
      string_utils::write_string_to_binary_file(StudyInstanceUID, file);
      string_utils::write_string_to_binary_file(SeriesInstanceUID, file);
      string_utils::write_string_to_binary_file(SequenceName, file);
      string_utils::write_string_to_binary_file(ProtocolName, file);
      file.write(reinterpret_cast<const char*>(&SliceLocation), sizeof(double));
      file.write(reinterpret_cast<const char*>(&AcquisitionTime), sizeof(double));
      file.write(reinterpret_cast<const char*>(&ImagePositionPatient[0]), sizeof(double));
      file.write(reinterpret_cast<const char*>(&ImagePositionPatient[1]), sizeof(double));
      file.write(reinterpret_cast<const char*>(&ImagePositionPatient[2]), sizeof(double));

      //string_utils::write_string_to_binary_file(StudyDescription, file);
      //string_utils::write_string_to_binary_file(SeriesDescription, file);
      //string_utils::write_string_to_binary_file(ImageType, file);
  }
  /// @}

  /// @{ -------------------------------------------------- LOAD
  bool DicomFileInfos::load(std::string_view filepath)
  {
      std::ifstream file(filepath.data(), std::ios_base::binary | std::ios_base::in);

      if (file.good())
      {
          load(file);
          file.close();
          return true;
      }

      return false;
  }

  void DicomFileInfos::load(std::ifstream& file)
  {
      filename = string_utils::read_string_from_binary_file(file);
      file.read(reinterpret_cast<char*>(&InstanceNumber), sizeof(std::uint32_t));
      StudyInstanceUID = string_utils::read_string_from_binary_file(file);
      SeriesInstanceUID = string_utils::read_string_from_binary_file(file);
      SequenceName = string_utils::read_string_from_binary_file(file);
      ProtocolName = string_utils::read_string_from_binary_file(file);
      file.read(reinterpret_cast<char*>(&SliceLocation), sizeof(double));
      file.read(reinterpret_cast<char*>(&AcquisitionTime), sizeof(double));
      file.read(reinterpret_cast<char*>(&ImagePositionPatient[0]), sizeof(double));
      file.read(reinterpret_cast<char*>(&ImagePositionPatient[1]), sizeof(double));
      file.read(reinterpret_cast<char*>(&ImagePositionPatient[2]), sizeof(double));

      //StudyDescription = string_utils::read_string_from_binary_file(file);
      //SeriesDescription = string_utils::read_string_from_binary_file(file);
      //ImageType = string_utils::read_string_from_binary_file(file);
  }
  /// @}
} // namespace bk


