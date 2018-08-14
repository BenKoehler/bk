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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <set>

[[nodiscard]] bool has_ending(std::string_view s, std::string_view ending)
{
    if (s.length() < ending.length())
    { return false; }

    auto its = s.rbegin();

    for (auto iten = ending.rbegin(); iten != ending.rend(); ++iten, ++its)
    {
        if (*iten != *its)
        { return false; }
    }

    return true;
}

[[nodiscard]] bool is_source_file(std::string_view s)
{ return has_ending(s, ".c") || has_ending(s, ".cpp") || has_ending(s, ".cxx") || has_ending(s, ".h") || has_ending(s, ".hpp") || has_ending(s, ".hxx"); }

[[nodiscard]] std::string read_text_file(std::string_view filename)
{
    std::ifstream txtfile(filename.data(), std::ios_base::in);

    std::stringstream ss;

    if (txtfile.is_open())
    {
        ss << txtfile.rdbuf() << std::endl;
        txtfile.close();
    }

    return ss.str();
}

int main(int argc, const char** argv)
{
    if (argc <= 1)
    {
        std::cerr << "specify path argument with \"localization_crawler <path0> OPTIONAL: <path1> ... <pathN>\"" << std::endl;
        return EXIT_FAILURE;
    }
    std::set<std::string> string_to_translate;

    for (unsigned int i = 1; i < argc; ++i)
    {
        for (auto p: std::filesystem::recursive_directory_iterator(argv[i]))
        {
            if (is_source_file(p.path().filename().string()))
            {
                //std::cout << p.path().string() << std::endl;
                std::string content = read_text_file(p.path().string());

                for (unsigned int i = 0; i < content.length() - 5; ++i)
                {
                    if (content[i] == '_' && content[i + 1] == '_' && content[i + 2] == '_' && content[i + 3] == '(' && content[i + 4] == '"')
                    {
                        i += 5; // skip opening

                        for (unsigned int k = i; k < content.length(); ++k)
                        {
                            if (content[k] == '"' && content[k - 1] != '\\')
                            {
                                std::cout << "\"" << content.substr(i, k - i) << "\" (" << p.path().filename().string() << ")" << std::endl;

                                string_to_translate.insert(content.substr(i, k - i));
                                i = k + 1;
                                break;
                            }
                        } // for k

                    } // if opening
                } // for i
            } // if source file
        } // for files
    } // for input paths

    //------------------------------------------------------------------------------------------------------
    // save
    //------------------------------------------------------------------------------------------------------
    std::string path = argv[0];

    unsigned int indexLastSlash = path.length() - 1;
    while (path[indexLastSlash] != '/' && path[indexLastSlash] != '\\')
    { --indexLastSlash; }

    path = path.substr(0, indexLastSlash+1);
    path += "strings_to_translate.txt";

    std::ofstream file(path, std::ios_base::out);

    unsigned int i  = 0;
    for (auto it = string_to_translate.begin(); it != string_to_translate.end(); ++i, ++it)
    { file << "\"" << *it << "\"" << std::endl; }

    file.close();

    std::cout << std::endl;
    std::cout << "-----------------------------------------------------------------" << std::endl;
    std::cout << string_to_translate.size() << " strings saved to \""<<path<<"\"" << std::endl;
    std::cout << "-----------------------------------------------------------------" << std::endl;

    return EXIT_SUCCESS;
}
