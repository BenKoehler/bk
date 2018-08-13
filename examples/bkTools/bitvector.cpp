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

#include <bkTools/BitVector>

int main(int, char**)
{
    using namespace bk;
    
    BitVector<4> a(1, 0, 0, 1);

    std::cout << "a: " << a << std::endl; // 1001
    std::cout << "a.to_number(): " << a.to_number() << std::endl; // 9
    std::cout << "a.size(): " << a.size() << std::endl; // 4

    std::cout << "bits of a: ";
    for (auto bit: a)
    { std::cout << bit; } // 1001
    std::cout << std::endl;

    std::cout << "(~a): " << (~a) << std::endl; // 0110; same as a.flip()
    std::cout << "(~a | a): " << (~a | a) << std::endl; // 1111
    std::cout << "((~a | a) & BitVector<4>(0, 0, 0, 1)): " << ((~a | a) & BitVector<4>(0, 0, 0, 1)) << std::endl; // 0001
    std::cout << "(a << 1): " << (a << 1) << std::endl; // 0010
    std::cout << "(a >> 3): " << (a >> 3) << std::endl; // 0001

    return EXIT_SUCCESS;
}

