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

#ifndef BKTYPETRAITS_VARIADIC_TEMPLATE_HELPERS_H
#define BKTYPETRAITS_VARIADIC_TEMPLATE_HELPERS_H

#include <utility>
#include <type_traits>

#include "type_wrapper.h"

namespace bk
{

  //====================================================================================================
    //===== TEMPLATE PARAMETER PACK
  //====================================================================================================

  /// @{ -------------------------------------------------- SIZE OF PARAMETER PACK
    //! number of template arguments in parameter pack
    template<typename... Args> struct template_parameter_pack_size : std::integral_constant<int, sizeof...(Args)>
    {
    };
    // shortcut
    template<typename... Args> constexpr int template_parameter_pack_size_v = template_parameter_pack_size<Args...>::value;
  /// @}

  /// @{ -------------------------------------------------- SKIP NUMBER OF ARGUMENTS IN PARAMETER PACK
    //! determine type or value of argument ID
    template<int ID, typename... TL> struct template_parameter_pack_arg
        : type_wrapper<typename template_parameter_pack_arg<ID - 1>::type>
    {
        template<typename... Args> static constexpr decltype(auto) value(Args&& ... args)
        { return template_parameter_pack_arg<ID - 1, TL...>::value(std::forward<Args>(args)...); }
    };
    //template<int ID> struct template_parameter_pack_arg<ID>
    //    : type_wrapper<typename template_parameter_pack_arg<ID - 1>::type>
    //{
    //    static constexpr decltype(auto) value()
    //    {
    //        return template_parameter_pack_arg<ID - 1>::value();
    //    }
    //};
    template<int ID, typename T, typename... TL> struct template_parameter_pack_arg<ID, T, TL...>
        : type_wrapper<typename template_parameter_pack_arg<ID - 1, TL...>::type>
    {
        template<typename A, typename... Args> static constexpr decltype(auto) value(A&& /*arg0*/, Args&& ... args)
        { return template_parameter_pack_arg<ID - 1, TL...>::value(std::forward<Args>(args)...); }
    };

    template<typename T, typename... TL> struct template_parameter_pack_arg<0, T, TL...>
        : type_wrapper<T>
    {
        template<typename A, typename... Args> static constexpr decltype(auto) value(A&& arg0, Args&& ... /*argn*/)
        { return std::forward<A>(arg0); }
    };
    template<typename T> struct template_parameter_pack_arg<0, T>
        : type_wrapper<T>
    {
        template<typename A, typename... HL> static constexpr decltype(auto) value(A&& arg0)
        { return std::forward<A>(arg0); }
    };

  template<> struct template_parameter_pack_arg<0> : type_wrapper<std::nullptr_t>
    {
    };

    // shortcut
    template<int ID, typename... TL> using template_parameter_pack_arg_t = typename template_parameter_pack_arg<ID, TL...>::type;
  /// @}


  //====================================================================================================
    //===== TEMPLATE INDEX PACK
  //====================================================================================================

  /// @{ -------------------------------------------------- SKIP NUMBER OF ARGUMENTS IN INDEX PACK
    //! determine value of argument ID
    template<int ID, long long int... IL> struct template_index_pack_arg : std::integral_constant<long long int, 0>
    {
    };
  template<int ID> struct template_index_pack_arg<ID> : std::integral_constant<long long int, 0>
    {
    };
  template<int ID, long long int I, long long int... IL> struct template_index_pack_arg<ID, I, IL...> : std::integral_constant<long long int, template_index_pack_arg<ID - 1, IL...>::value>
    {
    };
  template<long long int I, long long int... IL> struct template_index_pack_arg<0, I, IL...> : std::integral_constant<long long int, I>
    {
    };
  template<long long int I> struct template_index_pack_arg<0, I> : std::integral_constant<long long int, I>
    {
    };

    // shortcut
    template<int ID, long long int... IL> constexpr long long int template_index_pack_arg_v = template_index_pack_arg<ID, IL...>::value;
  /// @}

  /// @{ -------------------------------------------------- MULTIPLY TEMPLATE INDEX PACK
    //! determine the product of the template integrals of the index pack
    template<long long int... IL> struct template_index_pack_mult : std::integral_constant<long long int, 0>
    {
    };
  template<long long int I, long long int... IL> struct template_index_pack_mult<I, IL...> : std::integral_constant<long long int, I * template_index_pack_mult<IL...>::value>
    {
    };
  template<long long int I> struct template_index_pack_mult<I> : std::integral_constant<long long int, I>
    {
    };

    // shortcut
    template<long long int... IL> constexpr long long int template_index_pack_mult_v = template_index_pack_mult<IL...>::value;
  /// @}

  /// @{ -------------------------------------------------- TEMPLATE INDEX PACK ASSERTS
    //! static asserts that all indices are lesser/greater/equal given value; or even/odd

    // ALL INDICES == X

    // base (unused)
    template<long long int Val, long long int... IL> struct template_index_pack_all_equal;

    // empty index pack
    template<long long int Val> struct template_index_pack_all_equal<Val> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int Val, long long int I, long long int... IL> struct template_index_pack_all_equal<Val,I,IL...>
        : std::integral_constant<bool, I == Val && template_index_pack_all_equal<Val, IL...>::value>
    {
    };

    // last element of index pack
    template<long long int Val, long long int I> struct template_index_pack_all_equal<Val, I> : std::integral_constant<bool, I == Val>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_equal_v = template_index_pack_all_equal<Val, IL...>::value;

    // ALL INDICES > X

    // base (unused)
    template<long long int Val, long long int... IL> struct template_index_pack_all_greater;

    // empty index pack
    template<long long int Val> struct template_index_pack_all_greater<Val> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int Val, long long int I, long long int... IL> struct template_index_pack_all_greater<Val,I,IL...>
        : std::integral_constant<bool, (I > Val) && template_index_pack_all_greater<Val, IL...>::value>
    {
    };

    // last element of index pack
    template<long long int Val, long long int I> struct template_index_pack_all_greater<Val, I> : std::integral_constant<bool, (I > Val)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_greater_v = template_index_pack_all_greater<Val, IL...>::value;

    // ALL INDICES >= X

    // base (unused)
    template<long long int Val, long long int... IL> struct template_index_pack_all_greater_or_equal;

    // empty index pack
    template<long long int Val> struct template_index_pack_all_greater_or_equal<Val> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int Val, long long int I, long long int... IL> struct template_index_pack_all_greater_or_equal<Val,I,IL...>
        : std::integral_constant<bool, (I >= Val) && template_index_pack_all_greater_or_equal<Val, IL...>::value>
    {
    };

    // last element of index pack
    template<long long int Val, long long int I> struct template_index_pack_all_greater_or_equal<Val, I> : std::integral_constant<bool, (I >= Val)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_greater_or_equal_v = template_index_pack_all_greater_or_equal<Val, IL...>::value;

    // ALL INDICES < X

    // base (unused)
    template<long long int Val, long long int... IL> struct template_index_pack_all_lesser;

    // empty index pack
    template<long long int Val> struct template_index_pack_all_lesser<Val> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int Val, long long int I, long long int... IL> struct template_index_pack_all_lesser<Val,I,IL...>
        : std::integral_constant<bool, (I < Val) && template_index_pack_all_lesser<Val, IL...>::value>
    {
    };

    // last element of index pack
    template<long long int Val, long long int I> struct template_index_pack_all_lesser<Val, I> : std::integral_constant<bool, (I < Val)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_lesser_v = template_index_pack_all_lesser<Val, IL...>::value;

    // ALL INDICES <= X

    // base (unused)
    template<long long int Val, long long int... IL> struct template_index_pack_all_lesser_or_equal;

    // empty index pack
    template<long long int Val> struct template_index_pack_all_lesser_or_equal<Val> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int Val, long long int I, long long int... IL> struct template_index_pack_all_lesser_or_equal<Val,I,IL...>
        : std::integral_constant<bool, (I <= Val) && template_index_pack_all_lesser_or_equal<Val, IL...>::value>
    {
    };

    // last element of index pack
    template<long long int Val, long long int I> struct template_index_pack_all_lesser_or_equal<Val, I> : std::integral_constant<bool, (I <= Val)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_lesser_or_equal_v = template_index_pack_all_lesser_or_equal<Val, IL...>::value;

    // ALL INDICES ARE EVEN

    // base (unused)
    template<long long int... IL> struct template_index_pack_all_even;

    // empty index pack
    template<> struct template_index_pack_all_even<> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int I, long long int... IL> struct template_index_pack_all_even<I, IL...> : std::integral_constant<bool, (I % 2 == 0) && template_index_pack_all_even<IL...>::value>
    {
    };

    // last element of index pack
    template<long long int I> struct template_index_pack_all_even<I> : std::integral_constant<bool, (I % 2 == 0)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_even_v = template_index_pack_all_even<Val, IL...>::value;

    // ALL INDICES ARE ODD

    // base (unused)
    template<long long int... IL> struct template_index_pack_all_odd;

    // empty index pack
    template<> struct template_index_pack_all_odd<> : std::integral_constant<bool, true>
    {
    };

    // element I of index pack
    template<long long int I, long long int... IL> struct template_index_pack_all_odd<I, IL...> : std::integral_constant<bool, (I % 2 == 1) && template_index_pack_all_odd<IL...>::value>
    {
    };

    // last element of index pack
    template<long long int I> struct template_index_pack_all_odd<I> : std::integral_constant<bool, (I % 2 == 1)>
    {
    };

    // shortcut
    template<long long int Val, long long int... IL> constexpr bool template_index_pack_all_odd_v = template_index_pack_all_odd<Val, IL...>::value;
  /// @}



  //====================================================================================================
    //===== INDEX PACK
  //====================================================================================================

  /// @{ -------------------------------------------------- MULTIPLY INDEX PACK
    template<typename T> constexpr auto index_pack_mult(T x0) { return x0; }
    template<typename T, typename... TL> constexpr auto index_pack_mult(T x0, TL... xn) { return index_pack_mult(x0) * index_pack_mult(xn...); }
  /// @}

  /// @{ -------------------------------------------------- INDEX PACK ASSERTS
    //! static asserts that all indices are lesser/greater/equal given value; or even/odd

    // ALL INDICES == X
    template<typename V, typename T> bool index_pack_all_equal(V val, T x0) { return x0 == val; }
    template<typename V, typename T, typename... TL> bool index_pack_all_equal(V val, T x0, TL... xn) { return index_pack_all_equal(val, x0) && index_pack_all_equal(val, xn...); }

    // ALL INDICES > X
    template<typename V, typename T> bool index_pack_all_greater(V val, T x0) { return x0 > val; }
    template<typename V, typename T, typename... TL> bool index_pack_all_greater(V val, T x0, TL... xn) { return index_pack_all_greater(val, x0) && index_pack_all_greater(val, xn...); }

    // ALL INDICES >= X
    template<typename V, typename T> bool index_pack_all_greater_or_equal(V val, T x0) { return x0 >= val; }
    template<typename V, typename T, typename... TL> bool index_pack_all_greater_or_equal(V val, T x0, TL... xn) { return index_pack_all_greater_or_equal(val, x0) && index_pack_all_greater_or_equal(val, xn...); }

    // ALL INDICES < X
    template<typename V, typename T> bool index_pack_all_lesser(V val, T x0) { return x0 < val; }
    template<typename V, typename T, typename... TL> bool index_pack_all_lesser(V val, T x0, TL... xn) { return index_pack_all_lesser(val, x0) && index_pack_all_lesser(val, xn...); }

    // ALL INDICES <= X
    template<typename V, typename T> bool index_pack_all_lesser_or_equal(V val, T x0) { return x0 <= val; }
    template<typename V, typename T, typename... TL> bool index_pack_all_lesser_or_equal(V val, T x0, TL... xn) { return index_pack_all_lesser_or_equal(val, x0) && index_pack_all_lesser_or_equal(val, xn...); }

    // ALL INDICES ARE EVEN
    template<typename T> bool index_pack_all_even(T x0) { return x0 % 2 == 0; }
    template<typename T, typename... TL> bool index_pack_all_even(T x0, TL... xn) { return index_pack_all_even(x0) && index_pack_all_even(xn...); }

    // ALL INDICES ARE ODD
    template<typename T> bool index_pack_all_odd(T x0) { return x0 % 2 == 1; }
    template<typename T, typename... TL> bool index_pack_all_odd(T x0, TL... xn) { return index_pack_all_odd(x0) && index_pack_all_odd(xn...); }
  /// @}

} // namespace bk

#endif //BKTYPETRAITS_VARIADIC_TEMPLATE_HELPERS_H


