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

#ifndef BK_PHASEUNWRAPPING3DT_H
#define BK_PHASEUNWRAPPING3DT_H

#include <array>
#include <string_view>

#include <bk/Clock>
#include <bk/CopyablePIMPL>

#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage3DT;
    // -------------------- forward declaration END

    class BKCMR_EXPORT PhaseUnwrapping3DT
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
        PhaseUnwrapping3DT();
        PhaseUnwrapping3DT(const PhaseUnwrapping3DT&);
        PhaseUnwrapping3DT(PhaseUnwrapping3DT&&) noexcept;
        ~PhaseUnwrapping3DT();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] bool is_wrapped() const;
        [[nodiscard]] unsigned int num_wrapped_voxels() const;
        [[nodiscard]] unsigned int num_wrapped_voxels(unsigned int x0y1z2) const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] PhaseUnwrapping3DT& operator=(const PhaseUnwrapping3DT&);
        [[maybe_unused]] PhaseUnwrapping3DT& operator=(PhaseUnwrapping3DT&&) noexcept;

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();

        // the ff must not already be phase-unwrapped
        [[maybe_unused]] Clock init(const FlowImage3DT& ff, double venc);
        [[maybe_unused]] Clock init(const FlowImage3DT& ff, const std::array<double, 3>& venc);
        void apply(FlowImage3DT& ff, double venc) const;
        void apply(FlowImage3DT& ff, const std::array<double, 3>& venc) const;

        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class PhaseUnwrapping3DT
  } // inline namespace cmr
} // namespace bk

#endif //BK_PHASEUNWRAPPING3DT_H
