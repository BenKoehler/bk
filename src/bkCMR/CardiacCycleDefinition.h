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

#ifndef BK_CARDIACCYCLEDEFINITION_H
#define BK_CARDIACCYCLEDEFINITION_H

#include <string_view>

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage3DT;
    // -------------------- forward declaration END

    class BKCMR_EXPORT CardiacCycleDefinition
    {
        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
        class Impl;
        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        CardiacCycleDefinition();
        CardiacCycleDefinition(const CardiacCycleDefinition&);
        CardiacCycleDefinition(CardiacCycleDefinition&&) noexcept;
        ~CardiacCycleDefinition();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] bool is_initialized() const;
        [[nodiscard]] unsigned int num_times() const;
        [[nodiscard]] double temporal_resolution() const;
        [[nodiscard]] unsigned int id_systole_begin_diastole_end() const;
        [[nodiscard]] double ms_systole_begin_diastole_end() const;
        [[nodiscard]] unsigned int id_systole_end_diastole_begin() const;
        [[nodiscard]] double ms_systole_end_diastole_begin() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] CardiacCycleDefinition& operator=(const CardiacCycleDefinition&);
        [[maybe_unused]] CardiacCycleDefinition& operator=(CardiacCycleDefinition&&) noexcept;

        void set(unsigned int systole_begin, unsigned int systole_end, unsigned int numTimes, double temporalResolutionInMs);
        void set(unsigned int systole_begin, unsigned int systole_end, FlowImage3DT& ff);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        [[nodiscard]] unsigned int length_in_time_steps() const;
        [[nodiscard]] double length_in_ms() const;

        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class CardiacCycleDefinition
  } // inline namespace cmr
} // namespace bk

#endif //BK_CARDIACCYCLEDEFINITION_H
