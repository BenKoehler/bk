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

#ifndef BKTOOLS_LOCALIZATIONMANAGER_HPP
#define BKTOOLS_LOCALIZATIONMANAGER_HPP

#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>

#include <bkTools/localization/LocalizationManager.h>

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

              return _replace_parameters_impl(ss.str(), cnt, std::forward<T>(arg0), std::forward<Args>(args)...);
          }
      }

      // argument was not found
      if constexpr (sizeof...(Args) != 0)
      { return _replace_parameters_impl(text, ++cnt, std::forward<Args>(args)...); }
      else
      { return text.data(); }
  }

  template<typename T, typename... Args>
  std::string LocalizationManager::replace_parameters(std::string_view text, T&& arg0, Args&& ... args)
  { return _replace_parameters_impl(text, 0, std::forward<T>(arg0), std::forward<Args>(args)...); }

  template<typename... Args>
  std::string LocalizationManager::_translate_impl(const std::string& textInLanguage, Args&& ... args) const
  {
      constexpr unsigned int numArgs = sizeof...(Args);

      assert(numArgs == count_parameter_placeholders(textInLanguage) && "invalid number of placeholders");

      if constexpr (numArgs == 0)
      { return textInLanguage; }
      else
      { return _replace_parameters_impl(textInLanguage, 0, std::forward<Args>(args)...); }
  }

  template<typename... Args>
  std::string LocalizationManager::translate_tag(unsigned int id, Args&& ... args) const
  { return _translate_impl(get_tag(id), std::forward<Args>(args)...); }

  template<unsigned long long key, typename... Args>
  std::string LocalizationManager::translate_text(Args&& ... args) const
  { return _translate_impl(get_text(key), std::forward<Args>(args)...); }

  template<typename... Args>
  std::string LocalizationManager::translate_text(unsigned long long key, Args&& ... args) const
  { return _translate_impl(get_text(key), std::forward<Args>(args)...); }

  template<typename... Args>
  std::string LocalizationManager::translate_text(std::string_view text, Args&& ... args) const
  { return translate_text(bk::string_utils::hash(text), std::forward<Args>(args)...); }
} // namespace bk

#endif //BKTOOLS_LOCALIZATIONMANAGER_HPP
