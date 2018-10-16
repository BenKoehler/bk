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

#ifndef BK_ELANDMARKSEMANTIC_H
#define BK_ELANDMARKSEMANTIC_H

namespace bk
{
  inline namespace cmr
  {
    //------------------------------------------------------------------------------------------------------
    // ENUMS
    //------------------------------------------------------------------------------------------------------
    enum LandMarkSemantic_ : unsigned int
    {
        LandMarkSemantic_None = 0, //
        // aorta
        LandMarkSemantic_Aorta_AorticValve_SinusesOfValsalva = 1, //
        LandMarkSemantic_Aorta_AorticValve_SinotubularJunction = 2, //
        LandMarkSemantic_Aorta_MidAscendingAorta = 3, //
        LandMarkSemantic_Aorta_BrachiocephalicTrunk = 4, //
        LandMarkSemantic_Aorta_BetweenLeftCommonCarotid_and_LeftSubclavianArtery = 5, //
        LandMarkSemantic_Aorta_DistalToLeftSubclavianArtery = 6, //
        LandMarkSemantic_Aorta_MidDescendingAorta = 7, //
        LandMarkSemantic_Aorta_Diaphragm = 8, //
        LandMarkSemantic_Aorta_AbdominalAboveCoeliacTrunk = 9, //
        // pulmonary artery
        LandMarkSemantic_PulmonaryArtery_PulmonaryValve_Trunk = 10, //
        LandMarkSemantic_PulmonaryArtery_BeforeJunction = 11, //
        LandMarkSemantic_PulmonaryArtery_LeftPulmonaryArtery_Begin = 12, //
        LandMarkSemantic_PulmonaryArtery_LeftPulmonaryArtery_MidEnd = 13, //
        LandMarkSemantic_PulmonaryArtery_RightPulmonaryArtery_Begin = 14, //
        LandMarkSemantic_PulmonaryArtery_RightPulmonaryArtery_MidEnd = 15, //
    };

    using LandMarkSemantic = unsigned int;

    //------------------------------------------------------------------------------------------------------
    // STRUCTS
    //------------------------------------------------------------------------------------------------------
    struct LandMark
    {
        unsigned int centerline_id;
        unsigned int point_id;
        LandMarkSemantic semantic;
    };
  } // inline namespace cmr
} // namespace bk

#endif //BK_ELANDMARKSEMANTIC_H
