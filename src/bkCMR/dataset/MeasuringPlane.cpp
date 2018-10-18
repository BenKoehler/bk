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

#include <bkCMR/dataset/MeasuringPlane.h>
#include <bkCMR/dataset/FlowImage3DT.h>
#include <bkCMR/dataset/Vessel.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include <bk/Algorithm>
#include <bk/Mesh>
#include <bk/ThreadPool>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct MeasuringPlane::Impl
    {
        unsigned int vesselID;
        segmentation_type seg;
        // flow rates/volumes
        std::vector<double> flow_rate_per_time;
        double forward_flow_volume;
        double backward_flow_volume;
        double net_flow_volume;
        double percentaged_back_flow_volume;
        double cardiac_output;
        // flow velocities (abs)
        double max_velocity;
        double min_velocity;
        double mean_velocity;
        double median_velocity;
        // flow velocities (throughPlane)
        double min_velocity_throughPlane;
        double max_velocity_throughPlane;
        double mean_velocity_throughPlane;
        double median_velocity_throughPlane;
        // flow velocities (areal)
        std::vector<double> areal_mean_velocity_per_time;
        std::vector<double> areal_mean_velocity_throughPlane_per_time;
        // cross-section
        std::vector<double> area_mm2;
        double diameter_mm;
        bool normal_is__oriented_in_main_flow_direction;

        Impl()
            : vesselID(0),
              forward_flow_volume(0),
              backward_flow_volume(0),
              net_flow_volume(0),
              percentaged_back_flow_volume(0),
              cardiac_output(0),
              max_velocity(0),
              min_velocity(0),
              mean_velocity(0),
              median_velocity(0),
              min_velocity_throughPlane(0),
              max_velocity_throughPlane(0),
              mean_velocity_throughPlane(0),
              median_velocity_throughPlane(0),
              areal_mean_velocity_per_time(0),
              areal_mean_velocity_throughPlane_per_time(0),
              area_mm2(0),
              diameter_mm(0),
              normal_is__oriented_in_main_flow_direction(true)
        { /* do nothing */ }

        Impl(const Impl&) = default;
        Impl(Impl&&) noexcept = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    MeasuringPlane::MeasuringPlane() = default;
    MeasuringPlane::MeasuringPlane(const MeasuringPlane&) = default;
    MeasuringPlane::MeasuringPlane(MeasuringPlane&&) noexcept = default;
    MeasuringPlane::~MeasuringPlane() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    auto MeasuringPlane::_seg() -> segmentation_type&
    { return _pdata->seg; }

    auto MeasuringPlane::segmentation_cross_section() const -> const segmentation_type&
    { return _pdata->seg; }

    auto MeasuringPlane::segmentation_cross_section() -> segmentation_type&
    { return _pdata->seg; }

    unsigned int MeasuringPlane::vesselID() const
    { return _pdata->vesselID; }

    double MeasuringPlane::area_per_cell() const
    { return this->geometry().transformation().scale(0) * this->geometry().transformation().scale(1); }

    const std::vector<double>& MeasuringPlane::flow_rate_per_time() const
    { return _pdata->flow_rate_per_time; }

    double MeasuringPlane::min_flow_rate_per_time() const
    {
        if (_pdata->flow_rate_per_time.empty())
        { return 0; }

        return *std::min_element(_pdata->flow_rate_per_time.begin(), _pdata->flow_rate_per_time.end());
    }

    double MeasuringPlane::max_flow_rate_per_time() const
    {
        if (_pdata->flow_rate_per_time.empty())
        { return 0; }

        return *std::max_element(_pdata->flow_rate_per_time.begin(), _pdata->flow_rate_per_time.end());
    }

    double MeasuringPlane::mean_flow_rate_per_time() const
    {
        if (_pdata->flow_rate_per_time.empty())
        { return 0; }

        return bk::mean(_pdata->flow_rate_per_time.begin(), _pdata->flow_rate_per_time.end());
    }

    double MeasuringPlane::median_flow_rate_per_time() const
    {
        if (_pdata->flow_rate_per_time.empty())
        { return 0; }

        return bk::median(_pdata->flow_rate_per_time.begin(), _pdata->flow_rate_per_time.end());
    }

    double MeasuringPlane::forward_flow_volume() const
    { return _pdata->forward_flow_volume; }

    double MeasuringPlane::backward_flow_volume() const
    { return _pdata->backward_flow_volume; }

    double MeasuringPlane::net_flow_volume() const
    { return _pdata->net_flow_volume; }

    double MeasuringPlane::percentaged_back_flow_volume() const
    { return _pdata->percentaged_back_flow_volume; }

    double MeasuringPlane::cardiac_output() const
    { return _pdata->cardiac_output; }

    double MeasuringPlane::min_velocity() const
    { return _pdata->min_velocity; }

    double MeasuringPlane::max_velocity() const
    { return _pdata->max_velocity; }

    double MeasuringPlane::mean_velocity() const
    { return _pdata->mean_velocity; }

    double MeasuringPlane::median_velocity() const
    { return _pdata->median_velocity; }

    double MeasuringPlane::min_velocity_throughPlane() const
    { return _pdata->min_velocity_throughPlane; }

    double MeasuringPlane::max_velocity_throughPlane() const
    { return _pdata->max_velocity_throughPlane; }

    double MeasuringPlane::mean_velocity_throughPlane() const
    { return _pdata->mean_velocity_throughPlane; }

    double MeasuringPlane::median_velocity_throughPlane() const
    { return _pdata->median_velocity_throughPlane; }

    const std::vector<double>& MeasuringPlane::areal_mean_velocity_per_time() const
    { return _pdata->areal_mean_velocity_per_time; }

    double MeasuringPlane::min_areal_mean_velocity_per_time() const
    {
        if (_pdata->areal_mean_velocity_per_time.empty())
        { return 0; }

        return *std::min_element(_pdata->areal_mean_velocity_per_time.begin(), _pdata->areal_mean_velocity_per_time.end());
    }

    double MeasuringPlane::max_areal_mean_velocity_per_time() const
    {
        if (_pdata->areal_mean_velocity_per_time.empty())
        { return 0; }

        return *std::max_element(_pdata->areal_mean_velocity_per_time.begin(), _pdata->areal_mean_velocity_per_time.end());
    }

    double MeasuringPlane::mean_areal_mean_velocity_per_time() const
    {
        if (_pdata->areal_mean_velocity_per_time.empty())
        { return 0; }

        return bk::mean(_pdata->areal_mean_velocity_per_time.begin(), _pdata->areal_mean_velocity_per_time.end());
    }

    double MeasuringPlane::median_areal_mean_velocity_per_time() const
    {
        if (_pdata->areal_mean_velocity_per_time.empty())
        { return 0; }

        return bk::median(_pdata->areal_mean_velocity_per_time.begin(), _pdata->areal_mean_velocity_per_time.end());
    }

    const std::vector<double>& MeasuringPlane::areal_mean_velocitiy_throughPlane_per_time() const
    { return _pdata->areal_mean_velocity_throughPlane_per_time; }

    double MeasuringPlane::min_areal_mean_velocity_throughPlane_per_time() const
    {
        if (_pdata->areal_mean_velocity_throughPlane_per_time.empty())
        { return 0; }

        return *std::min_element(_pdata->areal_mean_velocity_throughPlane_per_time.begin(), _pdata->areal_mean_velocity_throughPlane_per_time.end());
    }

    double MeasuringPlane::max_areal_mean_velocity_throughPlane_per_time() const
    {
        if (_pdata->areal_mean_velocity_throughPlane_per_time.empty())
        { return 0; }

        return *std::max_element(_pdata->areal_mean_velocity_throughPlane_per_time.begin(), _pdata->areal_mean_velocity_throughPlane_per_time.end());
    }

    double MeasuringPlane::mean_areal_mean_velocity_throughPlane_per_time() const
    {
        if (_pdata->areal_mean_velocity_throughPlane_per_time.empty())
        { return 0; }

        return bk::mean(_pdata->areal_mean_velocity_throughPlane_per_time.begin(), _pdata->areal_mean_velocity_throughPlane_per_time.end());
    }

    double MeasuringPlane::median_areal_mean_velocity_throughPlane_per_time() const
    {
        if (_pdata->areal_mean_velocity_throughPlane_per_time.empty())
        { return 0; }

        return bk::median(_pdata->areal_mean_velocity_throughPlane_per_time.begin(), _pdata->areal_mean_velocity_throughPlane_per_time.end());
    }

    double MeasuringPlane::_mm_to_cm(double x) const
    { return x * 0.1; }

    double MeasuringPlane::_mm2_to_cm2(double x) const
    { return x * 0.01; }

    const std::vector<double>& MeasuringPlane::area_mm2() const
    { return _pdata->area_mm2; }

    double MeasuringPlane::max_area_mm2() const
    {
        if (_pdata->area_mm2.empty())
        { return 0; }

        return *std::max_element(_pdata->area_mm2.begin(), _pdata->area_mm2.end());
    }

    double MeasuringPlane::max_area_cm2() const
    { return _mm2_to_cm2(max_area_mm2()); }

    double MeasuringPlane::min_area_mm2() const
    {
        if (_pdata->area_mm2.empty())
        { return 0; }

        return *std::min_element(_pdata->area_mm2.begin(), _pdata->area_mm2.end());
    }

    double MeasuringPlane::min_area_cm2() const
    { return _mm2_to_cm2(min_area_mm2()); }

    double MeasuringPlane::mean_area_mm2() const
    {
        if (_pdata->area_mm2.empty())
        { return 0; }

        return bk::mean(_pdata->area_mm2.begin(), _pdata->area_mm2.end());
    }

    double MeasuringPlane::mean_area_cm2() const
    { return _mm2_to_cm2(mean_area_mm2()); }

    double MeasuringPlane::median_area_mm2() const
    {
        if (_pdata->area_mm2.empty())
        { return 0; }

        return bk::median(_pdata->area_mm2.begin(), _pdata->area_mm2.end());
    }

    double MeasuringPlane::median_area_cm2() const
    { return _mm2_to_cm2(median_area_mm2()); }

    double MeasuringPlane::diameter_mm() const
    { return _pdata->diameter_mm; }

    double MeasuringPlane::diameter_cm() const
    { return _mm_to_cm(_pdata->diameter_mm); }

    double MeasuringPlane::velocity_through_plane(const Vec3ui& gp) const
    { return velocity_through_plane(gp[0], gp[1], gp[2]); }

    double MeasuringPlane::velocity_through_plane(unsigned int x, unsigned int y, unsigned int t) const
    { return this->point_attribute_value_of_type<double>(VelocityThroughPlaneAttributeHash(), x, y, t); }

    double& MeasuringPlane::velocity_through_plane(const Vec3ui& gp)
    { return velocity_through_plane(gp[0], gp[1], gp[2]); }

    double& MeasuringPlane::velocity_through_plane(unsigned int x, unsigned int y, unsigned int t)
    { return this->point_attribute_value_of_type<double>(VelocityThroughPlaneAttributeHash(), x, y, t); }
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    void MeasuringPlane::set_size(unsigned int sx, unsigned int sy, unsigned int st)
    {
        base_type::set_size(sx, sy, st);
        this->geometry().transformation().set_grid_size(sx, sy, st);
        _seg().set_size(sx, sy, st);
        this->template add_point_attribute_vector_of_type<double>(VelocityThroughPlaneAttributeHash());
    }

    void MeasuringPlane::set_vesselID(unsigned int vid)
    { _pdata->vesselID = vid; }

    void MeasuringPlane::set_diameter_mm(double d_in_mm)
    { _pdata->diameter_mm = std::abs(d_in_mm); }

    MeasuringPlane& MeasuringPlane::operator=(const MeasuringPlane&) = default;
    MeasuringPlane& MeasuringPlane::operator=(MeasuringPlane&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void MeasuringPlane::clear_plane()
    {
        set_size(1, 1, 1);

        //unsigned int vesselID;
        //segmentation_type seg;
        // flow rates/volumes
        _pdata->flow_rate_per_time.clear();
        _pdata->forward_flow_volume = 0;
        _pdata->backward_flow_volume = 0;
        _pdata->net_flow_volume = 0;
        _pdata->percentaged_back_flow_volume = 0;
        _pdata->cardiac_output = 0;
        // flow velocities (abs)
        _pdata->min_velocity = 0;
        _pdata->max_velocity = 0;
        _pdata->mean_velocity = 0;
        _pdata->median_velocity = 0;
        // flow velocities (throughplane)
        _pdata->min_velocity_throughPlane = 0;
        _pdata->max_velocity_throughPlane = 0;
        _pdata->mean_velocity_throughPlane = 0;
        _pdata->median_velocity_throughPlane = 0;
        // flow velocities (areal)
        _pdata->areal_mean_velocity_per_time.clear();
        _pdata->areal_mean_velocity_throughPlane_per_time.clear();
        // cross-section
        _pdata->area_mm2.clear();
        _pdata->diameter_mm = 0;
    }

    void MeasuringPlane::sample_from_flow_field(FlowImage3DT& f)
    {
        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(this->geometry().size(0), ___("Sampling measuring plane from flow field"));
        #endif

        #pragma omp parallel for
        for (unsigned int x = 0; x < this->geometry().size(0); ++x)
        {
            for (unsigned int y = 0; y < this->geometry().size(1); ++y)
            {
                for (unsigned int t = 0; t < this->geometry().size(2); ++t)
                {
                    const Vec4d wp = this->geometry().transformation().to_world_coordinates(x, y, t);
                    this->operator()(x, y, t) = f.flow_vector_world_rotated_at_world_pos(wp);
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
    }

    void MeasuringPlane::sample_segmentation_from_vessel_mesh(Vessel& v)
    {
        if (!v.has_mesh())
        {
            std::cerr << "MeasuringPlane::sample_segmentation_from_vessel_mesh - vessel \"" << v.name() << "\" has no mesh" << std::endl;
            return;
        }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(this->geometry().size(0), ___("Sampling measuring plane segmentation from vessel mesh (\"@0\")", v.name()));
        #endif

        auto& mesh = v.mesh();

        if (!mesh.geometry().has_kdtree())
        { mesh.geometry().construct_kd_tree(); }

        #pragma omp parallel for schedule(dynamic, 10)
        for (unsigned int x = 0; x < this->geometry().size(0); ++x)
        {
            for (unsigned int y = 0; y < this->geometry().size(1); ++y)
            {
                const Vec4d wp = this->geometry().transformation().to_world_coordinates(x, y, 0);

                if (mesh.is_inside(wp.sub_vector<0, 2>()))
                {
                    for (unsigned int t = 0; t < this->geometry().size(2); ++t)
                    { _pdata->seg(x, y, t) = 1; }
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
    }

    void MeasuringPlane::sample_segmentation_from_vessel_segmentation(Vessel& v)
    {
        if (!v.has_segmentation3D())
        {
            std::cerr << "MeasuringPlane::sample_segmentation_from_vessel_segmentation - vessel \"" << v.name() << "\" has no segmentation" << std::endl;
            return;
        }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(this->geometry().size(0), ___("Sampling measuring plane segmentation from vessel segmentation (\"@0\")", v.name()));
        #endif

        const auto& seg3 = v.segmentation3D();

        #pragma omp parallel for schedule(dynamic, 10)
        for (unsigned int x = 0; x < this->geometry().size(0); ++x)
        {
            for (unsigned int y = 0; y < this->geometry().size(1); ++y)
            {
                const Vec4d wp = this->geometry().transformation().to_world_coordinates(x, y, 0);

                if (seg3.interpolate({wp[0], wp[1], wp[2]}) >= 0.5)
                {
                    for (unsigned int t = 0; t < this->geometry().size(2); ++t)
                    { _pdata->seg(x, y, t) = 1; }
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
    }

    bool MeasuringPlane::calc_statistics()
    {
        const auto& size = this->geometry().size();
        const unsigned int numTimes = size[2];

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(numTimes + 7, ___("Calculating measuring plane statistics"));
        #endif

        const bool hasSegmentation = _pdata->seg.num_values() == this->num_values();
        const Vec3d nrml = this->geometry().transformation().nz();
        std::vector<double> velocities;
        velocities.reserve((size[0] / 2) * (size[1] / 2) * numTimes);

        std::vector<double> velocities_throughPlane;
        velocities_throughPlane.reserve((size[0] / 2) * (size[1] / 2) * numTimes);

        _pdata->flow_rate_per_time.resize(numTimes, 0);

        const double area_per_rect = area_per_cell();

        _pdata->forward_flow_volume = 0;
        _pdata->backward_flow_volume = 0;
        _pdata->net_flow_volume = 0;

        _pdata->areal_mean_velocity_per_time.resize(numTimes, 0);
        _pdata->areal_mean_velocity_throughPlane_per_time.resize(numTimes, 0);

        _pdata->area_mm2.resize(numTimes, 0);

        #pragma omp parallel for
        for (unsigned int t = 0; t < numTimes; ++t)
        {
            double fr = 0;
            double areal_mean_velocity_this_time = 0;
            double areal_mean_velocity_throughPlane = 0;
            unsigned int cnt_in_seg = 0;

            for (unsigned int x = 0; x < size[0]; ++x)
            {
                for (unsigned int y = 0; y < size[1]; ++y)
                {
                    if (!hasSegmentation || (hasSegmentation && _pdata->seg(x, y, t) != 0))
                    {
                        const unsigned int lid = bk::grid_to_list_id(size, x, y, t);
                        const double velocity_throughPlane = (*this)[lid].dot(nrml);
                        const double velocity = (*this)[lid].norm();

                        fr += velocity_throughPlane;

                        #pragma omp critical
                        {
                            areal_mean_velocity_this_time += velocity;
                            areal_mean_velocity_throughPlane += velocity_throughPlane;

                            velocities.push_back(velocity);
                            velocities_throughPlane.push_back(velocity_throughPlane);

                            this->point_attribute_value_of_type<double>(VelocityThroughPlaneAttributeHash(), x, y, t) = velocity_throughPlane;

                            ++cnt_in_seg;
                        }
                    }
                } // for y
            } // for x

            _pdata->flow_rate_per_time[t] = fr * area_per_rect;

            #pragma omp critical
            {
                if (_pdata->flow_rate_per_time[t] > 0)
                { _pdata->forward_flow_volume += _pdata->flow_rate_per_time[t]; }
                else if (_pdata->flow_rate_per_time[t] < 0)
                { _pdata->backward_flow_volume += std::abs(_pdata->flow_rate_per_time[t]); }

                _pdata->net_flow_volume += _pdata->flow_rate_per_time[t];
            }

            _pdata->areal_mean_velocity_per_time[t] = areal_mean_velocity_this_time / std::max(1U, cnt_in_seg);
            _pdata->areal_mean_velocity_throughPlane_per_time[t] = areal_mean_velocity_throughPlane / std::max(1U, cnt_in_seg);

            _pdata->area_mm2[t] = cnt_in_seg * area_per_rect;

            #ifdef BK_EMIT_PROGRESS
                #pragma omp critical
            { prog.increment(1); }
            #endif
        } // for t

        constexpr const double mm3ToMlScale = 0.001;
        const double msPerTime = this->geometry().transformation().scale(2);

        _pdata->forward_flow_volume *= msPerTime * mm3ToMlScale;
        _pdata->backward_flow_volume *= msPerTime * mm3ToMlScale;
        _pdata->net_flow_volume *= msPerTime * mm3ToMlScale;

        // normal does not point in the main flow direction?

        _pdata->normal_is__oriented_in_main_flow_direction = !(std::signbit(_pdata->net_flow_volume) || std::signbit(_pdata->forward_flow_volume) || std::signbit(_pdata->backward_flow_volume));
        if (!_pdata->normal_is__oriented_in_main_flow_direction)
        {
            auto fut0 = bk_threadpool.enqueue([&]()
                                              {
                                                  for (double& f: _pdata->flow_rate_per_time)
                                                  { f = -f; }
                                              });

            auto fut1 = bk_threadpool.enqueue([&]()
                                              {
                                                  for (double& v: _pdata->areal_mean_velocity_throughPlane_per_time)
                                                  { v = -v; }
                                              });

            auto fut2 = bk_threadpool.enqueue([&]()
                                              {
                                                  for (double& v : velocities_throughPlane)
                                                  { v = -v; }
                                              });

            //auto fut3 = bk_threadpool.enqueue([&]()
            //                                  {
            //                                      for (unsigned int i = 0; i < this->num_values(); ++i)
            //                                      {
            //                                          const auto vtp_ptr = this->point_attributes().get_velocity_throughplane_value(i);
            //                                          if (vtp_ptr != nullptr)
            //                                          {
            //                                              const auto vtp = -*vtp_ptr;
            //                                              this->point_attributes().set_velocity_throughplane_value(i, vtp);
            //                                          }
            //                                      }
            //                                  });

            std::swap(_pdata->forward_flow_volume, _pdata->backward_flow_volume);

            _pdata->forward_flow_volume = std::abs(_pdata->forward_flow_volume);
            _pdata->backward_flow_volume = std::abs(_pdata->backward_flow_volume);
            _pdata->net_flow_volume = std::abs(_pdata->net_flow_volume);

            fut0.get();
            fut1.get();
            fut2.get();
            //fut3.get();
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        _pdata->percentaged_back_flow_volume = 100 * _pdata->backward_flow_volume / (_pdata->forward_flow_volume + _pdata->backward_flow_volume);
        _pdata->cardiac_output = 0.001 * _pdata->net_flow_volume * 60'000 / (msPerTime * numTimes);

        auto fut0 = bk_threadpool.enqueue([&]()
                                          {
                                              std::sort(velocities.begin(), velocities.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        auto fut1 = bk_threadpool.enqueue([&]()
                                          {
                                              std::sort(velocities_throughPlane.begin(), velocities_throughPlane.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        fut0.get();
        fut1.get();

        auto fut2 = bk_threadpool.enqueue([&]()
                                          {
                                              _pdata->mean_velocity = bk::mean(velocities.begin(), velocities.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        auto fut3 = bk_threadpool.enqueue([&]()
                                          {
                                              _pdata->median_velocity = bk::median(velocities.begin(), velocities.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        auto fut4 = bk_threadpool.enqueue([&]()
                                          {
                                              _pdata->mean_velocity_throughPlane = bk::mean(velocities_throughPlane.begin(), velocities_throughPlane.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        auto fut5 = bk_threadpool.enqueue([&]()
                                          {
                                              _pdata->median_velocity_throughPlane = bk::median(velocities_throughPlane.begin(), velocities_throughPlane.end());

                                              #ifdef BK_EMIT_PROGRESS
                                              prog.increment(1);
                                              #endif
                                          });

        _pdata->min_velocity = velocities.front();
        _pdata->max_velocity = velocities.back();
        _pdata->min_velocity_throughPlane = velocities_throughPlane.front();
        _pdata->max_velocity_throughPlane = velocities_throughPlane.back();

        fut2.get();
        fut3.get();
        fut4.get();
        fut5.get();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }
  } // inline namespace cmr
} // namespace bk


