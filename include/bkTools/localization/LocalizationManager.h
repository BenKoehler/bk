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

#ifndef BKTOOLS_LOCALIZATIONMANAGER_H
#define BKTOOLS_LOCALIZATIONMANAGER_H

#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bk
{
  class LocalizationManager
  {
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      std::unordered_map<std::string, std::string> entries_text;
      std::map<std::size_t, std::string>           entries_tag;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      LocalizationManager();
      LocalizationManager(const LocalizationManager&);
      LocalizationManager(LocalizationManager&&) noexcept = default;
      ~LocalizationManager();

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      std::string get(std::size_t key) const;
      std::string get(std::string_view referenceTextInEnglish) const;

      //! check if there is any tag with an arbitrary (first version) or specific number (second version) at this position
      [[nodiscard]] static bool is_tag_at_position(std::string_view text, unsigned int pos);
      [[nodiscard]] static bool is_tag_at_position(std::string_view text, unsigned int pos, std::string_view argId);

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      [[maybe_unused]] LocalizationManager& operator=(const LocalizationManager&);
      [[maybe_unused]] LocalizationManager& operator=(LocalizationManager&&) noexcept = default;

      void set(std::size_t, std::string_view textInLanguage);
      void set(std::string_view key, std::string_view textInLanguage);
      void set(std::string_view key_equals_textInLanguage);

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      #ifdef BK_DEBUG_MODE
      [[nodiscard]] static unsigned int count_parameter_placeholders(const std::string& text);
      #endif

      template<typename T, typename... Args>
      [[nodiscard]] static std::string _replace_parameters_impl(std::string_view text, unsigned int cnt, T&& arg0, Args&& ... args);

      template<typename... Args>
      [[nodiscard]] std::string _translate_impl(const std::string& textInLanguage, Args&& ... args) const;
    public:

      template<typename T, typename... Args>
      [[nodiscard]] static std::string replace_parameters(std::string_view text, T&& arg0, Args&& ... args);

      template<typename... Args>
      [[nodiscard]] std::string translate(std::size_t key, Args&& ... args) const;
      template<typename... Args>
      [[nodiscard]] std::string translate(std::string_view key, Args&& ... args) const;
  };
} // namespace bk

#include "LocalizationManager.hpp"

#endif //BKTOOLS_LOCALIZATIONMANAGER_H
