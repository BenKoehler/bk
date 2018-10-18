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

#include <bkCMR/FlowTimeShift.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <string>

#include <bk/StringUtils>
#include <bkCMR/dataset/FlowImage3DT.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct FlowTimeShift::Impl
    {
        unsigned int num_times;
        double temporal_resolution;
        unsigned int offset_id;
        double offset_ms;
        bool is_initialized;

        Impl()
        : num_times(0),temporal_resolution(0),offset_id(0),
          offset_ms(0),
              is_initialized(false)
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
    FlowTimeShift::FlowTimeShift() = default;
    FlowTimeShift::FlowTimeShift(const FlowTimeShift&) = default;
    FlowTimeShift::FlowTimeShift(FlowTimeShift&&) noexcept = default;
    FlowTimeShift::~FlowTimeShift() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    bool FlowTimeShift::is_initialized() const
    { return _pdata->is_initialized; }

    unsigned int FlowTimeShift::num_times() const
    { return _pdata->num_times; }

    double FlowTimeShift::temporal_resolution() const
    { return _pdata->temporal_resolution; }

    unsigned int FlowTimeShift::offset_id() const
    { return _pdata->offset_id; }

    double FlowTimeShift::offset_in_ms() const
    { return _pdata->offset_ms; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    FlowTimeShift& FlowTimeShift::operator=(const FlowTimeShift&) = default;
    FlowTimeShift& FlowTimeShift::operator=(FlowTimeShift&&) noexcept = default;

    void FlowTimeShift::set_shift(int shift, unsigned int numTimes, double temporalResolutionInMs)
    {
        if (shift < 0)
        { shift += static_cast<int>(numTimes); }
        else if (shift >= static_cast<int>(numTimes))
        { shift -= static_cast<int>(numTimes); }

        _pdata->num_times = numTimes;
        _pdata->temporal_resolution = temporalResolutionInMs;
        _pdata->offset_id = static_cast<unsigned int>(shift);
        _pdata->offset_ms = _pdata->offset_id * _pdata->temporal_resolution;

        _pdata->is_initialized = true;
    }

    void FlowTimeShift::set_shift(int shift, FlowImage3DT& ff)
    { set_shift(shift, ff.geometry().size(3), ff.geometry().transformation().scale(3)); }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void FlowTimeShift::apply(FlowImage3DT& ff) const
    {
        const auto temp = ff;

        #pragma omp parallel for schedule(dynamic, 10)
        for (unsigned int x = 0; x < ff.geometry().size(0); ++x)
        {
            for (unsigned int y = 0; y < ff.geometry().size(1); ++y)
            {
                for (unsigned int z = 0; z < ff.geometry().size(2); ++z)
                {
                    for (unsigned int t = 0; t < ff.geometry().size(3); ++t)
                    { ff(x, y, z, t) = temp(x, y, z, (t + _pdata->offset_id) % _pdata->num_times); } // for t
                } // for z
            } // for y
        } // for x
    }

    bool FlowTimeShift::save(std::string_view filepath) const
    {
        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint16_t ui16temp = static_cast<std::uint16_t>(_pdata->num_times);
        file.write(reinterpret_cast<const char*>(&ui16temp), sizeof(std::uint16_t));

        file.write(reinterpret_cast<const char*>(&_pdata->temporal_resolution), sizeof(double));

        ui16temp = static_cast<std::uint16_t>(_pdata->offset_id);
        file.write(reinterpret_cast<const char*>(&ui16temp), sizeof(std::uint16_t));

        file.write(reinterpret_cast<const char*>(&_pdata->offset_ms), sizeof(double));

        file.close();

        return true;
    }

    bool FlowTimeShift::load(std::string_view filepath)
    {
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint16_t ui16temp = 0;
        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->num_times = static_cast<unsigned int>(ui16temp);

        file.read(reinterpret_cast<char*>(&_pdata->temporal_resolution), sizeof(double));

        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->offset_id = static_cast<unsigned int>(ui16temp);

        file.read(reinterpret_cast<char*>(&_pdata->offset_ms), sizeof(double));

        file.close();

        _pdata->is_initialized = true;

        return true;
    }
  } // inline namespace cmr
} // namespace bk

