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

#ifndef BK_SHADERLIBRARY_CMR_H
#define BK_SHADERLIBRARY_CMR_H

#include <bkGL/shader/ShaderLibrary.h>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  inline namespace cmr
  {
    struct BKCMR_EXPORT ShaderLibrary_CMR : public bk::details::ShaderLibrary
    {
        /// @{ -------------------------------------------------- UBO
        [[nodiscard]] static std::string ubo_definition_measuring_plane_preview();
        /// @}

        //====================================================================================================
        //===== MEASURING PLANE PREVIEW
        //====================================================================================================
        struct measuring_plane_preview
        {
            [[nodiscard]] static std::string vert();
            [[nodiscard]] static std::string frag();
        };
    }; // struct ShaderLibrary_CMR
  } // inline namespace cmr
} // namespace bk

#endif //BK_SHADERLIBRARY_CMR_H
