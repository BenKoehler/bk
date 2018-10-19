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

#ifndef BK_VELOCITYOFFSETCORRECTOR3DT_H
#define BK_VELOCITYOFFSETCORRECTOR3DT_H

#include <string_view>

#include <bk/CopyablePIMPL>
#include <bk/Clock>
#include <bk/Image>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage3DT;
    // -------------------- forward declaration END

    class BKCMR_EXPORT VelocityOffsetCorrector3DT
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
        /// @{ -------------------------------------------------- CTOR
        VelocityOffsetCorrector3DT();
        VelocityOffsetCorrector3DT(const VelocityOffsetCorrector3DT&);
        VelocityOffsetCorrector3DT(VelocityOffsetCorrector3DT&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DTOR
        ~VelocityOffsetCorrector3DT();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] double ivsd_static_tissue_threshold() const;
        [[nodiscard]] unsigned int end_diastolic_time_point() const;
        [[nodiscard]] bool is_initialized() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] VelocityOffsetCorrector3DT& operator=(const VelocityOffsetCorrector3DT&);
        [[maybe_unused]] VelocityOffsetCorrector3DT& operator=(VelocityOffsetCorrector3DT&&) noexcept;
        /// @}

        void set_ivsd_static_tissue_threshold(double t);
        void set_end_diastolic_time_point(unsigned int id);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        [[maybe_unused]] Clock init(const FlowImage3DT& ff, const DicomImage<double, 3>& ivsd);
        [[maybe_unused]] bool apply(FlowImage3DT& ff) const;

        //====================================================================================================
        //===== I/O
        //====================================================================================================
        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class VelocityOffsetCorrector3DT
  } // inline namespace cmr
} // namespace bk

#endif //BK_VELOCITYOFFSETCORRECTOR3DT_H
