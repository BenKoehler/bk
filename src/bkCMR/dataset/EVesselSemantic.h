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

#ifndef BK_EVESSELSEMANTIC_H
#define BK_EVESSELSEMANTIC_H

namespace bk
{
  inline namespace cmr
  {
    enum VesselSemantic_ : unsigned int
    {
        VesselSemantic_None = 0, //
        VesselSemantic_LeftHeart_Aorta = 1, //
        VesselSemantic_LeftHeart_Ventricle = 1 << 1, //
        VesselSemantic_LeftHeart_Atrium = 1 << 2, //
        VesselSemantic_LeftHeart_PulmonaryVeins = 1 << 3, //
        VesselSemantic_RightHeart_PulmonaryArtery = 1 << 4, //
        VesselSemantic_RightHeart_Ventricle = 1 << 5, //
        VesselSemantic_RightHeart_Atrium = 1 << 6, //
        VesselSemantic_RightHeart_VenaCava = 1 << 7, //
    };

    using VesselSemantic = unsigned int;
  } // inline namespace cmr
} // namespace bk

#endif //BK_EVESSELSEMANTIC_H
