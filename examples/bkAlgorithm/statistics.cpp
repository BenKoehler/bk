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

#include <bkAlgorithm/sum.h>
#include <bkAlgorithm/mean.h>
#include <bkAlgorithm/variance.h>
#include <bkAlgorithm/standard_deviation.h>
#include <bkAlgorithm/quantile.h>
#include <bkAlgorithm/minmax_difference.h>

int main(int, char**)
{
    std::vector<double> a{1.0, 2.0, 3.0, 4.0, 5.0};

    std::cout << "vector: ";
    for (auto x: a)
    { std::cout << x << " "; }
    std::cout << std::endl << std::endl;


    // sum 
    std::cout << "sum: " << bk::sum(a.begin(), a.end()) << std::endl; // output: 15
    std::cout << "sum squared: " << bk::sum_of_squared_elements(a.begin(), a.end()) << std::endl; // output: 55


    // mean 
    std::cout << "mean: " << bk::mean(a.begin(), a.end()) << std::endl; // output: 3


    // variance 
    std::cout << "variance: " << bk::variance(a.begin(), a.end()) << std::endl; // output: 2


    // standard deviation 
    std::cout << "stdev: " << bk::standard_deviation(a.begin(), a.end()) << std::endl; // output: 1.41421


    // quantile 
    std::cout << "median: " << bk::median(a.begin(), a.end()) << std::endl; // output: 3

    auto q10 = bk::quantile(a.begin(), a.end(), 0.1); // output: 1
    std::cout << "10% quantile " << q10 << std::endl; // output: 2

    auto[q25, q75] = bk::interquartile_range(a.begin(), a.end());
    std::cout << "25% quantile " << q25 << std::endl; // output: 2
    std::cout << "75% quantile " << q75 << std::endl; // output: 4


    // min/max difference 
    auto[mindiff, maxdiff] = bk::minmax_difference(a.begin(), a.end());
    std::cout << "mindiff: " << mindiff << std::endl; // output: 1
    std::cout << "maxdiff: " << maxdiff << std::endl; // output: 4

    return EXIT_SUCCESS;
}

