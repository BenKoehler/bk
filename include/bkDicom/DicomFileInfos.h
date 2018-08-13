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

#ifndef BKDICOM_DICOMFILEINFOS_H
#define BKDICOM_DICOMFILEINFOS_H

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>

#include "../bkMath/Matrix"

#include "cmake-build-debug/include/bk/dicom/lib/export.h"

namespace bk
{
  class BK_DICOM_EXPORT DicomFileInfos
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DicomFileInfos;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    public:
      std::string filename;
      std::uint32_t InstanceNumber;
      std::string StudyInstanceUID;
      std::string SeriesInstanceUID;
      std::string SequenceName;
      std::string ProtocolName;
      double SliceLocation;
      double AcquisitionTime;
      Vec3d ImagePositionPatient;
      std::string StudyDescription;
      std::string SeriesDescription;
      std::string ImageType;

      //====================================================================================================
      //===== CTOR & DTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      DicomFileInfos();
      DicomFileInfos(const self_type&);
      DicomFileInfos(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DicomFileInfos();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(std::string_view filepath) const;
      void save(std::ofstream& file) const;
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filepath);
      void load(std::ifstream& file);
      /// @}
  }; // class DicomFileInfos
} // namespace bk

#endif //BKDICOM_DICOMFILEINFOS_H


