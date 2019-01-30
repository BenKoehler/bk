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

#include <bk/Singleton>

// class with whatever functionality
class MyClass
{
    using self_type = MyClass;

  public:
    MyClass() = default;
    MyClass(const self_type&) = default;
    MyClass(self_type&&) noexcept = default;

    ~MyClass() = default;

    self_type& operator=(const self_type&) = default;
    self_type& operator=(self_type&&) noexcept = default;

    void foo()
    {
        std::cout << "foo" << std::endl;
    }
};

// global singleton version of MyClass
class GlobalMyClass : public bk::Singleton<MyClass>
{
    using self_type = GlobalMyClass;

    GlobalMyClass() = delete;
    GlobalMyClass(const self_type&) = delete;
    GlobalMyClass(self_type&&) = delete;

    ~GlobalMyClass() = delete;

    self_type& operator=(const self_type&) = delete;
    self_type& operator=(self_type&&) noexcept = delete;
};

// convenience macro to access the object instance
#define myclass_singleton_macro GlobalMyClass::get_instance()


int main(int, char**)
{
    /*
     * The singleton will be created when first using the macro.
     *
     * Caution: If you want to use the singleton in multiple threads,
     * make sure that the object instance is already created, e.g.,
     * by calling the macro in you program's main():
     */
    myclass_singleton_macro.foo();

    return EXIT_SUCCESS;
}