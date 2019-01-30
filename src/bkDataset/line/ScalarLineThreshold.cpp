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

#include <bkDataset/line/ScalarLineThreshold.h>

#include <algorithm>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  ScalarLineThreshold::ScalarLineThreshold()
      : _thresholdMode(ThresholdMode::GreaterOrEqualThan),
        _valueMode(ValueMode::AbsoluteValue),
        _attributeType(AttributeType::PointAttribute),
        _threshold(0.0),
        _attributeName("")
  { /* do nothing */ }

  ScalarLineThreshold::ScalarLineThreshold(const self_type&) = default;
  ScalarLineThreshold::ScalarLineThreshold(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  ScalarLineThreshold::~ScalarLineThreshold() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET THRESHOLD MODE
  auto ScalarLineThreshold::threshold_mode() const -> const ThresholdMode&
  { return _thresholdMode; }

  bool ScalarLineThreshold::threshold_mode_is_greater() const
  { return _thresholdMode == ThresholdMode::GreaterThan; }

  bool ScalarLineThreshold::threshold_mode_is_greater_or_equal() const
  { return _thresholdMode == ThresholdMode::GreaterOrEqualThan; }

  bool ScalarLineThreshold::threshold_mode_is_lesser() const
  { return _thresholdMode == ThresholdMode::LesserThan; }

  bool ScalarLineThreshold::threshold_mode_is_lesser_or_equal() const
  { return _thresholdMode == ThresholdMode::LesserOrEqualThan; }

  bool ScalarLineThreshold::threshold_mode_is_equal() const
  { return _thresholdMode == ThresholdMode::Equal; }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE MODE
  auto ScalarLineThreshold::value_mode() const -> const ValueMode&
  { return _valueMode; }

  bool ScalarLineThreshold::value_mode_is_absolute() const
  { return _valueMode == ValueMode::AbsoluteValue; }

  bool ScalarLineThreshold::value_mode_is_relative() const
  { return _valueMode == ValueMode::RelativeValue; }
  /// @}

  /// @{ -------------------------------------------------- GET ATTRIBUTE TYPE
  auto ScalarLineThreshold::attribute_type() const -> const AttributeType&
  { return _attributeType; }

  bool ScalarLineThreshold::attribute_type_is_point_attribute() const
  { return _attributeType == AttributeType::PointAttribute; }

  bool ScalarLineThreshold::attribute_type_is_cell_attribute() const
  { return _attributeType == AttributeType::CellAttribute; }

  bool ScalarLineThreshold::attribute_type_is_object_attribute() const
  { return _attributeType == AttributeType::ObjectAttribute; }
  /// @}

  /// @{ -------------------------------------------------- GET THRESHOLD
  double ScalarLineThreshold::threshold() const
  { return _threshold; }
  /// @}

  /// @{ -------------------------------------------------- GET ATTRIBUTE NAME
  const std::string& ScalarLineThreshold::attribute_name() const
  { return _attributeName; }
  /// @}

  /// @{ -------------------------------------------------- GET ATTRIBUTE MIN/MAX
  double ScalarLineThreshold::attribute_min() const
  { return _attributeMin; }

  double ScalarLineThreshold::attribute_max() const
  { return _attributeMax; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- SET THRESHOLD MODE
  void ScalarLineThreshold::set_threshold_mode_to_greater()
  { _thresholdMode = ThresholdMode::GreaterThan; }

  void ScalarLineThreshold::set_threshold_mode_to_greater_or_equal()
  { _thresholdMode = ThresholdMode::GreaterOrEqualThan; }

  void ScalarLineThreshold::set_threshold_mode_to_lesser()
  { _thresholdMode = ThresholdMode::LesserThan; }

  void ScalarLineThreshold::set_threshold_mode_to_lesser_or_equal()
  { _thresholdMode = ThresholdMode::LesserOrEqualThan; }

  void ScalarLineThreshold::set_threshold_mode_to_equal()
  { _thresholdMode = ThresholdMode::Equal; }
  /// @}

  /// @{ -------------------------------------------------- SET VALUE MODE
  void ScalarLineThreshold::set_value_mode_to_absolute()
  { _valueMode = ValueMode::AbsoluteValue; }

  void ScalarLineThreshold::set_value_mode_to_relative()
  { _valueMode = ValueMode::RelativeValue; }
  /// @}

  /// @{ -------------------------------------------------- SET ATTRIBUTE TYPE
  void ScalarLineThreshold::set_attribute_type_to_point_attribute()
  { _attributeType = AttributeType::PointAttribute; }

  void ScalarLineThreshold::set_attribute_type_to_cell_attribute()
  { _attributeType = AttributeType::CellAttribute; }

  void ScalarLineThreshold::set_attribute_type_to_object_attribute()
  { _attributeType = AttributeType::ObjectAttribute; }
  /// @}

  /// @{ -------------------------------------------------- SET THRESHOLD
  void ScalarLineThreshold::set_threshold(double t)
  { _threshold = t; }
  /// @}

  /// @{ -------------------------------------------------- SET ATTRIBUTE NAME
  void ScalarLineThreshold::set_attribute_name(const std::string& aname)
  { _attributeName = aname; }
  /// @}

  /// @{ -------------------------------------------------- SET ATTRIBUTE MIN/MAX
  void ScalarLineThreshold::set_attribute_min_max(double vmin, double vmax)
  {
      _attributeMin = std::min(vmin, vmax);
      _attributeMax = std::max(vmin, vmax);
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR =
  auto ScalarLineThreshold::operator=(const self_type&) -> self_type& = default;
  auto ScalarLineThreshold::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- EVAL
  bool ScalarLineThreshold::_eval(double x) const
  {
      double v = x;

      if (_valueMode == ValueMode::RelativeValue)
      { v = (v - _attributeMin) / (_attributeMax - _attributeMin); }

      switch (_thresholdMode)
      {
          case ThresholdMode::GreaterOrEqualThan: return v >= _threshold;
          case ThresholdMode::GreaterThan: return v > _threshold;
          case ThresholdMode::LesserOrEqualThan: return v <= _threshold;
          case ThresholdMode::LesserThan: return v < _threshold;
          case ThresholdMode::Equal: return v == _threshold;
          default: return false;
      }
  }
  /// @}
} // namespace bk


