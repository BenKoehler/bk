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

#ifndef BKTOOLS_GLOBALLOCALIZATIONMANAGER_H
#define BKTOOLS_GLOBALLOCALIZATIONMANAGER_H

#include <bkTools/localization/LocalizationManager.h>

#include <memory>

#include <bk/Singleton>
#include <bk/StringUtils>
#include <bkTools/lib/bkTools_export.h>

#define bk_lm_unique_ptr bk::GlobalLocalizationManager::get_instance().localization_manager()
#define bk_lm (*bk_lm_unique_ptr.get())

namespace bk
{
  class BKTOOLS_EXPORT GlobalLocalizationManager : public bk::Singleton<GlobalLocalizationManager>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      friend Singleton;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      std::unique_ptr<LocalizationManager> _lm;

      //====================================================================================================
      //===== STANDARD FUNCTIONS
      //====================================================================================================
      GlobalLocalizationManager();
      GlobalLocalizationManager(const GlobalLocalizationManager&) = delete;
      GlobalLocalizationManager(GlobalLocalizationManager&&) noexcept = default;
      ~GlobalLocalizationManager();
      GlobalLocalizationManager& operator=(const GlobalLocalizationManager&) = delete;
      GlobalLocalizationManager& operator=(GlobalLocalizationManager&&) noexcept = default;

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    public:
      [[nodiscard]] std::unique_ptr<LocalizationManager>& localization_manager();
      [[nodiscard]] const std::unique_ptr<LocalizationManager>& localization_manager() const;
  }; // class GlobalLocalizationManager
} // namespace bk


#ifdef BK_LOCALIZATION
    #define TRANSLATION_FUNCTION bk_lm.translate_text
    #define ___1(String) TRANSLATION_FUNCTION (bk::string_utils::hash(String))
#else
namespace bk::details
{
  constexpr const char* bk_print(const char* text)
  { return text; }

  template<typename... Args>
  std::string bk_print(std::string_view text, Args&& ... args)
  {
      if constexpr (sizeof...(Args) != 0)
      { return LocalizationManager::replace_parameters(text, std::forward<Args>(args)...); }
      else
      { return text.data(); }
  }
} // namespace bk::details

    #define TRANSLATION_FUNCTION bk::details::bk_print
    #define ___1(String) TRANSLATION_FUNCTION (String)
#endif


//#define ___1(String) TRANSLATION_FUNCTION (String)
#define ___2(String, x0) TRANSLATION_FUNCTION (String, x0)
#define ___3(String, x0, x1) TRANSLATION_FUNCTION (String, x0, x1)
#define ___4(String, x0, x1, x2) TRANSLATION_FUNCTION (String, x0, x1, x2)
#define ___5(String, x0, x1, x2, x3) TRANSLATION_FUNCTION (String, x0, x1, x2, x3)
#define ___6(String, x0, x1, x2, x3, x4) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4)
#define ___7(String, x0, x1, x2, x3, x4, x5) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4, x5)
#define ___8(String, x0, x1, x2, x3, x4, x5, x6) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4, x5, x6)
#define ___9(String, x0, x1, x2, x3, x4, x5, x6, x7) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4, x5, x6, x7)
#define ___10(String, x0, x1, x2, x3, x4, x5, x6, x7, x8) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4, x5, x6, x7, x8)
#define ___11(String, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9) TRANSLATION_FUNCTION (String, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, NAME, ...) NAME
#define ___(...) GET_MACRO(__VA_ARGS__, ___11, ___10, ___9, ___8, ___7, ___6, ___5, ___4, ___3, ___2, ___1)(__VA_ARGS__)

#endif //TES// TPROJECT_GLOBALLOCALIZATIONMANAGER_H