/*
 * This FFT implementation is based on the one by Paul Bourke, which was
 * published in 1993 and can be freely used.
 *
 * http://paulbourke.net/miscellaneous/dft/
 * http://paulbourke.net/miscellaneous/
 *
 * "The contents of this web site are © Copyright Paul Bourke or a third party
 * contributor where indicated. You may print or save an electronic copy of
 * parts of this web site for your own personal use. Permission must be sought
 * for any other use. Any source code found here may be freely used provided
 * credits are given to the author. Purchase of credit free licenses of material
 * found on this site can be negotiated with the author. The author can also
 * quote for unique variations and/or higher resolution versions of the images
 * found on this site."
 *
 * -----------------------------------------------------
 *
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

#ifndef BKMATH_FFT_H
#define BKMATH_FFT_H

#include <complex>
#include <vector>
#include <type_traits>

#include "../lib/bkMath_export.h"

namespace bk
{
  //====================================================================================================
  //===== HELPERS
  //====================================================================================================
  namespace details
  {
    bool BKMATH_EXPORT power_of_2(unsigned int N, unsigned int& exponent_of_two, unsigned int& two_pow_exponent);
    bool BKMATH_EXPORT is_power_of_2(unsigned int N);
    void BKMATH_EXPORT inplace_FFT(int dir, unsigned int exponent_of_two, double* x, double* y, bool normalization = true);
    void BKMATH_EXPORT inplace_FFT(int dir, unsigned int exponent_of_two, std::complex<double>* c, bool normalization = true);
  } // namespace details

  //====================================================================================================
  //===== 1D
  //====================================================================================================
  namespace details
  {
    bool BKMATH_EXPORT FFT1D(std::complex<double>* c, unsigned int size, int dir, bool normalization = true);
  } // namespace details

  bool BKMATH_EXPORT FFT1D(std::complex<double>* c, unsigned int size, bool normalization = true);
  bool BKMATH_EXPORT IFFT1D(std::complex<double>* c, unsigned int size);

  //====================================================================================================
  //===== 2D
  //====================================================================================================
  namespace details
  {
    bool BKMATH_EXPORT FFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, int dir, bool normalization = true);
  } // namespace details

  bool BKMATH_EXPORT FFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, bool normalization = true);
  bool BKMATH_EXPORT IFFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey);

  template<typename IndexAccessibleContainer_>
  void FFTShift2D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey);
  template<typename IndexAccessibleContainer_>
  void IFFTShift2D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey);

  //====================================================================================================
  //===== 3D
  //====================================================================================================
  namespace details
  {
    bool BKMATH_EXPORT FFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, int dir, bool normalization = true);
  } // namespace details

  bool BKMATH_EXPORT FFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, bool normalization = true);
  bool BKMATH_EXPORT IFFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez);

  template<typename IndexAccessibleContainer_>
  void FFTShift3D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez);
  template<typename IndexAccessibleContainer_>
  void IFFTShift3D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez);

  //====================================================================================================
  //===== 4D
  //====================================================================================================
  namespace details
  {
    bool BKMATH_EXPORT FFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet, int dir, bool normalization = true);
  } // namespace details

  bool BKMATH_EXPORT FFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet, bool normalization = true);
  bool BKMATH_EXPORT IFFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet);

  template<typename IndexAccessibleContainer_>
  void FFTShift4D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet);
  template<typename IndexAccessibleContainer_>
  void IFFTShift4D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet);

} // namespace bk

#include "fft.hpp"

#endif //BKMATH_FFT_H


