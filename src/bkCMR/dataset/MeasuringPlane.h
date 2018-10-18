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

#ifndef BK_MEASURINGPLANE_H
#define BK_MEASURINGPLANE_H

#include <cstdint>

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bk/Matrix>
#include <bk/StringUtils>
#include <bkCMR/dataset/transformation/MeasuringPlaneTransformation.h>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage3DT;
    class Vessel;
    // -------------------- forward declaration END

    class BKCMR_EXPORT MeasuringPlane : public Image<Vec3d, 3, MeasuringPlaneTransformation>
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using base_type = Image<Vec3d, 3, MeasuringPlaneTransformation>;
        using segmentation_type = CartesianImage<std::uint8_t, 3>;

      public:
        /// @{ -------------------------------------------------- ATTRIBUTES
        [[nodiscard]] static constexpr const char* VelocityThroughPlaneAttributeName()
        { return "MeasuringPlane_velocity_through_plane"; }

        [[nodiscard]] static constexpr unsigned long long VelocityThroughPlaneAttributeHash()
        { return bk::string_utils::hash(VelocityThroughPlaneAttributeName()); }
        /// @}

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      private:
        class Impl;

        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR 
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CONSTRUCTORS
        MeasuringPlane();
        MeasuringPlane(const MeasuringPlane&);
        MeasuringPlane(MeasuringPlane&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        ~MeasuringPlane();
        /// @}

        //====================================================================================================
        //===== GETTER 
        //====================================================================================================
        /// @{ -------------------------------------------------- MEMBERS
      private:
        [[nodiscard]] auto _seg() -> segmentation_type&;
      public:
        /// @}

        /// @{ -------------------------------------------------- SEGMENTATION
        [[nodiscard]] auto segmentation_cross_section() const -> const segmentation_type&;
        [[nodiscard]] auto segmentation_cross_section() -> segmentation_type&;
        /// @}

        /// @{ -------------------------------------------------- GET VESSEL ID
        [[nodiscard]] unsigned int vesselID() const;
        /// @}

        /// @{ -------------------------------------------------- GET AREA
        [[nodiscard]] double area_per_cell() const;
        /// @}

        /// @{ -------------------------------------------------- GET FLOW RATE
        [[nodiscard]] const std::vector<double>& flow_rate_per_time() const;
        [[nodiscard]] double min_flow_rate_per_time() const;
        [[nodiscard]] double max_flow_rate_per_time() const;
        [[nodiscard]] double mean_flow_rate_per_time() const;
        [[nodiscard]] double median_flow_rate_per_time() const;
        /// @}

        /// @{ -------------------------------------------------- GET FLOW VOLUMES
        [[nodiscard]] double forward_flow_volume() const;
        [[nodiscard]] double backward_flow_volume() const;
        [[nodiscard]] double net_flow_volume() const;
        [[nodiscard]] double percentaged_back_flow_volume() const;
        [[nodiscard]] double cardiac_output() const;
        /// @}

        /// @{ -------------------------------------------------- VELOCITIES (ABSOLUTE)
        [[nodiscard]] double min_velocity() const;
        [[nodiscard]] double max_velocity() const;
        [[nodiscard]] double mean_velocity() const;
        [[nodiscard]] double median_velocity() const;
        /// @}

        /// @{ -------------------------------------------------- VELOCITIES (THROUGHPLANE)
        [[nodiscard]] double min_velocity_throughPlane() const;
        [[nodiscard]] double max_velocity_throughPlane() const;
        [[nodiscard]] double mean_velocity_throughPlane() const;
        [[nodiscard]] double median_velocity_throughPlane() const;
        /// @}

        /// @{ -------------------------------------------------- AREAL MEAN VELOCITIES PER TIME
        [[nodiscard]] const std::vector<double>& areal_mean_velocity_per_time() const;
        [[nodiscard]] double min_areal_mean_velocity_per_time() const;
        [[nodiscard]] double max_areal_mean_velocity_per_time() const;
        [[nodiscard]] double mean_areal_mean_velocity_per_time() const;
        [[nodiscard]] double median_areal_mean_velocity_per_time() const;
        /// @}

        /// @{ -------------------------------------------------- AREAL MEAN VELOCITIES (THROUGHPLANE) PER TIME
        [[nodiscard]] const std::vector<double>& areal_mean_velocitiy_throughPlane_per_time() const;
        [[nodiscard]] double min_areal_mean_velocity_throughPlane_per_time() const;
        [[nodiscard]] double max_areal_mean_velocity_throughPlane_per_time() const;
        [[nodiscard]] double mean_areal_mean_velocity_throughPlane_per_time() const;
        [[nodiscard]] double median_areal_mean_velocity_throughPlane_per_time() const;
        /// @}

        /// @{ -------------------------------------------------- GET CROSS-SECTION INFOS
      private:
        [[nodiscard]] double _mm_to_cm(double x) const;
        [[nodiscard]] double _mm2_to_cm2(double x) const;
      public:
        [[nodiscard]] const std::vector<double>& area_mm2() const;
        [[nodiscard]] double max_area_mm2() const;
        [[nodiscard]] double max_area_cm2() const;
        [[nodiscard]] double min_area_mm2() const;
        [[nodiscard]] double min_area_cm2() const;
        [[nodiscard]] double mean_area_mm2() const;
        [[nodiscard]] double mean_area_cm2() const;
        [[nodiscard]] double median_area_mm2() const;
        [[nodiscard]] double median_area_cm2() const;
        [[nodiscard]] double diameter_mm() const;
        [[nodiscard]] double diameter_cm() const;
        /// @}

        /// @{ -------------------------------------------------- GET THROUGH PLANE VELOCITY
        [[nodiscard]] double velocity_through_plane(const Vec3ui& gp) const;
        [[nodiscard]] double velocity_through_plane(unsigned int x, unsigned int y, unsigned int t) const;
      private:
        [[nodiscard]] double& velocity_through_plane(const Vec3ui& gp);
        [[nodiscard]] double& velocity_through_plane(unsigned int x, unsigned int y, unsigned int t);
      public:
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- SET SIZE
        void set_size(unsigned int sx, unsigned int sy, unsigned int st = 1);
        /// @}

        /// @{ -------------------------------------------------- SET VESSEL ID
        void set_vesselID(unsigned int vid);
        /// @}

        /// @{ -------------------------------------------------- SET SEGMENTATION
        template<typename Seg_>
        void set_measuring_plane_segmentation(const Seg_& seg)
        {
            _seg().set_size(seg.geometry().size(0), seg.geometry().size(1), seg.geometry().size(2));

            #pragma omp parallel for
            for (unsigned int i = 0; i < seg.num_values(); ++i)
            { _seg()[i] = seg[i] != 0 ? 1 : 0; }
        }
        /// @}

        /// @{ -------------------------------------------------- SET DIAMETER
        void set_diameter_mm(double d_in_mm);
        /// @}

        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] MeasuringPlane& operator=(const MeasuringPlane&);
        [[maybe_unused]] MeasuringPlane& operator=(MeasuringPlane&&) noexcept;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear_plane();
        void sample_from_flow_field(FlowImage3DT& f);
        void sample_segmentation_from_vessel_mesh(Vessel& v);
        void sample_segmentation_from_vessel_segmentation(Vessel& v);
        bool calc_statistics();

        //====================================================================================================
        //===== I/O
        //====================================================================================================
        // todo save/load
    }; // class MeasuringPlane
  } // inline namespace cmr
} // namespace bk

#endif //BK_MEASURINGPLANE_H
