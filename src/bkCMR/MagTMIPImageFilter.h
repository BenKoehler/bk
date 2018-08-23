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

#ifndef BK_MAGTMIPIMAGEFILTER_H
#define BK_MAGTMIPIMAGEFILTER_H

#include <memory>

#include <bk/Image>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    class BKCMR_EXPORT MagTMIPImageFilter
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = MagTMIPImageFilter;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        MagTMIPImageFilter();
        MagTMIPImageFilter(const MagTMIPImageFilter&);
        MagTMIPImageFilter(MagTMIPImageFilter&&) noexcept;
        ~MagTMIPImageFilter();

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        [[maybe_unused]] MagTMIPImageFilter& operator=(const MagTMIPImageFilter&);
        [[maybe_unused]] MagTMIPImageFilter& operator=(MagTMIPImageFilter&&) noexcept;

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        [[nodiscard]] static std::unique_ptr<DicomImage<double, 3>> apply(const DicomImage<double, 4>& mag);
    }; // class MagTMIPImageFilter
  } // inline namespace cmr
} // namespace bk

#endif //BK_MAGTMIPIMAGEFILTER_H
