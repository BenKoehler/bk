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

#ifndef BK_FLOWJETEXTRACTOR_H
#define BK_FLOWJETEXTRACTOR_H

#include <bk/CopyablePIMPL>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class Dataset;
    // -------------------- forward declaration END

    class FlowJetExtractor
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
        FlowJetExtractor();
        FlowJetExtractor(const FlowJetExtractor&);
        FlowJetExtractor(FlowJetExtractor&&) noexcept;
        ~FlowJetExtractor();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] double maxVelocity_clamp_quantile() const;
        [[nodiscard]] double percentaged_velocity_threshold() const;
        [[nodiscard]] unsigned int measuring_plane_gridSize() const;
        [[nodiscard]] double equiDistance_in_mm_of_measuring_planes() const;
        [[nodiscard]] unsigned int num_smooth_iterations() const;
        [[nodiscard]] unsigned int smooth_kernel_size() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR=
        [[maybe_unused]] FlowJetExtractor& operator=(const FlowJetExtractor&);
        [[maybe_unused]] FlowJetExtractor& operator=(FlowJetExtractor&&) noexcept;
        /// @}

        void set_maxVelocity_clamp_quantile(double percentage_between_zero_and_one);
        void set_percentaged_velocity_threshold(double percentage_between_zero_and_one);
        void set_measuring_plane_gridSize(unsigned int gs);
        void set_equiDistance_in_mm_of_measuring_planes(double mm);
        void set_num_smooth_iterations(unsigned int it);
        void set_smooth_kernel_size(unsigned int ks);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void calculate_flowjets(Dataset& ds) const;
    }; // class FlowJetExtractor
  } // inline namespace cmr
} // namespace bk

#endif //BK_FLOWJETEXTRACTOR_H
