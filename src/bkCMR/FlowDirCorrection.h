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

#ifndef BK_FLOWDIRCORRECTION_H
#define BK_FLOWDIRCORRECTION_H

#include <array>
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

    class BKCMR_EXPORT FlowDirCorrection
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
        FlowDirCorrection();
        FlowDirCorrection(const FlowDirCorrection&);
        FlowDirCorrection(FlowDirCorrection&&) noexcept;
        ~FlowDirCorrection();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] bool is_initialized() const;

        [[nodiscard]] int correction_x() const;
        [[nodiscard]] int correction_y() const;
        [[nodiscard]] int correction_z() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] FlowDirCorrection& operator=(const FlowDirCorrection&);
        [[maybe_unused]] FlowDirCorrection& operator=(FlowDirCorrection&&) noexcept;

        void set(bool x_forward, bool y_forward, bool z_forward);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void apply(FlowImage3DT& ff) const;

        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class FlowDirCorrection
  } // inline namespace cmr
} // namespace bk

#endif //BK_FLOWDIRCORRECTION_H
