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

#ifndef BK_MY_CUSTOM_LOCALIZATION_H
#define BK_MY_CUSTOM_LOCALIZATION_H

#include <memory>

#include <bkTools/Localization>

enum Text_ : std::size_t
{
    Text_language                   = __LINE__, //!< no parameters
    Text_button_ok                  = __LINE__, //!< no parameters
    Text_button_cancel              = __LINE__, //!< no parameters
    Text_calculation_time           = __LINE__, //!< @0: string "calculation name" ; @1: int N (seconds)
    Text_calculation_name_pathlines = __LINE__, //!< @0: string "calculation name"
};

template<typename TLoc>
void set_localization()
{
    //bk::GlobalLocalizationManager::get_instance().localization_manager().reset(nullptr);
    //bk::GlobalLocalizationManager::get_instance().localization_manager() = std::make_unique<TLoc>();

    // same as above; convenience macro
    bk_lm_unique_ptr.reset(nullptr);
    bk_lm_unique_ptr = std::make_unique<TLoc>();
}

//====================================================================================================
//===== ENGLISH
//====================================================================================================
struct LocalizationEnglish : public bk::LocalizationManager
{
    LocalizationEnglish();
    LocalizationEnglish(const LocalizationEnglish&);
    LocalizationEnglish(LocalizationEnglish&&) noexcept = default;
    ~LocalizationEnglish();
    LocalizationEnglish& operator=(const LocalizationEnglish&);
    LocalizationEnglish& operator=(LocalizationEnglish&&) noexcept = default;
};

void set_localization_english();

//====================================================================================================
//===== GERMAN
//====================================================================================================
struct LocalizationGerman : public bk::LocalizationManager
{
    LocalizationGerman();
    LocalizationGerman(const LocalizationGerman&);
    LocalizationGerman(LocalizationGerman&&) noexcept = default;
    ~LocalizationGerman();
    LocalizationGerman& operator=(const LocalizationGerman&);
    LocalizationGerman& operator=(LocalizationGerman&&) noexcept = default;
};

void set_localization_german();

#endif //BK_MY_CUSTOM_LOCALIZATION_H
