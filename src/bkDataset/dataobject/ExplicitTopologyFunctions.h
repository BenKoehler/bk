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

#ifndef BK_EXPLICITTOPOLOGYFUNCTIONS_H
#define BK_EXPLICITTOPOLOGYFUNCTIONS_H

#include <any>
#include <string_view>
#include <vector>

#include <bkDataset/dataobject/TopologyFunctions.h>
#include <bkDataset/attributes/attribute_info.h>

namespace bk::details
{
  template<typename TDerived> class TopologyFunctions<TDerived, true>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TopologyFunctions<TDerived, true>;
      using derived_type = TDerived;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      constexpr TopologyFunctions() = default;
      constexpr TopologyFunctions(const self_type&) = default;
      constexpr TopologyFunctions(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~TopologyFunctions() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET CELL ATTRIBUTE VECTOR
      template<typename T>
      [[nodiscard]] std::vector<T>& cell_attribute_vector_of_type(unsigned long long attribute_hash)
      { return *std::any_cast<std::vector<T>>(&deriv()->cell_attribute_map().attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] const std::vector<T>& cell_attribute_vector_of_type(unsigned long long attribute_hash) const
      { return *std::any_cast<std::vector<T>>(&deriv()->cell_attribute_map().attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] std::vector<T>& cell_attribute_vector_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = deriv()->cell_attribute_map().hash(attribute_name);
          return cell_attribute_vector_of_type<T>(h);
      }

      template<typename T>
      [[nodiscard]] const std::vector<T>& cell_attribute_vector_of_type(std::string_view attribute_name) const
      {
          const unsigned long long h = deriv()->cell_attribute_map().hash(attribute_name);
          return cell_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash> [[nodiscard]] std::vector<attribute_info::type_of_t<TAttributeHash>>& cell_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return cell_attribute_vector_of_type<T>(TAttributeHash);
      }

      template<unsigned long long TAttributeHash> [[nodiscard]] const std::vector<attribute_info::type_of_t<TAttributeHash>>& cell_attribute_vector() const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return cell_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}

      /// @{ -------------------------------------------------- GET CELL ATTRIBUTE VALUE
      template<typename T>
      [[nodiscard]] T& cell_attribute_value_of_type(unsigned long long attribute_hash, unsigned int cellId)
      { return cell_attribute_vector_of_type<T>(attribute_hash)[cellId]; }

      template<typename T>
      [[nodiscard]] const T& cell_attribute_value_of_type(unsigned long long attribute_hash, unsigned int cellId) const
      { return cell_attribute_vector_of_type<T>(attribute_hash)[cellId]; }

      template<typename T>
      [[nodiscard]] T& cell_attribute_value_of_type(std::string_view attribute_name, unsigned int cellId)
      {
          const unsigned long long h = deriv()->cell_attribute_map().hash(attribute_name);
          return cell_attribute_value_of_type<T>(h, cellId);
      }

      template<typename T>
      [[nodiscard]] const T& cell_attribute_value_of_type(std::string_view attribute_name, unsigned int cellId) const
      {
          const unsigned long long h = deriv()->cell_attribute_map().hash(attribute_name);
          return cell_attribute_value_of_type<T>(h, cellId);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] attribute_info::type_of_t<TAttributeHash>& cell_attribute_value(unsigned int cellId)
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return cell_attribute_value_of_type<T>(TAttributeHash, cellId);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] const attribute_info::type_of_t<TAttributeHash>& cell_attribute_value(unsigned int cellId) const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return cell_attribute_value_of_type<T>(TAttributeHash, cellId);
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- ADD CELL ATTRIBUTE VECTOR
      template<typename T>
      [[maybe_unused]] std::vector<T>& add_cell_attribute_vector_of_type(unsigned long long attribute_hash)
      {
          const unsigned int nCells = deriv()->topology().num_cells();
          std::vector<T> v(nCells);
          std::any& a = deriv()->cell_attribute_map().add_attribute(attribute_hash, std::any(std::move(v)));
          return *std::any_cast<std::vector<T>>(&a);
      }

      template<typename T>
      [[maybe_unused]] std::vector<T>& add_cell_attribute_vector_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = deriv()->cell_attribute_map().hash(attribute_name);
          return add_cell_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash> [[maybe_unused]] std::vector<attribute_info::type_of_t<TAttributeHash>>& add_cell_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return add_cell_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}

  }; // class TopologyFunctions
} // namespace bk::details

#endif //BK_EXPLICITTOPOLOGYFUNCTIONS_H
