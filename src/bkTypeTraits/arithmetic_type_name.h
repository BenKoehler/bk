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

#pragma once

#ifndef BKTYPETRAITS_ARITHMETIC_TYPE_NAME_H
#define BKTYPETRAITS_ARITHMETIC_TYPE_NAME_H

#include <type_traits>

namespace bk
{
  namespace details
  {
    template<typename T> struct _name_of
    {
        static constexpr const char* const name = "misc";
    };
    /// @{ BOOL
    template<> struct _name_of<bool>
    {
        static constexpr const char* const name = "bool";
    };
    template<> struct _name_of<std::add_const_t<bool>>
    {
        static constexpr const char* const name = "const bool";
    };
    template<> struct _name_of<std::add_pointer_t<bool>>
    {
        static constexpr const char* const name = "bool*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<bool>>>
    {
        static constexpr const char* const name = "const bool*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<bool>>
    {
        static constexpr const char* const name = "bool&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<bool>>>
    {
        static constexpr const char* const name = "const bool&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<bool>>
    {
        static constexpr const char* const name = "bool&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<bool>>>
    {
        static constexpr const char* const name = "const bool&&";
    };
    ///@}

    /// @{ CHAR
    template<> struct _name_of<char>
    {
        static constexpr const char* const name = "char";
    };
    template<> struct _name_of<std::add_const_t<char>>
    {
        static constexpr const char* const name = "const char";
    };
    template<> struct _name_of<std::add_pointer_t<char>>
    {
        static constexpr const char* const name = "char*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<char>>>
    {
        static constexpr const char* const name = "const char*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<char>>
    {
        static constexpr const char* const name = "char&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<char>>>
    {
        static constexpr const char* const name = "const char&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<char>>
    {
        static constexpr const char* const name = "char&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<char>>>
    {
        static constexpr const char* const name = "const char&&";
    };
    ///@}

    /// @{ UNSIGNED CHAR
    template<> struct _name_of<unsigned char>
    {
        static constexpr const char* const name = "unsigned char";
    };
    template<> struct _name_of<std::add_const_t<unsigned char>>
    {
        static constexpr const char* const name = "const unsigned char";
    };
    template<> struct _name_of<std::add_pointer_t<unsigned char>>
    {
        static constexpr const char* const name = "unsigned char*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<unsigned char>>>
    {
        static constexpr const char* const name = "const unsigned char*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<unsigned char>>
    {
        static constexpr const char* const name = "unsigned char&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<unsigned char>>>
    {
        static constexpr const char* const name = "const unsigned char&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<unsigned char>>
    {
        static constexpr const char* const name = "unsigned char&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<unsigned char>>>
    {
        static constexpr const char* const name = "const unsigned char&&";
    };
    ///@}

    /// @{ SHORT
    template<> struct _name_of<short>
    {
        static constexpr const char* const name = "short";
    };
    template<> struct _name_of<std::add_const_t<short>>
    {
        static constexpr const char* const name = "const short";
    };
    template<> struct _name_of<std::add_pointer_t<short>>
    {
        static constexpr const char* const name = "short*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<short>>>
    {
        static constexpr const char* const name = "const short*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<short>>
    {
        static constexpr const char* const name = "short&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<short>>>
    {
        static constexpr const char* const name = "const short&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<short>>
    {
        static constexpr const char* const name = "short&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<short>>>
    {
        static constexpr const char* const name = "const short&&";
    };
    ///@}

    /// @{ UNSIGNED SHORT
    template<> struct _name_of<unsigned short>
    {
        static constexpr const char* const name = "unsigned short";
    };
    template<> struct _name_of<std::add_const_t<unsigned short>>
    {
        static constexpr const char* const name = "const unsigned short";
    };
    template<> struct _name_of<std::add_pointer_t<unsigned short>>
    {
        static constexpr const char* const name = "unsigned short*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<unsigned short>>>
    {
        static constexpr const char* const name = "const unsigned short*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<unsigned short>>
    {
        static constexpr const char* const name = "unsigned short&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<unsigned short>>>
    {
        static constexpr const char* const name = "const unsigned short&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<unsigned short>>
    {
        static constexpr const char* const name = "unsigned short&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<unsigned short>>>
    {
        static constexpr const char* const name = "const unsigned short&&";
    };
    ///@}

    /// @{ INT
    template<> struct _name_of<int>
    {
        static constexpr const char* const name = "int";
    };
    template<> struct _name_of<std::add_const_t<int>>
    {
        static constexpr const char* const name = "const int";
    };
    template<> struct _name_of<std::add_pointer_t<int>>
    {
        static constexpr const char* const name = "int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<int>>>
    {
        static constexpr const char* const name = "const int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<int>>
    {
        static constexpr const char* const name = "int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<int>>>
    {
        static constexpr const char* const name = "const int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<int>>
    {
        static constexpr const char* const name = "int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<int>>>
    {
        static constexpr const char* const name = "const int&&";
    };
    ///@}

    /// @{ UNSIGNED INT
    template<> struct _name_of<unsigned int>
    {
        static constexpr const char* const name = "unsigned int";
    };
    template<> struct _name_of<std::add_const_t<unsigned int>>
    {
        static constexpr const char* const name = "const unsigned int";
    };
    template<> struct _name_of<std::add_pointer_t<unsigned int>>
    {
        static constexpr const char* const name = "unsigned int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<unsigned int>>>
    {
        static constexpr const char* const name = "const unsigned int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<unsigned int>>
    {
        static constexpr const char* const name = "unsigned int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<unsigned int>>>
    {
        static constexpr const char* const name = "const unsigned int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<unsigned int>>
    {
        static constexpr const char* const name = "unsigned int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<unsigned int>>>
    {
        static constexpr const char* const name = "const unsigned int&&";
    };
    ///@}

    /// @{ LONG INT
    template<> struct _name_of<long int>
    {
        static constexpr const char* const name = "long int";
    };
    template<> struct _name_of<std::add_const_t<long int>>
    {
        static constexpr const char* const name = "const long int";
    };
    template<> struct _name_of<std::add_pointer_t<long int>>
    {
        static constexpr const char* const name = "long int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<long int>>>
    {
        static constexpr const char* const name = "const long int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<long int>>
    {
        static constexpr const char* const name = "long int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<long int>>>
    {
        static constexpr const char* const name = "const long int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<long int>>
    {
        static constexpr const char* const name = "long int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<long int>>>
    {
        static constexpr const char* const name = "const long int&&";
    };
    ///@}

    /// @{ UNSIGNED LONG INT
    template<> struct _name_of<unsigned long int>
    {
        static constexpr const char* const name = "unsigned long int";
    };
    template<> struct _name_of<std::add_const_t<unsigned long int>>
    {
        static constexpr const char* const name = "const unsigned long int";
    };
    template<> struct _name_of<std::add_pointer_t<unsigned long int>>
    {
        static constexpr const char* const name = "unsigned long int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<unsigned long int>>>
    {
        static constexpr const char* const name = "const unsigned long int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<unsigned long int>>
    {
        static constexpr const char* const name = "unsigned long int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<unsigned long int>>>
    {
        static constexpr const char* const name = "const unsigned long int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<unsigned long int>>
    {
        static constexpr const char* const name = "unsigned long int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<unsigned long int>>>
    {
        static constexpr const char* const name = "const unsigned long int&&";
    };
    ///@}

    /// @{ LONG LONG INT
    template<> struct _name_of<long long int>
    {
        static constexpr const char* const name = "long long int";
    };
    template<> struct _name_of<std::add_const_t<long long int>>
    {
        static constexpr const char* const name = "const long long int";
    };
    template<> struct _name_of<std::add_pointer_t<long long int>>
    {
        static constexpr const char* const name = "long long int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<long long int>>>
    {
        static constexpr const char* const name = "const long long int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<long long int>>
    {
        static constexpr const char* const name = "long long int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<long long int>>>
    {
        static constexpr const char* const name = "const long long int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<long long int>>
    {
        static constexpr const char* const name = "long long int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<long long int>>>
    {
        static constexpr const char* const name = "const long long int&&";
    };
    ///@}

    /// @{ UNSIGNED LONG LONG INT
    template<> struct _name_of<unsigned long long int>
    {
        static constexpr const char* const name = "unsigned long long int";
    };
    template<> struct _name_of<std::add_const_t<unsigned long long int>>
    {
        static constexpr const char* const name = "const unsigned long long int";
    };
    template<> struct _name_of<std::add_pointer_t<unsigned long long int>>
    {
        static constexpr const char* const name = "unsigned long long int*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<unsigned long long int>>>
    {
        static constexpr const char* const name = "const unsigned long long int*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<unsigned long long int>>
    {
        static constexpr const char* const name = "unsigned long long int&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<unsigned long long int>>>
    {
        static constexpr const char* const name = "const unsigned long long int&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<unsigned long long int>>
    {
        static constexpr const char* const name = "unsigned long long int&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<unsigned long long int>>>
    {
        static constexpr const char* const name = "const unsigned long long int&&";
    };
    ///@}

    /// @{ FLOAT
    template<> struct _name_of<float>
    {
        static constexpr const char* const name = "float";
    };
    template<> struct _name_of<std::add_const_t<float>>
    {
        static constexpr const char* const name = "const float";
    };
    template<> struct _name_of<std::add_pointer_t<float>>
    {
        static constexpr const char* const name = "float*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<float>>>
    {
        static constexpr const char* const name = "const float*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<float>>
    {
        static constexpr const char* const name = "float&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<float>>>
    {
        static constexpr const char* const name = "const float&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<float>>
    {
        static constexpr const char* const name = "float&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<float>>>
    {
        static constexpr const char* const name = "const float&&";
    };
    ///@}

    /// @{ DOUBLE
    template<> struct _name_of<double>
    {
        static constexpr const char* const name = "double";
    };
    template<> struct _name_of<std::add_const_t<double>>
    {
        static constexpr const char* const name = "const double";
    };
    template<> struct _name_of<std::add_pointer_t<double>>
    {
        static constexpr const char* const name = "double*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<double>>>
    {
        static constexpr const char* const name = "const double*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<double>>
    {
        static constexpr const char* const name = "double&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<double>>>
    {
        static constexpr const char* const name = "const double&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<double>>
    {
        static constexpr const char* const name = "double&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<double>>>
    {
        static constexpr const char* const name = "const double&&";
    };
    ///@}

    /// @{ LONG DOUBLE
    template<> struct _name_of<long double>
    {
        static constexpr const char* const name = "long double";
    };
    template<> struct _name_of<std::add_const_t<long double>>
    {
        static constexpr const char* const name = "const long double";
    };
    template<> struct _name_of<std::add_pointer_t<long double>>
    {
        static constexpr const char* const name = "long double*";
    };
    template<> struct _name_of<std::add_pointer_t<std::add_const_t<long double>>>
    {
        static constexpr const char* const name = "const long double*";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<long double>>
    {
        static constexpr const char* const name = "long double&";
    };
    template<> struct _name_of<std::add_lvalue_reference_t<std::add_const_t<long double>>>
    {
        static constexpr const char* const name = "const long double&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<long double>>
    {
        static constexpr const char* const name = "long double&&";
    };
    template<> struct _name_of<std::add_rvalue_reference_t<std::add_const_t<long double>>>
    {
        static constexpr const char* const name = "const long double&&";
    };
    ///@}

  } // namespace details

  template<typename T>
  constexpr const char* arithmetic_type_name()
  { return details::_name_of<T>::name; }
} // namespace bk

#endif //BKTYPETRAITS_ARITHMETIC_TYPE_NAME_H


