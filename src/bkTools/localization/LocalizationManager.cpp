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
  std::string LocalizationManager::get_tag(unsigned int id) const
  {
      if (id < _tags.size())return _tags[id];

      std::stringstream s;
      s << "UNKNOWN-TAG(";
      s << id;
      s << ")";
      return s.str();
  }

  std::string LocalizationManager::get_text(unsigned long long hash) const
  {
      if (const auto it = _text.find(hash); it != _text.end())
      {return it->second;}

      std::stringstream s;
      s << "UNKNOWN-HASH(";
      s << hash;
      s << ")";
      return s.str();
  }

  std::string LocalizationManager::get_text(std::string_view referenceText) const
  {
      const unsigned long long hash = bk::string_utils::hash(referenceText);

      const auto it = _text.find(hash);

      if (_text.find(hash) == _text.end())
      { std::cerr << "text \"" << referenceText << "\" was not found in the database!" << std::endl; }

      if (it != _text.end())
      {return it->second;}

      std::stringstream s;
      s << "UNKNOWN-TEXT(";
      s << referenceText;
      s << ")";
      return s.str();
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

  void LocalizationManager::set_tag(unsigned int id, std::string_view textInLanguage)
  {
      if (id >= _tags.size())
      { _tags.resize(id + 1); }

      _tags[id] = textInLanguage;
  }

  void LocalizationManager::set_text(unsigned long long hash, std::string_view textInLanguage)
  {      _text.try_emplace(hash, std::string(textInLanguage));  }

  void LocalizationManager::set_text(std::string_view key, std::string_view textInLanguage)
  { set_text(bk::string_utils::hash(key.data()), textInLanguage); }

  void LocalizationManager::set_text(std::string_view key_equals_textInLanguage)
  { set_text(key_equals_textInLanguage, key_equals_textInLanguage); }

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
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
} // namespace bk