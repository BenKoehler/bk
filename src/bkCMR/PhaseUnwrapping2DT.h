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

#ifndef BK_PHASEUNWRAPPING2DT_H
#define BK_PHASEUNWRAPPING2DT_H

#include <string_view>

#include <bk/Clock>
#include <bk/CopyablePIMPL>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage2DT;
    // -------------------- forward declaration END

    class PhaseUnwrapping2DT
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
        PhaseUnwrapping2DT();
        PhaseUnwrapping2DT(const PhaseUnwrapping2DT&);
        PhaseUnwrapping2DT(PhaseUnwrapping2DT&&) noexcept;
        ~PhaseUnwrapping2DT();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] bool is_wrapped() const;
        [[nodiscard]] unsigned int num_wrapped_voxels() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] PhaseUnwrapping2DT& operator=(const PhaseUnwrapping2DT&);
        [[maybe_unused]] PhaseUnwrapping2DT& operator=(PhaseUnwrapping2DT&&) noexcept;

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();

        // the ff must not already be phase-unwrapped
        [[maybe_unused]] Clock init(const FlowImage2DT& ff, double venc);
        void apply(FlowImage2DT& ff, double venc) const;

        [[maybe_unused]] bool save(std::string_view filepath) const;
        [[maybe_unused]] bool load(std::string_view filepath);
    }; // class PhaseUnwrapping2DT
  } // inline namespace cmr
} // namespace bk

#endif //BK_PHASEUNWRAPPING2DT_H
