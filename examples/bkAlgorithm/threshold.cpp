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

#include <bkAlgorithm/threshold.h>

int main(int, char**)
{
    // create 100x100 image with values [0,255]
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    std::vector<int> img(100 * 100);
    std::for_each(img.begin(), img.end(), [&](int& x)
    { x = dist(mt); });

    /* -------------------- threshold (manual) -------------------- */

    // - values < 128 will be set to 0
    // - values >= 128 will be set to 1
    auto b(img);
    bk::threshold(b.begin(), b.end(), /*threshold =*/128, /*value below threshold =*/0, /*value above threshold =*/1);
    // bk::threshold(b.begin(), b.end(), 128); // same as above; 0 and 1 are default values

    std::cout << std::count(b.begin(), b.end(), 0) << std::endl; // output: 5029
    std::cout << std::count(b.begin(), b.end(), 1) << std::endl; // output: 4971

    /* -------------------- threshold (otsu) -------------------- */

    // threshold is automatically determined using otsu method

    auto c(img);
    // bk::otsu(c.begin(), c.end()); // output: 124.512
    bk::threshold(c.begin(), c.end());
    //bk::threshold(c.begin(), c.end(), bk::otsu(c.begin(), c.end())); // same as above

    std::cout << std::count(c.begin(), c.end(), 0) << std::endl; // output: 4857
    std::cout << std::count(c.begin(), c.end(), 1) << std::endl; // output: 5143

    /* -------------------- interval threshold -------------------- */

    // - values < 100 or values >= 150 will be set to 0
    // - values >= 100 or values < 150 will be set to 1
    auto d(img);
    bk::interval_threshold(d.begin(), d.end(), /*lower threshold =*/100, /*upper threshold =*/150, /*value inside threshold =*/1, /*value outside threshold =*/0);
    //bk::interval_threshold(d.begin(), d.end(), 100, 150); // same as above; 0 and 1 are default values

    std::cout << std::count(d.begin(), d.end(), 0) << std::endl; // output: 8019
    std::cout << std::count(d.begin(), d.end(), 1) << std::endl; // output:

    return EXIT_SUCCESS;
}

