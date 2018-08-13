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
#include <vector>

#include <bkAlgorithm/scale.h>
#include <bkAlgorithm/normalize.h>
#include <bkAlgorithm/invert_minmax.h>

int main(int, char**)
{
    std::vector<double> a{1.0, 2.0, 3.0};

    std::cout << "vector: ";
    for (auto x: a)
    { std::cout << x << " "; }
    std::cout << std::endl << std::endl;


    // scale
    bk::scale_to_range(a.begin(), a.end(), /*newMinValue =*/1, /*newMaxValue =*/9);

    std::cout << "scale to range [1,9]: ";
    for (auto x: a)
    { std::cout << x << " "; } // 1 5 9
    std::cout << std::endl;

    a = {1, 2, 3};
    bk::scale_by_factor(a.begin(), a.end(), /*factor =*/0.5);

    std::cout << "scale by factor 0.5: ";
    for (auto x: a)
    { std::cout << x << " "; } // 0.5 1 1.5
    std::cout << std::endl;


    // normalize 
    a = {1, 2, 3};
    std::cout << "norm : " << bk::norm(a.begin(), a.end()) << std::endl; // output: 3.74166

    bk::normalize(a.begin(), a.end());

    std::cout << "normalize: ";
    for (auto x: a)
    { std::cout << x << " "; } // 0.267261 0.534522 0.801784
    std::cout << "(norm: " << bk::norm(a.begin(), a.end()) << ")" << std::endl;

    a = {1, 2, 3};
    bk::normalize_to_length(a.begin(), a.end(), 5);

    std::cout << "normalize to length 5: ";
    for (auto x: a)
    { std::cout << x << " "; } // 1.33631 2.67261 4.00892
    std::cout << "(norm: " << bk::norm(a.begin(), a.end()) << ")" << std::endl; // output: 1


    // invert minmax
    a = {1, 2, 3};
    bk::invert_minmax(a.begin(), a.end());

    std::cout << "invert min max: ";
    for (auto x: a)
    { std::cout << x << " "; } // 3 2 1
    std::cout << std::endl;

    return EXIT_SUCCESS;
}

