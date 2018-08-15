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

#include <functional> // std::placeholders
#include <iostream>

#include <bk/Signal>

struct MyClass
{
    void bar(int x)
    {
        std::cout << "bar " << x << std::endl;
    }
};

int main(int, char**)
{
    bk::Signal<int> s;

    // connect signal to class member
    MyClass f;
    unsigned int signal_id0 = s.connect_member(&MyClass::bar, &f, std::placeholders::_1);

    // connect signal to lambda function
    unsigned int signal_id1 = s.connect([](const auto& x) { std::cout << "foo " << x << std::endl; });

    s.emit_signal(1); // 2 slots are connected
    // output:
    //    foo 1
    //    bar 1

    s.disconnect(signal_id1);

    s.emit_signal(2); // 1 slot is connected
    // output:
    //    bar 2

    // connect lambda function and push it to the first place in the slot list
    s.connect_front([](const auto& x) { std::cout << "foo " << x << std::endl; });

    s.emit_signal(3);
    // output:
    //    foo 3
    //    bar 3

    // mute the signal
    s.mute(true);

    s.emit_signal(4);
    // output:
    //

    // unmute the signal
    s.mute(false);

    s.emit_signal(4);
    // output:
    //    foo 4
    //    bar 4

    return EXIT_SUCCESS;
}

