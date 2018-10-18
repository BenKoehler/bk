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

#ifndef BK_PRESSUREMAPIMAGEFILTER_H
#define BK_PRESSUREMAPIMAGEFILTER_H

#include <memory>
#include <utility>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bk/Image>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  // -------------------- forward declaration
  class Clock;

  inline namespace cmr
  {
    class FlowImage3DT;
    class Vessel;
    // -------------------- forward declaration END

    class BKCMR_EXPORT PressureMapImageFilter
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = PressureMapImageFilter;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
        class Impl;
        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        PressureMapImageFilter();
        PressureMapImageFilter(const PressureMapImageFilter&);
        PressureMapImageFilter(PressureMapImageFilter&&) noexcept;
        ~PressureMapImageFilter();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] double density() const;
        [[nodiscard]] double viscosity() const;
        [[nodiscard]] unsigned int max_iterations() const;
        [[nodiscard]] bool convert_to_mmhg() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] PressureMapImageFilter& operator=(const PressureMapImageFilter&);
        [[maybe_unused]] PressureMapImageFilter& operator=(PressureMapImageFilter&&) noexcept;

        void set_density(double dens);
        void set_viscosity(double viscos);
        void set_max_iterations(unsigned int maxIter);
        void set_convert_to_mmhg(bool useMmhg);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        [[nodiscard]] std::unique_ptr<DicomImage<double, 4>> apply(const FlowImage3DT& ff, const std::vector<std::reference_wrapper<const Vessel>>& vessels);
    }; // class PressureMapImageFilter
  } // inline namespace cmr
} // namespace bk

#endif //BK_PRESSUREMAPIMAGEFILTER_H
