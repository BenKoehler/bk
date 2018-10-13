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

#include <bkCMR/CardiacCycleDefinition.h>

#include <array>
#include <cstdint>
#include <fstream>
#include <string>

#include <bk/StringUtils>
#include <bkCMR/FlowImage3DT.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct CardiacCycleDefinition::Impl
    {
        unsigned int num_times;
        double temporal_resolution;

        unsigned int id_systole_begin_diastole_end;
        double ms_systole_begin_diastole_end;

        unsigned int id_systole_end_diastole_begin;
        double ms_systole_end_diastole_begin;

        Impl()
            : num_times(0),
              temporal_resolution(0),
              id_systole_begin_diastole_end(0),
              ms_systole_begin_diastole_end(0),
              id_systole_end_diastole_begin(0),
              ms_systole_end_diastole_begin(0)
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
    CardiacCycleDefinition::CardiacCycleDefinition() = default;
    CardiacCycleDefinition::CardiacCycleDefinition(const CardiacCycleDefinition&) = default;
    CardiacCycleDefinition::CardiacCycleDefinition(CardiacCycleDefinition&&) noexcept = default;
    CardiacCycleDefinition::~CardiacCycleDefinition() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    bool CardiacCycleDefinition::is_initialized() const
    {
        const unsigned int t0 = _pdata->id_systole_begin_diastole_end;
        const unsigned int t1 = _pdata->id_systole_end_diastole_begin;
        return t0 != t1 && t0 != std::numeric_limits<unsigned int>::max() && t1 != std::numeric_limits<unsigned int>::max();
    }

    unsigned int CardiacCycleDefinition::num_times() const
    { return _pdata->num_times; }

    double CardiacCycleDefinition::temporal_resolution() const
    { return _pdata->temporal_resolution; }

    unsigned int CardiacCycleDefinition::id_systole_begin_diastole_end() const
    { return _pdata->id_systole_begin_diastole_end; }

    double CardiacCycleDefinition::ms_systole_begin_diastole_end() const
    { return _pdata->ms_systole_begin_diastole_end; }

    unsigned int CardiacCycleDefinition::id_systole_end_diastole_begin() const
    { return _pdata->id_systole_end_diastole_begin; }

    double CardiacCycleDefinition::ms_systole_end_diastole_begin() const
    { return _pdata->ms_systole_end_diastole_begin; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    CardiacCycleDefinition& CardiacCycleDefinition::operator=(const CardiacCycleDefinition&) = default;
    CardiacCycleDefinition& CardiacCycleDefinition::operator=(CardiacCycleDefinition&&) noexcept = default;

    void CardiacCycleDefinition::set(unsigned int systole_begin, unsigned int systole_end, unsigned int numTimes, double temporalResolutionInMs)
    {
        _pdata->num_times = numTimes;
        _pdata->temporal_resolution = temporalResolutionInMs;
        _pdata->id_systole_begin_diastole_end = systole_begin;
        _pdata->id_systole_end_diastole_begin = systole_end;

        _pdata->ms_systole_begin_diastole_end = _pdata->id_systole_begin_diastole_end * _pdata->temporal_resolution;
        _pdata->ms_systole_end_diastole_begin = _pdata->id_systole_end_diastole_begin * _pdata->temporal_resolution;
    }

    void CardiacCycleDefinition::set(unsigned int systole_begin, unsigned int systole_end, FlowImage3DT& ff)
    { set(systole_begin, systole_end, ff.geometry().size(3), ff.geometry().transformation().scale(3)); }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    unsigned int CardiacCycleDefinition::length_in_time_steps() const
    {
        if (_pdata->id_systole_end_diastole_begin > _pdata->id_systole_begin_diastole_end)
        { return _pdata->id_systole_end_diastole_begin - _pdata->id_systole_begin_diastole_end + 1; }
        else
        { return _pdata->num_times - _pdata->id_systole_begin_diastole_end + _pdata->id_systole_end_diastole_begin; }
    }

    double CardiacCycleDefinition::length_in_ms() const
    { return length_in_time_steps() * _pdata->temporal_resolution; }

    bool CardiacCycleDefinition::save(std::string_view filepath) const
    {
        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint16_t ui16temp = static_cast<std::uint16_t>(_pdata->num_times);
        file.write(reinterpret_cast<const char*>(&ui16temp), sizeof(std::uint16_t));

        file.write(reinterpret_cast<const char*>(&_pdata->temporal_resolution), sizeof(double));

        ui16temp = static_cast<std::uint16_t>(_pdata->id_systole_begin_diastole_end);
        file.write(reinterpret_cast<const char*>(&ui16temp), sizeof(std::uint16_t));

        file.write(reinterpret_cast<const char*>(&_pdata->ms_systole_begin_diastole_end), sizeof(double));

        ui16temp = static_cast<std::uint16_t>(_pdata->id_systole_end_diastole_begin);
        file.write(reinterpret_cast<const char*>(&ui16temp), sizeof(std::uint16_t));

        file.write(reinterpret_cast<const char*>(&_pdata->ms_systole_end_diastole_begin), sizeof(double));

        file.close();

        return true;
    }

    bool CardiacCycleDefinition::load(std::string_view filepath)
    {
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::uint16_t ui16temp = 0;
        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->num_times = static_cast<unsigned int>(ui16temp);

        file.read(reinterpret_cast<char*>(&_pdata->temporal_resolution), sizeof(double));

        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->id_systole_begin_diastole_end = static_cast<unsigned int>(ui16temp);

        file.read(reinterpret_cast<char*>(&_pdata->ms_systole_begin_diastole_end), sizeof(double));

        file.read(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        _pdata->id_systole_end_diastole_begin = static_cast<unsigned int>(ui16temp);

        file.read(reinterpret_cast<char*>(&_pdata->ms_systole_end_diastole_begin), sizeof(double));

        file.close();

        return true;
    }
  } // inline namespace cmr
} // namespace bk

