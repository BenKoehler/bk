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

#include <iostream>
#include <random>
#include <vector>

#include <bkAlgorithm/histogram.h>
#include <bkAlgorithm/histogram_equalization.h>

void histogram_example()
{
    std::cout << "Histogram example" << std::endl;
    std::cout << "-----------------" << std::endl;

    std::vector<int> a{4, 2, 5, 7, 3, 6, 7, 8, 2, 2, 5, 7, 7, 7, 7};

    std::cout << "values: ";
    for (auto x: a)
    { std::cout << x << " "; }
    std::cout << std::endl;

    /* -------------------- histogram -------------------- */

    std::cout << std::endl;
    std::cout << "histogram" << std::endl;

    auto h = bk::histogram(a.begin(), a.end()); // std::map<int, unsigned int>

    for (const auto&[x, N] : h)
    { std::cout << x << ": " << N << "x" << std::endl; }
    // output: 2: 3x
    //         3: 1x
    //         4: 1x
    //         5: 2x
    //         6: 1x
    //         7: 6x
    //         8: 1x

    /* -------------------- histogram_n -------------------- */

    std::cout << std::endl;
    std::cout << "histogram_n" << std::endl;

    auto hn0 = bk::histogram_n(a.begin(), a.end(), 10);

    for (const auto&[x, N] : hn0)
    { std::cout << x << ": " << N << "x" << std::endl; }
    // output: 2      : 3x
    //         2.66667: 0x
    //         3.33333: 1x
    //         4      : 1x
    //         4.66667: 0x
    //         5.33333: 2x
    //         6      : 1x
    //         6.66667: 0x
    //         7.33333: 6x
    //         8      : 1x

    auto hn1 = bk::histogram_n(a.begin(), a.end(), /*fixedHistogramMin =*/0, /*fixedHistogramMax =*/10, /*num_buckets =*/21);
    for (const auto&[x, N] : hn1)
    { std::cout << x << ": " << N << "x" << std::endl; }
    // output: 0  : 0x
    //         0.5: 0x
    //         1  : 0x
    //         1.5: 0x
    //         2  : 3x
    //         2.5: 0x
    //         3: 1x
    //         3.5: 0x
    //         4: 1x
    //         4.5: 0x
    //         5: 2x
    //         5.5: 0x
    //         6: 1x
    //         6.5: 0x
    //         7: 6x
    //         7.5: 0x
    //         8: 1x
    //         8.5: 0x
    //         9: 0x
    //         9.5: 0x
    //         10: 0x

    /* -------------------- histogram_precision -------------------- */

    std::cout << std::endl;
    std::cout << "histogram_precision" << std::endl;

    auto hp = bk::histogram_precision(a.begin(), a.end(), /*precision =*/2);
    for (const auto&[x, N] : hp)
    { std::cout << x << ": " << N << "x" << std::endl; }
    // output: 2: 3x
    //         4: 2x
    //         6: 3x
    //         8: 7x


    /* -------------------- histogram_n_precision -------------------- */

    std::cout << std::endl;
    std::cout << "histogram_n_precision" << std::endl;

    auto hnp = bk::histogram_n_precision(a.begin(), a.end(), /*fixedHistogramMin =*/0.0, /*fixedHistogramMax =*/10.0, /*precision =*/0.5);

    for (const auto&[x, N] : hnp)
    { std::cout << x << ": " << N << "x" << std::endl; }
    // output: 0: 0x
    //         0.5: 0x
    //         1: 0x
    //         1.5: 0x
    //         2: 3x
    //         2.5: 0x
    //         3: 1x
    //         3.5: 0x
    //         4: 1x
    //         4.5: 0x
    //         5: 2x
    //         5.5: 0x
    //         6: 1x
    //         6.5: 0x
    //         7: 6x
    //         7.5: 0x
    //         8: 1x
    //         8.5: 0x
    //         9: 0x
    //         9.5: 0x
    //         10: 0x
}

void histogram_equalization_example()
{
    std::cout << "Histogram equalization example" << std::endl;
    std::cout << "-----------------" << std::endl;

    std::vector<int> img(50'000);

    // random image that uses only value range [100,200] instead of possible [0,255]

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(100, 200);

    for (int& x: img)
    { x = dist(mt); }

    auto img_eq = img;
    bk::histogram_equalization(img_eq.begin(), img_eq.end(), 0, 255);

    auto h0 = bk::histogram_n_precision(img.begin(), img.end(), /*fixedHistogramMin =*/0, /*fixedHistogramMax =*/255, /*precision =*/15);
    auto h1 = bk::histogram_n_precision(img_eq.begin(), img_eq.end(), /*fixedHistogramMin =*/0, /*fixedHistogramMax =*/255, /*precision =*/15);

    // create rough histograms
    auto itBefore = h0.begin();
    auto itAfter = h1.begin();
    for(; itBefore != h0.end(); ++itBefore, ++itAfter)
    { std::cout << itBefore->first << ": before " << itBefore->second << "x, after " << itAfter->second << "x" << std::endl; }
    // output:   0: before    0x, after 1503x
    //           15: before    0x, after 2973x
    //           30: before    0x, after 2511x
    //           45: before    0x, after 3018x
    //           60: before    0x, after 2996x
    //           75: before    0x, after 2913x
    //           90: before    0x, after 3051x
    //           105: before 6469x, after 2878x
    //           120: before 7512x, after 2997x
    //           135: before 7402x, after 3022x
    //           150: before 7490x, after 2960x
    //           165: before 7182x, after 2849x
    //           180: before 7499x, after 2848x
    //           195: before 6446x, after 3016x
    //           210: before    0x, after 3051x
    //           225: before    0x, after 2964x
    //           240: before    0x, after 2928x
    //           255: before    0x, after 1522x
}

int main(int, char**)
{
    histogram_example();
    std::cout << std::endl;
    histogram_equalization_example();

    return EXIT_SUCCESS;
}

