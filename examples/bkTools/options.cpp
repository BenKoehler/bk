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
#include <memory>

#include <bk/Options>

#define myopt (*reinterpret_cast<MyOptions*>(&MyOptions::get_instance()))

struct MyOptions : public bk::Options
{
  BK_OPTIONS_DECLARE(int, N)
  BK_OPTIONS_DECLARE(std::string, path)

    struct Algorithms
    {
        friend MyOptions;
      BK_OPTIONS_DECLARE(unsigned int, numIterations);
      BK_OPTIONS_DECLARE(double, factor);
    } algorithms;

    void init();
};

void MyOptions::init()
{
    BK_OPTION_REGISTER_INT(N, 5)
    BK_OPTION_REGISTER_STRING(path, "/my/example/dir/")
    BK_OPTION_REGISTER_UINT(algorithms.numIterations, 1000)
    BK_OPTION_REGISTER_DOUBLE(algorithms.factor, 0.25)

    myopt.set_default();
}

int main(int, char**)
{
    myopt.init();

    myopt.signal_N_changed.connect([](int newN)
                                   { std::cout << "N changed to " << newN << std::endl; });

    std::cout << "N " << myopt.get_N() << std::endl;
    myopt.set_N(1337);
    std::cout << "N " << myopt.get_N() << std::endl;
    std::cout << "path " << myopt.get_path() << std::endl;
    std::cout << "algorithms.numIterations " << myopt.algorithms.get_numIterations() << std::endl;
    std::cout << "algorithms.factor " << myopt.algorithms.get_factor() << std::endl;
    myopt.set_path("");
    std::cout << "path " << myopt.get_path() << std::endl;

    // --------

    myopt.set_default();

    std::cout << std::endl;
    std::cout << "reset to default:" << std::endl;
    std::cout << "N " << myopt.get_N() << std::endl;
    std::cout << "path " << myopt.get_path() << std::endl;
    std::cout << "algorithms.numIterations " << myopt.algorithms.get_numIterations() << std::endl;
    std::cout << "algorithms.factor " << myopt.algorithms.get_factor() << std::endl;

    return EXIT_SUCCESS;
}

