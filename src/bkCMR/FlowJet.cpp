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

#include <bkCMR/FlowJet.h>

#include <limits>

#include <bkCMR/FlowJetPoint.h>
#include <bkMath/functions/binomial_coefficient.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct FlowJet::Impl
    {
        std::vector<std::vector<FlowJetPoint>> points;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    FlowJet::FlowJet() = default;
    FlowJet::FlowJet(const FlowJet&) = default;
    FlowJet::FlowJet(FlowJet&&) noexcept = default;
    FlowJet::~FlowJet() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    bool FlowJet::_is_valid_posID(unsigned int posID) const
    { return posID < num_positions(); }

    bool FlowJet::_is_valid_timeID(unsigned int timeID) const
    { return timeID < num_times(); }

    bool FlowJet::_is_valid_posID_and_timeID(unsigned int posID, unsigned int timeID) const
    { return _is_valid_posID(posID) && _is_valid_timeID(timeID); }

    unsigned int FlowJet::num_positions() const
    { return _pdata->points.size(); }

    unsigned int FlowJet::num_times() const
    { return _pdata->points.empty() ? 0 : _pdata->points[0].size(); }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    FlowJet& FlowJet::operator=(const FlowJet&) = default;
    FlowJet& FlowJet::operator=(FlowJet&& other) noexcept = default;

    void FlowJet::resize(unsigned int N, unsigned int numTimes)
    { _pdata->points.resize(N, std::vector<FlowJetPoint>(numTimes)); }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void FlowJet::clear()
    { _pdata->points.clear(); }

    void FlowJet::smooth_spatial_binomial(unsigned int iterations, unsigned int kernel_size)
    {
        if (iterations == 0 || kernel_size < 2)
        { return; }

        const unsigned int nPositions = num_positions();
        const unsigned int nTimes     = num_times();

        std::vector<std::vector<FlowJetPoint>> temp0(_pdata->points);
        std::vector<std::vector<FlowJetPoint>> temp1(_pdata->points);
        std::vector<std::vector<FlowJetPoint>>* write = nullptr;
        std::vector<std::vector<FlowJetPoint>>* read  = nullptr;

        const unsigned int  ks   = kernel_size + (kernel_size % 2 == 0 ? 1 : 0);
        std::vector<double> w(ks, 0);
        double              wsum = 0;

        for (unsigned int i = 0; i < ks; ++i)
        {
            w[i] = bk::binomial_coefficient(ks - 1, i);
            wsum += w[i];
        }

        for (unsigned int i = 0; i < ks; ++i)
        { w[i] /= wsum; }

        for (unsigned int it = 0; it < iterations; ++it)
        {
            write = it % 2 == 0 ? &temp1 : &temp0;
            read  = it % 2 == 0 ? &temp0 : &temp1;

            #pragma omp parallel for
            for (unsigned int pid = 0; pid < nPositions; ++pid)
            {
                for (unsigned int tid = 0; tid < nTimes; ++tid)
                {
                    bool     valid = true;
                    for (int i     = -static_cast<int>(ks / 2); i <= static_cast<int>(ks / 2); ++i)
                    {
                        if (!_is_valid_posID_and_timeID(pid + i, tid))
                        {
                            valid = false;
                            break;
                        }
                    }

                    if (valid)
                    {
                        std::vector<double> temp_w(w);
                        double              temp_w_sum = 0;
                        for (unsigned int   i          = 0; i < ks; ++i)
                        {
                            const int    off  = -static_cast<int>(ks / 2) + static_cast<int>(i);
                            const double velo = (*read)[pid + off][tid].peak_velocity;
                            temp_w[i] *= velo;

                            temp_w_sum += temp_w[i];
                        }

                        for (unsigned int i = 0; i < ks; ++i)
                        { temp_w[i] /= temp_w_sum; }

                        (*write)[pid][tid].peak_velocity_position.set_zero();
                        (*write)[pid][tid].peak_velocity = 0;
                        (*write)[pid][tid].area_center.set_zero();
                        (*write)[pid][tid].area_radius_x = 0;
                        (*write)[pid][tid].area_radius_y = 0;
                        (*write)[pid][tid].area_dir_x.set_zero();
                        (*write)[pid][tid].area_dir_y.set_zero();

                        for (unsigned int i = 0; i < ks; ++i)
                        {
                            const int off = -static_cast<int>(ks / 2) + static_cast<int>(i);

                            (*write)[pid][tid].peak_velocity_position += (*read)[pid + off][tid].peak_velocity_position * temp_w[i];
                            (*write)[pid][tid].peak_velocity += (*read)[pid + off][tid].peak_velocity * temp_w[i];
                            (*write)[pid][tid].area_center += (*read)[pid + off][tid].area_center * temp_w[i];
                            (*write)[pid][tid].area_radius_x += (*read)[pid + off][tid].area_radius_x * temp_w[i];
                            (*write)[pid][tid].area_radius_y += (*read)[pid + off][tid].area_radius_y * temp_w[i];
                            (*write)[pid][tid].area_dir_x += (*read)[pid + off][tid].area_dir_x * temp_w[i];
                            (*write)[pid][tid].area_dir_y += (*read)[pid + off][tid].area_dir_y * temp_w[i];
                        }

                        (*write)[pid][tid].area_dir_x.normalize_internal();
                        (*write)[pid][tid].area_dir_y.normalize_internal();
                    } // if valid
                    else
                    {
                        (*write)[pid][tid].peak_velocity_position = (*read)[pid][tid].peak_velocity_position;
                        (*write)[pid][tid].peak_velocity          = (*read)[pid][tid].peak_velocity;
                        (*write)[pid][tid].area_center            = (*read)[pid][tid].area_center;
                        (*write)[pid][tid].area_radius_x          = (*read)[pid][tid].area_radius_x;
                        (*write)[pid][tid].area_radius_y          = (*read)[pid][tid].area_radius_y;
                        (*write)[pid][tid].area_dir_x             = (*read)[pid][tid].area_dir_x;
                        (*write)[pid][tid].area_dir_y             = (*read)[pid][tid].area_dir_y;
                    }
                } // for tid: nTimes
            } // for pid: nPositions
        } // for it: iterations

        if (iterations % 2 != 0)
        { std::swap(_pdata->points, *write); }
        else
        { std::swap(_pdata->points, *read); }
    }
  } // inline namespace cmr
} // namespace bk


