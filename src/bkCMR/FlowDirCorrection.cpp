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

#include <bkCMR/FlowDirCorrection.h>

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
    struct FlowDirCorrection::Impl
    {
        std::array<int, 3> correction_factors;

        Impl() : correction_factors{+1, +1, +1} { /* do nothing */ }
        Impl(const Impl&) = default;
        Impl(Impl&&) noexcept = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = default;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    FlowDirCorrection::FlowDirCorrection() = default;
    FlowDirCorrection::FlowDirCorrection(const FlowDirCorrection&) = default;
    FlowDirCorrection::FlowDirCorrection(FlowDirCorrection&&) noexcept = default;
    FlowDirCorrection::~FlowDirCorrection() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    int FlowDirCorrection::correction_x() const
    { return _pdata->correction_factors[0]; }

    int FlowDirCorrection::correction_y() const
    { return _pdata->correction_factors[1]; }

    int FlowDirCorrection::correction_z() const
    { return _pdata->correction_factors[2]; }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    FlowDirCorrection& FlowDirCorrection::operator=(const FlowDirCorrection&) = default;
    FlowDirCorrection& FlowDirCorrection::operator=(FlowDirCorrection&&) noexcept = default;

    void FlowDirCorrection::set(bool x_forward, bool y_forward, bool z_forward)
    {
        _pdata->correction_factors[0] = x_forward ? +1 : -1;
        _pdata->correction_factors[1] = y_forward ? +1 : -1;
        _pdata->correction_factors[2] = z_forward ? +1 : -1;
    }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void FlowDirCorrection::apply(FlowImage3DT& ff) const
    {
        #pragma omp parallel for
        for (unsigned int i = 0; i < ff.num_values(); ++i)
        {
            auto& v = ff[i];

            for (unsigned int k = 0; k < 3; ++k)
            { v[k] *= _pdata->correction_factors[k]; }
        }
    }

    bool FlowDirCorrection::save(std::string_view filepath) const
    {
        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        if (!file.good())
        { return false; }

        for (unsigned int i = 0; i < 3; ++i)
        {
            const std::int8_t i8temp = static_cast<std::int8_t>(_pdata->correction_factors[i]);
            file.write(reinterpret_cast<const char*>(&i8temp), sizeof(std::int8_t));
        }

        file.close();

        return true;
    }

    bool FlowDirCorrection::load(std::string_view filepath)
    {
        std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

        if (!file.good())
        { return false; }

        std::int8_t i8temp = 0;

        for (unsigned int i = 0; i < 3; ++i)
        {
            file.read(reinterpret_cast<char*>(&i8temp), sizeof(std::int8_t));
            _pdata->correction_factors[i] = static_cast<int>(i8temp);
        }

        file.close();

        return true;
    }
  } // inline namespace cmr
} // namespace bk

