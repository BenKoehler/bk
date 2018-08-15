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

 /// IMPORTANT: DEFINE " BK_LOCALIZATION " PRE-PROCESSOR MACRO / COMPILE DEFINITION

#include <iostream>
#include <string_view>

#include <bkTools/Signal>
#include <bkTools/Localization>

#include "my_custom_localization.h"

bk::Signal <std::string_view> s_string;

void logic_class_function()
{
    constexpr int     N    = 10'000;
    const std::string name = ___("Calculating @0 pathlines", N);
    s_string.emit_signal(name);
}

void myPrint(std::string_view s)
{ std::cout << s << std::endl; }

//------------------------------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------------------------------
void print_stuff_to_translate()
{
    std::cout << bk_lm.translate_tag(Text_button_cancel) << std::endl;
    std::cout << bk_lm.translate_tag(Text_calculation_time, bk_lm.translate_tag(Text_calculation_name_pathlines), 5) << std::endl;
    logic_class_function();
    std::cout << ___("repeat me @0 times @1 @1 @1 (was @0x repeated)", 3, "bla") << std::endl;
}

int main(int, char**)
{
    s_string.connect(&myPrint);

    std::cout << "german translation:" << std::endl;
    std::cout << "-------------------" << std::endl;

    set_localization_german();
    print_stuff_to_translate();

    std::cout << std::endl;
    std::cout << "english translation:" << std::endl;
    std::cout << "--------------------" << std::endl;

    set_localization_english();
    print_stuff_to_translate();

    return EXIT_SUCCESS;
}