/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#pragma once

#ifndef BK_STRING_UTILS_H
#define BK_STRING_UTILS_H

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <bkTools/lib/bkTools_export.h>

namespace bk::string_utils
{
  //====================================================================================================
  //===== COMPARISON
  //====================================================================================================
  namespace strut_details
  {
    /// @{ -------------------------------------------------- HELPER: CONTAINS
    [[nodiscard]] bool BKTOOLS_EXPORT _contains(std::string_view s, std::string_view x, bool case_sensitive);
    /// @}

    /// @{ -------------------------------------------------- HELPER: STARTS/ENDS WITH
    [[nodiscard]] bool BKTOOLS_EXPORT _starts_with(std::string_view s, std::string_view x, bool case_sensitive);
    [[nodiscard]] bool BKTOOLS_EXPORT _ends_with(std::string_view s, std::string_view x, bool case_sensitive);
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- IS LOWER/UPPER CASE
  [[nodiscard]] bool BKTOOLS_EXPORT is_lower_case(std::string_view s);
  [[nodiscard]] bool BKTOOLS_EXPORT is_upper_case(std::string_view s);
  /// @}

  /// @{ -------------------------------------------------- EQUALS
  [[nodiscard]] bool BKTOOLS_EXPORT equals(std::string_view s, std::string_view x, bool case_sensitive = true);
  /// @}

  /// @{ -------------------------------------------------- CONTAINS
  [[nodiscard]] bool BKTOOLS_EXPORT contains(std::string_view s, std::string_view x, bool case_sensitive = true);
  /// @}

  /// @{ -------------------------------------------------- STARTS/ENDS WITH
  [[nodiscard]] bool BKTOOLS_EXPORT starts_with(std::string_view s, std::string_view x, bool case_sensitive = true);
  [[nodiscard]] bool BKTOOLS_EXPORT ends_with(std::string_view s, std::string_view x, bool case_sensitive = true);
  /// @}

  /// @{ -------------------------------------------------- COUNT OCCURRENCES
  [[nodiscard]] int BKTOOLS_EXPORT count_occurrences(std::string_view s, std::string_view pattern, bool case_sensitive = true);
  /// @}

  //====================================================================================================
  //===== MODIFICATION
  //====================================================================================================
  namespace strut_details
  {
    /// @{ -------------------------------------------------- HELPER: TO UPPER/LOWER CASE
    void BKTOOLS_EXPORT _to_upper_case(std::string& s);
    void BKTOOLS_EXPORT _to_lower_case(std::string& s);
    /// @}

    /// @{ -------------------------------------------------- HELPER: APPEND
    void BKTOOLS_EXPORT _append(std::string& s, std::string_view appendix);

    template<typename String0, typename String1>
    std::string _append_many(String0&& s, String1&& appendix)
    { return append(std::forward<String0>(s), std::forward<String1>(appendix)); }

    template<typename String0, typename String1, typename... Strings>
    std::string _append_many(String0&& s0, String1&& s1, Strings&& ... strings)
    { return _append_many(append(std::forward<String0>(s0), std::forward<String1>(s1)), std::forward<Strings>(strings)...); }
    /// @}

    /// @{ -------------------------------------------------- HELPER: REMOVE
    void BKTOOLS_EXPORT _remove(std::string& s, unsigned int pos, unsigned int n);
    void BKTOOLS_EXPORT _remove(std::string& s, std::string_view pattern, bool case_sensitive = true);
    void BKTOOLS_EXPORT _remove_all_letters(std::string& s);
    void BKTOOLS_EXPORT _remove_all_numbers(std::string& s);
    void BKTOOLS_EXPORT _remove_all_special_characters(std::string& s);
    void BKTOOLS_EXPORT _remove_characters(std::string& s, std::string_view characters, bool case_sensitive = true);
    void BKTOOLS_EXPORT _remove_all_except_characters(std::string& s, std::string_view characters);
    /// @}

    /// @{ -------------------------------------------------- HELPER: CHOP
    void BKTOOLS_EXPORT _chop_back(std::string& s, int n);
    void BKTOOLS_EXPORT _chop_after(std::string& s, int n);
    void BKTOOLS_EXPORT _chop_front(std::string& s, int n);
    /// @}

    /// @{ -------------------------------------------------- HELPER: FILL
    void BKTOOLS_EXPORT _fill(std::string& s, char c);
    /// @}

    /// @{ -------------------------------------------------- HELPER: INSERT
    void BKTOOLS_EXPORT _insert(std::string& s, int pos, std::string_view ins);
    void BKTOOLS_EXPORT _insert(std::string& s, int pos, char ins);
    /// @}

    /// @{ -------------------------------------------------- HELPER: REPLACE
    void BKTOOLS_EXPORT _replace_char(std::string& s, char cold, char cnew, bool case_sensitive);
    void BKTOOLS_EXPORT _replace_string(std::string& s, std::string_view pattern, std::string_view replacement, bool case_sensitive);
    void BKTOOLS_EXPORT _replace_range(std::string& s, int pos, int n, std::string_view replacement);
    void BKTOOLS_EXPORT _replace_range(std::string& s, int pos, int n, char replacement);
    /// @}

    /// @{ -------------------------------------------------- HELPER: REVERSE
    void BKTOOLS_EXPORT _reverse(std::string& s);
    /// @}

    /// @{ -------------------------------------------------- HELPER: TRIM
    void BKTOOLS_EXPORT _trim(std::string& s, std::string_view pattern);
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- TO UPPER/LOWER CASE
  [[nodiscard]] std::string BKTOOLS_EXPORT to_upper_case(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT to_upper_case(const std::string& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT to_lower_case(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT to_lower_case(const std::string& s);
  /// @}

  /// @{ -------------------------------------------------- APPEND
  [[nodiscard]] std::string BKTOOLS_EXPORT append(std::string&& s, std::string_view appendix);
  [[nodiscard]] std::string BKTOOLS_EXPORT append(const std::string& s, std::string_view appendix);

  template<typename... Strings, std::enable_if_t<(sizeof...(Strings) > 2)>* = nullptr>
  [[nodiscard]] std::string append(Strings&& ... strings)
  { return strut_details::_append_many(std::forward<Strings>(strings)...); }
  /// @}  

  /// @{ -------------------------------------------------- PREPEND
  [[nodiscard]] std::string BKTOOLS_EXPORT prepend(std::string&& s, std::string_view prefix);
  [[nodiscard]] std::string BKTOOLS_EXPORT prepend(const std::string& s, std::string_view prefix);
  /// @}

  /// @{ -------------------------------------------------- REMOVE
  [[nodiscard]] std::string BKTOOLS_EXPORT remove(std::string&& s, unsigned int pos, unsigned int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove(const std::string& s, unsigned int pos, unsigned int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove(std::string&& s, std::string_view pattern, bool case_sensitive = true);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove(const std::string& s, std::string_view pattern, bool case_sensitive = true);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_letters(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_letters(const std::string& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_numbers(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_numbers(const std::string& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_special_characters(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_special_characters(const std::string& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_characters(std::string&& s, std::string_view characters);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_characters(const std::string& s, std::string_view characters);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_except_characters(std::string&& s, std::string_view characters);
  [[nodiscard]] std::string BKTOOLS_EXPORT remove_all_except_characters(const std::string& s, std::string_view characters);
  /// @}

  /// @{ -------------------------------------------------- CHOP
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_back(std::string&& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_back(const std::string& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_after(std::string&& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_after(const std::string& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_front(std::string&& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_front(const std::string& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_before(std::string&& s, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT chop_before(const std::string& s, int n);
  /// @}

  /// @{ -------------------------------------------------- FILL
  [[nodiscard]] std::string BKTOOLS_EXPORT fill(std::string&& s, char c);
  [[nodiscard]] std::string BKTOOLS_EXPORT fill(const std::string& s, char c);
  /// @}

  /// @{ -------------------------------------------------- INSERT
  [[nodiscard]] std::string BKTOOLS_EXPORT insert(std::string&& s, int pos, std::string_view ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert(std::string&& s, int pos, char ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert(const std::string& s, int pos, std::string_view ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert(const std::string& s, int pos, char ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert_after(std::string&& s, int pos, std::string_view ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert_after(std::string&& s, int pos, char ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert_after(const std::string& s, int pos, std::string_view ins);
  [[nodiscard]] std::string BKTOOLS_EXPORT insert_after(const std::string& s, int pos, char ins);
  /// @}

  /// @{ -------------------------------------------------- REPLACE
  [[nodiscard]] std::string BKTOOLS_EXPORT replace(std::string&& s, std::string_view sold, std::string_view snew, bool case_sensitive = true);
  [[nodiscard]] std::string BKTOOLS_EXPORT replace(const std::string& s, std::string_view sold, std::string_view snew, bool case_sensitive = true);
  [[nodiscard]] std::string BKTOOLS_EXPORT replace(std::string&& s, int pos, int n, std::string_view replacement);
  [[nodiscard]] std::string BKTOOLS_EXPORT replace(const std::string& s, int pos, int n, std::string_view replacement);
  /// @}

  /// @{ -------------------------------------------------- REVERSE
  [[nodiscard]] std::string BKTOOLS_EXPORT reverse(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT reverse(const std::string& s);
  /// @}

  /// @{ -------------------------------------------------- TRIM
  //! remove all leading/trailing characters
  [[nodiscard]] std::string BKTOOLS_EXPORT trim(std::string&& s, std::string_view pattern);
  [[nodiscard]] std::string BKTOOLS_EXPORT trim(const std::string& s, std::string_view pattern);
  [[nodiscard]] std::string BKTOOLS_EXPORT trim(std::string&& s);
  [[nodiscard]] std::string BKTOOLS_EXPORT trim(const std::string& s);
  /// @}

  //====================================================================================================
  //===== MISC
  //====================================================================================================
  namespace strut_details
  {
    /// @{ -------------------------------------------------- HELPER: SUB STRING
    void BKTOOLS_EXPORT _sub_string(std::string& s, unsigned int pos, unsigned int n);
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- SUB STRING
  [[nodiscard]] std::string BKTOOLS_EXPORT sub_string(std::string&& s, int pos, int n);
  [[nodiscard]] std::string BKTOOLS_EXPORT sub_string(const std::string& s, int pos, int n);
  /// @}

  /// @{ -------------------------------------------------- SPLIT
  /*!
   * The split_string itself is excluded from the results; i.e. only sub strings before, after or between split_strings
   * are in the result vector, e.g. split("123abc456", "abc" results in ["123","456"].
   *
   * If a patterns appears twice in a row, an empty string between them will NOT be added to the result vector, e.g.
   * split("123xxx456", "x") yields ["123","456"] instead of ["123,"","","456"]
   *
   * If the split string is not found, a vector with one element - the input string - will be returned, e.g.
   * split("12345", "x") yields ["12345"]
   */
  [[nodiscard]] std::vector<std::string> BKTOOLS_EXPORT split(const std::string& s, std::string_view split_string, bool case_sensitive = true);
  [[nodiscard]] std::vector<std::string> BKTOOLS_EXPORT split(const std::string& s, char split_char, bool case_sensitive = true);
  /// @}

  //====================================================================================================
  //===== CONVERSION
  //====================================================================================================
  /// @{ -------------------------------------------------- TO NUMBER
  [[nodiscard]] int BKTOOLS_EXPORT to_int(const std::string& s);
  [[nodiscard]] unsigned int BKTOOLS_EXPORT to_uint(const std::string& s);
  [[nodiscard]] long BKTOOLS_EXPORT to_long(const std::string& s);
  [[nodiscard]] unsigned long BKTOOLS_EXPORT to_ulong(const std::string& s);
  [[nodiscard]] float BKTOOLS_EXPORT to_float(const std::string& s);
  [[nodiscard]] double BKTOOLS_EXPORT to_double(const std::string& s);
  [[nodiscard]] long double BKTOOLS_EXPORT to_long_double(const std::string& s);
  /// @}

  /// @{ -------------------------------------------------- FROM NUMBER
  template<typename T>
  [[nodiscard]] std::string from_number(T x)
  { return std::to_string(x); }

  [[nodiscard]] std::string BKTOOLS_EXPORT from_int(int x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_uint(unsigned int x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_long(long x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_ulong(unsigned long x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_float(float x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_double(double x);
  [[nodiscard]] std::string BKTOOLS_EXPORT from_long_double(long double x);
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- WRITE/READ STRING TO/FROM FILE
  void BKTOOLS_EXPORT write_string_to_binary_file(const std::string& s, std::ofstream& file);
  std::string BKTOOLS_EXPORT read_string_from_binary_file(std::ifstream& file);
  /// @}

  //====================================================================================================
  //===== HASH (FNV-1a algorithm)
  //====================================================================================================
  namespace strut_details
  {
    // compile-time hash helper function
    [[nodiscard]] constexpr unsigned long long hash_one(char c, const char* remain, unsigned long long value)
    { return c == 0 ? value : hash_one(remain[0], remain + 1, (value ^ c) * /*FNV-1a constant "prime"*/1099511628211ULL); }
  } // namespace strut_details

  // compile-time hash
  [[nodiscard]] constexpr unsigned long long hash(const char* str)
  { return strut_details::hash_one(str[0], str + 1, /*FNV-1a constant "basis"*/14695981039346656037ULL); }

  [[nodiscard]] unsigned long long BKTOOLS_EXPORT hash(std::string_view str);

  struct hash_obj
  {
      [[nodiscard]] unsigned long long BKTOOLS_EXPORT operator()(std::string_view str) const;

      [[nodiscard]] constexpr unsigned long long operator()(const char* str) const
      { return hash(str); }
  };

  //====================================================================================================
  //===== OUTPUT
  //====================================================================================================
  /// @{ -------------------------------------------------- STREAM OPERATOR FOR STRING VECTOR
  [[maybe_unused]] std::ostream& BKTOOLS_EXPORT operator<<(std::ostream& os, const std::vector<std::string>& strvec);
  /// @}
} // namespace bk::string_utils

#endif //BK_STRING_UTILS_H