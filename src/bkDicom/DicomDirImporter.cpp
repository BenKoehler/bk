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

#include <bkDicom/DicomDirImporter.h>

#include <algorithm>
#include <cassert>
#include <cinttypes>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmFile.h>
#include <gdcmFileMetaInformation.h>
#include <gdcmItem.h>
#include <gdcmTagKeywords.h>
#include <gdcmMediaStorage.h>
#include <gdcmStringFilter.h>

#include <bk/BitVector>
#include <bk/Image>
#include <bk/StringUtils>
#include <bkMath/functions/list_grid_id_conversion.h>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

#include <bkDicom/DicomFileInfos.h>
#include <bkDicom/DicomImageInfos.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct DicomDirImporter::Impl
  {
      std::string dir;
      std::string dataset_name;
      std::vector<DicomFileInfos> files;
      std::vector<DicomImageInfos> info;
      std::vector<std::pair<Vec2ui, std::vector<unsigned int>>> images_2d; //!< pair = <grid size, list of image ids>
      std::vector<std::pair<Vec3ui, std::vector<unsigned int>>> images_2dt; //!< pair = <grid size, list of image ids>
      std::vector<std::pair<Vec3ui, std::vector<unsigned int>>> images_3d; //!< pair = <grid size, list of image ids>
      std::vector<std::pair<Vec4ui, std::vector<unsigned int>>> images_3dt; //!< pair = <grid size, list of image ids>
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DicomDirImporter::DicomDirImporter() = default;
  DicomDirImporter::DicomDirImporter(const self_type&) = default;
  DicomDirImporter::DicomDirImporter(self_type&&) noexcept = default;

  DicomDirImporter::DicomDirImporter(const std::string& dir)
  {
      set_directory(dir);
      import();
  }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DicomDirImporter::~DicomDirImporter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET DIRECTORY
  const std::string& DicomDirImporter::directory() const
  { return _pdata->dir; }
  /// @}

  /// @{ -------------------------------------------------- GET DATASET NAME
  const std::string& DicomDirImporter::dataset_name() const
  { return _pdata->dataset_name; }
  /// @}

  /// @{ -------------------------------------------------- GET NUM IMAGES
  unsigned int DicomDirImporter::num_images() const
  { return _pdata->info.size(); }
  /// @}

  /// @{ -------------------------------------------------- GET ALL IMAGE IDS
  std::vector<unsigned int> DicomDirImporter::all_image_ids() const
  {
      std::vector<unsigned int> ids;

      const auto insert_images = [&](const std::vector<unsigned int>& newIds)
      {
          for (unsigned int id: newIds)
          { ids.push_back(id); }
      };

      for (const std::pair<Vec2ui, std::vector<unsigned int>>& p: _pdata->images_2d)
      { insert_images(p.second); }

      for (const std::pair<Vec3ui, std::vector<unsigned int>>& p: _pdata->images_2dt)
      { insert_images(p.second); }

      for (const std::pair<Vec3ui, std::vector<unsigned int>>& p: _pdata->images_3d)
      { insert_images(p.second); }

      for (const std::pair<Vec4ui, std::vector<unsigned int>>& p: _pdata->images_3dt)
      { insert_images(p.second); }

      return ids;
  }
  /// @}

  /// @{ -------------------------------------------------- GET IMAGE INFOS
  DicomImageInfos& DicomDirImporter::image_infos(unsigned int id)
  {
      assert(id < num_images());
      return _pdata->info[id];
  }

  const DicomImageInfos& DicomDirImporter::image_infos(unsigned int id) const
  {
      assert(id < num_images());
      return _pdata->info[id];
  }
  /// @}

  /// @{ -------------------------------------------------- GET NUM IMAGES IN GROUP
  unsigned int DicomDirImporter::num_image_3d_plus_time_groups() const
  { return _pdata->images_3dt.size(); }

  unsigned int DicomDirImporter::num_image_3d_groups() const
  { return _pdata->images_3d.size(); }

  unsigned int DicomDirImporter::num_image_2d_plus_time_groups() const
  { return _pdata->images_2dt.size(); }

  unsigned int DicomDirImporter::num_image_2d_groups() const
  { return _pdata->images_2d.size(); }
  /// @}

  /// @{ -------------------------------------------------- GET GROUPS
  const std::vector<unsigned int>& DicomDirImporter::image_3d_plus_time_group(unsigned int id) const
  {
      assert(id < num_image_3d_plus_time_groups());
      return _pdata->images_3dt[id].second;
  }

  const std::vector<unsigned int>& DicomDirImporter::image_3d_group(unsigned int id) const
  {
      assert(id < num_image_3d_groups());
      return _pdata->images_3d[id].second;
  }

  const std::vector<unsigned int>& DicomDirImporter::image_2d_plus_time_group(unsigned int id) const
  {
      assert(id < num_image_2d_plus_time_groups());
      return _pdata->images_2dt[id].second;
  }

  const std::vector<unsigned int>& DicomDirImporter::image_2d_group(unsigned int id) const
  {
      assert(id < num_image_2d_groups());
      return _pdata->images_2d[id].second;
  }
  /// @}

  /// @{ -------------------------------------------------- GET GRID SIZE OF GROUP
  const Vec4ui& DicomDirImporter::image_3d_plus_time_group_grid_size(unsigned int id) const
  {
      assert(id < num_image_3d_plus_time_groups());
      return _pdata->images_3dt[id].first;
  }

  const Vec3ui& DicomDirImporter::image_3d_group_grid_size(unsigned int id) const
  {
      assert(id < num_image_3d_groups());
      return _pdata->images_3d[id].first;
  }

  const Vec3ui& DicomDirImporter::image_2d_plus_time_group_grid_size(unsigned int id) const
  {
      assert(id < num_image_2d_plus_time_groups());
      return _pdata->images_2dt[id].first;
  }

  const Vec2ui& DicomDirImporter::image_2d_group_grid_size(unsigned int id) const
  {
      assert(id < num_image_2d_groups());
      return _pdata->images_2d[id].first;
  }
  /// @}

  /// @{ -------------------------------------------------- GET SUCCESS
  bool DicomDirImporter::is_import_successful() const
  { return !_pdata->files.empty() && !_pdata->info.empty(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET DIRECTORY
  void DicomDirImporter::set_directory(std::string_view dir)
  {
      /*
       * replace dicom directory
       */
      _pdata->dir = dir;
      _pdata->dir = bk::string_utils::replace(_pdata->dir, "\\", "/");

      if (!bk::string_utils::ends_with(_pdata->dir, "/"))
      { _pdata->dir = bk::string_utils::append(_pdata->dir, "/"); }

      /*
       * replace dicom directory in list of raw files
       *    - "filename" contains the filepath
       */
      #pragma omp parallel for
      for (unsigned int i = 0; i < _pdata->files.size(); ++i)
      {
          DicomFileInfos& fileinfo = _pdata->files[i];

          if (fileinfo.filename.empty())
          { continue; }

          int index_of_last_slash = static_cast<int>(fileinfo.filename.size()) - 1;

          for (; index_of_last_slash >= 0; --index_of_last_slash)
          {
              if (fileinfo.filename[index_of_last_slash] == '/')
              { break; }
          }

          fileinfo.filename = bk::string_utils::replace(fileinfo.filename, 0, index_of_last_slash, _pdata->dir);
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET DATASET NAME
  void DicomDirImporter::set_dataset_name(std::string_view name)
  {
      if (!name.empty())
      { _pdata->dataset_name = name; }
  }

  void DicomDirImporter::set_dataset_name_from_patient_name()
  {
      for (unsigned int i = 0; i < num_images(); ++i)
      {
          if (!_pdata->info[i].PatientName.empty())
          {
              set_dataset_name(_pdata->info[i].PatientName);
              break;
          }
      }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto DicomDirImporter::operator=(const self_type& other) -> self_type& = default;
  auto DicomDirImporter::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void DicomDirImporter::clear()
  {
      //_dir = "";
      /*
       * Clear is called first in the import function.
       * Thus, resetting the directory is not a good idea here
       */

      _pdata->files.clear();
      _pdata->info.clear();
      _pdata->dataset_name.clear();
      _pdata->images_2d.clear();
      _pdata->images_2dt.clear();
      _pdata->images_3d.clear();
      _pdata->images_3dt.clear();
      clear_impl();
  }

  void DicomDirImporter::clear_impl()
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- IMPORT
  bool DicomDirImporter::import()
  {
      clear();

      if (!_pdata->dir.empty())
      {
          scan_directory();

          if (!_pdata->files.empty() && !_pdata->info.empty())
          {
              remove_duplicate_images();
              scan_image_infos();
              scan_image_dimensions();
              set_dataset_name_from_patient_name();

              return true;
          }

          return false;
      }

      return false;
  }
  /// @}

  /// @{ -------------------------------------------------- SCAN DIRECTORY
  void DicomDirImporter::scan_directory()
  {
      gdcm::Directory d;
      d.Load(_pdata->dir, /*recursive =*/true);
      gdcm::Directory::FilenamesType dcmfilenames = d.GetFilenames();

      std::sort(dcmfilenames.begin(), dcmfilenames.end());

      /*
       * is there a DICOMDIR file?
       */
      unsigned int DICOMDIR_id = 0;
      bool hasDICOMDIR = false;

      for (; DICOMDIR_id < dcmfilenames.size(); ++DICOMDIR_id)
      {
          if (bk::string_utils::ends_with(dcmfilenames[DICOMDIR_id], "/dicomdir", false))
          {
              hasDICOMDIR = true;
              break;
          }
      }

      if (!(hasDICOMDIR && scan_directory_DICOMDIR(dcmfilenames, DICOMDIR_id)))
      { scan_directory_classic_all_files(dcmfilenames); }
  }
  /// @}

  /// @{ -------------------------------------------------- SCAN DIRECTORY (CLASSIC; ALL FILES)
  unsigned int DicomDirImporter::_scandir_classic_process_files(const gdcm::Directory::FilenamesType& dcmfilenames)
  {
      _pdata->files.reserve(dcmfilenames.size());

      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog1 = bk_progress.emplace_task(dcmfilenames.size() - 1, ___("Scanning DICOM files"));
      #endif

      for (unsigned int i = 0; i < dcmfilenames.size(); ++i)
      {
          gdcm::Reader reader;
          reader.SetFileName(dcmfilenames[i].c_str());

          if (!reader.Read())
          {
              std::cerr << "DicomDirImporter : could not read file \"" << dcmfilenames[i].c_str() << "\"!" << std::endl;
              continue;
          }

          const gdcm::File& dcm = reader.GetFile();

          gdcm::StringFilter sf;
          sf.SetFile(dcm);

          DicomFileInfos info;
          info.filename = dcmfilenames[i];
          info.InstanceNumber = bk::string_utils::to_uint(sf.ToString(gdcm::Keywords::InstanceNumber::GetTag()));
          info.StudyInstanceUID = bk::string_utils::trim(sf.ToString(gdcm::Keywords::StudyInstanceUID::GetTag()));
          info.SeriesInstanceUID = bk::string_utils::trim(sf.ToString(gdcm::Keywords::SeriesInstanceUID::GetTag()));
          info.SequenceName = bk::string_utils::trim(sf.ToString(gdcm::Keywords::SequenceName::GetTag()));
          info.ProtocolName = bk::string_utils::trim(sf.ToString(gdcm::Keywords::ProtocolName::GetTag()));
          info.SliceLocation = bk::string_utils::to_double(sf.ToString(gdcm::Keywords::SliceLocation::GetTag()));
          // format : hhmmss.ssssss -> h: hour, m: minute, s: fraction second
          const std::string at = sf.ToString(gdcm::Keywords::AcquisitionTime::GetTag());
          info.AcquisitionTime = bk::string_utils::to_double(bk::string_utils::sub_string(at, 0, 2)) * 3'600'000 /*h -> ms*/;
          info.AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 2, 2)) * 60'000 /*h -> ms*/;
          info.AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 4, 9)) * 1'000 /*s -> ms*/;

          //info.AcquisitionTime = bk::string_utils::to_double(sf.ToString(gdcm::Keywords::AcquisitionTime::GetTag()));

          const gdcm::DataSet& ds = dcm.GetDataSet();
          const std::string ipp = sf.ToString(ds.GetDataElement(gdcm::Keywords::ImagePositionPatient::GetTag()));
          const auto vipp = bk::string_utils::contains(ipp, "\\") ? bk::string_utils::split(ipp, "\\") : bk::string_utils::split(ipp, "/");
          if (vipp.size() == 3)
          {
              info.ImagePositionPatient[0] = bk::string_utils::to_double(vipp[0]);
              info.ImagePositionPatient[1] = bk::string_utils::to_double(vipp[1]);
              info.ImagePositionPatient[2] = bk::string_utils::to_double(vipp[2]);
          }

          info.StudyDescription = bk::string_utils::trim(sf.ToString(gdcm::Keywords::StudyDescription::GetTag()));
          info.SeriesDescription = bk::string_utils::trim(sf.ToString(gdcm::Keywords::SeriesDescription::GetTag()));
          info.ImageType = bk::string_utils::trim(sf.ToString(gdcm::Keywords::ImageType::GetTag()));

          _pdata->files.push_back(std::move(info));

          #ifdef BK_EMIT_PROGRESS
          prog1.set_current(i);
          #endif
      } // for i = 0; i < dcmfilenames.size()

      _pdata->files.shrink_to_fit();

      #ifdef BK_EMIT_PROGRESS
      prog1.set_finished();
      #endif

      return _pdata->files.size();
  }

  void DicomDirImporter::_scandir_classic_sort_files_to_separate_single_images()
  {
      // stable sort is important here to preserve the original file order
      // in case that they were already sorted

      std::stable_sort(std::begin(_pdata->files), std::end(_pdata->files), [](const DicomFileInfos& a, const DicomFileInfos& b)
      {
          if (a.SeriesInstanceUID != b.SeriesInstanceUID)
          { return a.SeriesInstanceUID < b.SeriesInstanceUID; }
          else if (a.SequenceName != b.SequenceName)
          { return a.SequenceName < b.SequenceName; }
          else if (a.StudyDescription != b.StudyDescription)
          { return a.StudyDescription < b.StudyDescription; }
          else if (a.SeriesDescription != b.SeriesDescription)
          { return a.SeriesDescription < b.SeriesDescription; }
          else
          { return a.ImageType < b.ImageType; }
      });
  }

  void DicomDirImporter::_scandir_classic_sort_images_by_dicomtags()
  {
      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog2 = bk_progress.emplace_task(_pdata->files.size() - 1, ___("Sorting DICOM images"));
      #endif

      //------------------------------------------------------------------------------------------------------
      // sort each image by SliceLocation and AcquisitionTime
      //------------------------------------------------------------------------------------------------------
      std::string tempSeriesInstanceUID;
      std::string tempSequenceName;
      std::string tempStudyInstanceUID;
      std::string tempProtocolName;
      std::string tempStudyDescription;
      std::string tempSeriesDescription;
      std::string tempImageType;
      unsigned int i = 0;

      for (auto it = _pdata->files.begin(); it != _pdata->files.end(); /*++it, ++i*/ /*increment happens within loop*/)
      {
          auto itend = it;

          tempSeriesInstanceUID = it->SeriesInstanceUID;
          tempSequenceName = it->SequenceName;
          tempStudyInstanceUID = it->StudyInstanceUID;
          tempProtocolName = it->ProtocolName;
          tempStudyDescription = it->StudyDescription;
          tempSeriesDescription = it->SeriesDescription;
          tempImageType = it->ImageType;

          DicomImageInfos imgInfo;
          imgInfo.id_file_start = i;
          imgInfo.SeriesInstanceUID = tempSeriesInstanceUID;
          imgInfo.SequenceName = tempSequenceName;
          imgInfo.StudyInstanceUID = tempStudyInstanceUID;
          imgInfo.ProtocolName = tempProtocolName;

          std::vector<double> slicePositions;
          std::vector<double> acquisitionTimes;
          slicePositions.reserve(500);
          acquisitionTimes.reserve(500);

          bool hasSlices = false;
          bool hasTemporalPositions = false;
          bool hasNumberOfFrames = false;

          gdcm::Reader reader0;
          reader0.SetFileName(_pdata->files[imgInfo.id_file_start].filename.c_str());

          if (reader0.Read())
          {
              const gdcm::File& file = reader0.GetFile();
              const gdcm::DataSet& ds = file.GetDataSet();
              gdcm::StringFilter sf;
              sf.SetFile(file);

              /*
               * Slices
               */
              if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
              {
                  if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1018))) // philips private tag for slices
                  {
                      imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1018))));
                      hasSlices = true;
                  }
              }
              else
              {
                  if (ds.FindDataElement(gdcm::Keywords::NumberOfSlices::GetTag()))
                  {
                      imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfSlices::GetTag())));
                      hasSlices = true;
                  }
              }

              /*
               * Temporal Positions
               */
              if (ds.FindDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag()))
              {
                  imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag())));
                  hasTemporalPositions = true;
              }
              else if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
              {
                  if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1017))) // philips private tag for num times
                  {
                      imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1017))));
                      hasTemporalPositions = true;
                  }
              }
              else if (ds.FindDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag()))
              {
                  imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag())));
                  hasTemporalPositions = true;
              }

              /*
               * Number of Frames
               */
              if (ds.FindDataElement(gdcm::Keywords::NumberOfFrames::GetTag()))
              {
                  imgInfo.NumberOfFrames = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfFrames::GetTag())));
                  hasNumberOfFrames = true;
              }
          }

          while (itend != _pdata->files.end() && itend->SeriesInstanceUID == tempSeriesInstanceUID && itend->SequenceName == tempSequenceName && itend->StudyInstanceUID == tempStudyInstanceUID && itend->ProtocolName == tempProtocolName && itend->StudyDescription == tempStudyDescription && itend->SeriesDescription == tempSeriesDescription && itend->ImageType == tempImageType) // while same image
          {
              if (!hasSlices || !hasTemporalPositions)
              {
                  gdcm::Reader reader;
                  reader.SetFileName(_pdata->files[i].filename.c_str());
                  if (reader.Read())
                  {
                      const gdcm::File& file = reader.GetFile();
                      gdcm::StringFilter sf;
                      sf.SetFile(file);

                      if (!hasSlices)
                      {
                          //slicePositions.push_back(bk::string_utils::to_double(sf.ToString(gdcm::Keywords::SliceLocation::GetTag())));

                          const gdcm::DataSet& ds = file.GetDataSet();

                          if (ds.FindDataElement(gdcm::Keywords::SliceLocation::GetTag()))
                          { slicePositions.push_back(bk::string_utils::to_double(sf.ToString(gdcm::Keywords::SliceLocation::GetTag()))); }
                          else if (ds.FindDataElement(gdcm::Keywords::SpacingBetweenSlices::GetTag()) || ds.FindDataElement(gdcm::Keywords::SliceThickness::GetTag()))
                          { slicePositions.push_back(i); }
                      }

                      if (!hasTemporalPositions)
                      {
                          // format : hhmmss.ssssss -> h: hour, m: minute, s: fraction second
                          const std::string at = sf.ToString(gdcm::Keywords::AcquisitionTime::GetTag());
                          double acqt = bk::string_utils::to_double(bk::string_utils::sub_string(at, 0, 2)) * 3'600'000 /*h -> ms*/;
                          acqt += bk::string_utils::to_double(bk::string_utils::sub_string(at, 2, 2)) * 60'000 /*h -> ms*/;
                          acqt += bk::string_utils::to_double(bk::string_utils::sub_string(at, 4, 9)) * 1'000 /*s -> ms*/;

                          acquisitionTimes.push_back(acqt);
                      }
                  }
              }

              ++itend;
              ++i;
          } // while
          imgInfo.id_file_end = i;

          const unsigned int expectedNumFiles = imgInfo.id_file_end - imgInfo.id_file_start;

          if (!hasNumberOfFrames)
          {
              imgInfo.NumberOfFrames = expectedNumFiles;
              hasNumberOfFrames = true;
          }

          if (!hasSlices && !slicePositions.empty())
          {
              std::sort(slicePositions.begin(), slicePositions.end());
              slicePositions.erase(std::unique(slicePositions.begin(), slicePositions.end()), slicePositions.end());
              imgInfo.Slices = static_cast<int>(slicePositions.size());
              hasSlices = true;
          }

          if (!hasTemporalPositions)
          {
              if (hasSlices && hasNumberOfFrames && imgInfo.NumberOfFrames == imgInfo.Slices)
              {
                  imgInfo.TemporalPositions = 1;
                  hasTemporalPositions = true;
              }
              else if (!acquisitionTimes.empty())
              {
                  std::sort(acquisitionTimes.begin(), acquisitionTimes.end());
                  acquisitionTimes.erase(std::unique(acquisitionTimes.begin(), acquisitionTimes.end()), acquisitionTimes.end());
                  imgInfo.TemporalPositions = static_cast<int>(acquisitionTimes.size());
                  hasTemporalPositions = true;
              }
          }

          // const unsigned int expected2 = imgInfo.Slices * imgInfo.TemporalPositions;
          const unsigned int expected2 = std::max(imgInfo.Slices,1) * std::max(imgInfo.TemporalPositions,1);
          
          bool this_is_a_split_image = false;

          if (hasSlices && hasTemporalPositions && expectedNumFiles != expected2)
          {
              // there are more files than Slices * TemporalPositions
              // --> there must be multiple images in this range

              this_is_a_split_image = true;
          }

          if (!this_is_a_split_image)
          {
              std::sort(it, itend, [](const DicomFileInfos& a, const DicomFileInfos& b)
              {
                  return a.SliceLocation != b.SliceLocation ? a.SliceLocation < b.SliceLocation : a.AcquisitionTime != b.AcquisitionTime ? a.AcquisitionTime < b.AcquisitionTime : a.InstanceNumber < b.InstanceNumber;
              });

              _pdata->info.push_back(std::move(imgInfo));
          }
          else
          {
              const unsigned int nSplitImages = expectedNumFiles / expected2;

              for (unsigned int s = 0; s < nSplitImages; ++s)
              {
                  DicomImageInfos imgInfoSplit(imgInfo);
                  imgInfoSplit.id_file_start += s * expected2;
                  imgInfoSplit.id_file_end = imgInfoSplit.id_file_start + expected2;

                  auto tempit = it + s * expected2;
                  auto tempitend = it + (s + 1) * expected2;

                  std::sort(tempit, tempitend, [](const DicomFileInfos& a, const DicomFileInfos& b)
                  {
                      return a.SliceLocation != b.SliceLocation ? a.SliceLocation < b.SliceLocation : a.AcquisitionTime != b.AcquisitionTime ? a.AcquisitionTime < b.AcquisitionTime : a.InstanceNumber < b.InstanceNumber;
                  });

                  _pdata->info.push_back(std::move(imgInfoSplit));
              }
          }

          it = std::move(itend);
          #ifdef BK_EMIT_PROGRESS
          prog2.set_current(i);
          #endif
      } // for i = 0; i < files.size()

      _pdata->info.shrink_to_fit();

      #ifdef BK_EMIT_PROGRESS
      prog2.set_finished();
      #endif
  }

  bool DicomDirImporter::scan_directory_classic_all_files(const gdcm::Directory::FilenamesType& dcmfilenames)
  {
      if (_scandir_classic_process_files(dcmfilenames) == 0)
      {
          std::cerr << "no DICOM files were found in the directory" << std::endl;
          return false;
      }

      _scandir_classic_sort_files_to_separate_single_images();
      _scandir_classic_sort_images_by_dicomtags();

      return true;
  }
  /// @}

  /// @{ -------------------------------------------------- SCAN DIRECTORY (DICOMDIR)
  void DicomDirImporter::_scandir_DICOMDIR_sort_images_by_dicomtags()
  {
      for (auto it = _pdata->info.begin(); it != _pdata->info.end(); ++it)
      {
          std::stable_sort(_pdata->files.begin() + it->id_file_start, _pdata->files.begin() + it->id_file_end, [](const DicomFileInfos& a, const DicomFileInfos& b)
          {
              if (a.SliceLocation != b.SliceLocation)
              { return a.SliceLocation < b.SliceLocation; }
              else if (a.AcquisitionTime != b.AcquisitionTime)
              { return a.AcquisitionTime < b.AcquisitionTime; }
              else
              { return a.InstanceNumber < b.InstanceNumber; }
          });
      } // for files
  }

  bool DicomDirImporter::scan_directory_DICOMDIR(const gdcm::Directory::FilenamesType& dcmfilenames, unsigned int DICOMDIR_id)
  {
      gdcm::Reader reader;
      reader.SetFileName(dcmfilenames[DICOMDIR_id].c_str());

      if (reader.Read())
      {
          gdcm::File& file = reader.GetFile();
          gdcm::DataSet& ds = file.GetDataSet();
          gdcm::FileMetaInformation& metaInfo = file.GetHeader();
          gdcm::MediaStorage ms;
          ms.SetFromFile(file);

          if (ms != gdcm::MediaStorage::MediaStorageDirectoryStorage)
          { return false; }
          else
          {
              // valid DICOMDIR file

              std::stringstream strm;
              std::string strmStr;
              const std::string MediaStorageSOPClassUID_Value("1.2.840.10008.1.3.10");

              if (metaInfo.FindDataElement(gdcm::Keywords::MediaStorageSOPClassUID::GetTag()))
              {
                  strm.str("");
                  metaInfo.GetDataElement(gdcm::Keywords::MediaStorageSOPClassUID::GetTag()).GetValue().Print(strm);
                  strmStr = strm.str();
              }
              if (MediaStorageSOPClassUID_Value == strmStr)
              {
                  _pdata->files.reserve(dcmfilenames.size());

                  for (gdcm::DataSet::DataElementSet::const_iterator itr = ds.GetDES().begin(); itr != ds.GetDES().end(); ++itr)
                  {
                      if (itr->GetTag() == gdcm::Keywords::DirectoryRecordSequence::GetTag())
                      {
                          const gdcm::DataElement& dataElt = (*itr);
                          gdcm::SmartPointer<gdcm::SequenceOfItems> seqOfItems = dataElt.GetValueAsSQ();
                          DicomImageInfos imgInfo;
                          bool lastWasImage = false;
                          unsigned int expectedNumFiles = 0;
                          unsigned int fileCnt = 0;
                          DicomFileInfos globalFileInfo;
                          bool this_is_a_split_image = false;
                          bool perform_image_splitting = false;
                          bool first_after_splitting = false;
                          unsigned int slicesBeforeSplitting = 0;
                          unsigned int timesBeforeSplitting = 0;
                          unsigned int numFramesBeforeSplitting = 0;
                          unsigned int expectedNumFilesBeforeSplitting = 0;

                          #ifdef BK_EMIT_PROGRESS
                          bk::Progress& prog = bk_progress.emplace_task(seqOfItems->GetNumberOfItems(), ___("Scanning DICOMDIR"));
                          prog.set_current(1);
                          #endif

                          for (unsigned int itemUsed = 1; itemUsed < seqOfItems->GetNumberOfItems(); ++itemUsed)
                          {
                              strm.str("");
                              if (seqOfItems->GetItem(itemUsed).FindDataElement(gdcm::Keywords::DirectoryRecordType::GetTag()))
                              {
                                  const gdcm::Item& item = seqOfItems->GetItem(itemUsed);
                                  std::string DirectoryRecordType = "";
                                  if (item.FindDataElement(gdcm::Keywords::DirectoryRecordType::GetTag()))
                                  {
                                      strm.str("");
                                      item.GetDataElement(gdcm::Keywords::DirectoryRecordType::GetTag()).GetValue().Print(strm);
                                      DirectoryRecordType = bk::string_utils::trim(strm.str());
                                  }

                                  /*
                                   * a series of "image" tags ended
                                   * -> save and reset
                                   */
                                  if (lastWasImage && itemUsed != 1 /*first item*/ && (!bk::string_utils::equals(DirectoryRecordType, "image", false) || perform_image_splitting))
                                  {
                                      imgInfo.id_file_end = _pdata->files.size() - 1;
                                      imgInfo.StudyInstanceUID = globalFileInfo.StudyInstanceUID;
                                      imgInfo.SeriesInstanceUID = globalFileInfo.SeriesInstanceUID;
                                      imgInfo.ProtocolName = globalFileInfo.ProtocolName;

                                      if (this_is_a_split_image)
                                      {
                                          //const std::string rnd = bk::string_utils::from_int(bk_rand.next_integral(0,10000000));
                                          globalFileInfo.StudyInstanceUID = bk::string_utils::append(globalFileInfo.StudyInstanceUID, "_");
                                          globalFileInfo.SeriesInstanceUID = bk::string_utils::append(globalFileInfo.SeriesInstanceUID, "_");
                                          globalFileInfo.ProtocolName = bk::string_utils::append(globalFileInfo.ProtocolName, "_");
                                          globalFileInfo.SequenceName = bk::string_utils::append(globalFileInfo.SequenceName, "_");
                                      }

                                      _pdata->info.emplace_back(DicomImageInfos(imgInfo));

                                      globalFileInfo.AcquisitionTime = 0;
                                      globalFileInfo.filename = "";
                                      globalFileInfo.SliceLocation = 0;

                                      if (!this_is_a_split_image)
                                      {
                                          globalFileInfo.ProtocolName = "";
                                          globalFileInfo.SequenceName = "";
                                          globalFileInfo.SeriesInstanceUID = "";
                                          globalFileInfo.StudyInstanceUID = "";
                                      }

                                      fileCnt = 0;

                                      if (this_is_a_split_image)
                                      {
                                          perform_image_splitting = false;
                                          first_after_splitting = true;

                                          slicesBeforeSplitting = imgInfo.Slices;
                                          timesBeforeSplitting = imgInfo.TemporalPositions;
                                          numFramesBeforeSplitting = imgInfo.NumberOfFrames;
                                          expectedNumFilesBeforeSplitting = expectedNumFiles;
                                      }
                                      else
                                      {
                                          expectedNumFiles = 0;

                                          slicesBeforeSplitting = 0;
                                          timesBeforeSplitting = 0;
                                          numFramesBeforeSplitting = 0;
                                          expectedNumFilesBeforeSplitting = 0;
                                      }
                                  }

                                  /*
                                   * new image setup: study / series tag
                                   */
                                  if (bk::string_utils::equals(DirectoryRecordType, "study", false))
                                  {
                                      if (item.FindDataElement(gdcm::Keywords::StudyInstanceUID::GetTag()))
                                      {
                                          strm.str("");
                                          item.GetDataElement(gdcm::Keywords::StudyInstanceUID::GetTag()).GetValue().Print(strm);
                                          globalFileInfo.StudyInstanceUID = bk::string_utils::trim(strm.str());
                                      }

                                      lastWasImage = false;
                                      this_is_a_split_image = false;
                                      first_after_splitting = false;
                                      perform_image_splitting = false;
                                  }
                                  else if (bk::string_utils::equals(DirectoryRecordType, "series", false))
                                  {
                                      if (item.FindDataElement(gdcm::Keywords::SeriesInstanceUID::GetTag()))
                                      {
                                          strm.str("");
                                          item.GetDataElement(gdcm::Keywords::SeriesInstanceUID::GetTag()).GetValue().Print(strm);
                                          globalFileInfo.SeriesInstanceUID = bk::string_utils::trim(strm.str());
                                      }

                                      if (item.FindDataElement(gdcm::Keywords::ProtocolName::GetTag()))
                                      {
                                          strm.str("");
                                          item.GetDataElement(gdcm::Keywords::ProtocolName::GetTag()).GetValue().Print(strm);
                                          globalFileInfo.ProtocolName = bk::string_utils::trim(strm.str());
                                      }

                                      lastWasImage = false;
                                      this_is_a_split_image = false;
                                      first_after_splitting = false;
                                      perform_image_splitting = false;
                                  }
                                      /*
                                       * image tag
                                       */
                                  else if (bk::string_utils::equals(DirectoryRecordType, "image", false))
                                  {
                                      DicomFileInfos fileInfo;
                                      fileInfo.StudyInstanceUID = globalFileInfo.StudyInstanceUID;
                                      fileInfo.SeriesInstanceUID = globalFileInfo.SeriesInstanceUID;
                                      fileInfo.ProtocolName = globalFileInfo.ProtocolName;
                                      fileInfo.SequenceName = globalFileInfo.SequenceName;

                                      if (item.FindDataElement(gdcm::Keywords::ReferencedFileID::GetTag()))
                                      {
                                          // ReferencedFileID is the filename relative to the root directory
                                          strm.str("");
                                          item.GetDataElement(gdcm::Keywords::ReferencedFileID::GetTag()).GetValue().Print(strm);
                                          fileInfo.filename = bk::string_utils::append(std::string(_pdata->dir), bk::string_utils::trim(strm.str()));
                                          // _dir is ensured to end with a slash "/"

                                          // ReferencedFileID uses backslashs
                                          fileInfo.filename = bk::string_utils::replace(fileInfo.filename, "\\", "/");
                                      }

                                      gdcm::ImageReader reader;
                                      reader.SetFileName(fileInfo.filename.c_str());
                                      if (reader.Read())
                                      {
                                          const gdcm::File& dcmfile = reader.GetFile();
                                          const gdcm::DataSet& ds = dcmfile.GetDataSet();
                                          gdcm::StringFilter sf;
                                          sf.SetFile(dcmfile);

                                          if (!lastWasImage || first_after_splitting)
                                          {
                                              // this is the first item with an IMAGE tag after a series of items € {SERIES; STUDY, ...}
                                              imgInfo.id_file_start = _pdata->files.size();

                                              if (first_after_splitting)
                                              {
                                                  imgInfo.Slices = slicesBeforeSplitting;
                                                  imgInfo.TemporalPositions = timesBeforeSplitting;
                                                  imgInfo.NumberOfFrames = numFramesBeforeSplitting;
                                                  expectedNumFiles = expectedNumFilesBeforeSplitting;
                                              }
                                              else
                                              {
                                                  /*
                                                   * - read the first image of the following sequence in order to determine
                                                   *   the number of slices and temporal positions
                                                   * - the product is the number of expected slice images
                                                   */

                                                  /*
                                                   * InstanceNumber
                                                   */
                                                  if (ds.FindDataElement(gdcm::Keywords::InstanceNumber::GetTag()))
                                                  { fileInfo.InstanceNumber = bk::string_utils::to_uint(sf.ToString(ds.GetDataElement(gdcm::Keywords::InstanceNumber::GetTag()))); }

                                                  /*
                                                   * Slices
                                                   */
                                                  bool hasSlices = false;

                                                  if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
                                                  {
                                                      if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1018))) // philips private tag for slices
                                                      {
                                                          imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1018))));
                                                          hasSlices = true;
                                                      }
                                                  }
                                                  else
                                                  {
                                                      if (ds.FindDataElement(gdcm::Keywords::NumberOfSlices::GetTag()))
                                                      {
                                                          imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfSlices::GetTag())));
                                                          hasSlices = true;
                                                      }
                                                  }

                                                  /*
                                                   * Temporal Positions
                                                   */
                                                  bool hasTemporalPositions = false;

                                                  if (ds.FindDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag()))
                                                  {
                                                      imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag())));
                                                      hasTemporalPositions = true;
                                                  }
                                                  else if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
                                                  {
                                                      if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1017))) // philips private tag for num times
                                                      {
                                                          imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1017))));
                                                          hasTemporalPositions = true;
                                                      }
                                                  }
                                                  else if (ds.FindDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag()))
                                                  {
                                                      imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag())));
                                                      hasTemporalPositions = true;
                                                  }

                                                  /*
                                                   * Number of Frames
                                                   */
                                                  if (ds.FindDataElement(gdcm::Keywords::NumberOfFrames::GetTag()))
                                                  { imgInfo.NumberOfFrames = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfFrames::GetTag()))); }

                                                  std::vector<double> slicePositions;
                                                  std::vector<double> acquisitionTimes;

                                                  // count files with image tag
                                                  expectedNumFiles = 0;
                                                  for (unsigned int tempItemUsed = itemUsed; tempItemUsed < seqOfItems->GetNumberOfItems(); ++tempItemUsed, ++expectedNumFiles)
                                                  {
                                                      const gdcm::Item& tempItem = seqOfItems->GetItem(tempItemUsed);
                                                      if (tempItem.FindDataElement(gdcm::Keywords::DirectoryRecordType::GetTag()))
                                                      {
                                                          strm.str("");
                                                          tempItem.GetDataElement(gdcm::Keywords::DirectoryRecordType::GetTag()).GetValue().Print(strm);

                                                          if (!bk::string_utils::equals(bk::string_utils::trim(strm.str()), "image", false))
                                                          { break; }

                                                          if (!hasSlices || !hasTemporalPositions)
                                                          {
                                                              if (tempItem.FindDataElement(gdcm::Keywords::ReferencedFileID::GetTag()))
                                                              {
                                                                  // ReferencedFileID is the filename relative to the root directory
                                                                  strm.str("");
                                                                  tempItem.GetDataElement(gdcm::Keywords::ReferencedFileID::GetTag()).GetValue().Print(strm);
                                                                  std::string tempFilename = bk::string_utils::append(std::string(_pdata->dir), bk::string_utils::trim(strm.str()));
                                                                  // _dir is ensured to end with a slash "/"

                                                                  // ReferencedFileID uses backslashs
                                                                  tempFilename = bk::string_utils::replace(tempFilename, "\\", "/");

                                                                  gdcm::ImageReader tempReader;
                                                                  tempReader.SetFileName(tempFilename.c_str());
                                                                  if (tempReader.Read())
                                                                  {

                                                                      const gdcm::File& tempdcmfile = tempReader.GetFile();
                                                                      gdcm::StringFilter tempsf;
                                                                      tempsf.SetFile(tempdcmfile);

                                                                      if (!hasSlices)
                                                                      { slicePositions.push_back(bk::string_utils::to_double(tempsf.ToString(gdcm::Keywords::SliceLocation::GetTag()))); }

                                                                      if (!hasTemporalPositions)
                                                                      {
                                                                          // format : hhmmss.ssssss -> h: hour, m: minute, s: fraction second
                                                                          const std::string at = tempsf.ToString(gdcm::Keywords::AcquisitionTime::GetTag());
                                                                          double acqt = bk::string_utils::to_double(bk::string_utils::sub_string(at, 0, 2)) * 3'600'000 /*h -> ms*/;
                                                                          acqt += bk::string_utils::to_double(bk::string_utils::sub_string(at, 2, 2)) * 60'000 /*h -> ms*/;
                                                                          acqt += bk::string_utils::to_double(bk::string_utils::sub_string(at, 4, 9)) * 1'000 /*s -> ms*/;

                                                                          acquisitionTimes.push_back(acqt);
                                                                      }
                                                                  }
                                                              }
                                                          }
                                                      }
                                                  }

                                                  if (!hasSlices && !slicePositions.empty())
                                                  {
                                                      std::sort(slicePositions.begin(), slicePositions.end());
                                                      slicePositions.erase(std::unique(slicePositions.begin(), slicePositions.end()), slicePositions.end());
                                                      imgInfo.Slices = static_cast<int>(slicePositions.size());
                                                      hasSlices = true;
                                                  }

                                                  if (!hasTemporalPositions && !acquisitionTimes.empty())
                                                  {
                                                      std::sort(acquisitionTimes.begin(), acquisitionTimes.end());
                                                      acquisitionTimes.erase(std::unique(acquisitionTimes.begin(), acquisitionTimes.end()), acquisitionTimes.end());
                                                      imgInfo.TemporalPositions = static_cast<int>(acquisitionTimes.size());
                                                      hasTemporalPositions = true;
                                                  }

                                                  if (hasSlices && hasTemporalPositions)
                                                  {
                                                      const unsigned int expected2 = imgInfo.Slices * imgInfo.TemporalPositions;
                                                      if (expectedNumFiles != expected2)
                                                      {
                                                          // there are more files than Slices * TemporalPositions
                                                          // --> there must be multiple images in this range

                                                          expectedNumFiles /*per image*/ = expected2;
                                                          this_is_a_split_image = true;
                                                      }
                                                  }
                                              }

                                              fileCnt = 0;
                                              first_after_splitting = false;
                                          } // if first image tag after study/series

                                          fileInfo.SliceLocation = bk::string_utils::to_double(sf.ToString(gdcm::Keywords::SliceLocation::GetTag()));

                                          // format : hhmmss.ssssss -> h: hour, m: minute, s: fraction second
                                          const std::string at = sf.ToString(gdcm::Keywords::AcquisitionTime::GetTag());
                                          fileInfo.AcquisitionTime = bk::string_utils::to_double(bk::string_utils::sub_string(at, 0, 2)) * 3'600'000 /*h -> ms*/;
                                          fileInfo.AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 2, 2)) * 60'000 /*h -> ms*/;
                                          fileInfo.AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 4, 9)) * 1'000 /*s -> ms*/;

                                          const std::string ipp = sf.ToString(ds.GetDataElement(gdcm::Keywords::ImagePositionPatient::GetTag()));
                                          const auto vipp = bk::string_utils::contains(ipp, "\\") ? bk::string_utils::split(ipp, "\\") : bk::string_utils::split(ipp, "/");
                                          if (vipp.size() == 3)
                                          {
                                              fileInfo.ImagePositionPatient[0] = bk::string_utils::to_double(vipp[0]);
                                              fileInfo.ImagePositionPatient[1] = bk::string_utils::to_double(vipp[1]);
                                              fileInfo.ImagePositionPatient[2] = bk::string_utils::to_double(vipp[2]);
                                          }
                                      } // if (reader.Read())


                                      _pdata->files.push_back(std::move(fileInfo));
                                      ++fileCnt;

                                      if (this_is_a_split_image && fileCnt == expectedNumFiles)
                                      { perform_image_splitting = true; }

                                      first_after_splitting = false;
                                      lastWasImage = true;
                                  } // if image tag
                              }
                              #ifdef BK_EMIT_PROGRESS
                              prog.set_current(itemUsed);
                              #endif
                          } // for num items

                          #ifdef BK_EMIT_PROGRESS
                          prog.set_finished();
                          #endif
                      }
                  } // for(;itr != ds.GetDES().end(); ++itr)
              }
              else
              {
                  // invalid DICOMDIR file
                  return false;
              }
          }
      }

      _scandir_DICOMDIR_sort_images_by_dicomtags();

      return true;
  }
  /// @}

  /// @{ -------------------------------------------------- REMOVE DUPLICATES
  void DicomDirImporter::remove_duplicate_images()
  {
      _pdata->info.erase(std::unique(_pdata->info.begin(), _pdata->info.end(), [&](const DicomImageInfos& a, const DicomImageInfos& b)
      { return a.id_file_start == b.id_file_start && a.id_file_end == b.id_file_end; }), _pdata->info.end());
  }
  /// @}

  /// @{ -------------------------------------------------- SCAN IMAGE INFOS
  void DicomDirImporter::scan_image_infos()
  {
      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog = bk_progress.emplace_task(num_images() - 1, ___("Scanning DICOM tags"));
      #endif

      //------------------------------------------------------------------------------------------------------
      // for each image
      //------------------------------------------------------------------------------------------------------
      for (unsigned int i = 0; i < num_images(); ++i)
      {
          DicomImageInfos& imgInfo = _pdata->info[i];

          const unsigned int estimated_size = imgInfo.id_file_end - imgInfo.id_file_start;

          std::vector<double> sliceLocations; // count how many different slice locations (numSlices) there are
          sliceLocations.reserve(estimated_size);

          std::vector<double> acquisitionTimes; // count how many different acquisition times (numTimes) there are
          acquisitionTimes.reserve(estimated_size);

          std::vector<Vec3d> imagePositionPatient; // required for world matrix calculation
          imagePositionPatient.reserve(estimated_size);

          constexpr unsigned int nInfos = 29;
          bool hasInfo[nInfos];

          for (unsigned int k = 0; k < nInfos; ++k)
          { hasInfo[k] = false; }

          bool& hasDimensions = hasInfo[0];
          bool& hasRows = hasInfo[1];
          bool& hasColumns = hasInfo[2];
          bool& hasSlices = hasInfo[3];
          bool& hasTemporalPositions = hasInfo[4];
          bool& hasNumberOfFrames = hasInfo[5];
          bool& hasSpacing = hasInfo[6];
          bool& hasSamplesPerPixel = hasInfo[7];
          bool& hasBitsAllocated = hasInfo[8];
          bool& hasBitsStored = hasInfo[9];
          bool& hasHighBit = hasInfo[10];
          //bool& hasImagePositionPatient = hasInfo[11];
          bool& hasImageOrientationPatient = hasInfo[12];
          bool& hasPatientName = hasInfo[13];
          bool& hasPatientID = hasInfo[14];
          bool& hasPatientSex = hasInfo[15];
          bool& hasPatientBirthDate = hasInfo[16];
          bool& hasPatientAge = hasInfo[17];
          bool& hasPatientWeight = hasInfo[18];
          bool& hasPatientPosition = hasInfo[19];
          bool& hasSequenceName = hasInfo[20];
          bool& hasModality = hasInfo[21];
          bool& hasStudyDescription = hasInfo[22];
          bool& hasSeriesDescription = hasInfo[23];
          bool& hasProtocolName = hasInfo[24];
          bool& hasAcquisitionDate = hasInfo[25];
          bool& hasInstitutionName = hasInfo[26];
          bool& hasNominalInterval = hasInfo[27];
          bool& hasHeartRate = hasInfo[28];

          hasDimensions = imgInfo.nDimensions != 0;
          hasRows = imgInfo.Rows != 0;
          hasColumns = imgInfo.Columns != 0;
          hasSlices = imgInfo.Slices != 0;
          hasTemporalPositions = imgInfo.TemporalPositions != 0;
          hasNumberOfFrames = imgInfo.NumberOfFrames != 0;
          hasSpacing = imgInfo.RowSpacing != 0 && imgInfo.ColSpacing != 0 && imgInfo.SliceSpacing != 0;
          hasSamplesPerPixel = imgInfo.SamplesPerPixel != 0;
          hasBitsAllocated = imgInfo.BitsAllocated != 0;
          hasBitsStored = imgInfo.BitsStored != 0;
          hasHighBit = imgInfo.HighBit != -1;
          //hasImagePositionPatient = imagePositionPatient.size() == 3;
          hasImageOrientationPatient = imgInfo.ImageOrientationPatientX.norm() != 0 && imgInfo.ImageOrientationPatientY.norm() != 0;
          hasPatientName = !imgInfo.PatientName.empty();
          hasPatientID = !imgInfo.PatientID.empty();
          hasPatientSex = !imgInfo.PatientSex.empty();
          hasPatientBirthDate = !imgInfo.PatientBirthDate.empty();
          hasPatientAge = imgInfo.PatientAge != 0;
          hasPatientWeight = imgInfo.PatientWeight != 0;
          hasPatientPosition = !imgInfo.PatientPosition.empty();
          hasSequenceName = (!imgInfo.SequenceName.empty() || !imgInfo.SequenceName_Private.empty());
          hasModality = !imgInfo.Modality.empty();
          hasStudyDescription = !imgInfo.StudyDescription.empty();
          hasSeriesDescription = !imgInfo.SeriesDescription.empty();
          hasProtocolName = !imgInfo.ProtocolName.empty();
          hasAcquisitionDate = !imgInfo.AcquisitionDate.empty();
          hasInstitutionName = !imgInfo.InstitutionName.empty();
          hasNominalInterval = imgInfo.TemporalResolution != 0;
          hasHeartRate = imgInfo.TemporalResolution != 0;

          for (int k = imgInfo.id_file_start; k < imgInfo.id_file_end; ++k)
          {
              bool hasSliceLocation = _pdata->files[k].SliceLocation != 0;
              bool hasAcquisitionTime = _pdata->files[k].AcquisitionTime != 0;

              if (hasSliceLocation)
              { sliceLocations.push_back(_pdata->files[k].SliceLocation); }

              if (hasAcquisitionTime)
              { acquisitionTimes.push_back(_pdata->files[k].AcquisitionTime); }

              imagePositionPatient.push_back(_pdata->files[k].ImagePositionPatient);

              gdcm::ImageReader imgreader;
              imgreader.SetFileName(_pdata->files[k].filename.c_str());

              if (!imgreader.Read())
              { continue; }

              const gdcm::File& dcmfile = imgreader.GetFile();
              const gdcm::DataSet& ds = dcmfile.GetDataSet();
              const gdcm::Image& img = imgreader.GetImage();

              gdcm::StringFilter sf;
              sf.SetFile(dcmfile);

              /*
               * SliceLocation
               */
              if (!hasSliceLocation && ds.FindDataElement(gdcm::Keywords::SliceLocation::GetTag()))
              {
                  const double SliceLocation = bk::string_utils::to_double(sf.ToString(gdcm::Keywords::SliceLocation::GetTag()));

                  _pdata->files[k].SliceLocation = SliceLocation;

                  sliceLocations.push_back(SliceLocation);
              }

              /*
               * AcquisitionTime
               */
              if (!hasAcquisitionTime && ds.FindDataElement(gdcm::Keywords::AcquisitionTime::GetTag()))
              {
                  // format : hhmmss.ssssss -> h: hour, m: minute, s: fraction second
                  const std::string at = sf.ToString(gdcm::Keywords::AcquisitionTime::GetTag());
                  double AcquisitionTime = bk::string_utils::to_double(bk::string_utils::sub_string(at, 0, 2)) * 3'600'000 /*h -> ms*/;
                  AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 2, 2)) * 60'000 /*h -> ms*/;
                  AcquisitionTime += bk::string_utils::to_double(bk::string_utils::sub_string(at, 4, 9)) * 1'000 /*s -> ms*/;

                  _pdata->files[k].AcquisitionTime = AcquisitionTime;

                  acquisitionTimes.push_back(AcquisitionTime);
              }

              /*
               * num Dimensions
               */
              if (imgInfo.nDimensions == 0)
              {
                  imgInfo.nDimensions = static_cast<int>(img.GetNumberOfDimensions());
                  hasDimensions = imgInfo.nDimensions != 0;
              }

              /*
               * Size (Rows, Cols, Slices)
               */
              if (imgInfo.Rows == 0)
              {
                  imgInfo.Rows = img.GetRows();
                  hasRows = imgInfo.Rows != 0;
              }

              if (imgInfo.Columns == 0)
              {
                  imgInfo.Columns = img.GetColumns();
                  hasColumns = imgInfo.Columns != 0;
              }

              if (imgInfo.Slices == 0)
              {
                  if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
                  {
                      if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1018))) // philips private tag for slices
                      { imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1018)))); }
                  }
                  else
                  {
                      if (ds.FindDataElement(gdcm::Keywords::NumberOfSlices::GetTag()))
                      { imgInfo.Slices = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfSlices::GetTag()))); }
                  }

                  hasSlices = imgInfo.Slices != 0;
              }

              /*
               * Temporal Positions
               */
              if (imgInfo.TemporalPositions == 0)
              {
                  if (ds.FindDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag()))
                  { imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::CardiacNumberOfImages::GetTag()))); }
                  else if (ds.FindDataElement(gdcm::Keywords::Manufacturer::GetTag()) && bk::string_utils::contains(sf.ToString(ds.GetDataElement(gdcm::Keywords::Manufacturer::GetTag())), "philips", false))
                  {
                      if (ds.FindDataElement(gdcm::Tag(0x2001, 0x1017))) // philips private tag for num times
                      { imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Tag(0x2001, 0x1017)))); }
                  }
                  else if (ds.FindDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag()))
                  { imgInfo.TemporalPositions = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfTemporalPositions::GetTag()))); }

                  hasTemporalPositions = imgInfo.TemporalPositions != 0;
              }
              if (imgInfo.NumberOfFrames == 0 && ds.FindDataElement(gdcm::Keywords::NumberOfFrames::GetTag()))
              {
                  imgInfo.NumberOfFrames = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::NumberOfFrames::GetTag())));
                  hasNumberOfFrames = imgInfo.NumberOfFrames != 0;
              }

              /*
               * Spacing (Rows, Cols, Slices)
               */
              if (imgInfo.RowSpacing == 0 || imgInfo.ColSpacing == 0)
              {
                  auto pixelspacing = bk::string_utils::split(sf.ToString(ds.GetDataElement(gdcm::Keywords::PixelSpacing::GetTag())), "\\");

                  if (pixelspacing.size() == 2)
                  {
                      imgInfo.ColSpacing = bk::string_utils::to_double(pixelspacing.front());
                      imgInfo.RowSpacing = bk::string_utils::to_double(pixelspacing.back());
                  }

                  hasSpacing = imgInfo.RowSpacing != 0 && imgInfo.ColSpacing != 0 && imgInfo.SliceSpacing != 0;
              }

              if (imgInfo.SliceSpacing == 0)
              {
                  if (ds.FindDataElement(gdcm::Keywords::SpacingBetweenSlices::GetTag()))
                  { imgInfo.SliceSpacing = bk::string_utils::to_double(sf.ToString(ds.GetDataElement(gdcm::Keywords::SpacingBetweenSlices::GetTag()))); }

                  if (imgInfo.SliceSpacing == 0 && ds.FindDataElement(gdcm::Keywords::SliceThickness::GetTag()))
                  { imgInfo.SliceSpacing = bk::string_utils::to_double(sf.ToString(ds.GetDataElement(gdcm::Keywords::SliceThickness::GetTag()))); }

                  hasSpacing = imgInfo.RowSpacing != 0 && imgInfo.ColSpacing != 0 && imgInfo.SliceSpacing != 0;
              }

              /*
               * Nominal Interval (Temporal Resolution)
               * - DICOM Documentation: "Average duration of accepted beats, in msec, of the R-R interval."
               */
              if (imgInfo.TemporalResolution == 0)
              {
                  if (ds.FindDataElement(gdcm::Keywords::NominalInterval::GetTag()))
                  {
                      imgInfo.TemporalResolution = bk::string_utils::to_double(sf.ToString(ds.GetDataElement(gdcm::Keywords::NominalInterval::GetTag())));
                      hasNominalInterval = imgInfo.TemporalResolution != 0;
                  }
                  else if (ds.FindDataElement(gdcm::Keywords::HeartRate::GetTag()))
                  {
                      imgInfo.TemporalResolution = bk::string_utils::to_double(sf.ToString(ds.GetDataElement(gdcm::Keywords::HeartRate::GetTag())));
                      hasHeartRate = imgInfo.TemporalResolution != 0;
                  }
              }

              /*
               * Pixel Storage
               */
              if (imgInfo.SamplesPerPixel == 0)
              {
                  imgInfo.SamplesPerPixel = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::SamplesPerPixel::GetTag())));
                  hasSamplesPerPixel = imgInfo.SamplesPerPixel != 0;
              }

              if (imgInfo.BitsAllocated == 0)
              {
                  imgInfo.BitsAllocated = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::BitsAllocated::GetTag())));
                  hasBitsAllocated = imgInfo.BitsAllocated != 0;
              }

              if (imgInfo.BitsStored == 0)
              {
                  imgInfo.BitsStored = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::BitsStored::GetTag())));
                  imgInfo.LargestImagePixelValue = std::pow(2, imgInfo.BitsStored);
                  hasBitsStored = imgInfo.BitsStored != 0;
              }

              if (imgInfo.HighBit == -1)
              {
                  imgInfo.HighBit = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::HighBit::GetTag())));
                  hasHighBit = imgInfo.HighBit != -1;
              }

              // ImageOrientation ("specifies the direction cosines of the first row and the first column with respect to the patient. These Attributes shall be provide as a pair.")
              if (imgInfo.ImageOrientationPatientX.norm() == 0 && imgInfo.ImageOrientationPatientY.norm() == 0 && ds.FindDataElement(gdcm::Keywords::ImageOrientationPatient::GetTag()))
              {
                  const std::string iop = sf.ToString(ds.GetDataElement(gdcm::Keywords::ImageOrientationPatient::GetTag()));
                  const auto viop = bk::string_utils::contains(iop, "\\") ? bk::string_utils::split(iop, "\\") : bk::string_utils::split(iop, "/");

                  if (viop.size() == 6)
                  {
                      for (int l = 0; l < 3; ++l)
                      {
                          imgInfo.ImageOrientationPatientX[l] = bk::string_utils::to_double(viop[l]);
                          imgInfo.ImageOrientationPatientY[l] = bk::string_utils::to_double(viop[3 + l]);
                      }
                  }

                  hasImageOrientationPatient = imgInfo.ImageOrientationPatientX.norm() != 0 && imgInfo.ImageOrientationPatientY.norm() != 0;
              }

              /*
               * Patient Information
               */
              // PatientName
              if (imgInfo.PatientName.empty() && ds.FindDataElement(gdcm::Keywords::PatientName::GetTag()))
              {
                  imgInfo.PatientName = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientName::GetTag())));
                  hasPatientName = !imgInfo.PatientName.empty();

                  if (hasPatientName)
                  {
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "^", "_");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "-", "_");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "/", "_");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "\\", "_");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, " ", "_");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "ä", "ae");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "ö", "oe");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "ü", "ue");
                      imgInfo.PatientName = bk::string_utils::replace(imgInfo.PatientName, "ß", "ss");

                      imgInfo.PatientName = bk::string_utils::remove_all_except_characters(imgInfo.PatientName, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
                  }
              }

              // PatientID
              if (imgInfo.PatientID.empty() && ds.FindDataElement(gdcm::Keywords::PatientID::GetTag()))
              {
                  imgInfo.PatientID = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientID::GetTag())));
                  hasPatientID = !imgInfo.PatientID.empty();
              }

              // PatientSex
              if (imgInfo.PatientSex.empty() && ds.FindDataElement(gdcm::Keywords::PatientSex::GetTag()))
              {
                  imgInfo.PatientSex = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientSex::GetTag())));
                  hasPatientSex = !imgInfo.PatientSex.empty();
              }

              // PatientBirthDate
              if (imgInfo.PatientBirthDate.empty() && ds.FindDataElement(gdcm::Keywords::PatientBirthDate::GetTag()))
              {
                  imgInfo.PatientBirthDate = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientBirthDate::GetTag())));
                  hasPatientBirthDate = !imgInfo.PatientBirthDate.empty();
              }

              // PatientAge
              if (imgInfo.PatientAge == 0 && ds.FindDataElement(gdcm::Keywords::PatientAge::GetTag()))
              {
                  imgInfo.PatientAge = bk::string_utils::to_int(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientAge::GetTag())));
                  hasPatientAge = imgInfo.PatientAge != 0;
              }

              // PatientWeight
              if (imgInfo.PatientWeight == 0 && ds.FindDataElement(gdcm::Keywords::PatientWeight::GetTag()))
              {
                  imgInfo.PatientWeight = bk::string_utils::to_double(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientWeight::GetTag())));
                  hasPatientWeight = imgInfo.PatientWeight != 0;
              }

              // PatientPosition
              if (imgInfo.PatientPosition.empty() && ds.FindDataElement(gdcm::Keywords::PatientPosition::GetTag()))
              {
                  imgInfo.PatientPosition = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::PatientPosition::GetTag())));
                  hasPatientPosition = !imgInfo.PatientPosition.empty();
              }

              /*
               * SequenceName
               */
              if (imgInfo.SequenceName.empty() && ds.FindDataElement(gdcm::Keywords::SequenceName::GetTag()))
              {
                  imgInfo.SequenceName = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::SequenceName::GetTag())));
                  hasSequenceName = (!imgInfo.SequenceName.empty() || !imgInfo.SequenceName_Private.empty());
              }

              // SequenceName_Private
              if (imgInfo.SequenceName_Private.empty() && ds.FindDataElement(gdcm::Tag(0x0051, 0x1014)))
              {
                  imgInfo.SequenceName_Private = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Tag(0x0051, 0x1014))));
                  hasSequenceName = (!imgInfo.SequenceName.empty() || !imgInfo.SequenceName_Private.empty());
              }

              /*
               * Modality
               */
              if (imgInfo.Modality.empty() && ds.FindDataElement(gdcm::Keywords::Modality::GetTag()))
              {
                  imgInfo.Modality = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::Modality::GetTag())));
                  hasModality = !imgInfo.Modality.empty();
              }

              /*
               * Study and Series Information
               */
              // StudyDescription
              if (imgInfo.StudyDescription.empty() && ds.FindDataElement(gdcm::Keywords::StudyDescription::GetTag()))
              {
                  imgInfo.StudyDescription = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::StudyDescription::GetTag())));
                  hasStudyDescription = !imgInfo.StudyDescription.empty();
              }

              // SeriesDescription
              if (imgInfo.SeriesDescription.empty() && ds.FindDataElement(gdcm::Keywords::SeriesDescription::GetTag()))
              {
                  imgInfo.SeriesDescription = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::SeriesDescription::GetTag())));
                  hasSeriesDescription = !imgInfo.SeriesDescription.empty();
              }

              // ProtocolName
              if (imgInfo.ProtocolName.empty() && ds.FindDataElement(gdcm::Keywords::ProtocolName::GetTag()))
              {
                  imgInfo.ProtocolName = sf.ToString(ds.GetDataElement(gdcm::Keywords::ProtocolName::GetTag()));
                  hasProtocolName = !imgInfo.ProtocolName.empty();
              }

              // AcquisitionDate
              if (imgInfo.AcquisitionDate.empty() && ds.FindDataElement(gdcm::Keywords::AcquisitionDate::GetTag()))
              {
                  imgInfo.AcquisitionDate = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::AcquisitionDate::GetTag())));
                  hasAcquisitionDate = !imgInfo.AcquisitionDate.empty();
              }

              // InstitutionName
              if (imgInfo.InstitutionName.empty() && ds.FindDataElement(gdcm::Keywords::InstitutionName::GetTag()))
              {
                  imgInfo.InstitutionName = bk::string_utils::trim(sf.ToString(ds.GetDataElement(gdcm::Keywords::InstitutionName::GetTag())));
                  hasInstitutionName = !imgInfo.InstitutionName.empty();
              }
          } // for k : files of image

          if (imgInfo.Slices == 0)
          {
              if (!sliceLocations.empty())
              {
                  std::sort(sliceLocations.begin(), sliceLocations.end());
                  sliceLocations.erase(std::unique(sliceLocations.begin(), sliceLocations.end()), sliceLocations.end());
                  imgInfo.Slices = static_cast<int>(sliceLocations.size());
              }
              else if (imgInfo.SliceSpacing != 0)
              { imgInfo.Slices = static_cast<int>(imgInfo.id_file_end - imgInfo.id_file_start); }
          }

          // this is done outside the "if" because it is needed a couple of lines below
          // for the determination of the temporal resolution
          std::sort(acquisitionTimes.begin(), acquisitionTimes.end());
          acquisitionTimes.erase(std::unique(acquisitionTimes.begin(), acquisitionTimes.end()), acquisitionTimes.end());

          if (imgInfo.TemporalPositions == 0)
          { imgInfo.TemporalPositions = static_cast<int>(acquisitionTimes.size()); }

          imgInfo.nDimensions = 0;
          if (imgInfo.Columns > 1) // x
          { ++imgInfo.nDimensions; }
          if (imgInfo.Rows > 1) // y
          { ++imgInfo.nDimensions; }
          if (imgInfo.Slices > 1) // z
          { ++imgInfo.nDimensions; }
          if (imgInfo.TemporalPositions > 1) // t
          { ++imgInfo.nDimensions; }

          // determine temporal resolution
          if (!hasNominalInterval && !hasHeartRate && acquisitionTimes.size() >= 2)
          {
              // there are larger jumps at slice changes -> need to be filtered

              double diffval = 0;
              bool first = true;
              double currentMean = 0;
              unsigned int cnt = 0;

              for (unsigned int i = 0; i < acquisitionTimes.size() - 1; ++i)
              {
                  const double current_diff = acquisitionTimes[i + 1] - acquisitionTimes[i];

                  if (first)
                  {
                      first = false;
                      diffval += current_diff;
                      currentMean = diffval;
                      ++cnt;
                  }
                  else
                  {
                      if (current_diff < 2 * currentMean) // else slice change
                      {
                          diffval += current_diff;
                          ++cnt;
                      }

                      currentMean = diffval / cnt;
                  }
              }

              imgInfo.TemporalResolution = currentMean;
          }
          else if (hasNominalInterval && !hasHeartRate && imgInfo.TemporalPositions != 0)
          { imgInfo.TemporalResolution /= imgInfo.TemporalPositions; }
          else if (!hasNominalInterval && hasHeartRate && imgInfo.TemporalPositions != 0)
          { imgInfo.TemporalResolution = 60'000.0 / (imgInfo.TemporalResolution * imgInfo.TemporalPositions); }

          if (!imgInfo.PatientPosition.empty() && !bk::string_utils::equals(imgInfo.PatientPosition, "HFS", false))
          { std::cerr << "PatientPosition is not HFS" << std::endl; }

          /*
           * compose world matrix
           * - see http://nipy.org/nibabel/dicom/dicom_orientation.html#dicom-voxel-to-patient-coordinate-system-mapping
           *   or https://stackoverflow.com/questions/21759013/dicom-affine-matrix-transformation-from-image-space-to-patient-space-in-matlab
           *   or https://dicomiseasy.blogspot.de/2013/06/getting-oriented-using-image-plane.html
           */
          if (!imagePositionPatient.empty())
          {
              imagePositionPatient.erase(std::unique(std::begin(imagePositionPatient), std::end(imagePositionPatient)), std::end(imagePositionPatient));
              const auto& T1 = imagePositionPatient.front();
              const auto& TN = imagePositionPatient.back();

              /*
               * either use --(1)-- T1 for the 4th column of the world matrix (W) and scale the third column by (1-N) == -(N-1)
               * or use --(2)-- TN for the 4th column of W and scale the third column by (N-1)
               *
               * MeVisLab uses --(2)--
               */
              //static constexpr const bool start_from_T1 = true;
              static const bool start_from_T1 = false;

              imgInfo.worldMatrix(0, 0) = imgInfo.ImageOrientationPatientX[0] * imgInfo.RowSpacing;
              imgInfo.worldMatrix(1, 0) = imgInfo.ImageOrientationPatientX[1] * imgInfo.RowSpacing;
              imgInfo.worldMatrix(2, 0) = imgInfo.ImageOrientationPatientX[2] * imgInfo.RowSpacing;
              imgInfo.worldMatrix(3, 0) = 0;

              imgInfo.worldMatrix(0, 1) = imgInfo.ImageOrientationPatientY[0] * imgInfo.ColSpacing;
              imgInfo.worldMatrix(1, 1) = imgInfo.ImageOrientationPatientY[1] * imgInfo.ColSpacing;
              imgInfo.worldMatrix(2, 1) = imgInfo.ImageOrientationPatientY[2] * imgInfo.ColSpacing;
              imgInfo.worldMatrix(3, 1) = 0;

              if (imagePositionPatient.size() == 1) // single slice version
              {
                  const auto ImageOrientationPatientZ = imgInfo.ImageOrientationPatientX.cross(imgInfo.ImageOrientationPatientY);
                  imgInfo.worldMatrix(0, 2) = ImageOrientationPatientZ[0] * imgInfo.SliceSpacing;
                  imgInfo.worldMatrix(1, 2) = ImageOrientationPatientZ[1] * imgInfo.SliceSpacing;
                  imgInfo.worldMatrix(2, 2) = ImageOrientationPatientZ[2] * imgInfo.SliceSpacing;
              }
              else // multi slice version
              {
                  // http://nipy.org/nibabel/dicom/dicom_orientation.html#dicom-affine-definitions

                  const double scale = static_cast<double>(imagePositionPatient.size()) - 1;
                  if (start_from_T1)
                  {
                      imgInfo.worldMatrix(0, 2) = (TN[0] - T1[0]) / scale;
                      imgInfo.worldMatrix(1, 2) = (TN[1] - T1[1]) / scale;
                      imgInfo.worldMatrix(2, 2) = (TN[2] - T1[2]) / scale;
                  }
                  else
                  {
                      imgInfo.worldMatrix(0, 2) = (T1[0] - TN[0]) / scale;
                      imgInfo.worldMatrix(1, 2) = (T1[1] - TN[1]) / scale;
                      imgInfo.worldMatrix(2, 2) = (T1[2] - TN[2]) / scale;
                  }
              }

              imgInfo.worldMatrix(3, 2) = 0;

              if (start_from_T1)
              {
                  imgInfo.worldMatrix(0, 3) = T1[0];
                  imgInfo.worldMatrix(1, 3) = T1[1];
                  imgInfo.worldMatrix(2, 3) = T1[2];
              }
              else // start from TN
              {
                  imgInfo.worldMatrix(0, 3) = TN[0];
                  imgInfo.worldMatrix(1, 3) = TN[1];
                  imgInfo.worldMatrix(2, 3) = TN[2];
              }

              imgInfo.worldMatrix(3, 3) = 1;

              // add half voxelscale to translation in order to center the data points
              constexpr const Vec4d v0(0, 0, 0, 1);
              constexpr const Vec4d v1(1, 1, 1, 1);
              auto p0 = imgInfo.worldMatrix * v0;
              auto p1 = imgInfo.worldMatrix * v1;
              p0[0] /= p0[3];
              p0[1] /= p0[3];
              p0[2] /= p0[3];
              p1[0] /= p1[3];
              p1[1] /= p1[3];
              p1[2] /= p1[3];
              const auto correction = (p1 - p0) * 0.5;

              if (start_from_T1)
              {
                  imgInfo.worldMatrix(0, 3) += correction[0];
                  imgInfo.worldMatrix(1, 3) += correction[1];
                  imgInfo.worldMatrix(2, 3) += correction[2];
              }
              else // start from TN
              {
                  imgInfo.worldMatrix(0, 3) -= correction[0];
                  imgInfo.worldMatrix(1, 3) -= correction[1];
                  imgInfo.worldMatrix(2, 3) -= correction[2];
              }

              const Vec3d sliceDirection = imgInfo.worldMatrix.col(2).sub_vector<0, 2>().normalize();
              const Vec3d startPoint = start_from_T1 ? T1 : TN;

              for (int k = imgInfo.id_file_start; k < imgInfo.id_file_end; ++k)
              { _pdata->files[k].SliceLocation = std::abs(sliceDirection.dot(startPoint - _pdata->files[k].ImagePositionPatient)); }

              std::stable_sort(_pdata->files.begin() + imgInfo.id_file_start, _pdata->files.begin() + imgInfo.id_file_end, [](const DicomFileInfos& a, const DicomFileInfos& b)
              {
                  return a.SliceLocation != b.SliceLocation ? a.SliceLocation < b.SliceLocation : a.AcquisitionTime != b.AcquisitionTime ? a.AcquisitionTime < b.AcquisitionTime : a.InstanceNumber < b.InstanceNumber;
              });
          }
          else
          {
              // no world matrix information given
              // -> use default identity matrix
              imgInfo.worldMatrix.set_identity();
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_current(i);
          #endif
      } // for i : num_images()

      #ifdef BK_EMIT_PROGRESS
      if (prog.current() != prog.max())
      { prog.set_finished(); }
      #endif
  }

  void DicomDirImporter::scan_image_dimensions()
  {
      const unsigned int nImages = this->num_images();

      #ifdef BK_EMIT_PROGRESS
      bk::Progress& prog = bk_progress.emplace_task(nImages - 1 + 4, ___("Scanning image dimensions"));
      #endif

      /*
       * - determine all image sizes
       * -> find 4d images with same size
       */
      std::vector<std::pair<unsigned int, Vec2ui>> image2d;
      image2d.reserve(nImages);

      std::vector<std::pair<unsigned int, Vec3ui>> image2d_t;
      image2d_t.reserve(nImages);

      std::vector<std::pair<unsigned int, Vec3ui>> image3d;
      image3d.reserve(nImages);

      std::vector<std::pair<unsigned int, Vec4ui>> image3d_t;
      image3d_t.reserve(nImages);

      for (unsigned int i = 0; i < nImages; ++i)
      {
          const DicomImageInfos& info = image_infos(i);

          if (info.Columns > 1 && info.Rows > 1 && info.Slices > 1 && info.TemporalPositions > 1)
          { image3d_t.emplace_back(i, Vec4ui(info.Columns, info.Rows, info.Slices, info.TemporalPositions)); }
          else if (info.Columns > 1 && info.Rows > 1 && info.Slices > 1)
          { image3d.emplace_back(i, Vec3ui(info.Columns, info.Rows, info.Slices)); }
          else if (info.Columns > 1 && info.Rows > 1 && info.TemporalPositions > 1)
          { image2d_t.emplace_back(i, Vec3ui(info.Columns, info.Rows, info.TemporalPositions)); }
          else if (info.Columns > 1 && info.Rows > 1)
          { image2d.emplace_back(i, Vec2ui(info.Columns, info.Rows)); }

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif
      }

      /*
       * - sort by lexicographically by image sizes
       */

      std::sort(image3d_t.begin(), image3d_t.end(), [](const std::pair<unsigned int, Vec4ui>& p0, const std::pair<unsigned int, Vec4ui>& p1) -> bool
      {
          const Vec4ui& x = p0.second;
          const Vec4ui& y = p1.second;
          return x[0] < y[0] ? true : x[0] > y[0] ? false : x[1] < y[1] ? true : x[1] > y[1] ? false : x[2] < y[2] ? true : x[2] > y[2] ? false : x[3] < y[3];
      });

      std::sort(image3d.begin(), image3d.end(), [](const std::pair<unsigned int, Vec3ui>& p0, const std::pair<unsigned int, Vec3ui>& p1) -> bool
      {
          const Vec3ui& x = p0.second;
          const Vec3ui& y = p1.second;
          return x[0] < y[0] ? true : x[0] > y[0] ? false : x[1] < y[1] ? true : x[1] > y[1] ? false : x[2] < y[2];
      });

      std::sort(image2d_t.begin(), image2d_t.end(), [](const std::pair<unsigned int, Vec3ui>& p0, const std::pair<unsigned int, Vec3ui>& p1) -> bool
      {
          const Vec3ui& x = p0.second;
          const Vec3ui& y = p1.second;
          return x[0] < y[0] ? true : x[0] > y[0] ? false : x[1] < y[1] ? true : x[1] > y[1] ? false : x[2] < y[2];
      });

      std::sort(image2d.begin(), image2d.end(), [](const std::pair<unsigned int, Vec2ui>& p0, const std::pair<unsigned int, Vec2ui>& p1) -> bool
      {
          const Vec2ui& x = p0.second;
          const Vec2ui& y = p1.second;
          return x[0] < y[0] ? true : x[0] > y[0] ? false : x[1] < y[1];
      });

      #ifdef BK_EMIT_PROGRESS
      prog.increment(2);
      #endif

      /*
       * - create groups of same-sized images
       */
      _pdata->images_3dt.reserve(image3d_t.size());
      _pdata->images_3d.reserve(image3d.size());
      _pdata->images_2dt.reserve(image2d_t.size());
      _pdata->images_2d.reserve(image2d.size());

      for (unsigned int i = 0; i < image3d_t.size(); ++i)
      {
          const Vec4ui& last_size = image3d_t[i].second;

          unsigned int duplicate_cnt = 0;
          for (unsigned int k = i + 1; k < image3d_t.size() && last_size == image3d_t[k].second; ++k)
          { ++duplicate_cnt; }

          // create new group
          _pdata->images_3dt.push_back(std::make_pair(last_size, std::vector<unsigned int>()));
          _pdata->images_3dt.back().second.reserve(duplicate_cnt + 1);

          for (unsigned int k = i; k < i + duplicate_cnt + 1; ++k)
          { _pdata->images_3dt.back().second.push_back(image3d_t[k].first); }

          i += duplicate_cnt;
      }

      for (unsigned int i = 0; i < image3d.size(); ++i)
      {
          const Vec3ui& last_size = image3d[i].second;
          unsigned int duplicate_cnt = 0;

          for (unsigned int k = i + 1; k < image3d.size() && last_size == image3d[k].second; ++k)
          { ++duplicate_cnt; }

          // create new group
          _pdata->images_3d.push_back(std::make_pair(last_size, std::vector<unsigned int>()));
          _pdata->images_3d.back().second.reserve(duplicate_cnt + 1);

          for (unsigned int k = i; k < i + duplicate_cnt + 1; ++k)
          { _pdata->images_3d.back().second.push_back(image3d[k].first); }

          i += duplicate_cnt;
      }

      for (unsigned int i = 0; i < image2d_t.size(); ++i)
      {
          const Vec3ui& last_size = image2d_t[i].second;
          unsigned int duplicate_cnt = 0;

          for (unsigned int k = i + 1; k < image2d_t.size() && last_size == image2d_t[k].second; ++k)
          { ++duplicate_cnt; }

          // create new group
          _pdata->images_2dt.push_back(std::make_pair(last_size, std::vector<unsigned int>()));
          _pdata->images_2dt.back().second.reserve(duplicate_cnt + 1);

          for (unsigned int k = i; k < i + duplicate_cnt + 1; ++k)
          { _pdata->images_2dt.back().second.push_back(image2d_t[k].first); }

          i += duplicate_cnt;
      }

      for (unsigned int i = 0; i < image2d.size(); ++i)
      {
          const Vec2ui& last_size = image2d[i].second;
          unsigned int duplicate_cnt = 0;

          for (unsigned int k = i + 1; k < image2d.size() && last_size == image2d[k].second; ++k)
          { ++duplicate_cnt; }

          // create new group
          _pdata->images_2d.push_back(std::make_pair(last_size, std::vector<unsigned int>()));
          _pdata->images_2d.back().second.reserve(duplicate_cnt + 1);

          for (unsigned int k = i; k < i + duplicate_cnt + 1; ++k)
          { _pdata->images_2d.back().second.push_back(image2d[k].first); }

          i += duplicate_cnt;
      }

      _pdata->images_3dt.shrink_to_fit();
      _pdata->images_3d.shrink_to_fit();
      _pdata->images_2dt.shrink_to_fit();
      _pdata->images_2d.shrink_to_fit();

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif
  }
  /// @}

  /// @{ -------------------------------------------------- PRINT IMAGE INFOS
  std::ostream& DicomDirImporter::print_image_infos(unsigned int id, std::ostream& s) const
  {
      if (id < _pdata->info.size())
      {
          const DicomImageInfos& imgInfo = _pdata->info[id];
          s << "-------------------- (" << id << ") --------------------" << std::endl;

          /*
           * PATIENT
           */
          s << "PATIENT: " << std::endl;
          if (!imgInfo.PatientName.empty())
          {
              s << "\tNAME: " << imgInfo.PatientName << "";
              if (!imgInfo.PatientID.empty())
              { s << " (ID: " << imgInfo.PatientID << ")" << std::endl; }
              else
              { s << std::endl; }
          }
          else if (!imgInfo.PatientID.empty())
          { s << "\tID: \"" << imgInfo.PatientID << "\")" << std::endl; }
          if (!imgInfo.PatientBirthDate.empty())
          {
              s << "\tDATE OF BIRTH: " << imgInfo.PatientBirthDate;
              if (imgInfo.PatientAge != 0)
              { s << " (AGE " << imgInfo.PatientAge << ")"; }
              s << std::endl;
          }
          else if (imgInfo.PatientAge != 0)
          { s << "\tAGE: " << imgInfo.PatientAge << std::endl; }
          if (!imgInfo.PatientSex.empty())
          { s << "\tGENDER: " << imgInfo.PatientSex << std::endl; }
          if (imgInfo.PatientWeight != 0)
          { s << "\tWEIGHT: " << imgInfo.PatientWeight << std::endl; }
          if (!imgInfo.PatientPosition.empty())
          { s << "\tPOSITION: " << imgInfo.PatientPosition << std::endl; }

          /*
           * SCAN
           */
          s << "SCAN: " << std::endl;
          if (!imgInfo.Modality.empty())
          { s << "\tMODALITY: " << imgInfo.Modality << std::endl; }
          if (!imgInfo.AcquisitionDate.empty())
          { s << "\tACQUISITION DATE: " << imgInfo.AcquisitionDate << std::endl; }
          if (!imgInfo.InstitutionName.empty())
          { s << "\tINSTITUTION NAME: " << imgInfo.InstitutionName << std::endl; }
          if (!imgInfo.StudyDescription.empty())
          { s << "\tSTUDY DESCRIPTION: " << imgInfo.StudyDescription << std::endl; }
          if (!imgInfo.SeriesDescription.empty())
          { s << "\tSERIES DESCRIPTION: " << imgInfo.SeriesDescription << std::endl; }
          if (!imgInfo.SequenceName.empty())
          {
              s << "\tSEQUENCE NAME: " << imgInfo.SequenceName;
              if (!imgInfo.SequenceName_Private.empty())
              { s << " / " << imgInfo.SequenceName_Private; }
              s << std::endl;
          }
          else if (!imgInfo.SequenceName_Private.empty())
          { s << "\tSEQUENCE NAME: " << imgInfo.SequenceName_Private << std::endl; }
          if (!imgInfo.ProtocolName.empty())
          { s << "\tPROTOCOL NAME: " << imgInfo.ProtocolName << std::endl; }

          /*
           * IMAGE
           */
          s << "IMAGE:" << std::endl;
          s << "\tGRID SIZE: " << imgInfo.Columns << " columns, " << imgInfo.Rows << " rows, " << imgInfo.Slices << " slices, " << imgInfo.TemporalPositions << " time steps" << std::endl;
          s << "\tRESOLUTION: " << imgInfo.ColSpacing << " x " << imgInfo.RowSpacing << " x " << imgInfo.SliceSpacing << " x " << imgInfo.TemporalResolution << std::endl;
          s << "\tWORLD MATRIX:" << std::endl;
          for (unsigned int r = 0; r < imgInfo.worldMatrix.num_rows(); ++r)
          {
              s << "\t\t";
              for (unsigned int c = 0; c < imgInfo.worldMatrix.num_cols() - 1; ++c)
              {
                  if (imgInfo.worldMatrix(r, c) >= 0)
                  { s << " "; }
                  s << imgInfo.worldMatrix(r, c) << " ";
              }
              s << imgInfo.worldMatrix(r, imgInfo.worldMatrix.num_cols() - 1) << std::endl;
          }


          //s << "\tNUM FRAMES: " << imgInfo.NumberOfFrames << std::endl;
      }
      return s;
  }

  std::ostream& DicomDirImporter::print_image_infos(unsigned int id) const
  { return print_image_infos(id, std::cout); }
  /// @}

  /// @{ -------------------------------------------------- HELPERS: READ
  int DicomDirImporter::_count_image_dimensions(const DicomImageInfos& imgInfo) const
  {
      int nDim = 0;
      if (imgInfo.Columns > 1) // size x
      { ++nDim; }
      if (imgInfo.Rows > 1) // size y
      { ++nDim; }
      if (imgInfo.Slices > 1) // size z
      { ++nDim; }
      if (imgInfo.TemporalPositions > 1) // size t
      { ++nDim; }

      return nDim;
  }

  void DicomDirImporter::_check_from_to_dimenson(unsigned int& xfrom, unsigned int& xto, unsigned int& yfrom, unsigned int& yto, unsigned int& zfrom, unsigned int& zto, unsigned int& tfrom, unsigned int& tto) const
  {
      if (xfrom > xto)
      { std::swap(xfrom, xto); }

      if (yfrom > yto)
      { std::swap(yfrom, yto); }

      if (zfrom > zto)
      { std::swap(zfrom, zto); }

      if (tfrom > tto)
      { std::swap(tfrom, tto); }
  }

  std::tuple<int, bool, bool, bool, bool> DicomDirImporter::_count_expected_image_dimensions(unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const
  {
      bool has_x = (xto - xfrom) != 0;
      bool has_y = (yto - yfrom) != 0;
      bool has_z = (zto - zfrom) != 0;
      bool has_t = (tto - tfrom) != 0;

      int N = (has_x ? 1 : 0) + (has_y ? 1 : 0) + (has_z ? 1 : 0) + (has_t ? 1 : 0);

      return std::tuple<int, bool, bool, bool, bool>(N, has_x, has_y, has_z, has_t);
  }

  void DicomDirImporter::_setup_image(const DicomImageInfos& imgInfo, std::unique_ptr<DicomImage<double, -1>>& img, bool /*has_x*/, bool /*has_y*/, bool has_z, bool has_t) const
  {
      img->geometry().transformation().set_world_matrix(imgInfo.worldMatrix);

      if (has_z)
      {
          if (has_t)
          {
              // 3d+t image
              img->set_size(imgInfo.Columns, imgInfo.Rows, imgInfo.Slices, imgInfo.TemporalPositions);
              img->geometry().transformation().set_dicom_image_type_3dt();
              img->geometry().transformation().set_temporal_resolution(imgInfo.TemporalResolution);
          }
          else
          {
              // 3d image
              img->set_size(imgInfo.Columns, imgInfo.Rows, imgInfo.Slices);
              img->geometry().transformation().set_dicom_image_type_3d();
          }
      }
      else
      {
          if (has_t)
          {
              // 2d+t image
              img->set_size(imgInfo.Columns, imgInfo.Rows, imgInfo.TemporalPositions);
              img->geometry().transformation().set_dicom_image_type_2dt();
              img->geometry().transformation().set_temporal_resolution(imgInfo.TemporalResolution);
          }
          else
          {
              // 2d image
              img->set_size(imgInfo.Columns, imgInfo.Rows);
              img->geometry().transformation().set_dicom_image_type_2d();
          }
      }
  }

  void DicomDirImporter::_set_image_val(std::unique_ptr<DicomImage<double, -1>>& img, unsigned int val, unsigned int nDim, bool has_x, bool has_y, bool has_z, bool has_t, unsigned int rowid, unsigned int colid, unsigned int slicePos, unsigned int temporalPos) const
  {
      switch (nDim)
      {
          case 1:
          {
              if (has_x)
              { img->operator[](rowid) = val; }
              else if (has_y)
              { img->operator[](colid) = val; }
              else if (has_z)
              { img->operator[](slicePos) = val; }
              else if (has_t)
              { img->operator[](temporalPos) = val; }
              break;
          }
          case 2:
          {
              if (has_x && has_y)
              { img->operator()(rowid, colid) = val; }
              else if (has_x && has_z)
              { img->operator()(rowid, slicePos) = val; }
              else if (has_x && has_t)
              { img->operator()(rowid, temporalPos) = val; }
              else if (has_y && has_z)
              { img->operator()(colid, slicePos) = val; }
              else if (has_y && has_t)
              { img->operator()(colid, temporalPos) = val; }
              else if (has_z && has_t)
              { img->operator()(slicePos, temporalPos) = val; }
              break;
          }
          case 3:
          {
              if (has_x && has_y && has_z)
              { img->operator()(rowid, colid, slicePos) = val; }
              else if (has_x && has_y && has_t)
              { img->operator()(rowid, colid, temporalPos) = val; }
              else if (has_x && has_z && has_t)
              { img->operator()(rowid, slicePos, temporalPos) = val; }
              else if (has_y && has_z && has_t)
              { img->operator()(colid, slicePos, temporalPos) = val; }
              break;
          }
          case 4:
          {
              img->operator()(rowid, colid, slicePos, temporalPos) = val;
              break;
          }
      }
  }

  [[nodiscard]] unsigned int DicomDirImporter::_get_value_from_raw_data(const char* valdata, int nBytesPerPixel, bool littleEndian, const DicomImageInfos& imgInfo) const
  {
      unsigned int val = 0;

      if (littleEndian)
      {
          if (imgInfo.BitsAllocated == 16)
          { val = static_cast<unsigned int>(*reinterpret_cast<const std::uint16_t*>(valdata)); }
          else if (imgInfo.BitsAllocated == 8)
          { val = static_cast<unsigned int>(*reinterpret_cast<const std::uint8_t*>(valdata)); }
          else if (imgInfo.BitsAllocated == 32)
          { val = static_cast<unsigned int>(*reinterpret_cast<const std::uint32_t*>(valdata)); }
          else if (imgInfo.BitsAllocated == 64)
          { val = static_cast<unsigned int>(*reinterpret_cast<const std::uint64_t*>(valdata)); }
          else
          {
              BitVectorX bits;
              bits.set_size(imgInfo.BitsAllocated);

              for (int v = 0; v < nBytesPerPixel; ++v)
              { bits.data()[nBytesPerPixel - 1 - v] = valdata[v]; }

              val = bits.to_number(littleEndian);
          }
      }
      else // big endian
      {
          BitVectorX bits;
          bits.set_size(imgInfo.BitsAllocated);

          for (int v = 0; v < nBytesPerPixel; ++v)
          { bits.data()[v] = valdata[v]; }

          val = bits.to_number(littleEndian);
      }

      return val;
  }
  /// @}

  /// @{ -------------------------------------------------- READ DICOM IMAGE
  std::unique_ptr<DicomImage<double, -1>> DicomDirImporter::read_slice_of_4d_image(unsigned int image_id, unsigned int z_id, unsigned int t_id) const
  {
      assert(image_id < _pdata->info.size());
      const DicomImageInfos& imgInfo = _pdata->info[image_id];
      return read_image_block(image_id, 0, imgInfo.Columns - 1, 0, imgInfo.Rows - 1, z_id, z_id, t_id, t_id);
  }

  std::unique_ptr<DicomImage<double, -1>> DicomDirImporter::read_image(unsigned int id) const
  {
      assert(id < _pdata->info.size());
      const DicomImageInfos& imgInfo = _pdata->info[id];
      return read_image_block(id, 0, imgInfo.Columns - 1, 0, imgInfo.Rows - 1, 0, imgInfo.Slices - 1, 0, imgInfo.TemporalPositions - 1);
  }

  std::unique_ptr<DicomImage<double, -1>> DicomDirImporter::read_image_block(unsigned int id, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const
  {
      std::unique_ptr<DicomImage<double, -1 >> img = std::make_unique<DicomImage<double, -1 >>();

      if (id >= _pdata->info.size())
      { return img; }

      _check_from_to_dimenson(xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto);
      const auto[nDimExpected, has_x, has_y, has_z, has_t] = _count_expected_image_dimensions(xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto);

      const DicomImageInfos& imgInfo = _pdata->info[id];
      int nDim = _count_image_dimensions(imgInfo);

      if (nDim == 0 || nDimExpected > nDim)
      { return img; }

      #ifdef BK_EMIT_PROGRESS
      Progress& prog = bk_progress.emplace_task(imgInfo.id_file_end - imgInfo.id_file_start + 1, ___("Reading DICOM image @0", bk::string_utils::from_number(id)));
      #endif

      nDim = nDimExpected;

      _setup_image(imgInfo, img, has_x, has_y, has_z, has_t);

      const int nPixelsPerSlice = imgInfo.Rows * imgInfo.Columns;
      const int nBytesPerPixel = imgInfo.BitsAllocated / 8; // bit to byte

      #ifdef BK_EMIT_PROGRESS
      prog.increment(1);
      #endif

      const bool littleEndian = imgInfo.HighBit != 0;
      //const bool littleEndian = imgInfo.HighBit == 0;

      /*
       * The files are sorted by SliceLocation and AcquisitionTime.
       * -> s0t0, s0t1, s0t2, s0t3, ..., s1t0, s1t1, s1t2, s1t3, s1t4, s1t5, ...
       */
      #pragma omp parallel for
      for (int i = imgInfo.id_file_start; i < imgInfo.id_file_end; ++i)
      {
          const unsigned int imgNum = static_cast<unsigned int>(i - imgInfo.id_file_start);
          const unsigned int temporalPos = imgNum % imgInfo.TemporalPositions;
          const unsigned int slicePos = imgNum / imgInfo.TemporalPositions;
          //BitVectorX bits;
          //bits.set_size(imgInfo.BitsAllocated);

          if (slicePos >= zfrom && slicePos <= zto && temporalPos >= tfrom && temporalPos <= tto)
          {
              gdcm::ImageReader imgreader;
              imgreader.SetFileName(_pdata->files[i].filename.c_str());

              //if (!imgreader.Read())
              //{ continue; }

              /*
               * This seems to solve the occasional read crashes.
               * Seems like separate instances of gdcm::ImageReader are not thread-safe
               */
              bool validFile = true;
              #pragma omp critical
              { validFile = imgreader.Read(); }
              if (!validFile)
              { continue; }

              const gdcm::Image& imgdata = imgreader.GetImage();
              const unsigned long bufferLength = imgdata.GetBufferLength();

              if (bufferLength == 0)
              { continue; }

              std::vector<char> buffer(bufferLength);
              imgdata.GetBuffer(buffer.data());

              unsigned int rowid = 0;
              unsigned int colid = 0;
              for (int k = 0; k < nPixelsPerSlice; ++k)
              {
                  if (rowid >= xfrom && rowid <= xto && colid >= yfrom && colid <= yto)
                  {
                      const char* valdata = std::addressof(buffer[k * nBytesPerPixel]);
                      unsigned int val = _get_value_from_raw_data(valdata, nBytesPerPixel, littleEndian, imgInfo);
                      _set_image_val(img, val, nDim, has_x, has_y, has_z, has_t, rowid, colid, slicePos, temporalPos);
                  }

                  //++xid;
                  //if (xid >= imgInfo.Columns)
                  //{
                  //    xid = 0;
                  //    ++yid;
                  //}

                  // same as the commented block above; avoiding if
                  colid += static_cast<int>(!static_cast<bool>(++rowid %= imgInfo.Columns));
              }
          }

          #ifdef BK_EMIT_PROGRESS
              #pragma omp critical
          { prog.increment(1); }
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return img;
  }
  /// @}

  /// @{ -------------------------------------------------- READ DICOM IMAGE BYTES
  std::vector<char> DicomDirImporter::read_image_bytes(unsigned int id) const
  {
      std::vector<char> imgbytes;

      if (id >= _pdata->info.size())
      { return imgbytes; }

      const DicomImageInfos& imgInfo = _pdata->info[id];
      const int nDim = _count_image_dimensions(imgInfo);

      if (nDim == 0)
      { return imgbytes; }

      #ifdef BK_EMIT_PROGRESS
      Progress& prog = bk_progress.emplace_task(imgInfo.id_file_end - imgInfo.id_file_start + 1, ___("Reading DICOM image bytes @0", bk::string_utils::from_number(id)));
      #endif

      const int nPixelsPerSlice = imgInfo.Rows * imgInfo.Columns;
      const int nBytesPerPixel = imgInfo.BitsAllocated / 8; // bit to byte

      imgbytes.reserve(nBytesPerPixel * nPixelsPerSlice * std::max(1, imgInfo.Slices) * std::max(1, imgInfo.TemporalPositions));

      #ifdef BK_EMIT_PROGRESS
      prog.increment(1);
      #endif

      //const bool littleEndian = imgInfo.HighBit != 0;

      /*
       * The files are sorted by SliceLocation and AcquisitionTime.
       * -> s0t0, s0t1, s0t2, s0t3, ..., s1t0, s1t1, s1t2, s1t3, s1t4, s1t5, ...
       */
      for (int i = imgInfo.id_file_start; i < imgInfo.id_file_end; ++i)
      {
          gdcm::ImageReader imgreader;
          imgreader.SetFileName(_pdata->files[i].filename.c_str());

          //if (!imgreader.Read())
          //{ continue; }

          /*
           * This seems to solve the occasional read crashes.
           * Seems like separate instances of gdcm::ImageReader are not thread-safe
           */
          bool validFile = true;
          #pragma omp critical
          { validFile = imgreader.Read(); }
          if (!validFile)
          {
              std::vector<char> zeros(nBytesPerPixel * nPixelsPerSlice, 0);
              imgbytes.insert(imgbytes.end(), zeros.begin(), zeros.end());
              std::cerr << "could not read fileID " << i << " (" << _pdata->files[i].filename.c_str() << ")" << std::endl;

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif

              continue;
          }

          const gdcm::Image& imgdata = imgreader.GetImage();
          const unsigned long bufferLength = imgdata.GetBufferLength();

          if (bufferLength == 0)
          {
              std::vector<char> zeros(nBytesPerPixel * nPixelsPerSlice, 0);
              imgbytes.insert(imgbytes.end(), zeros.begin(), zeros.end());
              std::cerr << "could not read fileID " << i << " (" << _pdata->files[i].filename.c_str() << ")" << std::endl;

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif

              continue;
          }

          std::vector<char> buffer(bufferLength);
          imgdata.GetBuffer(buffer.data());

          imgbytes.insert(imgbytes.end(), buffer.begin(), buffer.end());

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return imgbytes;
  }

  std::unique_ptr<DicomImage<double, -1>> DicomDirImporter::read_image_from_bytes(unsigned int id, const std::vector<char>& imgbytes) const
  {
      assert(id < _pdata->info.size());
      const DicomImageInfos& imgInfo = _pdata->info[id];
      return read_image_block_from_bytes(id, imgbytes, 0, imgInfo.Columns - 1, 0, imgInfo.Rows - 1, 0, imgInfo.Slices - 1, 0, imgInfo.TemporalPositions - 1);
  }

  std::unique_ptr<DicomImage<double, -1>> DicomDirImporter::read_image_block_from_bytes(unsigned int id, const std::vector<char>& imgbytes, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const
  {
      std::unique_ptr<DicomImage<double, -1 >> img = std::make_unique<DicomImage<double, -1 >>();

      if (id >= _pdata->info.size() || imgbytes.empty())
      { return img; }

      _check_from_to_dimenson(xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto);
      const auto[nDimExpected, has_x, has_y, has_z, has_t] = _count_expected_image_dimensions(xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto);

      const DicomImageInfos& imgInfo = _pdata->info[id];
      int nDim = _count_image_dimensions(imgInfo);

      if (nDim == 0 || nDimExpected > nDim)
      {
          std::cerr << "DicomDirImporter::read_image_block_from_bytes(id = " << id << ") - nDim == 0 || nDimExpected > nDim FAILED" << std::endl;
          return img;
      }

      #ifdef BK_EMIT_PROGRESS
      Progress& prog = bk_progress.emplace_task(imgInfo.id_file_end - imgInfo.id_file_start + 1, ___("Reading DICOM image bytes @0", bk::string_utils::from_number(id)));
      #endif

      nDim = nDimExpected;

      _setup_image(imgInfo, img, has_x, has_y, has_z, has_t);

      const int nPixelsPerSlice = imgInfo.Rows * imgInfo.Columns;
      const int nBytesPerPixel = imgInfo.BitsAllocated / 8; // bit to byte

      //if (imgbytes.size() < nBytesPerPixel * img->num_values())
      //{
      //    #ifdef BK_EMIT_PROGRESS
      //    prog.set_finished();
      //    #endif
      //
      //    std::cerr << "DicomDirImporter::read_image_block_from_bytes(id = " << id << ") - imgbytes.size() [" << imgbytes.size() << "] < nBytesPerPixel [" << nBytesPerPixel << "] * img->num_values() [" << img->num_values() << "] [= " << nBytesPerPixel * img->num_values() << "] FAILED" << std::endl;
      //
      //    return img;
      //}

      #ifdef BK_EMIT_PROGRESS
      prog.increment(1);
      #endif

      const bool littleEndian = imgInfo.HighBit != 0;

      /*
       * The files are sorted by SliceLocation and AcquisitionTime.
       * -> s0t0, s0t1, s0t2, s0t3, ..., s1t0, s1t1, s1t2, s1t3, s1t4, s1t5, ...
       */
      #pragma omp parallel for
      for (int i = imgInfo.id_file_start; i < imgInfo.id_file_end; ++i)
      {
          const unsigned int imgNum = static_cast<unsigned int>(i - imgInfo.id_file_start);
          const unsigned int temporalPos = imgNum % imgInfo.TemporalPositions;
          const unsigned int slicePos = imgNum / imgInfo.TemporalPositions;

          if (slicePos >= zfrom && slicePos <= zto && temporalPos >= tfrom && temporalPos <= tto)
          {
              const unsigned int off = nBytesPerPixel * nPixelsPerSlice * (temporalPos + slicePos * imgInfo.TemporalPositions);
              unsigned int rowid = 0;
              unsigned int colid = 0;
              for (int k = 0; k < nPixelsPerSlice; ++k)
              {
                  if (rowid >= xfrom && rowid <= xto && colid >= yfrom && colid <= yto)
                  {
                      const char* valdata = std::addressof(imgbytes[off + k * nBytesPerPixel]);
                      unsigned int val = _get_value_from_raw_data(valdata, nBytesPerPixel, littleEndian, imgInfo);
                      _set_image_val(img, val, nDim, has_x, has_y, has_z, has_t, rowid, colid, slicePos, temporalPos);
                  }

                  //++xid;
                  //if (xid >= imgInfo.Columns)
                  //{
                  //    xid = 0;
                  //    ++yid;
                  //}

                  // same as the commented block above; avoiding if
                  colid += static_cast<int>(!static_cast<bool>(++rowid %= imgInfo.Columns));
              }
          }

          #ifdef BK_EMIT_PROGRESS
              #pragma omp critical
          { prog.increment(1); }
          #endif
      }

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif

      return img;
  }
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- SAVE
  bool DicomDirImporter::save(std::string_view filepath) const
  {
      using file_size_type = std::uint16_t;

      std::ofstream file(filepath.data(), std::ios_base::binary | std::ios_base::out);

      if (file.good())
      {
          bk::string_utils::write_string_to_binary_file(_pdata->dir, file);
          bk::string_utils::write_string_to_binary_file(_pdata->dataset_name, file);

          // files
          file_size_type temp = _pdata->files.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          { _pdata->files[i].save(file); }

          // image info
          temp = _pdata->info.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          { _pdata->info[i].save(file); }

          // image2d group
          temp = _pdata->images_2d.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          {
              const std::pair<Vec2ui, std::vector<unsigned int>>& i2d = _pdata->images_2d[i];

              // grid size of group
              file_size_type temp2 = i2d.first[0];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i2d.first[1];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              // num elements in group
              temp2 = i2d.second.size();
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  const file_size_type temp3 = i2d.second[k];
                  file.write(reinterpret_cast<const char*>(&temp3), sizeof(file_size_type));
              }
          }

          // image2dt group
          temp = _pdata->images_2dt.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          {
              const std::pair<Vec3ui, std::vector<unsigned int>>& i2dt = _pdata->images_2dt[i];

              // grid size of group
              file_size_type temp2 = i2dt.first[0];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i2dt.first[1];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i2dt.first[2];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              // num elements in group
              temp2 = i2dt.second.size();
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  const file_size_type temp3 = i2dt.second[k];
                  file.write(reinterpret_cast<const char*>(&temp3), sizeof(file_size_type));
              }
          }

          // image3d group
          temp = _pdata->images_3d.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          {
              const std::pair<Vec3ui, std::vector<unsigned int>>& i3d = _pdata->images_3d[i];

              // grid size of group
              file_size_type temp2 = i3d.first[0];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i3d.first[1];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i3d.first[2];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              // num elements in group
              temp2 = i3d.second.size();
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  const file_size_type temp3 = i3d.second[k];
                  file.write(reinterpret_cast<const char*>(&temp3), sizeof(file_size_type));
              }
          }

          // image3dt group
          temp = _pdata->images_3dt.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          for (unsigned int i = 0; i < temp; ++i)
          {
              const std::pair<Vec4ui, std::vector<unsigned int>>& i3dt = _pdata->images_3dt[i];

              // grid size of group
              file_size_type temp2 = i3dt.first[0];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i3dt.first[1];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i3dt.first[2];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));
              temp2 = i3dt.first[3];
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              // num elements in group
              temp2 = i3dt.second.size();
              file.write(reinterpret_cast<const char*>(&temp2), sizeof(file_size_type));

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  const file_size_type temp3 = i3dt.second[k];
                  file.write(reinterpret_cast<const char*>(&temp3), sizeof(file_size_type));
              }
          }

          save_impl(file);

          file.close();
          return true;
      }

      return false;
  }

  void DicomDirImporter::save_impl(std::ofstream& /*file*/) const
  { /* overridden by derived class if necessary */ }

  bool DicomDirImporter::save_dcm_image_bytes(unsigned int id, std::string_view filepath) const
  { return save_dcm_image_bytes(read_image_bytes(id), filepath); }

  bool DicomDirImporter::save_dcm_image_bytes(const std::vector<char>& bytes, std::string_view filepath) const
  {
      std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

      if (!file.good())
      { return false; }

      file.write(bytes.data(), bytes.size() * sizeof(char));
      file.close();

      return true;
  }
  /// @}

  /// @{ -------------------------------------------------- LOAD
  bool DicomDirImporter::load(std::string_view filepath)
  {
      using file_size_type = std::uint16_t;

      std::ifstream file(filepath.data(), std::ios_base::binary | std::ios_base::in);

      if (file.good())
      {
          _pdata->dir = bk::string_utils::read_string_from_binary_file(file);
          _pdata->dataset_name = bk::string_utils::read_string_from_binary_file(file);

          // files
          file_size_type temp = 0;
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->files.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          { _pdata->files[i].load(file); }

          // image info
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->info.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          { _pdata->info[i].load(file); }

          // image2d group
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->images_2d.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          {
              std::pair<Vec2ui, std::vector<unsigned int>>& i2d = _pdata->images_2d[i];

              // grid size of group
              file_size_type temp2 = 0;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2d.first[0] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2d.first[1] = temp2;

              // num elements in group
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2d.second.resize(temp2);

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  file_size_type temp3 = 0;
                  file.read(reinterpret_cast<char*>(&temp3), sizeof(file_size_type));
                  i2d.second[k] = temp3;
              }
          }

          // image2dt group
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->images_2dt.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          {
              std::pair<Vec3ui, std::vector<unsigned int>>& i2dt = _pdata->images_2dt[i];

              // grid size of group
              file_size_type temp2 = 0;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2dt.first[0] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2dt.first[1] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2dt.first[2] = temp2;

              // num elements in group
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i2dt.second.resize(temp2);

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  file_size_type temp3 = 0;
                  file.read(reinterpret_cast<char*>(&temp3), sizeof(file_size_type));
                  i2dt.second[k] = temp3;
              }
          }

          // image3d group
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->images_3d.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          {
              std::pair<Vec3ui, std::vector<unsigned int>>& i3d = _pdata->images_3d[i];

              // grid size of group
              file_size_type temp2 = 0;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3d.first[0] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3d.first[1] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3d.first[2] = temp2;

              // num elements in group
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3d.second.resize(temp2);

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  file_size_type temp3 = 0;
                  file.read(reinterpret_cast<char*>(&temp3), sizeof(file_size_type));
                  i3d.second[k] = temp3;
              }
          }

          // image3dt group
          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->images_3dt.resize(temp);
          for (unsigned int i = 0; i < temp; ++i)
          {
              std::pair<Vec4ui, std::vector<unsigned int>>& i3dt = _pdata->images_3dt[i];

              // grid size of group
              file_size_type temp2 = 0;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3dt.first[0] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3dt.first[1] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3dt.first[2] = temp2;
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3dt.first[3] = temp2;

              // num elements in group
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));
              i3dt.second.resize(temp2);

              for (unsigned int k = 0; k < temp2; ++k)
              {
                  file_size_type temp3 = 0;
                  file.read(reinterpret_cast<char*>(&temp3), sizeof(file_size_type));
                  i3dt.second[k] = temp3;
              }
          }

          load_impl(file);

          file.close();
          return true;
      }

      return false;
  }

  void DicomDirImporter::load_impl(std::ifstream& /*file*/)
  { /* overridden by derived class if necessary */ }

  std::vector<char> DicomDirImporter::load_dcm_image_bytes(std::string_view filepath, bool* success) const
  {
      const unsigned int filesize = std::filesystem::file_size(filepath);
      std::vector<char> bytes(filesize);

      std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);
      file.read(bytes.data(), filesize * sizeof(char));
      file.close();

      if (success != nullptr)
      { *success = filesize != 0; }

      return bytes;
  }
  /// @}
} // namespace bk