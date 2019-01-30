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

#pragma once

#ifndef BK_GRIDGEOMETRYFUNCTIONS_H
#define BK_GRIDGEOMETRYFUNCTIONS_H

#include <any>
#include <string_view>

#include <bk/NDContainer>

#include <bkDataset/dataobject/GeometryFunctions.h>
#include <bkDataset/attributes/attribute_info.h>

namespace bk::details
{
  template<typename TDerived> class GeometryFunctions<TDerived, false>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GeometryFunctions<TDerived, true>;
      using derived_type = TDerived;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    protected:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      constexpr GeometryFunctions() = default;

      constexpr GeometryFunctions(const self_type&)
      { /* do nothing */ }

      constexpr GeometryFunctions(self_type&&)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~GeometryFunctions() = default;
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

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE ND VECTOR
      template<typename T>
      [[nodiscard]] NDVector<T>& point_attribute_vector_of_type(unsigned long long attribute_hash)
      {
          return *std::any_cast<NDVector<T>>(&deriv()->point_attribute_map().attribute(attribute_hash));
      }

      template<typename T>
      [[nodiscard]] const NDVector<T>& point_attribute_vector_of_type(unsigned long long attribute_hash) const
      {
          return *std::any_cast<NDVector<T>>(&deriv()->point_attribute_map().attribute(attribute_hash));
      }

      template<typename T>
      [[nodiscard]] NDVector<T>& point_attribute_vector_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_vector_of_type<T>(h);
      }

      template<typename T>
      [[nodiscard]] const NDVector<T>& point_attribute_vector_of_type(std::string_view attribute_name) const
      {
          const unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] NDVector<attribute_info::type_of_t<TAttributeHash>>& point_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_vector_of_type<T>(TAttributeHash);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] const NDVector<attribute_info::type_of_t<TAttributeHash>>& point_attribute_vector() const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE VALUE
      template<typename T, typename... TIds>
      [[nodiscard]] T& point_attribute_value_of_type(unsigned long long attribute_hash, TIds&& ... ids)
      { return point_attribute_vector_of_type<T>(attribute_hash)(std::forward<TIds>(ids)...); }

      template<typename T, typename... TIds>
      [[nodiscard]] const T& point_attribute_value_of_type(unsigned long long attribute_hash, TIds&& ... ids) const
      { return point_attribute_vector_of_type<T>(attribute_hash)(std::forward<TIds>(ids)...); }

      template<typename T, typename... TIds>
      [[nodiscard]] T& point_attribute_value_of_type(std::string_view attribute_name, TIds&& ... ids)
      {
          const unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_value_of_type<T>(h, std::forward<TIds>(ids)...);
      }

      template<typename T, typename... TIds>
      [[nodiscard]] const T& point_attribute_value_of_type(std::string_view attribute_name, TIds&& ... ids) const
      {
          const unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_value_of_type<T>(h, std::forward<TIds>(ids)...);
      }

      template<unsigned long long TAttributeHash, typename... TIds>
      [[nodiscard]] attribute_info::type_of_t<TAttributeHash>& point_attribute_value(TIds&& ... ids)
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_value_of_type<T>(TAttributeHash, std::forward<TIds>(ids)...);
      }

      template<unsigned long long TAttributeHash, typename... TIds>
      [[nodiscard]] const attribute_info::type_of_t<TAttributeHash>& point_attribute_value(TIds&& ... ids) const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_value_of_type<T>(TAttributeHash, std::forward<TIds>(ids)...);
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&)
      { return *this; }

      constexpr self_type& operator=(self_type&&) noexcept
      { return *this; }
      /// @}

      /// @{ -------------------------------------------------- ADD POINT ATTRIBUTE VECTOR
      template<typename T>
      [[maybe_unused]] NDVector<T>& add_point_attribute_vector_of_type(unsigned long long attribute_hash)
      {
          const auto  s = deriv()->geometry().size();
          NDVector<T> v(s.begin(), s.end());
          std::any& a = deriv()->point_attribute_map().add_attribute(attribute_hash, std::any(std::move(v)));
          return *std::any_cast<NDVector<T>>(&a);
      }

      template<typename T>
      [[maybe_unused]] NDVector<T>& add_point_attribute_vector_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return add_point_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] NDVector<attribute_info::type_of_t<TAttributeHash>>& add_point_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return add_point_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}
  }; // class GeometryFunctions
} // namespace bk::details

#endif //BK_GRIDGEOMETRYFUNCTIONS_H


