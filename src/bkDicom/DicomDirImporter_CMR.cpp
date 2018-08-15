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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <utility>

#ifdef BK_EMIT_PROGRESS
    #include "../../include/bkTools/Localization"
    #include "../../include/bkTools/Progress"
#endif

#include "../../include/bkTools/ThreadPool"
#include "../../include/bkDicom/DicomDirImporter_CMR.h"
#include "../../include/bkDicom/DicomImageInfos.h"
#include "../../include/bkDicom/EFlowImageOrdering.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct DicomDirImporter_CMR::Impl
  {
      std::map<unsigned int /*dcm_img_id*/, details::DicomImageClass> classification;
      details::FlowImageOrdering flow_image_order;
      double venc_3dt_in_m_per_s;
      double venc_2dt_in_m_per_s;

      Impl()
          : flow_image_order(details::FlowImageOrdering::XYZ),
            venc_3dt_in_m_per_s(1),
            venc_2dt_in_m_per_s(1)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = default;
      Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  DicomDirImporter_CMR::DicomDirImporter_CMR()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  DicomDirImporter_CMR::DicomDirImporter_CMR(const self_type& other)
      : base_type(other),
        _pdata(new Impl(*other._pdata.get()))
  { /* do nothing */ }

  DicomDirImporter_CMR::DicomDirImporter_CMR(self_type&&) = default;

  DicomDirImporter_CMR::DicomDirImporter_CMR(const std::string& dir)
      : base_type(dir),
        _pdata(std::make_unique<Impl>())
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  DicomDirImporter_CMR::~DicomDirImporter_CMR() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET IDS OF IMAGE CLASS
  std::vector<unsigned int> DicomDirImporter_CMR::ids_of_image_class(details::DicomImageClass tag) const
  {
      std::vector<unsigned int> ids;
      for (auto it = _pdata->classification.begin(); it != _pdata->classification.end(); ++it)
      {
          if (it->second == tag)
          { ids.push_back(it->first); }
      }

      return ids;
  }

  std::vector<unsigned int> DicomDirImporter_CMR::class_3dt_flow_images(bool sort_xyz) const
  {
      std::vector<unsigned int> ids = ids_of_image_class(details::DicomImageClass::FlowImage_3DT);

      if (sort_xyz)
      {
          std::sort(ids.begin(), ids.end()); // corresponds to xyz ordering

          if (flow_images_are_ordered_xzy())
          {
              std::swap(ids[1], ids[2]); // xyz -> xzy
          }
          else if (flow_images_are_ordered_yxz())
          {
              std::swap(ids[0], ids[1]); // xyz -> yxz
          }
          else if (flow_images_are_ordered_yzx())
          {
              std::swap(ids[0], ids[1]); // xyz -> yxz
              std::swap(ids[1], ids[2]); // yxz -> yzx
          }
          else if (flow_images_are_ordered_zxy())
          {
              std::swap(ids[0], ids[2]); // xyz -> zyx
              std::swap(ids[1], ids[2]); // zyx -> zxy
          }
          else if (flow_images_are_ordered_zyx())
          {
              std::reverse(ids.begin(), ids.end()); // xyz -> zyx
          }
      }

      return ids;
  }

  std::vector<unsigned int> DicomDirImporter_CMR::class_3dt_magnitude_images(bool sort_xyz) const
  {
      // the magnitude images should have the same ordering as the flow images

      std::vector<unsigned int> ids = ids_of_image_class(details::DicomImageClass::MagnitudeImage_3DT);

      if (sort_xyz)
      {
          std::sort(ids.begin(), ids.end()); // corresponds to xyz ordering

          if (flow_images_are_ordered_xzy())
          {
              std::swap(ids[1], ids[2]); // xyz -> xzy
          }
          else if (flow_images_are_ordered_yxz())
          {
              std::swap(ids[0], ids[1]); // xyz -> yxz
          }
          else if (flow_images_are_ordered_yzx())
          {
              std::swap(ids[0], ids[1]); // xyz -> yxz
              std::swap(ids[1], ids[2]); // yxz -> yzx
          }
          else if (flow_images_are_ordered_zxy())
          {
              std::swap(ids[0], ids[2]); // xyz -> zyx
              std::swap(ids[1], ids[2]); // zyx -> zxy
          }
          else if (flow_images_are_ordered_zyx())
          {
              std::reverse(ids.begin(), ids.end()); // xyz -> zyx
          }
      }

      return ids;
  }

  std::vector<unsigned int> DicomDirImporter_CMR::class_3dt_anatomical_images() const
  { return ids_of_image_class(details::DicomImageClass::AnatomicalImage_3DT); }

  std::vector<unsigned int> DicomDirImporter_CMR::class_3dt_signal_intensity_images() const
  { return ids_of_image_class(details::DicomImageClass::SignalIntensityImage_3DT); }

  std::vector<unsigned int> DicomDirImporter_CMR::class_3d_anatomical_images() const
  { return ids_of_image_class(details::DicomImageClass::AnatomicalImage_3D); }

  std::vector<unsigned int> DicomDirImporter_CMR::class_2dt_flow_images() const
  { return ids_of_image_class(details::DicomImageClass::FlowImage_2DT); }

  std::vector<unsigned int> DicomDirImporter_CMR::class_2dt_anatomical_images() const
  { return ids_of_image_class(details::DicomImageClass::AnatomicalImage_2DT); }

  std::vector<unsigned int> DicomDirImporter_CMR::class_2d_anatomical_images() const
  { return ids_of_image_class(details::DicomImageClass::AnatomicalImage_2D); }
  /// @}

  /// @{ -------------------------------------------------- IS CLASS
  bool DicomDirImporter_CMR::is_class(unsigned int dcm_img_id, details::DicomImageClass tag) const
  {
      const auto it = _pdata->classification.find(dcm_img_id);
      return it != _pdata->classification.end() ? it->second == tag : false;
  }

  bool DicomDirImporter_CMR::is_3dt_flow_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::FlowImage_3DT); }

  bool DicomDirImporter_CMR::is_3dt_magnitude_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::MagnitudeImage_3DT); }

  bool DicomDirImporter_CMR::is_3dt_anatomical_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_3DT); }

  bool DicomDirImporter_CMR::is_3dt_signal_intensity_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::SignalIntensityImage_3DT); }

  bool DicomDirImporter_CMR::is_3d_anatomical_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_3D); }

  bool DicomDirImporter_CMR::is_2dt_flow_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::FlowImage_2DT); }

  bool DicomDirImporter_CMR::is_2dt_anatomical_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_2DT); }

  bool DicomDirImporter_CMR::is_2d_anatomical_image(unsigned int dcm_img_id) const
  { return is_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_2D); }
  /// @}

  /// @{ -------------------------------------------------- GET FLOW IMAGE ORDER
  bool DicomDirImporter_CMR::flow_images_are_ordered_xyz() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::XYZ; }

  bool DicomDirImporter_CMR::flow_images_are_ordered_xzy() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::XZY; }

  bool DicomDirImporter_CMR::flow_images_are_ordered_yxz() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::YXZ; }

  bool DicomDirImporter_CMR::flow_images_are_ordered_yzx() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::YZX; }

  bool DicomDirImporter_CMR::flow_images_are_ordered_zxy() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::ZXY; }

  bool DicomDirImporter_CMR::flow_images_are_ordered_zyx() const
  { return _pdata->flow_image_order == details::FlowImageOrdering::ZYX; }
  /// @}

  /// @{ -------------------------------------------------- GET VENC
  double DicomDirImporter_CMR::venc_3dt_in_m_per_s() const
  { return _pdata->venc_3dt_in_m_per_s; }

  double DicomDirImporter_CMR::venc_2dt_in_m_per_s() const
  { return _pdata->venc_2dt_in_m_per_s; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- ADD IMAGE TO CLASS
  bool DicomDirImporter_CMR::add_to_class(unsigned int dcm_img_id, details::DicomImageClass tag)
  { return _pdata->classification.insert(std::pair<unsigned int, details::DicomImageClass>(dcm_img_id, tag)).second; }

  bool DicomDirImporter_CMR::add_3dt_flow_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::FlowImage_3DT); }

  bool DicomDirImporter_CMR::add_3dt_magnitude_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::MagnitudeImage_3DT); }

  bool DicomDirImporter_CMR::add_3dt_anatomical_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_3DT); }

  bool DicomDirImporter_CMR::add_3dt_signal_intensity_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::SignalIntensityImage_3DT); }

  bool DicomDirImporter_CMR::add_3d_anatomical_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_3D); }

  bool DicomDirImporter_CMR::add_2dt_flow_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::FlowImage_2DT); }

  bool DicomDirImporter_CMR::add_2dt_anatomical_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_2DT); }

  bool DicomDirImporter_CMR::add_2d_anatomical_image(unsigned int dcm_img_id)
  { return add_to_class(dcm_img_id, details::DicomImageClass::AnatomicalImage_2D); }
  /// @}

  /// @{ -------------------------------------------------- SET FLOW IMAGE ORDERING
  void DicomDirImporter_CMR::set_flow_image_ordering_xyz()
  { _pdata->flow_image_order = details::FlowImageOrdering::XYZ; }

  void DicomDirImporter_CMR::set_flow_image_ordering_xzy()
  { _pdata->flow_image_order = details::FlowImageOrdering::XZY; }

  void DicomDirImporter_CMR::set_flow_image_ordering_yxz()
  { _pdata->flow_image_order = details::FlowImageOrdering::YXZ; }

  void DicomDirImporter_CMR::set_flow_image_ordering_yzx()
  { _pdata->flow_image_order = details::FlowImageOrdering::YZX; }

  void DicomDirImporter_CMR::set_flow_image_ordering_zxy()
  { _pdata->flow_image_order = details::FlowImageOrdering::ZXY; }

  void DicomDirImporter_CMR::set_flow_image_ordering_zyx()
  { _pdata->flow_image_order = details::FlowImageOrdering::ZYX; }
  /// @}

  /// @{ -------------------------------------------------- SET VENC
  void DicomDirImporter_CMR::set_venc_3dt_in_m_per_s(double venc_3dt_in_m_per_s)
  { _pdata->venc_3dt_in_m_per_s = std::max(0.01, venc_3dt_in_m_per_s); }

  void DicomDirImporter_CMR::set_venc_2dt_in_m_per_s(double venc_2dt_in_m_per_s)
  { _pdata->venc_2dt_in_m_per_s = std::max(0.01, venc_2dt_in_m_per_s); }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto DicomDirImporter_CMR::operator=(const self_type& other) -> self_type&
  {
      _pdata.reset(new Impl(*other._pdata.get()));
      return *this;
  }

  auto DicomDirImporter_CMR::operator=(self_type&&) -> self_type& = default;
  /// @}


  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void DicomDirImporter_CMR::clear_impl()
  {
      clear_classification();
      set_flow_image_ordering_xyz();
      _pdata->venc_3dt_in_m_per_s = 1;
      _pdata->venc_2dt_in_m_per_s = 1;
  }

  void DicomDirImporter_CMR::clear_classification()
  { _pdata->classification.clear(); }
  /// @}

  /// @{ -------------------------------------------------- HELPER: EVAL STDEV IN CORNERS
  double DicomDirImporter_CMR::eval_corner_tstdev(unsigned int id, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const
  { return eval_corner_tstdev(read_image_block(id, xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto), xfrom, xto, yfrom, yto, zfrom, zto, tfrom, tto); }

  double DicomDirImporter_CMR::eval_corner_tstdev(const std::unique_ptr<DicomImage<double, -1>>& img, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const
  {
      double tstdev_sum = 0;

      for (unsigned int x = xfrom; x <= xto; ++x)
      {
          for (unsigned int y = yfrom; y <= yto; ++y)
          {
              for (unsigned int z = zfrom; z <= zto; ++z)
              {
                  double mean = 0;
                  double tstdev = 0;

                  for (unsigned int t = tfrom; t <= tto; ++t)
                  { mean += img->operator()(x, y, z, t); }

                  mean /= (tto - tfrom + 1);

                  for (unsigned int t = 0; t <= tto; ++t)
                  {
                      const double temp = img->operator()(x, y, z, t) - mean;
                      tstdev = temp * temp;
                  }

                  tstdev /= (tto - tfrom + 1);
                  tstdev_sum += tstdev;
              }
          }
      }

      return tstdev_sum / ((xto - xfrom + 1) * (yto - yfrom + 1) * (zto - zfrom + 1));
  }
  /// @}

  /// @{ -------------------------------------------------- DETERMINE FLOW IMAGES
  void DicomDirImporter_CMR::determine_flow_images(int CORNER_PORTION)
  {
      std::vector<std::vector<std::pair<unsigned int, double >>> tstdev(this->num_image_3d_plus_time_groups());

      #ifdef BK_EMIT_PROGRESS
      int maxProg = 0;

      for (unsigned int i = 0; i < num_image_3d_plus_time_groups(); ++i)
      { maxProg += 1 /*other*/ + static_cast<int>(image_3d_plus_time_group(i).size()) /*1 for each corner scan*/; }

      Progress& prog = bk_progress.emplace_task(std::max(1, maxProg), ___("searching flow images"));
      #endif

      //------------------------------------------------------------------------------------------------------
      // evaluate the corner temporal standard deviations for each pixel in the 8 corners
      //    - This is based on the assumption that the image corners are regions outside the body, i.e. air.
      //      Air in the 4d pc-mri scans is extremely noisy and should allow to identify the flow images.
      //    - From manual experiments, the tstdev is 10^3 to 10^4 times higher in the flow images.
      //------------------------------------------------------------------------------------------------------

      bk::ThreadPool threadpool(8);

      for (unsigned int i = 0; i < num_image_3d_plus_time_groups(); ++i)
      {
          const std::vector<unsigned int>& ids = image_3d_plus_time_group(i);
          const unsigned int group_size = ids.size();

          // at least 3 (flow) images in the group are required
          if (ids.size() < 3)
          {
              #ifdef BK_EMIT_PROGRESS
              prog.increment(group_size + 1);
              #endif

              continue;
          }

          const auto& grid_size = image_3d_plus_time_group_grid_size(i);

          Vec3ui corner_size = grid_size.sub_vector<0, 2>() / CORNER_PORTION;
          corner_size.max_cwise_internal(2);

          constexpr const unsigned int xmin0 = 0;
          const unsigned int xmax0 = corner_size[0] - 1;
          const unsigned int xmin1 = grid_size[0] - corner_size[0];
          const unsigned int xmax1 = grid_size[0] - 1;

          constexpr const unsigned int ymin0 = 0;
          const unsigned int ymax0 = corner_size[1] - 1;
          const unsigned int ymin1 = grid_size[1] - corner_size[1];
          const unsigned int ymax1 = grid_size[1] - 1;

          constexpr const unsigned int zmin0 = 0;
          const unsigned int zmax0 = corner_size[2] - 1;
          const unsigned int zmin1 = grid_size[2] - corner_size[2];
          const unsigned int zmax1 = grid_size[2] - 1;

          constexpr const unsigned int tmin = 0;
          const unsigned int tmax = grid_size[3] - 1;

          for (unsigned int k = 0; k < group_size; ++k)
          {
              const auto img0 = this->read_image_block(ids[k], xmin0, xmax0, ymin0, ymax0, zmin0, zmax0, tmin, tmax);
              std::future<double> fut0 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img0, xmin0, xmax0, ymin0, ymax0, zmin0, zmax0, tmin, tmax); });

              const auto img1 = this->read_image_block(ids[k], xmin0, xmax0, ymin0, ymax0, zmin1, zmax1, tmin, tmax);
              std::future<double> fut1 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img1, xmin0, xmax0, ymin0, ymax0, zmin1, zmax1, tmin, tmax); });

              const auto img2 = this->read_image_block(ids[k], xmin0, xmax0, ymin1, ymax1, zmin0, zmax0, tmin, tmax);
              std::future<double> fut2 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img2, xmin0, xmax0, ymin1, ymax1, zmin0, zmax0, tmin, tmax); });

              const auto img3 = this->read_image_block(ids[k], xmin0, xmax0, ymin1, ymax1, zmin1, zmax1, tmin, tmax);
              std::future<double> fut3 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img3, xmin0, xmax0, ymin1, ymax1, zmin1, zmax1, tmin, tmax); });

              const auto img4 = this->read_image_block(ids[k], xmin1, xmax1, ymin0, ymax0, zmin0, zmax0, tmin, tmax);
              std::future<double> fut4 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img4, xmin1, xmax1, ymin0, ymax0, zmin0, zmax0, tmin, tmax); });

              const auto img5 = this->read_image_block(ids[k], xmin1, xmax1, ymin0, ymax0, zmin1, zmax1, tmin, tmax);
              std::future<double> fut5 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img5, xmin1, xmax1, ymin0, ymax0, zmin1, zmax1, tmin, tmax); });

              const auto img6 = this->read_image_block(ids[k], xmin1, xmax1, ymin1, ymax1, zmin0, zmax0, tmin, tmax);
              std::future<double> fut6 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img6, xmin1, xmax1, ymin1, ymax1, zmin0, zmax0, tmin, tmax); });

              const auto img7 = this->read_image_block(ids[k], xmin1, xmax1, ymin1, ymax1, zmin1, zmax1, tmin, tmax);
              std::future<double> fut7 = threadpool.enqueue([&]()
                                                            { return eval_corner_tstdev(img7, xmin1, xmax1, ymin1, ymax1, zmin1, zmax1, tmin, tmax); });
              const double tstdev_sum = (fut0.get() + fut1.get() + fut2.get() + fut3.get() + fut4.get() + fut5.get() + fut6.get() + fut7.get()) / 8; // num corners

              tstdev[i].push_back(std::make_pair(ids[k], tstdev_sum));

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif
          }

          // sort images by their id
          // -> removal of duplicates always removes the higher id
          std::sort(tstdev[i].begin(), tstdev[i].end(), [](const std::pair<unsigned int, double>& x, const std::pair<unsigned int, double>& y)
          { return x.first < y.first; });

          // sort images by their stdev
          std::sort(tstdev[i].begin(), tstdev[i].end(), [](const std::pair<unsigned int, double>& x, const std::pair<unsigned int, double>& y)
          { return x.second < y.second; });

          // remove images that have exactly the same stdev -> they are probably duplicates
          tstdev[i].erase(std::unique(tstdev[i].begin(), tstdev[i].end(), [](const auto& x, const auto& y)
          { return x.second == y.second; }), tstdev[i].end());

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif
      }

      //------------------------------------------------------------------------------------------------------
      // Now determine the flow image ids (w.r.t. the DICOM data)
      //------------------------------------------------------------------------------------------------------

      // iteration: same-sized groups
      for (unsigned int i = 0; i < tstdev.size(); ++i)
      {
          const unsigned int N = tstdev[i].size();

          if (N < 3) // should always be 0 or >= 3
          { continue; }

          std::vector<unsigned int> ids;
          ids.reserve(3);

          // exactly 3 images => should be the flow images
          if (N == 3)
          {
              ids.push_back(tstdev[i][N - 1].first);
              ids.push_back(tstdev[i][N - 2].first);
              ids.push_back(tstdev[i][N - 3].first);

              // flow images are either xyz or zyx in patient coordinate system
              std::sort(ids.begin(), ids.end());

              add_3dt_flow_image(ids[0]);
              add_3dt_flow_image(ids[1]);
              add_3dt_flow_image(ids[2]);
              continue;
          }

          // more than 3 images

          // find min stdev
          const auto minstdev = std::min_element(tstdev[i].begin(), tstdev[i].end(), [](const auto& x, const auto& y)
          { return x.second < y.second; })->second;
          const auto thresh = 100 * minstdev; // todo: options

          // are there 3 images with a significantly higher stdev than the rest?
          for (unsigned int k = 0; k < tstdev[i].size(); ++k)
          {
              if (tstdev[i][k].second >= thresh)
              { ids.push_back(tstdev[i][k].first); }
          }
          if (ids.size() == 3)
          {
              // exactly 3 images with significantly higher stdev were found => should be the flow images

              // flow images are either xyz or zyx in patient coordinate system
              std::sort(ids.begin(), ids.end());
              //_3dt_flow_images.emplace_back(i, std::move(ids));
              //add_3dt_flow_image_triplet(ids);
              add_3dt_flow_image(ids[0]);
              add_3dt_flow_image(ids[1]);
              add_3dt_flow_image(ids[2]);
              continue;
          }
          else
          {
              // more than 3 images with high stdev

              ids.clear();

              // search for all potential flow images, i.e., images with a very high stdev
              for (unsigned int k = 0; k < tstdev[i].size(); ++k)
              {
                  if (tstdev[i][k].second >= image_infos(tstdev[i][k].first).LargestImagePixelValue)
                  {
                      // - the image has huge deviations
                      // - the deviations of the signal intensities and magnitude images are much smaller (experimentally determined)
                      ids.push_back(tstdev[i][k].first);
                  }
              }

              if (ids.size() < 3)
              {
                  // there are probably no flow images in this group
                  continue;
              }

              // search for groups of sequenceNames (non-empty duplicates)
              std::unordered_map<std::string, std::vector<unsigned int>, string_utils::hash_obj> snames;

              for (unsigned int k = 0; k < tstdev[i].size(); ++k)
              {
                  std::string sn = image_infos(tstdev[i][k].first).SequenceName;
                  if (sn.empty())
                  { sn = image_infos(tstdev[i][k].first).SequenceName_Private; }
                  if (!sn.empty())
                  {
                      auto it = snames.find(sn);
                      if (it == snames.end())
                      { it = snames.insert(std::make_pair(std::move(sn), std::vector<unsigned int>())).first; }
                      it->second.push_back(tstdev[i][k].first);
                  }
              }

              if (snames.size() == 3)
              {
                  // there are exactly 3 groups of identical sequenceNames
                  // => take the image with the smaller id from each group

                  for (auto it = snames.begin(); it != snames.end(); ++it)
                  { ids.push_back(*std::min_element(it->second.begin(), it->second.end())); }

                  // flow images are either xyz or zyx in patient coordinate system
                  std::sort(ids.begin(), ids.end());
                  //_3dt_flow_images.emplace_back(i, std::move(ids));
                  //add_3dt_flow_image_triplet(ids);
                  add_3dt_flow_image(ids[0]);
                  add_3dt_flow_image(ids[1]);
                  add_3dt_flow_image(ids[2]);
                  continue;
              }
              else
              {
                  // search for image triplets with successive indices

                  // flow images are either xyz or zyx in patient coordinate system
                  std::sort(ids.begin(), ids.end());

                  for (unsigned int k = 0; k < ids.size() - 2; ++k) // ids.size() can only be >= 3 here
                  {
                      if (ids[k + 1] == ids[k] + 1 && ids[k + 2] == ids[k] + 2)
                      {
                          ids = {ids[k], ids[k + 1], ids[k + 2]};
                          break;
                      }
                  }

                  if (ids.size() != 3)
                  {
                      // no image triplets with successive indices were found
                      // => search for 2-successive images (e.g. 0,2,4)

                      for (unsigned int k = 0; k < ids.size() - 2; ++k) // ids.size() can only be >= 3 here
                      {
                          if (ids[k + 1] == ids[k] + 2 && ids[k + 2] == ids[k] + 4)
                          {
                              ids = {ids[k], ids[k + 1], ids[k + 2]};
                              break;
                          }
                      }

                      if (ids.size() != 3)
                      {
                          // => guess and take the first 3 images
                          ids = {ids[0], ids[1], ids[2]};
                      }
                  }

                  //std::sort(ids.begin(), ids.end()); // already sorted in this subroutine
                  add_3dt_flow_image(ids[0]);
                  add_3dt_flow_image(ids[1]);
                  add_3dt_flow_image(ids[2]);
              }
          }
      } // for i: same-sized groups

      #ifdef BK_EMIT_PROGRESS
      prog.set_finished();
      #endif
  }
  /// @}

  /// @{ -------------------------------------------------- GUESS 2D+TIME IMAGES TEMPORAL RESOLUTION
  void DicomDirImporter_CMR::guess_2dt_from_4dt_temporal_resolution()
  {
      const unsigned int n2DTGroups = this->num_image_2d_plus_time_groups();

      if (n2DTGroups == 0)
      { return; }

      double avg_heartbeat_ms = 0;
      unsigned int cnt = 0;
      const unsigned int n3DTGroups = this->num_image_3d_plus_time_groups();

      for (unsigned int i = 0; i < n3DTGroups; ++i)
      {
          const auto group = this->image_3d_plus_time_group(i);
          const unsigned int nImages = group.size();

          for (unsigned int k = 0; k < nImages; ++k)
          {
              const unsigned int imgID = group[k];

              if (image_infos(imgID).TemporalResolution != 0)
              {
                  const double heartbeat_ms = image_infos(imgID).TemporalResolution * image_infos(imgID).TemporalPositions;

                  if (heartbeat_ms > 2000)
                  {
                      // this would correspond to a heart rate of <= 30
                      // => skip implausible value
                      continue;
                  }

                  avg_heartbeat_ms += heartbeat_ms;
                  ++cnt;
              }
          }
      }

      if (cnt == 0)
      { return; }

      avg_heartbeat_ms /= cnt;

      for (unsigned int i = 0; i < n2DTGroups; ++i)
      {
          const auto group = this->image_2d_plus_time_group(i);
          const unsigned int nImages = group.size();

          for (unsigned int k = 0; k < nImages; ++k)
          {
              const unsigned int imgID = group[k];

              if (image_infos(imgID).TemporalResolution == 0 && image_infos(imgID).TemporalPositions > 1)
              { image_infos(imgID).TemporalResolution = avg_heartbeat_ms / image_infos(imgID).TemporalPositions; }
          }
      }
  }
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- SAVE / LOAD
  void DicomDirImporter_CMR::save_impl(std::ofstream& file) const
  {
      assert(file.good());

      if (file.good())
      {
          using file_size_type = std::uint16_t;

          file_size_type temp = _pdata->classification.size();
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));

          for (auto it = _pdata->classification.begin(); it != _pdata->classification.end(); ++it)
          {
              temp = static_cast<file_size_type>(it->first);
              file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));

              temp = static_cast<file_size_type>(it->second);
              file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));
          }

          temp = static_cast<file_size_type>(_pdata->flow_image_order);
          file.write(reinterpret_cast<const char*>(&temp), sizeof(file_size_type));

          // venc
          file.write(reinterpret_cast<const char*>(&_pdata->venc_3dt_in_m_per_s), sizeof(double));
          file.write(reinterpret_cast<const char*>(&_pdata->venc_2dt_in_m_per_s), sizeof(double));
      }
  }

  void DicomDirImporter_CMR::load_impl(std::ifstream& file)
  {
      if (file.good())
      {
          using file_size_type = std::uint16_t;

          file_size_type temp = 0;
          file_size_type temp2 = 0;
          file_size_type nClasses = 0;
          file.read(reinterpret_cast<char*>(&nClasses), sizeof(file_size_type));

          for (file_size_type i = 0; i < nClasses; ++i)
          {
              file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
              file.read(reinterpret_cast<char*>(&temp2), sizeof(file_size_type));

              _pdata->classification.insert(std::pair<unsigned int, details::DicomImageClass>(static_cast<unsigned int>(temp), static_cast<details::DicomImageClass>(temp2)));
          }

          file.read(reinterpret_cast<char*>(&temp), sizeof(file_size_type));
          _pdata->flow_image_order = static_cast<details::FlowImageOrdering>(temp);

          // venc
          file.read(reinterpret_cast<char*>(&_pdata->venc_3dt_in_m_per_s), sizeof(double));
          file.read(reinterpret_cast<char*>(&_pdata->venc_2dt_in_m_per_s), sizeof(double));
      }
  }
  /// @}
} // namespace bk
