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

#ifndef BKDICOM_DICOMDIRIMPORTER_H
#define BKDICOM_DICOMDIRIMPORTER_H

#include <cmath>
#include <fstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <gdcmDirectory.h>

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bkDicom/lib/bkDicom_export.h>
#include <bk/Matrix>
#include <bk/BitVector>

namespace bk
{
  // -------------------- forward declaration
  class DicomImageInfos;
  // -------------------- forward declaration END

  class BKDICOM_EXPORT DicomDirImporter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DicomDirImporter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      DicomDirImporter();
      DicomDirImporter(const self_type& other);
      DicomDirImporter(self_type&& other) noexcept;
      DicomDirImporter(const std::string& dir);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DicomDirImporter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DIRECTORY
      [[nodiscard]] const std::string& directory() const;
      /// @}

      /// @{ -------------------------------------------------- GET DATASET NAME
      [[nodiscard]] const std::string& dataset_name() const;
      /// @}

      /// @{ -------------------------------------------------- GET NUM IMAGES
      [[nodiscard]] unsigned int num_images() const;
      /// @}

      /// @{ -------------------------------------------------- GET ALL IMAGE IDS
      [[nodiscard]] std::vector<unsigned int> all_image_ids() const;
      /// @}

      /// @{ -------------------------------------------------- GET IMAGE INFOS
      //! Mainly a collection of Dicom Tags
      [[nodiscard]] DicomImageInfos& image_infos(unsigned int id);
      [[nodiscard]] const DicomImageInfos& image_infos(unsigned int id) const;
      /// @}

      /// @{ -------------------------------------------------- GET NUM IMAGES IN GROUP
      //! images are grouped by image grid sizes
      [[nodiscard]] unsigned int num_image_3d_plus_time_groups() const;
      [[nodiscard]] unsigned int num_image_3d_groups() const;
      [[nodiscard]] unsigned int num_image_2d_plus_time_groups() const;
      [[nodiscard]] unsigned int num_image_2d_groups() const;
      /// @}

      /// @{ -------------------------------------------------- GET GROUPS
      [[nodiscard]] const std::vector<unsigned int>& image_3d_plus_time_group(unsigned int id) const;
      [[nodiscard]] const std::vector<unsigned int>& image_3d_group(unsigned int id) const;
      [[nodiscard]] const std::vector<unsigned int>& image_2d_plus_time_group(unsigned int id) const;
      [[nodiscard]] const std::vector<unsigned int>& image_2d_group(unsigned int id) const;
      /// @}

      /// @{ -------------------------------------------------- GET GRID SIZE OF GROUP
      [[nodiscard]] const Vec4ui& image_3d_plus_time_group_grid_size(unsigned int id) const;
      [[nodiscard]] const Vec3ui& image_3d_group_grid_size(unsigned int id) const;
      [[nodiscard]] const Vec3ui& image_2d_plus_time_group_grid_size(unsigned int id) const;
      [[nodiscard]] const Vec2ui& image_2d_group_grid_size(unsigned int id) const;
      /// @}

      /// @{ -------------------------------------------------- GET SUCCESS
      [[nodiscard]] bool is_import_successful() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET DIRECTORY
      void set_directory(std::string_view dir);
      /// @}

      /// @{ -------------------------------------------------- SET DATASET NAME
      void set_dataset_name(std::string_view name);
      void set_dataset_name_from_patient_name();
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear();
    protected:
      virtual void clear_impl();
    public:
      /// @}

      /// @{ -------------------------------------------------- IMPORT
      [[maybe_unused]] bool import();
      /// @}

      /// @{ -------------------------------------------------- SCAN DIRECTORY
    private:
      void scan_directory();
    public:
      /// @}

      /// @{ -------------------------------------------------- SCAN DIRECTORY (CLASSIC; ALL FILES)
    private:
      [[maybe_unused]] unsigned int _scandir_classic_process_files(const gdcm::Directory::FilenamesType& dcmfilenames);
      void _scandir_classic_sort_files_to_separate_single_images();
      void _scandir_classic_sort_images_by_dicomtags();
      [[maybe_unused]] bool scan_directory_classic_all_files(const gdcm::Directory::FilenamesType& dcmfilenames);
    public:
      /// @}

      /// @{ -------------------------------------------------- SCAN DIRECTORY (DICOMDIR)
    private:
      void _scandir_DICOMDIR_sort_images_by_dicomtags();
      [[maybe_unused]] bool scan_directory_DICOMDIR(const gdcm::Directory::FilenamesType& dcmfilenames, unsigned int DICOMDIR_id);
    public:
      /// @}

      /// @{ -------------------------------------------------- SCAN IMAGE INFOS
      void scan_image_infos();
    private:
      void scan_image_dimensions();
    public:
      /// @}

      /// @{ -------------------------------------------------- PRINT IMAGE INFOS
      [[maybe_unused]] std::ostream& print_image_infos(unsigned int id, std::ostream& s) const;
      [[maybe_unused]] std::ostream& print_image_infos(unsigned int id) const;
      /// @}

      /// @{ -------------------------------------------------- HELPERS: READ
    private:
      [[nodiscard]] int _count_image_dimensions(const DicomImageInfos& imgInfo) const;
      void _check_from_to_dimenson(unsigned int& xfrom, unsigned int& xto, unsigned int& yfrom, unsigned int& yto, unsigned int& zfrom, unsigned int& zto, unsigned int& tfrom, unsigned int& tto) const;
      [[nodiscard]] std::tuple<int,bool,bool,bool,bool> _count_expected_image_dimensions(unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const;
      void _setup_image(const DicomImageInfos& imgInfo, std::unique_ptr<DicomImage<double, -1>>& img, bool /*has_x*/, bool /*has_y*/, bool has_z, bool has_t) const;
      void _set_image_val(std::unique_ptr<DicomImage<double, -1>>& img, unsigned int val, unsigned int nDim, bool has_x, bool has_y, bool has_z, bool has_t, unsigned int rowid, unsigned int colid, unsigned int slicePos, unsigned int temporalPos) const;
      [[nodiscard]] unsigned int _get_value_from_raw_data(const char* valdata, int nBytesPerPixel, bool littleEndian, const DicomImageInfos& imgInfo) const;
    public:
      /// @}

      /// @{ -------------------------------------------------- READ DICOM IMAGE
      [[nodiscard]] std::unique_ptr <DicomImage<double, -1>> read_slice_of_4d_image(unsigned int image_id, unsigned int z_id, unsigned int t_id) const;
      [[nodiscard]] std::unique_ptr <DicomImage<double, -1>> read_image(unsigned int id) const;
      [[nodiscard]] std::unique_ptr <DicomImage<double, -1>> read_image_block(unsigned int id, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom = 0, unsigned int zto = 0, unsigned int tfrom = 0, unsigned int tto = 0) const;
      /// @}

      /// @{ -------------------------------------------------- READ DICOM IMAGE BYTES
      [[nodiscard]] std::vector<char> read_image_bytes(unsigned int id) const;
      [[nodiscard]] std::vector<char> read_image_block_bytes(unsigned int id, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom = 0, unsigned int zto = 0, unsigned int tfrom = 0, unsigned int tto = 0) const;
      [[nodiscard]] std::unique_ptr <DicomImage<double, -1>> read_image_from_bytes(unsigned int id, const std::vector<char>& imgbytes) const;
      [[nodiscard]] std::unique_ptr <DicomImage<double, -1>> read_image_block_from_bytes(unsigned int id, const std::vector<char>& imgbytes, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom = 0, unsigned int zto = 0, unsigned int tfrom = 0, unsigned int tto = 0) const;
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(std::string_view filepath) const;
    protected:
      virtual void save_impl(std::ofstream& file) const;
    public:
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filepath);
    protected:
      virtual void load_impl(std::ifstream& file);
    public:
      /// @}
  }; // class DicomDirImporter
} // namespace bk

#endif //BKDICOM_DICOMDIRIMPORTER_H