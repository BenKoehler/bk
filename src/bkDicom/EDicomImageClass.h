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

#ifndef BKDICOM_DICOMIMAGECLASS_H
#define BKDICOM_DICOMIMAGECLASS_H

namespace bk
{
  enum DicomImageClass_ : unsigned char
  {
      DicomImageClass_None                = 0, //
      DicomImageClass_3DT_Flow            = 1, //
      DicomImageClass_3DT_Anatomy         = 2, //
      DicomImageClass_3DT_Magnitude       = 3, //
      DicomImageClass_3DT_SignalIntensity = 4, //
      DicomImageClass_3D_Anatomy          = 5, //
      DicomImageClass_2DT_Flow            = 6, //
      DicomImageClass_2DT_Anatomy         = 7, //
      DicomImageClass_2D_Anatomy          = 8, //
      DicomImageClass_2DT_Magnitude       = 9, //
  };
} // namespace bk

#endif //BKDICOM_DICOMIMAGECLASS_H


