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
#include <ostream>
#include <random>
#include <vector>

#include <bkAlgorithm/smooth.h>

// helper function: calculate mean square error
[[nodiscard]] double mse(const std::vector<double>& a, const std::vector<double>& b)
{
    double m = 0;

    for (unsigned int i = 0; i < a.size(); ++i)
    { m += (a[i] - b[i]) * (a[i] - b[i]); }

    return m;
}

std::ostream& operator<<(std::ostream& o, const std::vector<double>& v)
{
    for (auto x: v)
    { o << x << " "; }

    return o;
}

int main(int, char**)
{
    std::vector<double> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "vector: " << a << std::endl;

    // add noise

    auto a_noise{a};

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-0.1, +0.1);

    for (double& x: a_noise)
    { x += dist(mt); }

    std::cout << std::endl;
    std::cout << "noisy vector: " << a_noise << " (mse = " << mse(a, a_noise) << ")" << std::endl;
    // 0.0311635

    // binomial smoothing

    auto b(a_noise);
    bk::smooth_binomial(b.begin(), b.end(), /*num_iterations =*/50, /*kernel_size =*/3);

    std::cout << std::endl;
    std::cout << "binomial smoothing: " << b << " (mse = " << mse(a, b) << ")" << std::endl;

    // volume-preserving lambda/mu smoothing

    auto c(a_noise);
    bk::smooth_lambda_mu(c.begin(), c.end(), /*num_iterations =*/50, /*kernel_size =*/3, /*lambda =*/0.1, /*mu =*/-0.1);

    std::cout << std::endl;
    std::cout << "lambda/mu smoothing: " << b << " (mse = " << mse(a, c) << ")" << std::endl;

    return EXIT_SUCCESS;
}

