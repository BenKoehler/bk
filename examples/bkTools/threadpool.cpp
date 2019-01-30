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

#include <bk/ThreadPool>

int main(int, char**)
{
    bk::ThreadPool tp(8); // spawn 8 threads

    std::cout << "num_threads: " << tp.num_threads() << std::endl;
    // output: 8

    auto future = tp.enqueue([](){
        /* ... do stuff ... */
        return 42.0;
    });

    auto result = future.get(); // waits for task to finish

    std::cout << "result " << result << std::endl;


    tp.enqueue([](){ /* ... task A ... */ });
    tp.enqueue([](){ /* ... task B ... */ });
    tp.enqueue([](){ /* ... task C ... */ });

    std::cout << "num_tasks_remaining: " << tp.num_tasks_remaining() << std::endl;
    // output: 3

    tp.wait_for_all_tasks_to_finish();

    return EXIT_SUCCESS;
}

