/*
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

#include <iostream>

#include <bk/Clock>

int main(int, char**)
{
    bk::Clock clock;

    clock.start(); // set start time point
    /* ... calc stuff ... */
    clock.stop(); // set end time point

    /*
     * print the time split into h, min, s, ms:
     * e.g.,
     *      130 s -> 0 h 2 min 10 s 0 ms
     */
    std::cout << clock << std::endl;

    // alternative version:
    auto[h, min, s, ms] = clock.time_in_h_min_s_ms();
    std::cout << h << " h " << min << " min " << s << " s " << ms << " ms" << std::endl;



    /* ... calc more ... */
    clock.stop(); // reset end time point

    /*
     * output the full time as h, min, s, or ms:
     * e.g.,
     *      130 s -> 0 h
     *      130 s -> 2 min
     *      130 s -> 130 s
     *      130 s -> 130000 ms
     */
    std::cout << clock.time_in_h() << " h" << std::endl;
    std::cout << clock.time_in_min() << " min" << std::endl;
    std::cout << clock.time_in_sec() << " s" << std::endl;
    std::cout << clock.time_in_milli_sec() << " ms" << std::endl;
    std::cout << clock.time_in_nano_sec() << " ns" << std::endl;



    clock.start(); // re-use clock to measure another calculation
    /* ... calc other stuff ... */
    clock.stop();

    return EXIT_SUCCESS;
}

