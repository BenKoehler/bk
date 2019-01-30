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

#include <bkMath/fft/fft.h>

#include <array>
#include <cmath>
#include <utility>

namespace bk
{
  //====================================================================================================
  //===== HELPERS
  //====================================================================================================
  namespace details
  {
    bool power_of_2(unsigned int N, unsigned int& exponent_of_two, unsigned int& two_pow_exponent)
    {
        exponent_of_two = 0;
        two_pow_exponent = 1;

        while (two_pow_exponent < N)
        {
            ++exponent_of_two;
            two_pow_exponent <<= 1;
        }

        return two_pow_exponent == N;
    }

    bool is_power_of_2(unsigned int N)
    {
        unsigned int exponent_of_two = 0;
        unsigned int two_pow_exponent = 0;

        return power_of_2(N, exponent_of_two, two_pow_exponent);
    }

    /*
     * - inplace complex-to-complex FFT
     * - dir = 1 -> forward
     * - dir = -1 -> backward
     * - 2^m points required
     */
    void inplace_FFT(int dir, unsigned int exponent_of_two, double* x, double* y, bool normalization)
    {
        const unsigned int nn = 1 << exponent_of_two;

        const unsigned int i2 = nn >> 1;
        unsigned int j = 0;
        for (unsigned int i = 0; i < nn - 1; ++i)
        {
            if (i < j)
            {
                std::swap(x[i], x[j]);
                std::swap(y[i], y[j]);
            }

            unsigned int k = i2;
            for (; k <= j; k >>= 1)
            { j -= k; }

            j += k;
        }

        double c1 = -1.0;
        double c2 = 0.0;
        unsigned int l2 = 1;
        for (unsigned int l = 0; l < exponent_of_two; ++l)
        {
            const unsigned int l1 = l2;
            l2 <<= 1;
            double u1 = 1.0;
            double u2 = 0.0;
            for (j = 0; j < l1; ++j)
            {
                for (unsigned int i = j; i < nn; i += l2)
                {
                    const unsigned int i1 = i + l1;
                    const double t1 = u1 * x[i1] - u2 * y[i1];
                    const double t2 = u1 * y[i1] + u2 * x[i1];
                    x[i1] = x[i] - t1;
                    y[i1] = y[i] - t2;
                    x[i] += t1;
                    y[i] += t2;
                }
                const double z = u1 * c1 - u2 * c2;
                u2 = u1 * c2 + u2 * c1;
                u1 = z;
            }
            c2 = -dir * std::sqrt(0.5 * (1.0 - c1));
            c1 = std::sqrt(0.5 * (1.0 + c1));
        }

        if (normalization && dir == 1)
        {
            for (int i = 0; i < static_cast<int>(nn); ++i)
            {
                x[i] /= nn;
                y[i] /= nn;
            }    
        }
    }

    void inplace_FFT(int dir, unsigned int exponent_of_two, std::complex<double>* c, bool normalization)
    {
        const unsigned int nn = 1 << exponent_of_two;

        const unsigned int i2 = nn >> 1;
        unsigned int j = 0;
        for (unsigned int i = 0; i < nn - 1; ++i)
        {
            if (i < j)
            { std::swap(c[i], c[j]); }

            unsigned int k = i2;
            for (; k <= j; k >>= 1)
            { j -= k; }

            j += k;
        }

        double c1 = -1.0;
        double c2 = 0.0;
        unsigned int l2 = 1;
        for (unsigned int l = 0; l < exponent_of_two; ++l)
        {
            const unsigned int l1 = l2;
            l2 <<= 1;
            double u1 = 1.0;
            double u2 = 0.0;
            for (j = 0; j < l1; ++j)
            {
                for (unsigned int i = j; i < nn; i += l2)
                {
                    const unsigned int i1 = i + l1;
                    const double t1 = u1 * c[i1].real() - u2 * c[i1].imag();
                    const double t2 = u1 * c[i1].imag() + u2 * c[i1].real();
                    c[i1].real(c[i].real() - t1);
                    c[i1].imag(c[i].imag() - t2);
                    c[i].real(c[i].real() + t1);
                    c[i].imag(c[i].imag() + t2);
                }

                const double z = u1 * c1 - u2 * c2;
                u2 = u1 * c2 + u2 * c1;
                u1 = z;
            }

            c2 = -dir * std::sqrt(0.5 * (1.0 - c1));
            c1 = std::sqrt(0.5 * (1.0 + c1));
        }

        if (normalization && dir == 1)
        {
            for (int i = 0; i < static_cast<int>(nn); ++i)
            {
                c[i].real(c[i].real() / nn);
                c[i].imag(c[i].imag() / nn);
            }
        }
    }
  } // namespace details

  //====================================================================================================
  //===== 1D
  //====================================================================================================
  namespace details
  {
    bool FFT1D(std::complex<double>* c, unsigned int size, int dir, bool normalization)
    {
        unsigned int two_pow_exponent = 0;
        unsigned int exponent_of_two = 0;
        if (!details::power_of_2(size, exponent_of_two, two_pow_exponent) || two_pow_exponent != size)
        { return false; }

        details::inplace_FFT(dir, exponent_of_two, c, normalization);

        return true;
    }
  } // namespace details

  bool FFT1D(std::complex<double>* c, unsigned int size, bool normalization)
  { return details::FFT1D(c, size, 1, normalization); }

  bool IFFT1D(std::complex<double>* c, unsigned int size)
  { return details::FFT1D(c, size, -1); }

  //====================================================================================================
  //===== 2D
  //====================================================================================================
  namespace details
  {
    bool FFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, int dir, bool normalization)
    {
        unsigned int two_pow_exponent = 0;
        unsigned int exponent_of_two_x = 0;
        if (!details::power_of_2(sizex, exponent_of_two_x, two_pow_exponent) || two_pow_exponent != sizex)
        { return false; }

        unsigned int exponent_of_two_y = 0;
        if (!details::power_of_2(sizey, exponent_of_two_y, two_pow_exponent) || two_pow_exponent != sizey)
        { return false; }

        const std::array<unsigned int,2> size{{sizex, sizey}};

        /*
         * transform rows
         */
        #pragma omp parallel for
        for (int j_ = 0; j_ < static_cast<int>(sizey); ++j_)
        {
            const unsigned int j = static_cast<unsigned int>(j_);
            std::vector<double> real(sizex);
            std::vector<double> imag(sizex);

            for (unsigned int i = 0; i < sizex; ++i)
            {
                const unsigned int lid = grid_to_list_id(size, i, j);
                real[i] = c[lid].real();
                imag[i] = c[lid].imag();
            }

            details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

            for (unsigned int i = 0; i < sizex; ++i)
            {
                const unsigned int lid = grid_to_list_id(size, i, j);
                c[lid].real(real[i]);
                c[lid].imag(imag[i]);
            }
        }

        /*
         * transform columns
         */
        #pragma omp parallel for
        for (int i_ = 0; i_ < static_cast<int>(sizex); ++i_)
        {
            const unsigned int i = static_cast<unsigned int>(i_);
            std::vector<double> real(sizey);
            std::vector<double> imag(sizey);

            for (unsigned int j = 0; j < sizey; ++j)
            {
                const unsigned int lid = grid_to_list_id(size, i, j);
                real[j] = c[lid].real();
                imag[j] = c[lid].imag();
            }

            details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

            for (unsigned int j = 0; j < sizey; ++j)
            {
                const unsigned int lid = grid_to_list_id(size, i, j);
                c[lid].real(real[j]);
                c[lid].imag(imag[j]);
            }
        }

        return true;
    }
  } // namespace details

  bool FFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, bool normalization)
  { return details::FFT2D(c, sizex, sizey, 1, normalization); }

  bool IFFT2D(std::complex<double>* c, unsigned int sizex, unsigned int sizey)
  { return details::FFT2D(c, sizex, sizey, -1); }

  //====================================================================================================
  //===== 3D
  //====================================================================================================
  namespace details
  {
    bool FFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, int dir, bool normalization)
    {
        unsigned int two_pow_exponent = 0;
        unsigned int exponent_of_two_x = 0;
        if (!details::power_of_2(sizex, exponent_of_two_x, two_pow_exponent) || two_pow_exponent != sizex)
        { return false; }

        unsigned int exponent_of_two_y = 0;
        if (!details::power_of_2(sizey, exponent_of_two_y, two_pow_exponent) || two_pow_exponent != sizey)
        { return false; }

        unsigned int exponent_of_two_z = 0;
        if (!details::power_of_2(sizez, exponent_of_two_z, two_pow_exponent) || two_pow_exponent != sizez)
        { return false; }

        const std::array<unsigned int,3> size{{sizex, sizey, sizez}};

        /*
         * transform x
         */
        if (sizey >= sizez)
        {
            #pragma omp parallel for
            for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
            {
                const unsigned int y = static_cast<unsigned int>(y_);
                std::vector<double> real(sizex);
                std::vector<double> imag(sizex);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[x] = c[lid].real();
                        imag[x] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[x]);
                        c[lid].imag(imag[x]);
                    }
                }
            }
        }
        else // if (sizez > sizey)
        {
            #pragma omp parallel for
            for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
            {
                const unsigned int z = static_cast<unsigned int>(z_);
                std::vector<double> real(sizex);
                std::vector<double> imag(sizex);

                for (unsigned int y = 0; y < sizey; ++y)
                {
                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[x] = c[lid].real();
                        imag[x] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[x]);
                        c[lid].imag(imag[x]);
                    }
                }
            }
        }


        /*
         * transform y
         */
        if (sizex >= sizez)
        {
            #pragma omp parallel for
            for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
            {
                const unsigned int x = static_cast<unsigned int>(x_);
                std::vector<double> real(sizey);
                std::vector<double> imag(sizey);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[y] = c[lid].real();
                        imag[y] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[y]);
                        c[lid].imag(imag[y]);
                    }
                }
            }
        }
        else // if (sizez > sizex)
        {
            #pragma omp parallel for
            for (int z_ = 0; z_ < static_cast<int>(sizez); ++z_)
            {
                const unsigned int z = static_cast<unsigned int>(z_);
                std::vector<double> real(sizey);
                std::vector<double> imag(sizey);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[y] = c[lid].real();
                        imag[y] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[y]);
                        c[lid].imag(imag[y]);
                    }
                }
            }
        }



        /*
         * transform z
         */
        if (sizex >= sizey)
        {
            #pragma omp parallel for
            for (int x_ = 0; x_ < static_cast<int>(sizex); ++x_)
            {
                const unsigned int x = static_cast<unsigned int>(x_);
                std::vector<double> real(sizez);
                std::vector<double> imag(sizez);

                for (unsigned int y = 0; y < sizey; ++y)
                {
                    for (unsigned int z = 0; z < sizez; ++z)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[z] = c[lid].real();
                        imag[z] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_z, real.data(), imag.data(), normalization);

                    for (unsigned int z = 0; z < sizez; ++z)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[z]);
                        c[lid].imag(imag[z]);
                    }
                }
            }
        }
        else // if (sizey > sizex)
        {
            #pragma omp parallel for
            for (int y_ = 0; y_ < static_cast<int>(sizey); ++y_)
            {
                const unsigned int y = static_cast<unsigned int>(y_);
                std::vector<double> real(sizez);
                std::vector<double> imag(sizez);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int z = 0; z < sizez; ++z)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        real[z] = c[lid].real();
                        imag[z] = c[lid].imag();
                    }

                    details::inplace_FFT(dir, exponent_of_two_z, real.data(), imag.data(), normalization);

                    for (unsigned int z = 0; z < sizez; ++z)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z);
                        c[lid].real(real[z]);
                        c[lid].imag(imag[z]);
                    }
                }
            }
        }

        return true;
    }
  } // namespace details

  bool FFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, bool normalization)
  { return details::FFT3D(c, sizex, sizey, sizez, 1, normalization); }

  bool IFFT3D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez)
  { return details::FFT3D(c, sizex, sizey, sizez, -1); }

  //====================================================================================================
  //===== 4D
  //====================================================================================================
  namespace details
  {
    bool FFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet, int dir, bool normalization)
    {
        unsigned int two_pow_exponent = 0;
        unsigned int exponent_of_two_x = 0;
        if (!details::power_of_2(sizex, exponent_of_two_x, two_pow_exponent) || two_pow_exponent != sizex)
        { return false; }

        unsigned int exponent_of_two_y = 0;
        if (!details::power_of_2(sizey, exponent_of_two_y, two_pow_exponent) || two_pow_exponent != sizey)
        { return false; }

        unsigned int exponent_of_two_z = 0;
        if (!details::power_of_2(sizez, exponent_of_two_z, two_pow_exponent) || two_pow_exponent != sizez)
        { return false; }

        unsigned int exponent_of_two_t = 0;
        if (!details::power_of_2(sizet, exponent_of_two_t, two_pow_exponent) || two_pow_exponent != sizet)
        { return false; }


        const std::array<unsigned int,4> size{{sizex, sizey, sizez, sizet}};
        const unsigned int stride_x = stride_of_dim(size, 0, 4);
        const unsigned int stride_y = stride_of_dim(size, 1, 4);
        const unsigned int stride_z = stride_of_dim(size, 2, 4);
        const unsigned int stride_t = stride_of_dim(size, 3, 4);

        /*
         * transform x
         */
        if (sizey >= sizez && sizey >= sizet)
        {
            #pragma omp parallel for
            for (int y = 0; y < static_cast<int>(sizey); ++y)
            {
                std::vector<double> real(sizex);
                std::vector<double> imag(sizex);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int t = 0; t < sizet; ++t)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, 0, y, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            real[x] = c[lid].real();
                            imag[x] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            c[lid].real(real[x]);
                            c[lid].imag(imag[x]);
                        }
                    }
                }
            }
        }
        else if (sizez >= sizey && sizez >= sizet)
        {
            #pragma omp parallel for
            for (int z = 0; z < static_cast<int>(sizez); ++z)
            {
                std::vector<double> real(sizex);
                std::vector<double> imag(sizex);

                for (unsigned int t = 0; t < sizet; ++t)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, 0, y, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            real[x] = c[lid].real();
                            imag[x] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            c[lid].real(real[x]);
                            c[lid].imag(imag[x]);
                        }
                    }
                }
            }
        }
        else if (sizet >= sizey && sizet >= sizez)
        {
            #pragma omp parallel for
            for (int t = 0; t < static_cast<int>(sizet); ++t)
            {
                std::vector<double> real(sizex);
                std::vector<double> imag(sizex);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, 0, y, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            real[x] = c[lid].real();
                            imag[x] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_x, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int x = 0; x < sizex; ++x, lid += stride_x)
                        {
                            c[lid].real(real[x]);
                            c[lid].imag(imag[x]);
                        }
                    }
                }
            }
        }

        /*
         * transform y
         */
        if (sizex >= sizez && sizex >= sizet)
        {
            #pragma omp parallel for
            for (int x = 0; x < static_cast<int>(sizex); ++x)
            {
                std::vector<double> real(sizey);
                std::vector<double> imag(sizey);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int t = 0; t < sizet; ++t)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, 0, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            real[y] = c[lid].real();
                            imag[y] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            c[lid].real(real[y]);
                            c[lid].imag(imag[y]);
                        }
                    }
                }
            }
        }
        else if (sizez >= sizex && sizez >= sizet)
        {
            #pragma omp parallel for
            for (int z = 0; z < static_cast<int>(sizez); ++z)
            {
                std::vector<double> real(sizey);
                std::vector<double> imag(sizey);

                for (unsigned int t = 0; t < sizet; ++t)
                {
                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, 0, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            real[y] = c[lid].real();
                            imag[y] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            c[lid].real(real[y]);
                            c[lid].imag(imag[y]);
                        }
                    }
                }
            }
        }
        else if (sizet >= sizex && sizet >= sizez)
        {
            #pragma omp parallel for
            for (int t = 0; t < static_cast<int>(sizet); ++t)
            {
                std::vector<double> real(sizey);
                std::vector<double> imag(sizey);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int x = 0; x < sizex; ++x)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, 0, z, t);
                        unsigned int lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            real[y] = c[lid].real();
                            imag[y] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_y, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int y = 0; y < sizey; ++y, lid += stride_y)
                        {
                            c[lid].real(real[y]);
                            c[lid].imag(imag[y]);
                        }
                    }
                }
            }
        }

        /*
         * transform z
         */
        if (sizex >= sizey && sizex >= sizet)
        {
            #pragma omp parallel for
            for (int x = 0; x < static_cast<int>(sizex); ++x)
            {
                std::vector<double> real(sizez);
                std::vector<double> imag(sizez);

                for (unsigned int t = 0; t < sizet; ++t)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, 0, t);
                        unsigned int lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            real[z] = c[lid].real();
                            imag[z] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_z, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            c[lid].real(real[z]);
                            c[lid].imag(imag[z]);
                        }
                    }
                }
            }
        }
        else if (sizey >= sizex && sizey >= sizet)
        {
            #pragma omp parallel for
            for (int y = 0; y < static_cast<int>(sizey); ++y)
            {
                std::vector<double> real(sizez);
                std::vector<double> imag(sizez);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int t = 0; t < sizet; ++t)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, 0, t);
                        unsigned int lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            real[z] = c[lid].real();
                            imag[z] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_z, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            c[lid].real(real[z]);
                            c[lid].imag(imag[z]);
                        }
                    }
                }
            }
        }
        else if (sizet >= sizex && sizet >= sizey)
        {
            #pragma omp parallel for
            for (int t = 0; t < static_cast<int>(sizet); ++t)
            {
                std::vector<double> real(sizez);
                std::vector<double> imag(sizez);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, 0, t);
                        unsigned int lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            real[z] = c[lid].real();
                            imag[z] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_z, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int z = 0; z < sizez; ++z, lid += stride_z)
                        {
                            c[lid].real(real[z]);
                            c[lid].imag(imag[z]);
                        }
                    }
                }
            }
        }


        /*
         * transform t
         */
        if (sizex >= sizey && sizex >= sizez)
        {
            #pragma omp parallel for
            for (int x = 0; x < static_cast<int>(sizex); ++x)
            {
                std::vector<double> real(sizet);
                std::vector<double> imag(sizet);

                for (unsigned int z = 0; z < sizez; ++z)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, z, 0);
                        unsigned int lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            real[t] = c[lid].real();
                            imag[t] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_t, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            c[lid].real(real[t]);
                            c[lid].imag(imag[t]);
                        }
                    }
                }
            }
        }
        else if (sizey >= sizex && sizey >= sizez)
        {
            #pragma omp parallel for
            for (int y = 0; y < static_cast<int>(sizey); ++y)
            {
                std::vector<double> real(sizet);
                std::vector<double> imag(sizet);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int z = 0; z < sizez; ++z)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, z, 0);
                        unsigned int lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            real[t] = c[lid].real();
                            imag[t] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_t, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            c[lid].real(real[t]);
                            c[lid].imag(imag[t]);
                        }
                    }
                }
            }
        }
        else if (sizez >= sizex && sizez >= sizey)
        {
            #pragma omp parallel for
            for (int z = 0; z < static_cast<int>(sizez); ++z)
            {
                std::vector<double> real(sizet);
                std::vector<double> imag(sizet);

                for (unsigned int x = 0; x < sizex; ++x)
                {
                    for (unsigned int y = 0; y < sizey; ++y)
                    {
                        const unsigned int lid0 = grid_to_list_id(size, x, y, z, 0);
                        unsigned int lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            real[t] = c[lid].real();
                            imag[t] = c[lid].imag();
                        }

                        details::inplace_FFT(dir, exponent_of_two_t, real.data(), imag.data(), normalization);

                        lid = lid0;

                        for (unsigned int t = 0; t < sizet; ++t, lid += stride_t)
                        {
                            c[lid].real(real[t]);
                            c[lid].imag(imag[t]);
                        }
                    }
                }
            }
        }

        return true;
    }
  } // namespace details

  bool FFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet, bool normalization)
  { return details::FFT4D(c, sizex, sizey, sizez, sizet, 1, normalization); }

  bool IFFT4D(std::complex<double>* c, unsigned int sizex, unsigned int sizey, unsigned int sizez, unsigned int sizet)
  { return details::FFT4D(c, sizex, sizey, sizez, sizet, -1); }
} // namespace bk


