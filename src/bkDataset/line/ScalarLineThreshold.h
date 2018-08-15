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

#ifndef BK_SCALARLINETHRESHOLD_H
#define BK_SCALARLINETHRESHOLD_H

#include <cstdint>
#include <string>

#include <bkDataset/line/Line.h>
#include <bkDataset/lib/bkDataset_export.h>

namespace bk
{
  class BKDATASET_EXPORT ScalarLineThreshold
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ScalarLineThreshold;

    public:

      //====================================================================================================
      //===== ENUMS
      //====================================================================================================
      enum class ThresholdMode : std::uint8_t
      {
          GreaterThan = 0, GreaterOrEqualThan = 1, LesserThan = 2, LesserOrEqualThan = 3, Equal = 4
      };
      enum class ValueMode : std::uint8_t
      {
          AbsoluteValue = 0, RelativeValue = 1
      };
      enum class AttributeType : std::uint8_t
      {
          PointAttribute = 0, CellAttribute = 1, ObjectAttribute = 2
      };

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      ThresholdMode _thresholdMode;
      ValueMode _valueMode;
      AttributeType _attributeType;
      double _threshold;
      std::string _attributeName;
      double _attributeMin;
      double _attributeMax;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ScalarLineThreshold();
      ScalarLineThreshold(const self_type&);
      ScalarLineThreshold(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ScalarLineThreshold();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET THRESHOLD MODE
      [[nodiscard]] auto threshold_mode() const -> const ThresholdMode&;
      [[nodiscard]] bool threshold_mode_is_greater() const;
      [[nodiscard]] bool threshold_mode_is_greater_or_equal() const;
      [[nodiscard]] bool threshold_mode_is_lesser() const;
      [[nodiscard]] bool threshold_mode_is_lesser_or_equal() const;
      [[nodiscard]] bool threshold_mode_is_equal() const;
      /// @}

      /// @{ -------------------------------------------------- GET VALUE MODE
      [[nodiscard]] auto value_mode() const -> const ValueMode&;
      [[nodiscard]] bool value_mode_is_absolute() const;
      [[nodiscard]] bool value_mode_is_relative() const;
      /// @}

      /// @{ -------------------------------------------------- GET ATTRIBUTE TYPE
      [[nodiscard]] auto attribute_type() const -> const AttributeType&;
      [[nodiscard]] bool attribute_type_is_point_attribute() const;
      [[nodiscard]] bool attribute_type_is_cell_attribute() const;
      [[nodiscard]] bool attribute_type_is_object_attribute() const;
      /// @}

      /// @{ -------------------------------------------------- GET THRESHOLD
      [[nodiscard]] double threshold() const;
      /// @}

      /// @{ -------------------------------------------------- GET ATTRIBUTE NAME
      [[nodiscard]] const std::string& attribute_name() const;
      /// @}

      /// @{ -------------------------------------------------- GET ATTRIBUTE MIN/MAX
      [[nodiscard]] double attribute_min() const;
      [[nodiscard]] double attribute_max() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET THRESHOLD MODE
      void set_threshold_mode_to_greater();
      void set_threshold_mode_to_greater_or_equal();
      void set_threshold_mode_to_lesser();
      void set_threshold_mode_to_lesser_or_equal();
      void set_threshold_mode_to_equal();
      /// @}

      /// @{ -------------------------------------------------- SET VALUE MODE
      void set_value_mode_to_absolute();
      void set_value_mode_to_relative();
      /// @}

      /// @{ -------------------------------------------------- SET ATTRIBUTE TYPE
      void set_attribute_type_to_point_attribute();
      void set_attribute_type_to_cell_attribute();
      void set_attribute_type_to_object_attribute();
      /// @}

      /// @{ -------------------------------------------------- SET THRESHOLD
      void set_threshold(double t);
      /// @}

      /// @{ -------------------------------------------------- SET ATTRIBUTE NAME
      void set_attribute_name(const std::string& aname);
      /// @}

      /// @{ -------------------------------------------------- SET ATTRIBUTE MIN/MAX
      void set_attribute_min_max(double vmin, double vmax);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- EVAL
    private:
      [[nodiscard]] bool _eval(double x) const;
    public:

      template<int Dims>
      [[nodiscard]] bool eval(const Line<Dims>& line, unsigned int id = 0) const
      {
          switch (_attributeType)
          {
              case AttributeType::PointAttribute: return _eval(line.template point_attribute_value_of_type<double>(_attributeName, id));
              case AttributeType::CellAttribute: return _eval(line.template cell_attribute_value_of_type<double>(_attributeName, id));
              case AttributeType::ObjectAttribute: return _eval(line.template object_attribute_value_of_type<double>(_attributeName));
              default: return false;
          }
      }
      /// @}

  }; // class ScalarLineThreshold
} // namespace bk

#endif //BK_SCALARLINETHRESHOLD_H


