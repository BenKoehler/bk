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

#ifndef BK_EXPLICITGEOMETRYFUNCTIONS_H
#define BK_EXPLICITGEOMETRYFUNCTIONS_H

#include <algorithm>
#include <any>
#include <string_view>
#include <vector>

#include <bkDataset/dataobject/GeometryFunctions.h>
#include <bkDataset/dataobject/filter/SmoothPointValuesFilter.h>
#include <bkDataset/attributes/attribute_info.h>

namespace bk::details
{
  template<typename TDerived> class GeometryFunctions<TDerived, true>
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
      constexpr GeometryFunctions(const self_type&) = default;
      constexpr GeometryFunctions(self_type&&) = default;
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

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE VECTOR
      template<typename T>
      [[nodiscard]] std::vector<T>& point_attribute_vector_of_type(unsigned long long attribute_hash)
      { return *std::any_cast<std::vector<T>>(&deriv()->point_attribute_map().attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] const std::vector<T>& point_attribute_vector_of_type(unsigned long long attribute_hash) const
      { return *std::any_cast<std::vector<T>>(&deriv()->point_attribute_map().attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] std::vector<T>& point_attribute_vector_of_type(std::string_view attribute_name)
      {
          unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_vector_of_type<T>(h);
      }

      template<typename T>
      [[nodiscard]] const std::vector<T>& point_attribute_vector_of_type(std::string_view attribute_name) const
      {
          unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] std::vector<attribute_info::type_of_t<TAttributeHash>>& point_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_vector_of_type<T>(TAttributeHash);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] const std::vector<attribute_info::type_of_t<TAttributeHash>>& point_attribute_vector() const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE VALUE
      template<typename T>
      [[nodiscard]] T& point_attribute_value_of_type(unsigned long long attribute_hash, unsigned int pointId)
      { return point_attribute_vector_of_type<T>(attribute_hash)[pointId]; }

      template<typename T>
      [[nodiscard]] const T& point_attribute_value_of_type(unsigned long long attribute_hash, unsigned int pointId) const
      { return point_attribute_vector_of_type<T>(attribute_hash)[pointId]; }

      template<typename T>
      [[nodiscard]] T& point_attribute_value_of_type(std::string_view attribute_name, unsigned int pointId)
      {
          unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_value_of_type<T>(h, pointId);
      }

      template<typename T>
      [[nodiscard]] const T& point_attribute_value_of_type(std::string_view attribute_name, unsigned int pointId) const
      {
          unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return point_attribute_value_of_type<T>(h, pointId);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] attribute_info::type_of_t<TAttributeHash>& point_attribute_value(unsigned int pointId)
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_value_of_type<T>(TAttributeHash, pointId);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] const attribute_info::type_of_t<TAttributeHash>& point_attribute_value(unsigned int pointId) const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return point_attribute_value_of_type<T>(TAttributeHash, pointId);
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- ADD POINT ATTRIBUTE VECTOR
      template<typename T>
      [[maybe_unused]] std::vector<T>& add_point_attribute_vector_of_type(unsigned long long attribute_hash)
      {
          const unsigned int nPoints = deriv()->geometry().num_points();
          std::vector<T> v(nPoints);
          std::any& a = deriv()->point_attribute_map().add_attribute(attribute_hash, std::any(std::move(v)));
          return *std::any_cast<std::vector<T>>(&a);
      }

      template<typename T>
      [[maybe_unused]] std::vector<T>& add_point_attribute_vector_of_type(std::string_view attribute_name)
      {
          unsigned long long h = deriv()->point_attribute_map().hash(attribute_name);
          return add_point_attribute_vector_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] std::vector<attribute_info::type_of_t<TAttributeHash>>& add_point_attribute_vector()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return add_point_attribute_vector_of_type<T>(TAttributeHash);
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- SMOOTHING (LAMBDA/MU ALGORITHM)
      //! @param lambda must be positive € [0,1]
      //! @param mu must be negative € [-1,lambda)
      [[maybe_unused]] bool smooth_points_laplace_lambda_mu(unsigned int num_iterations = 50, double lambda = 0.5, double mu = -0.5)
      {
          SmoothPointValuesFilter f;
          f.set_num_iterations(num_iterations);
          f.set_lambda(lambda);
          f.set_mu(mu);

          using point_type = typename derived_type::geometry_type::point_type;

          std::vector<point_type> copied_points(deriv()->geometry().begin(), deriv()->geometry().end());
          auto smoothed_points = f.apply(*deriv(), std::move(copied_points), point_type::Zero());

          if (smoothed_points.size() == deriv()->geometry().num_points())
          {
              std::copy(smoothed_points.begin(), smoothed_points.end(), deriv()->geometry().begin());
              return true;
          }
          else
          { return false; }
      }
      /// @}

      // TODO: decimation

  }; // class GeometryFunctions
} // namespace bk::details

#endif //BK_EXPLICITGEOMETRYFUNCTIONS_H


