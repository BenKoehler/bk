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

#include "Options.h"

#include <algorithm>
#include <fstream>
#include <iostream>

namespace bk
{
  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void Options::set_default()
  {
      #pragma omp parallel for
      for (unsigned int i = 0; i < _set_default.size(); ++i)
      { std::invoke(_set_default[i]); }
  }

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  bool Options::save(std::string_view filename) const
  {
      std::ofstream file(filename.data(), std::ios_base::out);

      if (!file.good())
      { return false; }

      file.precision(8);
      file << std::fixed;

      /*
       * unordered saving (faster)
       */
      //for (auto it = _params_string.begin(); it != _params_string.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}
      //
      //for (auto it = _params_double.begin(); it != _params_double.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}
      //
      //for (auto it = _params_float.begin(); it != _params_float.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}
      //
      //for (auto it = _params_int.begin(); it != _params_int.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}
      //
      //for (auto it = _params_uint.begin(); it != _params_uint.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}
      //
      //for (auto it = _params_bool.begin(); it != _params_bool.end(); ++it)
      //{
      //    file << it->first << ";" << *it->second << std::endl;
      //}

      /*
       * sorted saving (slower; better readability)
       */
      std::vector<std::string> param_names;
      param_names.reserve(_params_string.size() + _params_double.size() + _params_float.size() + _params_uint.size() + _params_bool.size() + _params_int.size());

      for (auto it = _params_string.begin(); it != _params_string.end(); ++it)
      { param_names.push_back(it->first); }

      for (auto it = _params_double.begin(); it != _params_double.end(); ++it)
      { param_names.push_back(it->first); }

      for (auto it = _params_float.begin(); it != _params_float.end(); ++it)
      { param_names.push_back(it->first); }

      for (auto it = _params_int.begin(); it != _params_int.end(); ++it)
      { param_names.push_back(it->first); }

      for (auto it = _params_uint.begin(); it != _params_uint.end(); ++it)
      { param_names.push_back(it->first); }

      for (auto it = _params_bool.begin(); it != _params_bool.end(); ++it)
      { param_names.push_back(it->first); }

      std::sort(param_names.begin(), param_names.end());
      for (auto it = param_names.begin(); it != param_names.end(); ++it)
      {
          if (auto it_string = _params_string.find(*it); it_string != _params_string.end())
          { file << it_string->first << ";" << *it_string->second << std::endl; }
          else
          {
              if (auto it_double = _params_double.find(*it); it_double != _params_double.end())
              { file << it_double->first << ";" << *it_double->second << std::endl; }
              else
              {
                  if (auto it_float = _params_float.find(*it); it_float != _params_float.end())
                  { file << it_float->first << ";" << *it_float->second << std::endl; }
                  else
                  {
                      if (auto it_int = _params_int.find(*it); it_int != _params_int.end())
                      { file << it_int->first << ";" << *it_int->second << std::endl; }
                      else
                      {
                          if (auto it_uint = _params_uint.find(*it); it_uint != _params_uint.end())
                          { file << it_uint->first << ";" << *it_uint->second << std::endl; }
                          else
                          {
                              if (auto it_bool = _params_bool.find(*it); it_bool != _params_bool.end())
                              { file << it_bool->first << ";" << static_cast<int>(*it_bool->second) << std::endl; }
                          }
                      }
                  }
              }
          }
      }

      file.close();

      return true;
  }

  bool Options::load(std::string_view filename)
  {
      std::ifstream file(filename.data(), std::ios_base::in);

      if (!file.is_open())
      { return false; }

      while (file.good())
      {
          std::string sline;
          std::getline(file, sline);

          std::vector<std::string> items = bk::string_utils::split(sline, ';');

          if (items.size() < 2) // should never occur
          {
              std::cerr << "Options::load - found invalid line \"" << sline << "\" (num \";\" != 1?)" << std::endl;
              continue;
          }

          if (bk::string_utils::starts_with(items[0], "#")) // line is comment
          { continue; }

          if (auto it_string = _params_string.find(items[0]); it_string != _params_string.end())
          { *it_string->second = items[1]; }
          else
          {
              if (auto it_double = _params_double.find(items[0]); it_double != _params_double.end())
              { *it_double->second = bk::string_utils::to_float(items[1]); }
              else
              {
                  if (auto it_float = _params_float.find(items[0]); it_float != _params_float.end())
                  { *it_float->second = bk::string_utils::to_float(items[1]); }
                  else
                  {
                      if (auto it_int = _params_int.find(items[0]); it_int != _params_int.end())
                      { *it_int->second = bk::string_utils::to_int(items[1]); }
                      else
                      {
                          if (auto it_uint = _params_uint.find(items[0]); it_uint != _params_uint.end())
                          { *it_uint->second = bk::string_utils::to_uint(items[1]); }
                          else
                          {
                              if (auto it_bool = _params_bool.find(items[0]); it_bool != _params_bool.end())
                              { *it_bool->second = static_cast<bool>(bk::string_utils::to_int(items[1])); }
                          }
                      }
                  }
              }
          }
      }

      file.close();

      return true;
  }
} // namespace bk

//#undef BK_OPTION_SET_DEFAULT
//
//#undef BK_OPTION_REGISTER_STRING
//#undef BK_OPTION_REGISTER_DOUBLE
//#undef BK_OPTION_REGISTER_INT
//#undef BK_OPTION_REGISTER_UINT
//#undef BK_OPTION_REGISTER_BOOL