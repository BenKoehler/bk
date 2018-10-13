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

#ifndef BK_FLOWTIMESHIFT_H
#define BK_FLOWTIMESHIFT_H

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

    class BKCMR_EXPORT FlowTimeShift
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
        FlowTimeShift();
        FlowTimeShift(const FlowTimeShift&);
        FlowTimeShift(FlowTimeShift&&) noexcept;
        ~FlowTimeShift();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] bool is_initialized() const;
        [[nodiscard]] unsigned int num_times() const;
        [[nodiscard]] double temporal_resolution() const;
        [[nodiscard]] unsigned int offset_id() const;
        [[nodiscard]] double offset_in_ms() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] FlowTimeShift& operator=(const FlowTimeShift&);
        [[maybe_unused]] FlowTimeShift& operator=(FlowTimeShift&&) noexcept;

        void set_shift(int shift, unsigned int numTimes, double temporalResolutionInMs);
        void set_shift(int shift, FlowImage3DT& ff);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void apply(FlowImage3DT& ff) const;

        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class FlowTimeShift
  } // inline namespace cmr
} // namespace bk

#endif //BK_FLOWTIMESHIFT_H
