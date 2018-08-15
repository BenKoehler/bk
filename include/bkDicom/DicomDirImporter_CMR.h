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

#ifndef BKDICOM_DICOMDIRIMPORTER_CMR_H
#define BKDICOM_DICOMDIRIMPORTER_CMR_H

#include <map>
#include <memory>

#include "../bkDataset/Image"

#include "DicomDirImporter.h"
#include "EDicomImageClass.h"
#include "lib/bkDicom_export.h"

namespace bk
{
  class BKDICOM_EXPORT DicomDirImporter_CMR : public DicomDirImporter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DicomDirImporter_CMR;
      using base_type = DicomDirImporter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;

      std::unique_ptr <Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      DicomDirImporter_CMR();
      DicomDirImporter_CMR(const self_type& other);
      DicomDirImporter_CMR(self_type&&);
      DicomDirImporter_CMR(const std::string& dir);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DicomDirImporter_CMR();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET IDS OF IMAGE CLASS
      /*!
       * flow and magnitude images are returned in X Y Z order
       * (considering flow image ordering)
       */
    private:
      std::vector<unsigned int> ids_of_image_class(details::DicomImageClass tag) const;
    public:
      std::vector<unsigned int> class_3dt_flow_images(bool sort_xyz = true) const;
      std::vector<unsigned int> class_3dt_magnitude_images(bool sort_xyz = true) const;
      std::vector<unsigned int> class_3dt_anatomical_images() const;
      std::vector<unsigned int> class_3dt_signal_intensity_images() const;
      std::vector<unsigned int> class_3d_anatomical_images() const;
      std::vector<unsigned int> class_2dt_flow_images() const;
      std::vector<unsigned int> class_2dt_anatomical_images() const;
      std::vector<unsigned int> class_2d_anatomical_images() const;
      /// @}

      /// @{ -------------------------------------------------- IS CLASS
    private:
      [[nodiscard]] bool is_class(unsigned int dcm_img_id, details::DicomImageClass tag) const;
    public:
      [[nodiscard]] bool is_3dt_flow_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_3dt_magnitude_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_3dt_anatomical_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_3dt_signal_intensity_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_3d_anatomical_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_2dt_flow_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_2dt_anatomical_image(unsigned int dcm_img_id) const;
      [[nodiscard]] bool is_2d_anatomical_image(unsigned int dcm_img_id) const;
      /// @}

      /// @{ -------------------------------------------------- GET FLOW IMAGE ORDER
      [[nodiscard]] bool flow_images_are_ordered_xyz() const;
      [[nodiscard]] bool flow_images_are_ordered_xzy() const;
      [[nodiscard]] bool flow_images_are_ordered_yxz() const;
      [[nodiscard]] bool flow_images_are_ordered_yzx() const;
      [[nodiscard]] bool flow_images_are_ordered_zxy() const;
      [[nodiscard]] bool flow_images_are_ordered_zyx() const;
      /// @}

      /// @{ -------------------------------------------------- GET VENC
      [[nodiscard]] double venc_3dt_in_m_per_s() const;
      [[nodiscard]] double venc_2dt_in_m_per_s() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- ADD IMAGE TO CLASS
    private:
      [[maybe_unused]] bool add_to_class(unsigned int dcm_img_id, details::DicomImageClass tag);
    public:
      [[maybe_unused]] bool add_3dt_flow_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_3dt_magnitude_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_3dt_anatomical_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_3dt_signal_intensity_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_3d_anatomical_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_2dt_flow_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_2dt_anatomical_image(unsigned int dcm_img_id);
      [[maybe_unused]] bool add_2d_anatomical_image(unsigned int dcm_img_id);
      /// @}

      /// @{ -------------------------------------------------- SET FLOW IMAGE ORDERING
      void set_flow_image_ordering_xyz();
      void set_flow_image_ordering_xzy();
      void set_flow_image_ordering_yxz();
      void set_flow_image_ordering_yzx();
      void set_flow_image_ordering_zxy();
      void set_flow_image_ordering_zyx();
      /// @}

      /// @{ -------------------------------------------------- SET VENC
      void set_venc_3dt_in_m_per_s(double venc_3dt_in_m_per_s);
      void set_venc_2dt_in_m_per_s(double venc_2dt_in_m_per_s);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
    protected:
      void clear_impl() override;
    public:
      void clear_classification();
      /// @}

      /// @{ -------------------------------------------------- HELPER: EVAL STDEV IN CORNERS
    private:
      [[nodiscard]] double eval_corner_tstdev(unsigned int id, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const;
      [[nodiscard]] double eval_corner_tstdev(const std::unique_ptr <DicomImage<double, -1>>& img, unsigned int xfrom, unsigned int xto, unsigned int yfrom, unsigned int yto, unsigned int zfrom, unsigned int zto, unsigned int tfrom, unsigned int tto) const;
    public:
      /// @}

      /// @{ -------------------------------------------------- DETERMINE FLOW IMAGES
      void determine_flow_images(int CORNER_PORTION = 8);
      /// @}

      /// @{ -------------------------------------------------- GUESS 2D+TIME IMAGES TEMPORAL RESOLUTION
      void guess_2dt_from_4dt_temporal_resolution();
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE / LOAD
    protected:
      virtual void save_impl(std::ofstream& file) const override;
      virtual void load_impl(std::ifstream& file) override;
    public:
      /// @}
  }; // class DicomDirImporter_CMR
} // namespace bk

#endif //BKDICOM_DICOMDIRIMPORTER_CMR_H


