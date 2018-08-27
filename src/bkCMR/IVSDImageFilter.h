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

#ifndef BK_IVSDIMAGEFILTER_H
#define BK_IVSDIMAGEFILTER_H

#include <memory>

#include <bk/Image>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowImage3DT;
    // -------------------- forward declaration END

    class BKCMR_EXPORT IVSDImageFilter
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = IVSDImageFilter;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        IVSDImageFilter();
        IVSDImageFilter(const IVSDImageFilter&);
        IVSDImageFilter(IVSDImageFilter&&) noexcept;
        ~IVSDImageFilter();

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] IVSDImageFilter& operator=(const IVSDImageFilter&);
        [[maybe_unused]] IVSDImageFilter& operator=(IVSDImageFilter&&) noexcept;

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        [[nodiscard]] static std::unique_ptr<DicomImage<double, 3>> apply(const FlowImage3DT& ff);
    }; // class IVSDImageFilter
  } // inline namespace cmr
} // namespace bk

#endif //BK_IVSDIMAGEFILTER_H
