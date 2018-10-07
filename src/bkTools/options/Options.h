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

#pragma once

#ifndef BKTOOLS_OPTIONS_H
#define BKTOOLS_OPTIONS_H

#include <unordered_map>
#include <utility>
#include <string>
#include <string_view>

#include <bk/Signal>
#include <bk/Singleton>
#include <bk/StringUtils>
#include <bkTools/lib/bkTools_export.h>

#define BK_OPTIONS_DECLARE(T, x) \
private:\
inline static T x;\
public:\
    inline static bk::Signal<T> signal_##x##_changed;\
    [[nodiscard]] static bk::Signal<T>& get_signal_##x() {return signal_##x##_changed;}\
    [[nodiscard]] T get_##x() const {return x;}\
    void set_##x(T _##x) {if (x != _##x){x = _##x; signal_##x##_changed.emit_signal(x);}}

#define BK_OPTION_SET_DEFAULT(x, DEFAULT) _set_default.push_back([&](){x = DEFAULT;});

#define BK_OPTION_REGISTER_STRING(x, DEFAULT) _params_string.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)
#define BK_OPTION_REGISTER_DOUBLE(x, DEFAULT) _params_double.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)
#define BK_OPTION_REGISTER_FLOAT(x, DEFAULT) _params_float.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)
#define BK_OPTION_REGISTER_INT(x, DEFAULT) _params_int.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)
#define BK_OPTION_REGISTER_UINT(x, DEFAULT) _params_uint.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)
#define BK_OPTION_REGISTER_BOOL(x, DEFAULT) _params_bool.emplace(#x, std::addressof(x)); BK_OPTION_SET_DEFAULT(x,DEFAULT)

namespace bk
{
  class BKTOOLS_EXPORT Options : public Singleton<Options>
  {
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      std::unordered_map<std::string, std::string*, bk::string_utils::hash_obj>  _params_string;
      std::unordered_map<std::string, bool*, bk::string_utils::hash_obj>         _params_bool;
      std::unordered_map<std::string, int*, bk::string_utils::hash_obj>          _params_int;
      std::unordered_map<std::string, unsigned int*, bk::string_utils::hash_obj> _params_uint;
      std::unordered_map<std::string, double*, bk::string_utils::hash_obj>       _params_double;
      std::unordered_map<std::string, float*, bk::string_utils::hash_obj>        _params_float;

      std::vector<std::function<void()>> _set_default;

    public:
      //====================================================================================================
      //===== OPTIONS
      //====================================================================================================
      //BK_OPTIONS_DECLARE(double,myParameter)

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      void set_default();

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      [[maybe_unused]] bool save(std::string_view filename) const;
      [[maybe_unused]] bool load(std::string_view filename);
  }; // class Options
} // namespace bk

//#undef BK_OPTIONS_DECLARE

#endif //BKTOOLS_OPTIONS_H

