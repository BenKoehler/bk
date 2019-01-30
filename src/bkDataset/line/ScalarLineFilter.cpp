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

#include <bkDataset/line/ScalarLineFilter.h>

#include <algorithm>
#include <string_view>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ScalarLineFilter::ScalarLineFilter()
      : _concatenationMode(ConcatenationMode::And)
  { /* do nothing */ }

  ScalarLineFilter::ScalarLineFilter(const self_type&) = default;
  ScalarLineFilter::ScalarLineFilter(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ScalarLineFilter::~ScalarLineFilter() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- NUM THRESHOLDS
  unsigned int ScalarLineFilter::num_thresholds() const
  { return _thresholds.size(); }

  bool ScalarLineFilter::has_thresholds() const
  { return !_thresholds.empty(); }
  /// @}

  /// @{ -------------------------------------------------- GET CONCATENATION MODE
  auto ScalarLineFilter::concatenation_mode() const -> const ConcatenationMode&
  { return _concatenationMode; }

  bool ScalarLineFilter::concatenation_mode_is_and() const
  { return _concatenationMode == ConcatenationMode::And; }

  bool ScalarLineFilter::concatenation_mode_is_or() const
  { return _concatenationMode == ConcatenationMode::Or; }
  /// @}

  /// @{ -------------------------------------------------- GET THRESHOLD
  ScalarLineThreshold* ScalarLineFilter::get_threshold(const std::string& attributeName)
  {
      auto it = std::find_if(_thresholds.begin(), _thresholds.end(), [&](const ScalarLineThreshold& x) -> bool
      {
          // case insensitive string comparison
          std::string_view s = x.attribute_name();

          return (s.size() != attributeName.size()) ? false : std::equal(std::begin(s), std::end(s), std::begin(attributeName), [&](const std::string_view::value_type& a, const std::string_view::value_type& b) -> bool
          { return std::tolower(a) == std::tolower(b); });
      });

      return it == _thresholds.end() ? nullptr : &*it;
  }

  ScalarLineThreshold* ScalarLineFilter::get_threshold(unsigned int id)
  { return id < _thresholds.size() ? &_thresholds[id] : nullptr; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET CONCATENATION MODE
  void ScalarLineFilter::set_concatenation_mode_and()
  { _concatenationMode = ConcatenationMode::And; }

  void ScalarLineFilter::set_concatenation_mode_or()
  { _concatenationMode = ConcatenationMode::Or; }
  /// @}

  /// @{ -------------------------------------------------- ADD THRESHOLD
  ScalarLineThreshold& ScalarLineFilter::add_threshold(const std::string& attributeName)
  {
      ScalarLineThreshold t;
      t.set_attribute_name(attributeName);
      return add_threshold(std::move(t));
  }

  ScalarLineThreshold& ScalarLineFilter::add_threshold(const ScalarLineThreshold& t)
  { return add_threshold(std::move(ScalarLineThreshold(t))); }

  ScalarLineThreshold& ScalarLineFilter::add_threshold(ScalarLineThreshold&& t)
  {
      _thresholds.push_back(std::move(t));
      return _thresholds.back();
  }
  /// @}

  /// @{ -------------------------------------------------- REMOVE THRESHOLD
  void ScalarLineFilter::remove_threshold(const std::string& attributeName)
  {
      _thresholds.erase(std::remove_if(_thresholds.begin(), _thresholds.end(), [&](const ScalarLineThreshold& x) -> bool
      {
          // case insensitive string comparison
          std::string_view s = x.attribute_name();

          return (s.size() != attributeName.size()) ? false : std::equal(std::begin(s), std::end(s), std::begin(attributeName), [&](const std::string_view::value_type& a, const std::string_view::value_type& b) -> bool
          { return std::tolower(a) == std::tolower(b); });
      }), _thresholds.end());
  }

  void ScalarLineFilter::remove_threshold(unsigned int id)
  {
      if (id < _thresholds.size())
      { _thresholds.erase(_thresholds.begin() + id); }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto ScalarLineFilter::operator=(const self_type&) -> self_type& = default;
  auto ScalarLineFilter::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void ScalarLineFilter::clear()
  { _thresholds.clear(); }
  /// @}
} // namespace bk


