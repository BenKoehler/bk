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

#include <bkTools/string_utils/string_utils.h>

#include <cinttypes>
#include <fstream>

namespace bk::string_utils
{
  //====================================================================================================
  //===== COMPARISON
  //====================================================================================================
  namespace strut_details
  {
    /// @{ -------------------------------------------------- HELPER: CONTAINS
    bool _contains(std::string_view s, std::string_view x, bool case_sensitive)
    { return case_sensitive ? s.find(x) != s.npos : _contains(to_lower_case(std::string(s.data())), to_lower_case(std::string(x.data())), true); }
    /// @}

    /// @{ -------------------------------------------------- HELPER: STARTS/ENDS WITH
    bool _starts_with(std::string_view s, std::string_view x, bool case_sensitive)
    { return case_sensitive ? s.compare(0, x.size(), x) == 0 : _starts_with(to_lower_case(std::string(s.data())), to_lower_case(std::string(x.data())), true); }

    bool _ends_with(std::string_view s, std::string_view x, bool case_sensitive)
    { return case_sensitive ? s.compare(static_cast<std::string_view::size_type>(std::max(0, static_cast<int>(s.size()) - static_cast<int>(x.size()))), x.size(), x) == 0 : _ends_with(to_lower_case(std::string(s.data())), to_lower_case(std::string(x.data())), true); }
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- IS LOWER/UPPER CASE
  bool is_lower_case(std::string_view s)
  {
      return std::all_of(std::begin(s), std::end(s), [](const std::string_view::value_type& c) -> bool
      { return std::islower(c); });
  }

  bool is_upper_case(std::string_view s)
  {
      return std::all_of(std::begin(s), std::end(s), [](const std::string_view::value_type& c) -> bool
      { return std::isupper(c); });
  }
  /// @}

  /// @{ -------------------------------------------------- EQUALS
  bool equals(std::string_view s, std::string_view x, bool case_sensitive)
  {
      return (s.size() != x.size()) ? false : std::equal(std::begin(s), std::end(s), std::begin(x), [&](const std::string_view::value_type& a, const std::string_view::value_type& b) -> bool
      { return case_sensitive ? a == b : std::tolower(a) == std::tolower(b); });
  }
  /// @}

  /// @{ -------------------------------------------------- CONTAINS
  bool contains(std::string_view s, std::string_view x, bool case_sensitive)
  { return strut_details::_contains(std::move(s), std::move(x), case_sensitive); }
  /// @}

  /// @{ -------------------------------------------------- STARTS/ENDS WITH
  bool starts_with(std::string_view s, std::string_view x, bool case_sensitive)
  { return strut_details::_starts_with(std::move(s), std::move(x), case_sensitive); }

  bool ends_with(std::string_view s, std::string_view x, bool case_sensitive)
  { return strut_details::_ends_with(std::move(s), std::move(x), case_sensitive); }
  /// @}

  /// @{ -------------------------------------------------- COUNT OCCURRENCES
  int count_occurrences(std::string_view s, std::string_view pattern, bool case_sensitive)
  {
      using size_type = std::string::size_type;
      const size_type len = s.size();
      const size_type len_pattern = pattern.size();
      size_type i = 0;
      int cnt = 0;

      while (len_pattern < len && i <= len - len_pattern)
      {
          bool match = true;

          for (size_type k = i; k < i + len_pattern && match; ++k)
          { match = case_sensitive ? s[k] == pattern[k - i] : std::tolower(s[k]) == std::tolower(pattern[k - i]); }

          if (!match)
          { ++i; }
          else
          {
              i += len_pattern;
              ++cnt;
          }
      } // while

      return cnt;
  }
  /// @}

  //====================================================================================================
  //===== MODIFICATION
  //====================================================================================================
  namespace strut_details
  {
    /// @{ -------------------------------------------------- HELPER: TO UPPER/LOWER CASE
    void _to_upper_case(std::string& s)
    {
        std::for_each(std::begin(s), std::end(s), [](std::string::value_type& c)
        { c = std::toupper(c); });
    }

    void _to_lower_case(std::string& s)
    {
        std::for_each(std::begin(s), std::end(s), [](std::string::value_type& c)
        { c = std::tolower(c); });
    }
    /// @}

    /// @{ -------------------------------------------------- HELPER: APPEND
    void _append(std::string& s, std::string_view appendix)
    { s += appendix; }
    /// @}

    /// @{ -------------------------------------------------- HELPER: REMOVE
    void _remove(std::string& s, unsigned int pos, unsigned int n)
    {
        if (!s.empty())
        { s.erase(std::max(0U, std::min(pos, static_cast<unsigned int>(s.size() - 1))), std::max(0U, n)); }
    } // namespace strut_details

    void _remove(std::string& s, std::string_view pattern, bool case_sensitive)
    {
        using size_type = std::string::size_type;
        size_type len = s.size();
        const size_type len_pattern = pattern.size();
        size_type i = 0;

        while (len_pattern < len && i <= len - len_pattern)
        {
            bool match = true;

            for (size_type k = i; k < i + len_pattern && match; ++k)
            { match = case_sensitive ? s[k] == pattern[k - i] : std::tolower(s[k]) == std::tolower(pattern[k - i]); }

            if (!match)
            { ++i; }
            else
            {
                _remove(s, i, len_pattern);
                len = s.size();
            }
        } // while
    }

    void _remove_all_letters(std::string& s)
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), [](const char& c) -> bool
        {
            return std::isalpha(c);
        }), std::end(s));
    }

    void _remove_all_numbers(std::string& s)
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), [](const char& c) -> bool
        {
            return std::isdigit(c);
        }), std::end(s));
    }

    void _remove_all_special_characters(std::string& s)
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), [](const char& c) -> bool
        {
            return !std::isalnum(c);
        }), std::end(s));
    }

    void _remove_characters(std::string& s, std::string_view characters, bool case_sensitive)
    {
        s.erase(std::remove_if(std::begin(s), std::end(s), [&](const char& c) -> bool
        {
            return contains(characters, std::string(1, c), case_sensitive);
        }), std::end(s));
    }

    void _remove_all_except_characters(std::string& s, std::string_view characters)
    {
        using size_type = std::string::size_type;
        size_type N = s.size();
        const size_type nChars = characters.size();
        size_type i = 0;

        while (i < N)
        {
            bool equals_char = false;

            for (size_type k = 0; k < nChars && !equals_char; ++k)
            { equals_char = s[i] == characters[k]; }

            if (equals_char)
            { ++i; }
            else
            {
                s = remove(s, i, 1);
                --N;
            }
        }
    }
    /// @}



    /// @{ -------------------------------------------------- HELPER: CHOP
    void _chop_back(std::string& s, int n)
    { _remove(s, static_cast<int>(s.size()) - n, n); }

    void _chop_after(std::string& s, int n)
    { _chop_back(s, static_cast<int>(s.size()) - n - 1); }

    void _chop_front(std::string& s, int n)
    { _remove(s, 0, n); }
    /// @}

    /// @{ -------------------------------------------------- HELPER: FILL
    void _fill(std::string& s, char c)
    { std::fill(std::begin(s), std::end(s), c); }
    /// @}

    /// @{ -------------------------------------------------- HELPER: INSERT
    void _insert(std::string& s, int pos, std::string_view ins)
    {
        using size_type = std::string_view::size_type;
        const size_type pos_clamped = static_cast<size_type>(std::max(0, std::min(pos, static_cast<int>(s.size()))));
        s.insert(pos_clamped, ins);
    }

    void _insert(std::string& s, int pos, char ins)
    {
        using size_type = std::string::size_type;
        const size_type pos_clamped = static_cast<size_type>(std::max(0, std::min(pos, static_cast<int>(s.size()))));
        s.insert(pos_clamped, 1, ins);
    }
    /// @}

    /// @{ -------------------------------------------------- HELPER: REPLACE
    void _replace_char(std::string& s, char cold, char cnew, bool case_sensitive)
    {
        if (case_sensitive)
        { std::replace(std::begin(s), std::end(s), cold, cnew); }
        else
        {
            std::replace_if(std::begin(s), std::end(s), [&](const std::string::value_type& c)
            {
                return std::tolower(c) == std::tolower(cold);
            }, cnew);
        }
    }

    void _replace_string(std::string& s, std::string_view pattern, std::string_view replacement, bool case_sensitive)
    {
        if (pattern == replacement || pattern.empty() || s.empty())
        { return; }

        using size_type = std::string::size_type;

        size_type i = 0;
        size_type len = s.size();
        const size_type len_pattern = pattern.size();
        const size_type len_replacement = replacement.size();

        while (len_pattern < len && i <= len - len_pattern)
        {
            bool match = true;

            for (size_type k = i; k < i + len_pattern && match; ++k)
            { match = case_sensitive ? s[k] == pattern[k - i] : std::tolower(s[k]) == std::tolower(pattern[k - i]); }

            if (!match)
            {
                ++i;
                continue;
            }

            _remove(s, i, len_pattern);
            _insert(s, i, replacement);

            i += len_replacement;
            len = s.size();
        } // while (i < len)
    }

    void _replace_range(std::string& s, int pos, int n, std::string_view replacement)
    {
        _remove(s, pos, n);
        _insert(s, pos, replacement);
    }

    void _replace_range(std::string& s, int pos, int n, char replacement)
    {
        _remove(s, pos, n);
        _insert(s, pos, replacement);
    }
    /// @}

    /// @{ -------------------------------------------------- HELPER: REVERSE
    void _reverse(std::string& s)
    { std::reverse(std::begin(s), std::end(s)); }
    /// @}

    /// @{ -------------------------------------------------- HELPER: TRIM
    void _trim(std::string& s, std::string_view pattern)
    {
        while (starts_with(s, pattern))
        { s = chop_front(s, 1); }

        while (ends_with(s, pattern))
        { s = chop_back(s, 1); }
    }
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- TO UPPER/LOWER CASE
  std::string to_upper_case(std::string&& s)
  {
      strut_details::_to_upper_case(s);
      return std::move(s);
  }

  std::string to_upper_case(const std::string& s)
  {
      std::string r = s;
      strut_details::_to_upper_case(r);
      return r;
  }

  std::string to_lower_case(std::string&& s)
  {
      strut_details::_to_lower_case(s);
      return std::move(s);
  }

  std::string to_lower_case(const std::string& s)
  {
      std::string r = s;
      strut_details::_to_lower_case(r);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- APPEND
  std::string append(std::string&& s, std::string_view appendix)
  {
      strut_details::_append(s, appendix);
      return std::move(s);
  }

  std::string append(const std::string& s, std::string_view appendix)
  {
      std::string r = s;
      strut_details::_append(r, appendix);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- PREPEND
  std::string prepend(std::string&& s, std::string_view prefix)
  { return append(std::string(prefix.data()), std::move(s)); }

  std::string prepend(const std::string& s, std::string_view prefix)
  { return append(std::string(prefix.data()), s); }
  /// @}

  /// @{ -------------------------------------------------- REMOVE
  std::string remove(std::string&& s, unsigned int pos, unsigned int n)
  {
      strut_details::_remove(s, pos, n);
      return std::move(s);
  }

  std::string remove(const std::string& s, unsigned int pos, unsigned int n)
  {
      std::string r = s;
      strut_details::_remove(r, pos, n);
      return r;
  }

  std::string remove(std::string&& s, std::string_view pattern, bool case_sensitive)
  {
      strut_details::_remove(s, pattern, case_sensitive);
      return std::move(s);
  }

  std::string remove(const std::string& s, std::string_view pattern, bool case_sensitive)
  {
      std::string r = s;
      strut_details::_remove(r, pattern, case_sensitive);
      return r;
  }

  std::string remove_all_letters(std::string&& s)
  {
      strut_details::_remove_all_letters(s);
      return std::move(s);
  }

  std::string remove_all_letters(const std::string& s)
  {
      std::string r = s;
      strut_details::_remove_all_letters(r);
      return r;
  }

  std::string remove_all_numbers(std::string&& s)
  {
      strut_details::_remove_all_numbers(s);
      return std::move(s);
  }

  std::string remove_all_numbers(const std::string& s)
  {
      std::string r = s;
      strut_details::_remove_all_numbers(r);
      return r;
  }

  std::string remove_all_special_characters(std::string&& s)
  {
      strut_details::_remove_all_special_characters(s);
      return std::move(s);
  }

  std::string remove_all_special_characters(const std::string& s)
  {
      std::string r = s;
      strut_details::_remove_all_special_characters(r);
      return r;
  }

  std::string remove_characters(std::string&& s, std::string_view characters)
  {
      strut_details::_remove_characters(s, characters);
      return std::move(s);
  }

  std::string remove_characters(const std::string& s, std::string_view characters)
  {
      std::string r = s;
      strut_details::_remove_characters(r, characters);
      return r;
  }

  std::string remove_all_except_characters(std::string&& s, std::string_view characters)
  {
      strut_details::_remove_all_except_characters(s, characters);
      return std::move(s);
  }

  std::string remove_all_except_characters(const std::string& s, std::string_view characters)
  {
      std::string r = s;
      strut_details::_remove_all_except_characters(r, characters);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- CHOP
  std::string chop_back(std::string&& s, int n)
  {
      strut_details::_chop_back(s, n);
      return std::move(s);
  }

  std::string chop_back(const std::string& s, int n)
  {
      std::string r = s;
      strut_details::_chop_back(r, n);
      return r;
  }

  std::string chop_after(std::string&& s, int n)
  {
      strut_details::_chop_after(s, n);
      return std::move(s);
  }

  std::string chop_after(const std::string& s, int n)
  {
      std::string r = s;
      strut_details::_chop_after(r, n);
      return r;
  }

  std::string chop_front(std::string&& s, int n)
  {
      strut_details::_chop_front(s, n);
      return std::move(s);
  }

  std::string chop_front(const std::string& s, int n)
  {
      std::string r = s;
      strut_details::_chop_front(r, n);
      return r;
  }

  std::string chop_before(std::string&& s, int n)
  {
      strut_details::_chop_front(s, n);
      return std::move(s);
  }

  std::string chop_before(const std::string& s, int n)
  {
      std::string r = s;
      strut_details::_chop_front(r, n);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- FILL
  std::string fill(std::string&& s, char c)
  {
      strut_details::_fill(s, c);
      return std::move(s);
  }

  std::string fill(const std::string& s, char c)
  {
      std::string r = s;
      strut_details::_fill(r, c);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- INSERT
  std::string insert(std::string&& s, int pos, std::string_view ins)
  {
      strut_details::_insert(s, pos, ins);
      return std::move(s);
  }

  std::string insert(std::string&& s, int pos, char ins)
  {
      strut_details::_insert(s, pos, ins);
      return std::move(s);
  }

  std::string insert(const std::string& s, int pos, std::string_view ins)
  {
      std::string r = s;
      strut_details::_insert(r, pos, ins);
      return r;
  }

  std::string insert(const std::string& s, int pos, char ins)
  {
      std::string r = s;
      strut_details::_insert(r, pos, ins);
      return r;
  }

  std::string insert_after(std::string&& s, int pos, std::string_view ins)
  { return insert(std::move(s), pos + 1, ins); }

  std::string insert_after(std::string&& s, int pos, char ins)
  { return insert(std::move(s), pos + 1, ins); }

  std::string insert_after(const std::string& s, int pos, std::string_view ins)
  { return insert(s, pos + 1, ins); }

  std::string insert_after(const std::string& s, int pos, char ins)
  { return insert(s, pos + 1, ins); }
  /// @}

  /// @{ -------------------------------------------------- REPLACE
  std::string replace(std::string&& s, std::string_view sold, std::string_view snew, bool case_sensitive)
  {
      strut_details::_replace_string(s, sold, snew, case_sensitive);
      return std::move(s);
  }

  std::string replace(const std::string& s, std::string_view sold, std::string_view snew, bool case_sensitive)
  {
      std::string r = s;
      strut_details::_replace_string(r, sold, snew, case_sensitive);
      return r;
  }

  std::string replace(std::string&& s, int pos, int n, std::string_view replacement)
  {
      strut_details::_replace_range(s, pos, n, replacement);
      return std::move(s);
  }

  std::string replace(const std::string& s, int pos, int n, std::string_view replacement)
  {
      std::string r = s;
      strut_details::_replace_range(r, pos, n, replacement);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- REVERSE
  std::string reverse(std::string&& s)
  {
      strut_details::_reverse(s);
      return std::move(s);
  }

  std::string reverse(const std::string& s)
  {
      std::string r = s;
      strut_details::_reverse(r);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- TRIM
  std::string trim(std::string&& s, std::string_view pattern)
  {
      strut_details::_trim(s, pattern);
      return std::move(s);
  }

  std::string trim(const std::string& s, std::string_view pattern)
  {
      std::string r = s;
      strut_details::_trim(r, pattern);
      return r;
  }

  std::string trim(std::string&& s)
  { return trim(std::move(s), " "); }

  std::string trim(const std::string& s)
  { return trim(s, " "); }
  /// @}

  //====================================================================================================
  //===== MISC
  //====================================================================================================
  namespace strut_details
  {
    /// @{ --------------------------------------------------  HELPER: SUB STRING
    void _sub_string(std::string& s, unsigned int pos, unsigned int n)
    {
        if (n == 0)
        { s.clear(); }
        else
        {
            _chop_front(s, pos);
            _chop_after(s, n - 1);
        }
    }
    /// @}
  } // namespace strut_details

  /// @{ -------------------------------------------------- SUB STRING
  std::string sub_string(std::string&& s, int pos, int n)
  {
      strut_details::_sub_string(s, pos, n);
      return std::move(s);
  }

  std::string sub_string(const std::string& s, int pos, int n)
  {
      std::string r = s;
      strut_details::_sub_string(r, pos, n);
      return r;
  }
  /// @}

  /// @{ -------------------------------------------------- SPLIT
  std::vector<std::string> split(const std::string& s, std::string_view split_string, bool case_sensitive)
  {
      using size_type = std::string::size_type;

      const size_type len = s.size();
      const size_type len_pattern = split_string.size();

      std::vector<std::string> strvec;
      size_type i = 0;
      size_type indexOfLastFoundPattern = i;

      while (len_pattern < len && i <= len - len_pattern)
      {
          bool match = true;

          for (size_type k = i; k < i + len_pattern && match; ++k)
          { match = case_sensitive ? s[k] == split_string[k - i] : std::tolower(s[k]) == std::tolower(split_string[k - i]); }

          if (!match)
          {
              ++i;
              continue;
          }

          if (indexOfLastFoundPattern != i)
          { strvec.push_back(sub_string(s, indexOfLastFoundPattern, i - indexOfLastFoundPattern)); }

          i += len_pattern;
          indexOfLastFoundPattern = i;
      } // while

      if (!strvec.empty())
      { strvec.push_back(sub_string(s, indexOfLastFoundPattern, s.length() - indexOfLastFoundPattern)); }

      return strvec;
  }

  std::vector<std::string> split(const std::string& s, char split_char, bool case_sensitive)
  { return split(s, std::string(1, split_char), case_sensitive); }
  /// @}

  //====================================================================================================
  //===== CONVERSION
  //====================================================================================================
  /// @{ -------------------------------------------------- TO NUMBER
  int to_int(const std::string& s)
  { return s.empty() ? 0 : std::stoi(s); }

  unsigned int to_uint(const std::string& s)
  { return s.empty() ? 0 : static_cast<unsigned int>(std::stoul(s)); }

  long to_long(const std::string& s)
  { return s.empty() ? 0 : std::stol(s); }

  unsigned long to_ulong(const std::string& s)
  { return s.empty() ? 0 : std::stoul(s); }

  float to_float(const std::string& s)
  { return s.empty() ? 0 : std::stof(s); }

  double to_double(const std::string& s)
  { return s.empty() ? 0 : std::stod(s); }

  long double to_long_double(const std::string& s)
  { return s.empty() ? 0 : std::stold(s); }
  /// @}

  /// @{ -------------------------------------------------- FROM NUMBER
  std::string from_int(int x)
  { return from_number(x); }

  std::string from_uint(unsigned int x)
  { return from_number(x); }

  std::string from_long(long x)
  { return from_number(x); }

  std::string from_ulong(unsigned long x)
  { return from_number(x); }

  std::string from_float(float x)
  { return from_number(x); }

  std::string from_double(double x)
  { return from_number(x); }

  std::string from_long_double(long double x)
  { return from_number(x); }
  /// @}

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  /// @{ -------------------------------------------------- WRITE/READ STRING TO/FROM FILE
  void write_string_to_binary_file(const std::string& s, std::ofstream& file)
  {
      using size_type = std::uint16_t;

      const size_type temp = s.size();
      file.write(reinterpret_cast<const char*>(&temp), sizeof(size_type));
      file.write(s.c_str(), temp);
  }

  std::string read_string_from_binary_file(std::ifstream& file)
  {
      if (file.good())
      {
          using size_type = std::uint16_t;

          size_type temp = 0;
          file.read(reinterpret_cast<char*>(&temp), sizeof(size_type));

          if (temp != 0)
          {
              std::string s(temp, ' ');
              file.read(s.data(), temp);
              return s;
          }
      }

      return "";
  }
  /// @}

  //====================================================================================================
  //===== HASH (FNV-1a algorithm)
  //====================================================================================================
  unsigned long long hash(std::string_view str)
  { return strut_details::hash_one(str[0], str.data() + 1, /*FNV-1a constant "basis"*/14695981039346656037ULL); }

  unsigned long long hash_obj::operator()(std::string_view str) const
  { return hash(str); }

  //====================================================================================================
  //===== OUTPUT
  //====================================================================================================
  /// @{ -------------------------------------------------- STREAM OPERATOR FOR STRING VECTOR
  std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& strvec)
  {
      if (strvec.empty())
      {
          os << "[]";
          return os;
      }

      using size_type = std::vector<std::string>::size_type;
      const size_type N = strvec.size();

      os << "[";

      for (size_type i = 0; i < N - 1; ++i)
      { os << "\"" << strvec[i] << "\", "; }

      os << "\"" << strvec[N - 1] << "\"]";

      return os;
  }
  /// @}
} // namespace bk::string_utils