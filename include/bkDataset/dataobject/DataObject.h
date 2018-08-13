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

#ifndef BK_DATAOBJECT_H
#define BK_DATAOBJECT_H

#include <algorithm>
#include <any>
#include <string_view>
#include <utility>

#include "../attributes/AttributeMap.h"
#include "../attributes/attribute_info.h"
#include "GridGeometryFunctions.h"
#include "ExplicitGeometryFunctions.h"
#include "GridTopologyFunctions.h"
#include "ExplicitTopologyFunctions.h"
#include "filter/SmoothPointValuesFilter.h"

namespace bk
{
  template<typename TGeometry, typename TTopology> class DataObject
      : public details::GeometryFunctions<DataObject<TGeometry, TTopology>, TGeometry::IsExplicit()>,
        public details::TopologyFunctions<DataObject<TGeometry, TTopology>, TTopology::IsExplicit()>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = DataObject<TGeometry, TTopology>;
    public:
      using geometry_type = TGeometry;
      using topology_type = TTopology;
      using attribute_map_type = AttributeMap<std::any>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      geometry_type _geometry;
      topology_type _topology;
      attribute_map_type _point_attributes;
      attribute_map_type _cell_attributes;
      attribute_map_type _object_attributes;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      DataObject() = default;
      DataObject(const self_type&) = default;
      DataObject(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~DataObject() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET GEOMETRY
      [[nodiscard]] geometry_type& geometry()
      { return _geometry; }

      [[nodiscard]] const geometry_type& geometry() const
      { return _geometry; }
      /// @}

      /// @{ -------------------------------------------------- GET TOPOLOGY
      [[nodiscard]] topology_type& topology()
      { return _topology; }

      [[nodiscard]] const topology_type& topology() const
      { return _topology; }
      /// @}

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE MAP
      [[nodiscard]] attribute_map_type& point_attribute_map()
      { return _point_attributes; }

      [[nodiscard]] const attribute_map_type& point_attribute_map() const
      { return _point_attributes; }
      /// @}

      /// @{ -------------------------------------------------- GET POINT ATTRIBUTE
      [[nodiscard]] std::any& point_attribute(unsigned long long attribute_hash)
      { return _point_attributes.attribute(attribute_hash); }

      [[nodiscard]] const std::any& point_attribute(unsigned long long attribute_hash) const
      { return _point_attributes.attribute(attribute_hash); }
      /// @}

      /// @{ -------------------------------------------------- GET CELL ATTRIBUTE MAP
      [[nodiscard]] attribute_map_type& cell_attribute_map()
      { return _cell_attributes; }

      [[nodiscard]] const attribute_map_type& cell_attribute_map() const
      { return _cell_attributes; }
      /// @}

      /// @{ -------------------------------------------------- GET CELL ATTRIBUTE
      [[nodiscard]] std::any& cell_attribute(unsigned long long attribute_hash)
      { return _cell_attributes.attribute(attribute_hash); }

      [[nodiscard]] const std::any& cell_attribute(unsigned long long attribute_hash) const
      { return _cell_attributes.attribute(attribute_hash); }
      /// @}

      /// @{ -------------------------------------------------- GET OBJECT ATTRIBUTE MAP
      [[nodiscard]] attribute_map_type& object_attribute_map()
      { return _object_attributes; }

      [[nodiscard]] const attribute_map_type& object_attribute_map() const
      { return _object_attributes; }
      /// @}

      /// @{ -------------------------------------------------- GET OBJECT ATTRIBUTE
      //! from hash
      [[nodiscard]] std::any& object_attribute(unsigned long long attribute_hash)
      { return _object_attributes.attribute(attribute_hash); }

      [[nodiscard]] const std::any& object_attribute(unsigned long long attribute_hash) const
      { return _object_attributes.attribute(attribute_hash); }

      //! from name
      [[nodiscard]] std::any& object_attribute(std::string_view attribute_name)
      {
          const unsigned long long h = _object_attributes.hash(attribute_name);
          return _object_attributes.attribute(h);
      }

      [[nodiscard]] const std::any& object_attribute(std::string_view attribute_name) const
      {
          const unsigned long long h = _object_attributes.hash(attribute_name);
          return _object_attributes.attribute(h);
      }
      /// @}

      /// @{ -------------------------------------------------- GET OBJECT ATTRIBUTE VALUE
      template<typename T>
      [[nodiscard]] T& object_attribute_value_of_type(unsigned long long attribute_hash)
      { return *std::any_cast<T>(&_object_attributes.attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] const T& object_attribute_value_of_type(unsigned long long attribute_hash) const
      { return *std::any_cast<T>(&_object_attributes.attribute(attribute_hash)); }

      template<typename T>
      [[nodiscard]] T& object_attribute_value_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = _object_attributes.hash(attribute_name);
          return object_attribute_value_of_type<T>(h);
      }

      template<typename T>
      [[nodiscard]] const T& object_attribute_value_of_type(std::string_view attribute_name) const
      {
          const unsigned long long h = _object_attributes.hash(attribute_name);
          return object_attribute_value_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] attribute_info::type_of_t<TAttributeHash>& object_attribute_value()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return object_attribute_value_of_type<T>(TAttributeHash);
      }

      template<unsigned long long TAttributeHash>
      [[nodiscard]] const attribute_info::type_of_t<TAttributeHash>& object_attribute_value() const
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return object_attribute_value_of_type<T>(TAttributeHash);
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- ADD OBJECT ATTRIBUTE
      template<typename T>
      [[maybe_unused]] T& add_object_attribute_of_type(unsigned long long attribute_hash)
      { return *std::any_cast<T>(&_object_attributes.add_attribute(attribute_hash, std::any(T()))); }

      template<typename T>
      [[maybe_unused]] T& add_object_attribute_of_type(std::string_view attribute_name)
      {
          const unsigned long long h = _object_attributes.hash(attribute_name);
          return add_object_attribute_of_type<T>(h);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] attribute_info::type_of_t<TAttributeHash>& add_object_attribute()
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return add_object_attribute_of_type<T>(TAttributeHash);
      }
      /// @}

      /// @{ -------------------------------------------------- OBJECT ATTRIBUTE TO CELL ATTRIBUTE
      template<typename TObjectAttribute, typename TPointAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_point_attribute_of_type(unsigned long long object_attribute_hash, unsigned long long point_attribute_hash)
      {
          if (!_object_attributes.has_attribute(object_attribute_hash))
          { return false; }

          if (_geometry.num_points() == 0)
          { return false; }

          const TObjectAttribute& objAttrib = object_attribute_value_of_type<TObjectAttribute>(object_attribute_hash);
          auto& /*std::vector or NDVector*/ ptAttrib = this->template add_point_attribute_vector_of_type<TPointAttribute>(point_attribute_hash);

          for (TPointAttribute& pa: ptAttrib)
          { pa = objAttrib; }

          return true;
      }

      template<typename TAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_point_attribute_of_type(unsigned long long attribute_hash)
      { return object_attribute_of_type_to_point_attribute_of_type<TAttribute, TAttribute>(attribute_hash, attribute_hash); }

      template<typename TObjectAttribute, typename TPointAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_point_attribute_of_type(std::string_view object_attribute_name, std::string_view point_attribute_name)
      {
          const unsigned long long objh = _object_attributes.hash(object_attribute_name);
          const unsigned long long pth = _point_attributes.hash(point_attribute_name);
          return object_attribute_of_type_to_point_attribute_of_type<TObjectAttribute, TPointAttribute>(objh, pth);
      }

      template<typename TAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_point_attribute_of_type(std::string_view attribute_name)
      { return object_attribute_of_type_to_point_attribute_of_type<TAttribute, TAttribute>(attribute_name, attribute_name); }

      template<unsigned long long TObjectAttributeHash, unsigned long long TPointAttributeHash>
      [[maybe_unused]] bool object_attribute_to_point_attribute()
      {
          using object_attrib_type = attribute_info::type_of_t<TObjectAttributeHash>;
          using point_attrib_type = attribute_info::type_of_t<TPointAttributeHash>;
          return object_attribute_of_type_to_point_attribute_of_type<object_attrib_type, point_attrib_type>(TObjectAttributeHash, TPointAttributeHash);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] bool object_attribute_to_point_attribute()
      { return object_attribute_to_point_attribute<TAttributeHash, TAttributeHash>(); }
      /// @}

      /// @{ -------------------------------------------------- OBJECT ATTRIBUTE TO CELL ATTRIBUTE
      template<typename TObjectAttribute, typename TCellAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_cell_attribute_of_type(unsigned long long object_attribute_hash, unsigned long long cell_attribute_hash)
      {
          if (!_object_attributes.has_attribute(object_attribute_hash))
          { return false; }

          if (_topology.num_cells() == 0)
          { return false; }

          const TObjectAttribute& objAttrib = object_attribute_value_of_type<TObjectAttribute>(object_attribute_hash);
          auto& /*std::vector or NDVector*/ clAttrib = this->template add_cell_attribute_vector_of_type<TCellAttribute>(cell_attribute_hash);

          for (TCellAttribute& pa: clAttrib)
          { pa = objAttrib; }

          return true;
      }

      template<typename TAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_cell_attribute_of_type(unsigned long long attribute_hash)
      { return object_attribute_of_type_to_cell_attribute_of_type<TAttribute, TAttribute>(attribute_hash, attribute_hash); }

      template<typename TObjectAttribute, typename TCellAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_cell_attribute_of_type(std::string_view object_attribute_name, std::string_view cell_attribute_name)
      {
          const unsigned long long objh = _object_attributes.hash(object_attribute_name);
          const unsigned long long clh = _cell_attributes.hash(cell_attribute_name);
          return object_attribute_of_type_to_cell_attribute_of_type<TObjectAttribute, TCellAttribute>(objh, clh);
      }

      template<typename TAttribute>
      [[maybe_unused]] bool object_attribute_of_type_to_cell_attribute_of_type(std::string_view attribute_name)
      { return object_attribute_of_type_to_cell_attribute_of_type<TAttribute, TAttribute>(attribute_name, attribute_name); }

      template<unsigned long long TObjectAttributeHash, unsigned long long TCellAttributeHash>
      [[maybe_unused]] bool object_attribute_to_cell_attribute()
      {
          using object_attrib_type = attribute_info::type_of_t<TObjectAttributeHash>;
          using cell_attrib_type = attribute_info::type_of_t<TCellAttributeHash>;
          return object_attribute_of_type_to_cell_attribute_of_type<object_attrib_type, cell_attrib_type>(TObjectAttributeHash, TCellAttributeHash);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] bool object_attribute_to_cell_attribute()
      { return object_attribute_to_cell_attribute<TAttributeHash, TAttributeHash>(); }
      /// @}

      //====================================================================================================
      //===== ATTRIBUTE SMOOTHING
      //====================================================================================================
      /// @{ -------------------------------------------------- SMOOTH POINT ATTRIBUTE(LAMBDA/MU ALGORITHM)
      //! @param lambda must be positive € [0,1]
      //! @param mu must be negative € [-1,lambda)
      template<typename T>
      [[maybe_unused]] bool smooth_point_attribute_laplace_lambda_mu_of_type(unsigned long long attribute_hash, unsigned int num_iterations = 50, double lambda = 0.5, double mu = -0.5)
      {
          SmoothPointValuesFilter f;
          f.set_num_iterations(num_iterations);
          f.set_lambda(lambda);
          f.set_mu(mu);

          auto& attribute_values = this->template point_attribute_vector_of_type<T>(attribute_hash);
          std::vector<T> copied_attribute_values(attribute_values.begin(), attribute_values.end());
          auto smoothed_attribute_values = f.apply(*this, std::move(copied_attribute_values), T());

          if (smoothed_attribute_values.size() == _geometry.num_points())
          {
              std::copy(smoothed_attribute_values.begin(), smoothed_attribute_values.end(), attribute_values.begin());
              return true;
          }
          else
          { return false; }
      }

      template<typename T>
      [[maybe_unused]] bool smooth_point_attribute_laplace_lambda_mu_of_type(std::string_view attribute_name, unsigned int num_iterations = 50, double lambda = 0.5, double mu = -0.5)
      {
          const unsigned long long h = _point_attributes.hash(attribute_name);
          return smooth_point_attribute_laplace_lambda_mu_of_type<T>(h, num_iterations, lambda, mu);
      }

      template<unsigned long long TAttributeHash>
      [[maybe_unused]] bool smooth_point_attribute_laplace_lambda_mu(unsigned int num_iterations = 50, double lambda = 0.5, double mu = -0.5)
      {
          using T = attribute_info::type_of_t<TAttributeHash>;
          return smooth_point_attribute_laplace_lambda_mu_of_type<T>(TAttributeHash, num_iterations, lambda, mu);
      }
      /// @}
  }; // class DataObject
} // namespace bk

#endif //BK_DATAOBJECT_H


