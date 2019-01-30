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
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#ifndef BKMATH_FFT_IMPL_H
#define BKMATH_FFT_IMPL_H

#include <array>
#include <type_traits>

#include <bkMath/fft/fft.h>
#include <bkMath/functions/list_grid_id_conversion.h>

namespace bk
{
  //====================================================================================================
  //===== 1D
  //====================================================================================================
  template<typename IndexAccessibleContainer_>
  void FFTShift1D(IndexAccessibleContainer_& c, unsigned int size)
  {
      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (size % 2 == 0)
      {
          for (unsigned int x = 0; x < size >> 1; ++x)
          {
              const unsigned int x2 = x + (size >> 1);
              std::swap(c[x], c[x2]);
          }
      }
      else
      {
          std::vector<value_type> temp(size);

          for (unsigned int x = 0; x < size; ++x)
          { temp[x] = c[x]; }

          // fill left half
          for (unsigned int x = 0; x < (size >> 1); ++x)
          { c[x] = temp[x + (size >> 1) + 1]; }

          // fill right half
          for (unsigned int x = (size >> 1); x < size; ++x)
          { c[x] = temp[x - (size >> 1)]; }
      }
  }

  template<typename IndexAccessibleContainer_>
  void IFFTShift1D(IndexAccessibleContainer_& c, unsigned int size)
  {
      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (size % 2 == 0)
      {
          for (unsigned int x = 0; x < size >> 1; ++x)
          {
              const unsigned int x2 = x + (size >> 1);
              std::swap(c[x], c[x2]);
          }
      }
      else
      {
          std::vector<value_type> temp(size);

          for (unsigned int x = 0; x < size; ++x)
          { temp[x] = c[x]; }

          // fill left half
          for (unsigned int x = 0; x <= (size >> 1); ++x)
          { c[x] = temp[x + (size >> 1)]; }

          // fill right half
          for (unsigned int x = (size >> 1) + 1; x < size; ++x)
          { c[x] = temp[x - (size >> 1) - 1]; }
      }
  }
  
  //====================================================================================================
  //===== 2D
  //====================================================================================================
  template<typename IndexAccessibleContainer_>
  void FFTShift2D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey)
  {
      const std::array<unsigned int,2> size{{sizex, sizey}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              const unsigned int x2 = x + (sizex >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              { std::swap(c[bk::grid_to_list_id(size, x, y)], c[bk::grid_to_list_id(size, x2, y)]); }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              std::vector<value_type> temp(sizex);

              for (unsigned int x = 0; x < sizex; ++x)
              { temp[x] = c[bk::grid_to_list_id(size, x, y)]; }

              // fill left half
              for (unsigned int x = 0; x < (sizex >> 1); ++x)
              { c[bk::grid_to_list_id(size, x, y)] = temp[x + (sizex >> 1) + 1]; }

              // fill right half
              for (unsigned int x = (sizex >> 1); x < sizex; ++x)
              { c[bk::grid_to_list_id(size, x, y)] = temp[x - (sizex >> 1)]; }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              const unsigned int y2 = y + (sizey >> 1);

              for (unsigned int x = 0; x < sizex; ++x)
              { std::swap(c[bk::grid_to_list_id(size, x, y)], c[bk::grid_to_list_id(size, x, y2)]); }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              std::vector<value_type> temp(sizey);

              for (unsigned int y = 0; y < sizey; ++y)
              { temp[y] = c[bk::grid_to_list_id(size, x, y)]; }

              // fill left half
              for (unsigned int y = 0; y < (sizey >> 1); ++y)
              { c[bk::grid_to_list_id(size, x, y)] = temp[y + (sizey >> 1) + 1]; }

              // fill right half
              for (unsigned int y = (sizey >> 1); y < sizey; ++y)
              { c[bk::grid_to_list_id(size, x, y)] = temp[y - (sizey >> 1)]; }
          }
      }
  }

  template<typename IndexAccessibleContainer_>
  void IFFTShift2D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey)
  {
      const std::array<unsigned int,2> size{{sizex, sizey}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              const unsigned int x2 = x + (sizex >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              { std::swap(c[bk::grid_to_list_id(size, x, y)], c[bk::grid_to_list_id(size, x2, y)]); }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              std::vector<value_type> temp(sizex);

              for (unsigned int x = 0; x < sizex; ++x)
              { temp[x] = c[bk::grid_to_list_id(size, x, y)]; }

              // fill left half
              for (unsigned int x = 0; x <= (sizex >> 1); ++x)
              { c[bk::grid_to_list_id(size, x, y)] = temp[x + (sizex >> 1)]; }

              // fill right half
              for (unsigned int x = (sizex >> 1) + 1; x < sizex; ++x)
              { c[bk::grid_to_list_id(size, x, y)] = temp[x - (sizex >> 1) - 1]; }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              const unsigned int y2 = y + (sizey >> 1);

              for (unsigned int x = 0; x < sizex; ++x)
              { std::swap(c[bk::grid_to_list_id(size, x, y)], c[bk::grid_to_list_id(size, x, y2)]); }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              std::vector<value_type> temp(sizey);

              for (unsigned int y = 0; y < sizey; ++y)
              { temp[y] = c[bk::grid_to_list_id(size, x, y)]; }

              // fill left half
              for (unsigned int y = 0; y <= (sizey >> 1); ++y)
              { c[bk::grid_to_list_id(size, x, y)] = temp[y + (sizey >> 1)]; }

              // fill right half
              for (unsigned int y = (sizey >> 1) + 1; y < sizey; ++y)
              { c[bk::grid_to_list_id(size, x, y)] = temp[y - (sizey >> 1) - 1]; }
          }
      }
  }

  //====================================================================================================
  //===== 3D
  //====================================================================================================
  template<typename IndexAccessibleContainer_>
  void FFTShift3D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez)
  {
      const std::array<unsigned int,3> size{{sizex, sizey, sizez}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              const unsigned int x2 = x + (sizex >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x2, y, z)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              std::vector<value_type> temp(sizex);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  { temp[x] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int x = 0; x < (sizex >> 1); ++x)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[x + (sizex >> 1) + 1]; }

                  // fill right half
                  for (unsigned int x = (sizex >> 1); x < sizex; ++x)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[x - (sizex >> 1)]; }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              const unsigned int y2 = y + (sizey >> 1);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x, y2, z)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              std::vector<value_type> temp(sizey);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  { temp[y] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int y = 0; y < (sizey >> 1); ++y)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[y + (sizey >> 1) + 1]; }

                  // fill right half
                  for (unsigned int y = (sizey >> 1); y < sizey; ++y)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[y - (sizey >> 1)]; }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // z
      //------------------------------------------------------------------------------------------------------
      if (sizez % 2 == 0)
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez >> 1); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              const unsigned int z2 = z + (sizez >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x, y, z2)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              std::vector<value_type> temp(sizez);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { temp[z] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int z = 0; z < (sizez >> 1); ++z)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[z + (sizez >> 1) + 1]; }

                  // fill right half
                  for (unsigned int z = (sizez >> 1); z < sizez; ++z)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[z - (sizez >> 1)]; }
              }
          }
      }
  }

  template<typename IndexAccessibleContainer_>
  void IFFTShift3D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez)
  {
      const std::array<unsigned int,3> size{{sizex, sizey, sizez}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              const unsigned int x2 = x + (sizex >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x2, y, z)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              std::vector<value_type> temp(sizex);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  { temp[x] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int x = 0; x <= (sizex >> 1); ++x)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[x + (sizex >> 1)]; }

                  // fill right half
                  for (unsigned int x = (sizex >> 1) + 1; x < sizex; ++x)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[x - (sizex >> 1) - 1]; }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              const unsigned int y2 = y + (sizey >> 1);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x, y2, z)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              std::vector<value_type> temp(sizey);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  { temp[y] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int y = 0; y <= (sizey >> 1); ++y)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[y + (sizey >> 1)]; }

                  // fill right half
                  for (unsigned int y = (sizey >> 1) + 1; y < sizey; ++y)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[y - (sizey >> 1) - 1]; }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // z
      //------------------------------------------------------------------------------------------------------
      if (sizez % 2 == 0)
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez >> 1); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              const unsigned int z2 = z + (sizez >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  { std::swap(c[bk::grid_to_list_id(size, x, y, z)], c[bk::grid_to_list_id(size, x, y, z2)]); }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              std::vector<value_type> temp(sizez);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  { temp[z] = c[bk::grid_to_list_id(size, x, y, z)]; }

                  // fill left half
                  for (unsigned int z = 0; z <= (sizez >> 1); ++z)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[z + (sizez >> 1)]; }

                  // fill right half
                  for (unsigned int z = (sizez >> 1) + 1; z < sizez; ++z)
                  { c[bk::grid_to_list_id(size, x, y, z)] = temp[z - (sizez >> 1) - 1]; }
              }
          }
      }
  }

  //====================================================================================================
  //===== 4D
  //====================================================================================================
  template<typename IndexAccessibleContainer_>
  void FFTShift4D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet)
  {
      const std::array<unsigned int,4> size{{sizex, sizey, sizez, sizet}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          //#pragma omp parallel for
          //for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          //{
          //    const unsigned int x = static_cast<unsigned int>(x_);
          //    const unsigned int x2 = x + (sizex >> 1);
          //
          //    for (unsigned int y = 0; y < sizey; ++y)
          //    {
          //        for (unsigned int z = 0; z < sizez; ++z)
          //        {
          //            for (unsigned int t = 0; t < sizet; ++t)
          //            {
          //                std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x2, y, z, t)]);
          //            }
          //        }
          //    }
          //}

          const unsigned int stride_x = stride_of_dim(size, 0, 4);

          #pragma omp parallel for
          for (int y = 0; y < static_cast<int>(sizey); ++y)
          {
              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int t = 0; t < sizet; ++t)
                  {
                      unsigned int lidx1 = bk::grid_to_list_id(size, 0, y, z, t);
                      unsigned int lidx2 = bk::grid_to_list_id(size, sizex >> 1, y, z, t);

                      for (unsigned int x = 0; x < (sizex >> 1); ++x, lidx1 += stride_x, lidx2 += stride_x)
                      { std::swap(c[lidx1], c[lidx2]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizex);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  {
                      for (unsigned int x = 0; x < sizex; ++x)
                      { temp[x] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int x = 0; x < (sizex >> 1); ++x)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[x + (sizex >> 1) + 1]; }

                      // fill right half
                      for (unsigned int x = (sizex >> 1); x < sizex; ++x)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[x - (sizex >> 1)]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          //#pragma omp parallel for
          //for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          //{
          //    const unsigned int y = static_cast<unsigned int>(y_);
          //    const unsigned int y2 = y + (sizey >> 1);
          //
          //    for (unsigned int x = 0; x < sizex; ++x)
          //    {
          //        for (unsigned int z = 0; z < sizez; ++z)
          //        {
          //            for (unsigned int t = 0; t < sizet; ++t)
          //            {
          //                std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y2, z, t)]);
          //            }
          //        }
          //    }
          //}

          const unsigned int stride_y = stride_of_dim(size, 1, 4);

          #pragma omp parallel for
          for (int x = 0; x < static_cast<int>(sizex); ++x)
          {
              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int t = 0; t < sizet; ++t)
                  {
                      unsigned int lidy1 = bk::grid_to_list_id(size, x, 0, z, t);
                      unsigned int lidy2 = bk::grid_to_list_id(size, x, sizey >> 1, z, t);

                      for (unsigned int y = 0; y < (sizey >> 1); ++y, lidy1 += stride_y, lidy2 += stride_y)
                      { std::swap(c[lidy1], c[lidy2]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizey);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  {
                      for (unsigned int y = 0; y < sizey; ++y)
                      { temp[y] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int y = 0; y < (sizey >> 1); ++y)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[y + (sizey >> 1) + 1]; }

                      // fill right half
                      for (unsigned int y = (sizey >> 1); y < sizey; ++y)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[y - (sizey >> 1)]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // z
      //------------------------------------------------------------------------------------------------------
      if (sizez % 2 == 0)
      {
          //#pragma omp parallel for
          //for (int z_ = 0; z_ < static_cast<int>(sizez >> 1); ++z_)
          //{
          //    const unsigned int z = static_cast<unsigned int>(z_);
          //    const unsigned int z2 = z + (sizez >> 1);
          //
          //    for (unsigned int y = 0; y < sizey; ++y)
          //    {
          //        for (unsigned int x = 0; x < sizex; ++x)
          //        {
          //            for (unsigned int t = 0; t < sizet; ++t)
          //            {
          //                std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y, z2, t)]);
          //            }
          //        }
          //    }
          //}

          const unsigned int stride_z = stride_of_dim(size, 2, 4);

          #pragma omp parallel for
          for (int x = 0; x < static_cast<int>(sizex); ++x)
          {
              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int t = 0; t < sizet; ++t)
                  {
                      unsigned int lidz1 = bk::grid_to_list_id(size, x, y, 0, t);
                      unsigned int lidz2 = bk::grid_to_list_id(size, x, y, sizez >> 1, t);

                      for (unsigned int z = 0; z < (sizez >> 1); ++z, lidz1 += stride_z, lidz2 += stride_z)
                      { std::swap(c[lidz1], c[lidz2]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizez);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  {
                      for (unsigned int z = 0; z < sizez; ++z)
                      { temp[z] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int z = 0; z < (sizez >> 1); ++z)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[z + (sizez >> 1) + 1]; }

                      // fill right half
                      for (unsigned int z = (sizez >> 1); z < sizez; ++z)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[z - (sizez >> 1)]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // t
      //------------------------------------------------------------------------------------------------------
      if (sizet % 2 == 0)
      {
          //#pragma omp parallel for
          //for (int t_ = 0; t_ < static_cast<int>(sizet >> 1); ++t_)
          //{
          //    const unsigned int t = static_cast<unsigned int>(t_);
          //    const unsigned int t2 = t + (sizet >> 1);
          //
          //    for (unsigned int y = 0; y < sizey; ++y)
          //    {
          //        for (unsigned int z = 0; z < sizez; ++z)
          //        {
          //            for (unsigned int x = 0; x < sizex; ++x)
          //            {
          //                std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y, z, t2)]);
          //            }
          //        }
          //    }
          //}

          const unsigned int stride_t = stride_of_dim(size, 3, 4);

          #pragma omp parallel for
          for (int x = 0; x < static_cast<int>(sizex); ++x)
          {
              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  {
                      unsigned int lidt1 = bk::grid_to_list_id(size, x, y, z, 0);
                      unsigned int lidt2 = bk::grid_to_list_id(size, x, y, z, sizet >> 1);

                      for (unsigned int t = 0; t < (sizet >> 1); ++t, lidt1 += stride_t, lidt2 += stride_t)
                      { std::swap(c[lidt1], c[lidt2]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              std::vector<value_type> temp(sizet);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  {
                      for (unsigned int t = 0; t < sizet; ++t)
                      { temp[t] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int t = 0; t < (sizet >> 1); ++t)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[t + (sizet >> 1) + 1]; }

                      // fill right half
                      for (unsigned int t = (sizet >> 1); t < sizet; ++t)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[t - (sizet >> 1)]; }
                  }
              }
          }
      }
  }

  template<typename IndexAccessibleContainer_>
  void IFFTShift4D(IndexAccessibleContainer_& c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet)
  {
      const std::array<unsigned int,4> size{{sizex, sizey, sizez, sizet}};

      using value_type = std::decay_t<decltype(c[0])>;

      //------------------------------------------------------------------------------------------------------
      // x
      //------------------------------------------------------------------------------------------------------
      if (sizex % 2 == 0)
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex >> 1); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              const unsigned int x2 = x + (sizex >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  {
                      for (unsigned int t = 0; t < sizet; ++t)
                      { std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x2, y, z, t)]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizex);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  {
                      for (unsigned int x = 0; x < sizex; ++x)
                      { temp[x] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int x = 0; x <= (sizex >> 1); ++x)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[x + (sizex >> 1)]; }

                      // fill right half
                      for (unsigned int x = (sizex >> 1) + 1; x < sizex; ++x)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[x - (sizex >> 1) - 1]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // y
      //------------------------------------------------------------------------------------------------------
      if (sizey % 2 == 0)
      {
          #pragma omp parallel for
          for (int y_ = 0; y_ < static_cast<int>(sizey >> 1); ++y_)
          {
              const unsigned int y = static_cast<unsigned int>(y_);
              const unsigned int y2 = y + (sizey >> 1);

              for (unsigned int x = 0; x < sizex; ++x)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  {
                      for (unsigned int t = 0; t < sizet; ++t)
                      { std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y2, z, t)]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizey);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  {
                      for (unsigned int y = 0; y < sizey; ++y)
                      { temp[y] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int y = 0; y <= (sizey >> 1); ++y)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[y + (sizey >> 1)]; }

                      // fill right half
                      for (unsigned int y = (sizey >> 1) + 1; y < sizey; ++y)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[y - (sizey >> 1) - 1]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // z
      //------------------------------------------------------------------------------------------------------
      if (sizez % 2 == 0)
      {
          #pragma omp parallel for
          for (int z_ = 0; z_ < static_cast<int>(sizez >> 1); ++z_)
          {
              const unsigned int z = static_cast<unsigned int>(z_);
              const unsigned int z2 = z + (sizez >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  {
                      for (unsigned int t = 0; t < sizet; ++t)
                      { std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y, z2, t)]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              std::vector<value_type> temp(sizez);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int x = 0; x < sizex; ++x)
                  {
                      for (unsigned int z = 0; z < sizez; ++z)
                      { temp[z] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int z = 0; z <= (sizez >> 1); ++z)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[z + (sizez >> 1)]; }

                      // fill right half
                      for (unsigned int z = (sizez >> 1) + 1; z < sizez; ++z)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[z - (sizez >> 1) - 1]; }
                  }
              }
          }
      }

      //------------------------------------------------------------------------------------------------------
      // t
      //------------------------------------------------------------------------------------------------------
      if (sizet % 2 == 0)
      {
          #pragma omp parallel for
          for (int t_ = 0; t_ < static_cast<int>(sizet >> 1); ++t_)
          {
              const unsigned int t = static_cast<unsigned int>(t_);
              const unsigned int t2 = t + (sizet >> 1);

              for (unsigned int y = 0; y < sizey; ++y)
              {
                  for (unsigned int z = 0; z < sizez; ++z)
                  {
                      for (unsigned int x = 0; x < sizex; ++x)
                      { std::swap(c[bk::grid_to_list_id(size, x, y, z, t)], c[bk::grid_to_list_id(size, x, y, z, t2)]); }
                  }
              }
          }
      }
      else
      {
          #pragma omp parallel for
          for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
          {
              const unsigned int x = static_cast<unsigned int>(x_);
              std::vector<value_type> temp(sizet);

              for (unsigned int z = 0; z < sizez; ++z)
              {
                  for (unsigned int y = 0; y < sizey; ++y)
                  {
                      for (unsigned int t = 0; t < sizet; ++t)
                      { temp[t] = c[bk::grid_to_list_id(size, x, y, z, t)]; }

                      // fill left half
                      for (unsigned int t = 0; t <= (sizet >> 1); ++t)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[t + (sizet >> 1)]; }

                      // fill right half
                      for (unsigned int t = (sizet >> 1) + 1; t < sizet; ++t)
                      { c[bk::grid_to_list_id(size, x, y, z, t)] = temp[t - (sizet >> 1) - 1]; }
                  }
              }
          }
      }
  }
} // namespace bk

#endif // BKMATH_FFT_IMPL_H


