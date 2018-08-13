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

#ifndef BK_SCALARLINEFILTER_H
#define BK_SCALARLINEFILTER_H

#include <cstdint>
#include <vector>

#include "ScalarLineThreshold.h"
#include "Line.h"
#include "../lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT ScalarLineFilter
  {
      //====================================================================================================
      //===== ENUMS
      //====================================================================================================
    public:
      enum class ConcatenationMode : std::uint8_t
      {
          And = 0, Or = 1
      };

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
    private:
      using self_type = ScalarLineFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      ConcatenationMode _concatenationMode;
      std::vector<ScalarLineThreshold> _thresholds;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ScalarLineFilter();
      ScalarLineFilter(const self_type&);
      ScalarLineFilter(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ScalarLineFilter();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      /// @{ -------------------------------------------------- NUM THRESHOLDS
      [[nodiscard]] unsigned int num_thresholds() const;
      [[nodiscard]] bool has_thresholds() const;
      /// @}

      /// @{ -------------------------------------------------- GET CONCATENATION MODE
      [[nodiscard]] auto concatenation_mode() const -> const ConcatenationMode&;
      [[nodiscard]] bool concatenation_mode_is_and() const;
      [[nodiscard]] bool concatenation_mode_is_or() const;
      /// @}

      /// @{ -------------------------------------------------- GET THRESHOLD
      [[nodiscard]] ScalarLineThreshold* get_threshold(const std::string& attributeName);
      [[nodiscard]] ScalarLineThreshold* get_threshold(unsigned int id);
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET CONCATENATION MODE
      void set_concatenation_mode_and();
      void set_concatenation_mode_or();
      /// @}

      /// @{ -------------------------------------------------- ADD THRESHOLD
      [[maybe_unused]] ScalarLineThreshold& add_threshold(const std::string& attributeName = "");
      [[maybe_unused]] ScalarLineThreshold& add_threshold(const ScalarLineThreshold& t);
      [[maybe_unused]] ScalarLineThreshold& add_threshold(ScalarLineThreshold&& t);
      /// @}

      /// @{ -------------------------------------------------- REMOVE THRESHOLD
      void remove_threshold(const std::string& attributeName);
      void remove_threshold(unsigned int id);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear();
      /// @}

      /// @{ -------------------------------------------------- EVAL
      template<int Dims>
      [[nodiscard]] bool eval(const Line<Dims>& line, unsigned int id = 0) const
      {
          bool valid = _concatenationMode == ConcatenationMode::And ? true : false;

          for (const ScalarLineThreshold& t: _thresholds)
          {
              if (_concatenationMode == ConcatenationMode::And)
              { valid = valid && t.eval(line, id); }
              else if (_concatenationMode == ConcatenationMode::Or)
              { valid = valid || t.eval(line, id); }
          }

          return valid;
      }
      /// @}
  }; // class ScalarLineFilter
} // namespace bk

#endif //BK_SCALARLINEFILTER_H


