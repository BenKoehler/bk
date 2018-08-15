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

#ifndef BKCMR_FLOWJET_H
#define BKCMR_FLOWJET_H

#include <memory>
#include <utility>
#include <vector>

#include "bkCMR/lib/bkCMR_export.h"

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowJetPoint;
    // -------------------- forward declaration END

    class BKCMR_EXPORT FlowJet
    {
        //====================================================================================================
        // MEMBERS
      private:
        class Impl;
        std::unique_ptr<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        FlowJet();
        FlowJet(const FlowJet&);
        FlowJet(FlowJet&&) noexcept;
        ~FlowJet();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
      private:
        [[nodiscard]] bool _is_valid_posID(unsigned int posID) const;
        [[nodiscard]] bool _is_valid_timeID(unsigned int timeID) const;
        [[nodiscard]] bool _is_valid_posID_and_timeID(unsigned int posID, unsigned int timeID) const;
      public:

        [[nodiscard]] unsigned int num_positions() const;
        [[nodiscard]] unsigned int num_times() const;

        [[nodiscard]]       FlowJetPoint& point(unsigned int posID, unsigned int timeID);
        [[nodiscard]] const FlowJetPoint& point(unsigned int posID, unsigned int timeID) const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] FlowJet& operator=(const FlowJet& other);
        [[maybe_unused]] FlowJet& operator=(FlowJet&&) noexcept;

        void resize(unsigned int N, unsigned int numTimes);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        void clear();
        void smooth_spatial_binomial(unsigned int iterations, unsigned int kernel_size);
    }; // class FlowJet
  } // inline namespace cmr
} // namespace bk

#endif //BKCMR_FLOWJET_H


