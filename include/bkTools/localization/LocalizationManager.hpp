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

#ifndef BKTOOLS_LOCALIZATIONMANAGER_HPP
#define BKTOOLS_LOCALIZATIONMANAGER_HPP

#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

#include "LocalizationManager.h"

namespace bk
{
  template<typename T, typename... Args>
  std::string LocalizationManager::_replace_parameters_impl(std::string_view text, unsigned int cnt, T&& arg0, Args&& ... args)
  {
      std::stringstream currentArg;
      currentArg << cnt;

      for (unsigned int i = 0; i < text.size(); ++i)
      {
          if (is_tag_at_position(text, i, currentArg.str()))
          {
              std::stringstream ss;

              ss << text.substr(0, i); // text before tag
              ss << std::forward<T>(arg0); // insert argument

              // text after tag
              i += 1; // skip @
              i += currentArg.str().length(); // skip number
              if (i < text.size())
              { ss << text.substr(i, text.size() - i); }

              //if constexpr (sizeof...(Args) != 0)
              //{ return _replace_parameters_impl(ss.str(), ++cnt, std::forward<Args>(args)...); }
              //else
              //{ return ss.str(); }

              return _replace_parameters_impl(ss.str(), cnt, std::forward<T>(arg0), std::forward<Args>(args)...);
          }
      }

      // argument was not found
      if constexpr (sizeof...(Args) != 0)
      { return _replace_parameters_impl(text, ++cnt, std::forward<Args>(args)...); }
      else
      { return text.data(); }

      //std::cerr << "error finding all replacement tags in \"" << text << "\"" << " - wrong tag indices?" << std::endl;
      //return text.data();
  }

  template<typename T, typename... Args>
  std::string LocalizationManager::replace_parameters(std::string_view text, T&& arg0, Args&& ... args)
  { return _replace_parameters_impl(text, 0, std::forward<T>(arg0), std::forward<Args>(args)...); }

  template<typename... Args>
  std::string LocalizationManager::_translate_impl(const std::string& textInLanguage, Args&& ... args) const
  {
      constexpr unsigned int numArgs = sizeof...(Args);

      #ifdef BK_DEBUG_MODE
      const unsigned int numParameterPlaceholders = count_parameter_placeholders(textInLanguage);

    if (numParameterPlaceholders != numArgs)
    {
        std::cerr << "invalid number of arguments for localization string (" << numParameterPlaceholders << " required, " << numArgs << " given, text \"" << textInLanguage << "\")" << std::endl;
        assert(numParameterPlaceholders == numArgs);
    }
      #endif

      if constexpr (numArgs == 0)
      { return textInLanguage; }
      else
      { return _replace_parameters_impl(textInLanguage, 0, std::forward<Args>(args)...); }
  }

  template<typename... Args>
  std::string LocalizationManager::translate(std::size_t key, Args&& ... args) const
  { return _translate_impl(get(key), std::forward<Args>(args)...); }

  template<typename... Args>
  std::string LocalizationManager::translate(std::string_view key, Args&& ... args) const
  { return _translate_impl(get(key), std::forward<Args>(args)...); }
} // namespace bk

#endif //BKTOOLS_LOCALIZATIONMANAGER_HPP
