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

#include <unordered_set>

#include <bkTools/localization/LocalizationManager.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  LocalizationManager::LocalizationManager() = default;
  LocalizationManager::LocalizationManager(const LocalizationManager&) = default;
  LocalizationManager::~LocalizationManager() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  std::string LocalizationManager::get(std::size_t key) const
  {
      const auto it = entries_tag.find(key);

      #ifdef BK_DEBUG_MODE
      if (it == entries_tag.end())
    { std::cerr << "tag " << key << " was not found in the database!" << std::endl; }
      #endif

      return it != entries_tag.end() ? it->second : "UNKNOWN-TAG";
  }

  std::string LocalizationManager::get(std::string_view referenceTextInEnglish) const
  {
      const auto it = entries_text.find(std::string(referenceTextInEnglish));

      #ifdef BK_DEBUG_MODE
      if (it == entries_text.end())
    { std::cerr << "text \"" << referenceTextInEnglish << "\" was not found in the database!" << std::endl; }
      #endif

      return it != entries_text.end() ? it->second : referenceTextInEnglish.data();
  }

  bool LocalizationManager::is_tag_at_position(std::string_view text, unsigned int pos)
  { return !text.empty() && pos < text.size() - 1 && text[pos] == '@' && std::isdigit(text[pos + 1]); }

  bool LocalizationManager::is_tag_at_position(std::string_view text, unsigned int pos, std::string_view argId)
  {
      if (is_tag_at_position(text, pos))
      {
          auto it0 = text.begin() + pos + 1;
          auto it1 = argId.begin();

          for (; it1 != argId.end(); ++it0, ++it1)
          {
              if (it0 == text.end() || *it0 != *it1)
              { return false; }
          }

          return true;
      }

      return false;
  }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  LocalizationManager& LocalizationManager::operator=(const LocalizationManager&) = default;

  void LocalizationManager::set(std::size_t key, std::string_view textInLanguage)
  { entries_tag.try_emplace(key, textInLanguage); }

  void LocalizationManager::set(std::string_view key, std::string_view textInLanguage)
  { entries_text.try_emplace(std::string(key.data()), std::string(textInLanguage)); }

  void LocalizationManager::set(std::string_view key_equals_textInLanguage)
  { set(key_equals_textInLanguage, key_equals_textInLanguage); }

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  #ifdef BK_DEBUG_MODE

  unsigned int LocalizationManager::count_parameter_placeholders(const std::string& text)
  {
      std::unordered_set<std::string> placeholders;

      for (unsigned int i = 0; i < text.length(); ++i)
      {
          if (text[i] == '@')
          {
              unsigned int k = i + 1;

              while (std::isdigit(text[k]) && k < text.length())
              { ++k; }

              if (k != i + 1)
              { placeholders.insert(text.substr(i, k - i)); }
          }
      }

      return placeholders.size();
  }

  #endif
} // namespace bk