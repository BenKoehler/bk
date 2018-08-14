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

#include <bkTools/Localization>

#include "my_custom_localization.h"

//====================================================================================================
//===== ENGLISH
//====================================================================================================
LocalizationEnglish::LocalizationEnglish()
    : LocalizationManager()
{
    set(Text_language, "English");
    set(Text_button_ok, "Ok");
    set(Text_button_cancel, "Cancel");
    set(Text_calculation_time, "@0 finished in @1 seconds");
    set(Text_calculation_name_pathlines, "Pathline calculation");

    set("Calculate pathlines");
    set("Calculating @0 pathlines");
    set("repeat me @0 times @1 @1 @1 (was @0x repeated)");
}

LocalizationEnglish::LocalizationEnglish(const LocalizationEnglish&) = default;
LocalizationEnglish::~LocalizationEnglish() = default;
LocalizationEnglish& LocalizationEnglish::operator=(const LocalizationEnglish&) = default;

void set_localization_english()
{ set_localization<LocalizationEnglish>(); }

//====================================================================================================
//===== GERMAN
//====================================================================================================
LocalizationGerman::LocalizationGerman()
    : LocalizationManager()
{
    set(Text_language, "Deutsch");
    set(Text_button_ok, "Ok");
    set(Text_button_cancel, "Abbrechen");
    set(Text_calculation_time, "@0 abgeschlossen in @1 Sekunden");
    //set(Text_calculation_time, "@1 s benoetigt fuer @0"); // switched argument order; works!
    //set(Text_calculation_time, "@1 abgeschlossen in @2 Sekunden"); // error finding replacement tags
    set(Text_calculation_name_pathlines, "Pfadlinien Berechnung");

    set("Calculate pathlines", "Berechne Pfadlinien");
    set("Calculating @0 pathlines", "Berechne @0 Pfadlinien");
    set("repeat me @0 times @1 @1 @1 (was @0x repeated)", "wiederhole mich @0 mal @1 @1 @1 (wurde @0x wiederholt)");
}

LocalizationGerman::LocalizationGerman(const LocalizationGerman&) = default;
LocalizationGerman::~LocalizationGerman() = default;
LocalizationGerman& LocalizationGerman::operator=(const LocalizationGerman&) = default;

void set_localization_german()
{ set_localization<LocalizationGerman>(); }