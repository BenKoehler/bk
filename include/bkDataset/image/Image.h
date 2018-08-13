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

#ifndef BK_IMAGE_H
#define BK_IMAGE_H

#include <algorithm>
#include <cassert>
#include <functional>
#include <limits>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string_view>
#include <utility>

#include "../../bkMath/Matrix"
#include "../../bkMath/functions/list_grid_id_conversion.h"
#include "../../bkTypeTraits/complex_traits.h"
#include "../../bkTypeTraits/has_index_operator.h"
#include "../../bkTypeTraits/variadic_template_helpers.h"
#include "../../bkTools/NDContainer"
#include "../../bkTools/StringUtils"

#include "../dataobject/DataObject.h"
#include "../geometry/GridGeometry.h"
#include "../geometry/TransformableGeometry.h"
#include "../topology/GridTopology.h"
#include "../transformation/ScaleTransformation.h"
#include "../transformation/WorldMatrixTransformation.h"
#include "../transformation/NoTransformation.h"
#include "../transformation/DicomTransformation.h"
#include "filter/ConvolutionImageFilter.h"
#include "interpolation/NearestNeighborImageInterpolation.h"
#include "interpolation/LinearImageInterpolation.h"

#ifdef BK_LIB_PNG_AVAILABLE

    #include <png.h>

#endif

namespace bk
{
  template<typename TValue, int TDims, typename TTransformation = NoTransformation<TDims>>
  class Image : public DataObject<TransformableGeometry<GridGeometry<TDims>, TTransformation>, GridTopology<TDims>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || TDims > 0);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Image<TValue, TDims, TTransformation>;
      using base_type = DataObject<TransformableGeometry<GridGeometry<TDims>, TTransformation>, GridTopology<TDims>>;
    public:
      using value_type = TValue;
      template<typename TValue_ = TValue, int TDims_ = TDims> using self_template_type = Image<TValue_, TDims_>;

      /// @{ -------------------------------------------------- GET COMPILE-TIME PARAMETERS
      [[nodiscard]] static constexpr unsigned int NumDimensionsAtCompileTime()
      { return static_cast<unsigned int>(std::max(TDims, 0)); }

      [[nodiscard]] static constexpr const char* DefaultAttributeName()
      { return "default_image_value"; }

      [[nodiscard]] static constexpr unsigned long long DefaultAttributeHash()
      { return bk::string_utils::hash(DefaultAttributeName()); }
      /// @}

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCTORS
      Image() = default;
      Image(const self_type&) = default;
      Image(self_type&&) = default;

      template<typename TValue_, int TDims_, typename TTransformation_>
      Image(const Image<TValue_, TDims_, TTransformation_>& other)
      { *this = other; }
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~Image() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- HELPER: GET VALUE ND VECTOR
      [[nodiscard]] NDVector<value_type>& _value_vector()
      { return this->template point_attribute_vector_of_type<value_type>(DefaultAttributeHash()); }

      [[nodiscard]] const NDVector<value_type>& _value_vector() const
      { return this->template point_attribute_vector_of_type<value_type>(DefaultAttributeHash()); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: VALID NUMBER OF ARGUMENTS
      template<typename... T>
      [[nodiscard]] static constexpr bool _valid_num_arguments(const T& ...)
      {
          constexpr unsigned int N = sizeof...(T);
          return TDims == -1 || N == TDims || (N == 1 && bk::has_index_operator_v<bk::template_parameter_pack_arg_t<0, T...>>);
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: HAS DEFAULT VALUE ATTRIBUTE
      [[nodiscard]] bool _has_default_value_attribute() const
      { return this->point_attribute_map().has_attribute(DefaultAttributeHash()); }
      /// @}
    public:

      /// @{ -------------------------------------------------- GET DATA VECTOR
      [[nodiscard]] NDVector<value_type>& data()
      { return this->template point_attribute_vector_of_type<value_type>(DefaultAttributeHash()); }

      [[nodiscard]] const NDVector<value_type>& data() const
      { return this->template point_attribute_vector_of_type<value_type>(DefaultAttributeHash()); }
      /// @}

      /// @{ -------------------------------------------------- IS VALID GRID POS
      template<typename TIndexAccessible>
      [[nodiscard]] bool is_valid_grid_pos(const TIndexAccessible& gp) const
      {
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          {
              if (static_cast<int>(gp[i]) < 0 || static_cast<int>(gp[i]) >= static_cast<int>(size(i)))
              { return false; }
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] unsigned int num_dimensions() const
      { return this->geometry().num_dimensions(); }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] auto size() const
      { return this->geometry().size(); }

      [[nodiscard]] unsigned int size(unsigned int dimId) const
      { return this->geometry().size(dimId); }
      /// @}

      /// @{ -------------------------------------------------- GET NUM VALUES
      [[nodiscard]] unsigned int num_values() const
      { // the total number of pixels
          return this->geometry().num_points();
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      [[nodiscard]] value_type& operator[](unsigned int id)
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector()[id];
      }

      [[nodiscard]] const value_type& operator[](unsigned int id) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector()[id];
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR()
      template<typename... TIds>
      [[nodiscard]] value_type& operator()(const TIds& ... ids)
      {
          static_assert(_valid_num_arguments(ids...));
          assert(_has_default_value_attribute() && "call set_size() first");

          return this->template point_attribute_value_of_type<value_type>(DefaultAttributeHash(), ids...);
      }

      template<typename... TIds>
      [[nodiscard]] const value_type& operator()(const TIds& ... ids) const
      {
          static_assert(_valid_num_arguments(ids...));
          assert(_has_default_value_attribute() && "call set_size() first");

          return this->template point_attribute_value_of_type<value_type>(DefaultAttributeHash(), ids...);
      }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] auto begin()
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().begin();
      }

      [[nodiscard]] auto begin() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().begin();
      }

      [[nodiscard]] auto cbegin() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().cbegin();
      }

      [[nodiscard]] auto end()
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().end();
      }

      [[nodiscard]] auto end() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().end();
      }

      [[nodiscard]] auto cend() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().cend();
      }

      [[nodiscard]] auto rbegin()
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().rbegin();
      }

      [[nodiscard]] auto rbegin() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().rbegin();
      }

      [[nodiscard]] auto crbegin() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().crbegin();
      }

      [[nodiscard]] auto rend()
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().rend();
      }

      [[nodiscard]] auto rend() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().rend();
      }

      [[nodiscard]] auto crend() const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return _value_vector().crend();
      }
      /// @}

      /// @{ -------------------------------------------------- GET SUB IMAGE
      // todo: extract sub image (block) between 2 given positions
      /// @}

      /// @{ -------------------------------------------------- GET MIN VALUE
      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] auto min_element_iterator(TCompareLess comp = TCompareLess())
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return std::min_element(begin(), end(), comp);
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] value_type min_value(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          if (num_values() == 0)
          { return value_type(); }
          else
          {
              value_type x = (*this)[0];

              for (unsigned int i = 1; i < num_values(); ++i)
              { x = std::min(x, (*this)[i], comp); }

              return x;
          }
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] std::pair<value_type, unsigned int> min_value_with_id(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          if (num_values() == 0)
          { return value_type(); }
          else
          {
              value_type x = (*this)[0];
              unsigned int listId = 0;

              for (unsigned int i = 1; i < num_values(); ++i)
              {
                  if ((*this)[i] < x)
                  {
                      x = (*this)[i];
                      listId = i;
                  }
              }

              return std::pair<value_type, unsigned int>(x, listId);
          }
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] auto min_value_with_pos(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          auto[minVal, listId] = min_value_with_id(comp);
          return std::make_pair(minVal, bk::list_to_grid_id(size(), listId));
      }
      /// @}

      /// @{ -------------------------------------------------- GET MIN VALUE CWISE
      [[nodiscard]] value_type min_value_cwise() const
      {
          if constexpr (bk::is_matrix_v<value_type>)
          {
              using T = typename value_type::value_type;

              value_type m = value_type::Constant(std::numeric_limits<T>::max());

              for (unsigned int i = 0; i < num_values(); ++i)
              {
                  for (unsigned int k = 0; k < m.num_elements(); ++k)
                  { m[k] = std::min(m[k], operator[](i)[k]); }
              }

              return m;
          }
          else
          { return min_value(); }
      }
      /// @}

      /// @{ -------------------------------------------------- GET MAX VALUE
      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] auto max_element_iterator(TCompareLess comp = TCompareLess())
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return std::max_element(begin(), end(), comp);
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] value_type max_value(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          if (num_values() == 0)
          { return value_type(); }
          else
          {
              value_type x = (*this)[0];

              for (unsigned int i = 1; i < num_values(); ++i)
              { x = std::max(x, (*this)[i], comp); }

              return x;
          }
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] std::pair<value_type, unsigned int> max_value_with_id(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          if (num_values() == 0)
          { return value_type(); }
          else
          {
              value_type x = (*this)[0];
              unsigned int listId = 0;

              for (unsigned int i = 1; i < num_values(); ++i)
              {
                  if ((*this)[i] < x)
                  {
                      x = (*this)[i];
                      listId = i;
                  }
              }

              return std::pair<value_type, unsigned int>(x, listId);
          }
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] auto max_value_with_pos(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");

          auto[maxVal, listId] = max_value_with_id(comp);
          return std::make_pair(maxVal, bk::list_to_grid_id(size(), listId));
      }
      /// @}

      /// @{ -------------------------------------------------- GET MAX VALUE CWISE
      [[nodiscard]] value_type max_value_cwise() const
      {
          if constexpr (bk::is_matrix_v<value_type>)
          {
              using T = typename value_type::value_type;

              value_type m = value_type::Constant(std::numeric_limits<T>::lowest());

              for (unsigned int i = 0; i < num_values(); ++i)
              {
                  for (unsigned int k = 0; k < m.num_elements(); ++k)
                  { m[k] = std::max(m[k], operator[](i)[k]); }
              }

              return m;
          }
          else
          { return max_value(); }
      }
      /// @}

      /// @{ -------------------------------------------------- GET MIN/MAX VALUE
      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] auto minmax_element_iterator(TCompareLess comp = TCompareLess())
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return {min_element_iterator(comp), max_element_iterator(comp)};
      }

      template<typename TCompareLess = std::less<value_type>>
      [[nodiscard]] std::pair<value_type, value_type> minmax_value(TCompareLess comp = TCompareLess()) const
      {
          assert(_has_default_value_attribute() && "call set_size() first");
          return {min_value(comp), max_value(comp)};
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: GET NEIGHBORHOOD VALUES
    private:
      template<typename TIndexAccessible, typename TIndexAccessible2, typename TIndexAccessible3>
      void _values_of_neighborhood(unsigned int dimId, const TIndexAccessible& gid, TIndexAccessible2& off, TIndexAccessible3& neighborhood_size, std::vector<value_type>& values) const
      {
          const int halfsize = neighborhood_size[dimId] >> 1; // integer division

          if (dimId < num_dimensions() - 1)
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;
                  _values_of_neighborhood(dimId + 1, gid, off, neighborhood_size, values);
              } // for i
          }
          else
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;

                  auto gidoff = MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);
                  bool valid = true;

                  for (unsigned int d = 0; d < num_dimensions(); ++d)
                  {
                      gidoff[d] = gid[d] + off[d];

                      if (static_cast<int>(gidoff[d]) < 0 || static_cast<int>(gidoff[d]) >= static_cast<int>(size(dimId)))
                      {
                          valid = false;
                          break;
                      }
                  }

                  if (valid)
                  { values.push_back(operator()(gidoff)); }
              } // for i
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET NEIGHBORHOOD VALUES
      template<typename TIndexAccessible, typename TIndexAccessible2, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible> && bk::has_index_operator_v<TIndexAccessible2>>* = nullptr>
      [[nodiscard]] std::vector<value_type> values_of_neighborhood(const TIndexAccessible& gid, const TIndexAccessible2& neighborhood_size) const
      {
          auto gidoff = MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);

          std::vector<value_type> neighbor_values;
          const unsigned int numValuesInNeighborhood = std::accumulate(neighborhood_size.begin(), neighborhood_size.end(), 1, [](unsigned int x, unsigned int y)
          { return x * y; });

          if (numValuesInNeighborhood > 0)
          {
              neighbor_values.reserve(numValuesInNeighborhood);
              _values_of_neighborhood(0, gid, gidoff, neighborhood_size, neighbor_values);
          }

          return neighbor_values;
      }

      template<typename TIndexAccessible, std::enable_if_t<bk::has_index_operator_v<TIndexAccessible>>* = nullptr>
      [[nodiscard]] std::vector<value_type> values_of_neighborhood(unsigned int listId, const TIndexAccessible& neighborhood_size) const
      { return values_of_neighborhood(bk::list_to_grid_id(size(), listId), neighborhood_size); }

      [[nodiscard]] std::vector<value_type> values_of_neighborhood(unsigned int listId, unsigned int neighborhood_size) const
      { return values_of_neighborhood(bk::list_to_grid_id(size(), listId), MatrixFactory::Constant<unsigned int, TDims, 1>(neighborhood_size, num_dimensions(), 1)); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;

      template<typename TValue_, int TDims_, typename TTransformation_>
      [[maybe_unused]] self_type& operator=(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(NumDimensionsAtCompileTime() == TDims_ || NumDimensionsAtCompileTime() == -1 || TDims_ == -1, "dimension mismatch");

          set_size(other.size());
          std::copy(other.cbegin(), other.cend(), begin());

          return *this;
      }
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      template<typename... TIds>
      void set_size(const TIds& ... ids)
      {
          static_assert(_valid_num_arguments(ids...));

          this->geometry().set_size(ids...);
          this->topology().set_size(ids...);
          this->template add_point_attribute_vector_of_type<value_type>(DefaultAttributeHash());
      }
      /// @}

      /// @{ -------------------------------------------------- SET CONSTANT
      //! sets each element to a given value
      void set_constant(const value_type& x)
      { _value_vector().fill(x); }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- ALLOCATE VALUE
      template<typename T = value_type>
      [[nodiscard]] auto allocate_value() const
      {
          if constexpr(bk::is_matrix_v<value_type>)
          {
              typename value_type::template self_template_type<T> res;

              if constexpr(bk::is_dynamic_matrix_v<value_type>)
              { res.set_size(operator[](0).num_rows(), operator[](0).num_cols()); }

              return res;
          }
          else if constexpr (bk::is_complex_v<value_type>)
          { return std::complex<T>(); }
          else if constexpr (std::is_scalar_v<T>)
          { return static_cast<T>(0); }
          else
          { return T(); }
      }
      /// @}

      /// @{ -------------------------------------------------- SWAP
      template<typename TValue_, int TDims_, typename TTransformation_>
      void swap(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(NumDimensionsAtCompileTime() == TDims_ || NumDimensionsAtCompileTime() == -1 || TDims_ == -1, "dimension mismatch");

          self_type temp(*this);
          *this = other;
          other = std::move(temp);
      }

      void swap(self_type&& other)
      { *this = std::move(other); }
      /// @}

      /// @{ -------------------------------------------------- CLAMP TO SIZE
      template<typename T>
      [[nodiscard]] T clamp_to_size(T&& x) const
      {
          T res(std::forward<T>(x));

          for (unsigned int dimId = 0; dimId < num_dimensions(); ++dimId)
          { res[dimId] = clamp_to_size_of_dim(res[dimId], dimId); }

          return res;
      }

      template<typename T>
      [[nodiscard]] T clamp_to_size_of_dim(T i, unsigned int dimId) const
      {
          static_assert(std::is_scalar_v<T>, "parameter must have scalar type");
          assert(dimId < num_dimensions() && "invalid dimId");

          return std::clamp(i, static_cast<T>(0), static_cast<T>(this->size(dimId) - 1));
      }
      /// @}

      /// @{ -------------------------------------------------- INTERPOLATE GRID POS
      template<typename TIndexAccessible, typename TInterpolator = LinearImageInterpolation>
      [[nodiscard]] auto interpolate_at_grid_pos(TIndexAccessible&& grid_pos, TInterpolator interp = TInterpolator()) const
      { return interp(*this, std::forward<TIndexAccessible>(grid_pos)); }

      template<typename T, typename TInterpolator = LinearImageInterpolation>
      [[nodiscard]] auto interpolate_at_grid_pos(std::initializer_list<T> grid_pos, TInterpolator interp = TInterpolator()) const
      { return interp(*this, std::vector<T>(grid_pos.begin(), grid_pos.end())); }
      /// @}

      /// @{ -------------------------------------------------- INTERPOLATE WORLD POS
      template<typename TIndexAccessible, typename TInterpolator = LinearImageInterpolation>
      [[nodiscard]] auto interpolate(TIndexAccessible&& world_pos, TInterpolator interp = TInterpolator()) const
      { return interpolate_at_grid_pos(this->geometry().transformation().to_object_coordinates(std::forward<TIndexAccessible>(world_pos)), interp); }

      template<typename T, typename TInterpolator = LinearImageInterpolation>
      [[nodiscard]] auto interpolate(std::initializer_list<T> world_pos, TInterpolator interp = TInterpolator()) const
      { return interpolate_at_grid_pos(this->geometry().transformation().to_object_coordinates(world_pos), interp); }
      /// @}

      /// @{ -------------------------------------------------- JACOBIAN
      template<typename TIndexAccessible>
      [[nodiscard]] MatXd jacobian_at_grid_pos(const TIndexAccessible& grid_pos) const
      {
          auto temp0 = MatrixFactory::create<double, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);
          auto temp1 = MatrixFactory::create<double, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);

          std::vector<double> dimscale(num_dimensions());

          for (unsigned int dimId = 0; dimId < num_dimensions(); ++dimId)
          {
              temp0.set_zero();
              temp0 = this->geometry().transformation().to_world_coordinates(temp0);

              temp1.set_zero();
              temp1[dimId] = 1;
              temp1 = this->geometry().transformation().to_world_coordinates(temp1);

              dimscale[dimId] = temp0.distance(temp1);
          }

          MatXd J;

          if constexpr (bk::is_matrix_v<value_type>)
          {
              const value_type& v = operator[](0);
              J.set_size(std::max(v.num_rows(), v.num_cols()), v.num_elements());
          }
          else
          { J.set_size(num_dimensions(), 1); }

          // helper function
          constexpr auto assign_to_jacobian = [](MatXd& j, unsigned int d, auto&& x)
          {
              if constexpr (bk::is_matrix_v<value_type>)
              { j.col_ref(d) = x; }
              else
              { j[d] = x; }
          };

          for (unsigned int dimId = 0; dimId < num_dimensions(); ++dimId)
          {
              temp0 = grid_pos;
              temp1 = grid_pos;

              if (static_cast<double>(grid_pos[dimId]) >= dimscale[dimId] && grid_pos[dimId] <= static_cast<double>(this->geometry().size(dimId) - 1) - dimscale[dimId])
              { // central difference
                  temp0[dimId] -= dimscale[dimId];
                  temp1[dimId] += dimscale[dimId];
                  assign_to_jacobian(J, dimId, (interpolate_at_grid_pos(temp1) - interpolate_at_grid_pos(temp0)) / (2 * dimscale[dimId]));
              }
              else if (grid_pos[dimId] < dimscale[dimId])
              { // forward difference
                  temp1[dimId] += dimscale[dimId];
                  assign_to_jacobian(J, dimId, (interpolate_at_grid_pos(temp1) - interpolate_at_grid_pos(temp0)) / dimscale[dimId]);
              }
              else
              { // backward difference
                  temp0[dimId] -= dimscale[dimId];
                  assign_to_jacobian(J, dimId, (interpolate_at_grid_pos(temp1) - interpolate_at_grid_pos(temp0)) / dimscale[dimId]);
              }
          }

          return J;
      }

      template<typename TIndexAccessible>
      [[nodiscard]] auto jacobian(TIndexAccessible&& world_pos) const
      { return jacobian_at_grid_pos(this->geometry().transformation().to_object_coordinates(std::forward<TIndexAccessible>(world_pos))); }

      template<typename T>
      [[nodiscard]] auto jacobian(std::initializer_list<T> world_pos) const
      { return jacobian_at_grid_pos(this->geometry().transformation().to_object_coordinates(world_pos)); }
      /// @}

      /// @{ -------------------------------------------------- GRADIENT
      //! alias for jacobian
      template<typename TIndexAccessible>
      [[nodiscard]] MatXd gradient_at_grid_pos(const TIndexAccessible& grid_pos) const
      { return jacobian_at_grid_pos(grid_pos); }

      template<typename TIndexAccessible>
      [[nodiscard]] auto gradient(TIndexAccessible&& world_pos) const
      { return jacobian(std::forward<TIndexAccessible>(world_pos)); }

      template<typename T>
      [[nodiscard]] auto gradient(std::initializer_list<T> world_pos) const
      { return jacobian(world_pos); }
      /// @}

      /// @{ -------------------------------------------------- GRADIENT STRENGTH
      //! length of the gradient vector (norm of jacobian matrix)
      template<typename TIndexAccessible>
      [[nodiscard]] double gradient_strength_at_grid_pos(const TIndexAccessible& grid_pos) const
      { return jacobian_at_grid_pos(grid_pos).norm(); }

      template<typename TIndexAccessible>
      [[nodiscard]] double gradient_strength(TIndexAccessible&& world_pos) const
      { return gradient_strength_at_grid_pos(this->geometry().transformation().to_object_coordinates(std::forward<TIndexAccessible>(world_pos))); }

      template<typename T>
      [[nodiscard]] auto gradient_strength(std::initializer_list<T> world_pos) const
      { return gradient_strength_at_grid_pos(this->geometry().transformation().to_object_coordinates(world_pos)); }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: CONVOLUTION
    private:
      template<typename TKernel, typename TIndexAccessible, typename TIndexAccessible2, typename TIndexAccessible3, typename T>
      void _apply_convolution_kernel(unsigned int dimId, const TKernel& kernel, const TIndexAccessible& gid, TIndexAccessible2& off, TIndexAccessible3& kernel_gid, T& newval) const
      {
          const int halfsize = kernel.size(dimId) >> 1; // integer division

          if (dimId < num_dimensions() - 1)
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;
                  kernel_gid[dimId] = i + halfsize;
                  _apply_convolution_kernel(dimId + 1, kernel, gid, off, kernel_gid, newval);
              } // for i
          }
          else
          {
              for (int i = -halfsize; i <= halfsize; ++i)
              {
                  off[dimId] = i;
                  kernel_gid[dimId] = i + halfsize;

                  auto gidoff = MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);

                  for (unsigned int d = 0; d < num_dimensions(); ++d)
                  { gidoff[d] = gid[d] + off[d]; }

                  gidoff = clamp_to_size(std::move(gidoff));

                  newval += operator()(gidoff) * kernel(kernel_gid);
              } // for i
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- CONVOLUTION
      //! apply convolution kernel to a single position 
      template<typename TKernel, typename TIndexAccessible>
      [[nodiscard]] auto apply_convolution_kernel(const TKernel& kernel, const TIndexAccessible& gid) const
      {
          auto gidoff = MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);
          auto kernel_gid = MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1);
          auto res = allocate_value<double>();

          _apply_convolution_kernel(0, kernel, gid, gidoff, kernel_gid, res);

          return res;
      }
      /// @}

      //====================================================================================================
      //===== FILTERS
      //====================================================================================================
      /// @{ -------------------------------------------------- FILTER: CONVOLUTION
      template<typename TKernel>
      [[nodiscard]] self_type filter_convolution(const TKernel& kernel) const
      {
          ConvolutionImageFilter f;
          return f.apply(*this, kernel);
      }
      /// @}

      /// @{ -------------------------------------------------- APPLY FILTER
      template<typename TFilter>
      [[nodiscard]] auto filter(const TFilter& f) const
      { return f.apply(*this); }

      template<typename TFilter>
      [[nodiscard]] auto filter(TFilter& f) const
      { return f.apply(*this); }
      /// @}

      //====================================================================================================
      //===== MATH OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR +=
      template<typename TValue_, int TDims_, typename TTransformation_>
      void operator+=(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(TDims == TDims_ || TDims == -1 || TDims_ == -1, "dimension mismatch");
          assert(size() == other.size() && "size mismatch");

          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) += other[i]; }
      }

      void operator+=(const value_type& x)
      {
          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) += x; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR -=
      template<typename TValue_, int TDims_, typename TTransformation_>
      void operator-=(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(TDims == TDims_ || TDims == -1 || TDims_ == -1, "dimension mismatch");
          assert(size() == other.size() && "size mismatch");

          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) -= other[i]; }
      }

      void operator-=(const value_type& x)
      {
          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) -= x; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR *=
      template<typename TValue_, int TDims_, typename TTransformation_>
      void operator*=(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(TDims == TDims_ || TDims == -1 || TDims_ == -1, "dimension mismatch");
          assert(size() == other.size() && "size mismatch");

          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) *= other[i]; }
      }

      void operator*=(const value_type& x)
      {
          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) *= x; }
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR /=
      template<typename TValue_, int TDims_, typename TTransformation_>
      void operator/=(const Image<TValue_, TDims_, TTransformation_>& other)
      {
          static_assert(TDims == TDims_ || TDims == -1 || TDims_ == -1, "dimension mismatch");
          assert(size() == other.size() && "size mismatch");

          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) /= other[i]; }
      }

      void operator/=(const value_type& x)
      {
          #pragma omp parallel for
          for (unsigned int i = 0; i < num_values(); ++i)
          { operator[](i) /= x; }
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      #ifdef BK_LIB_PNG_AVAILABLE
      /// @{ -------------------------------------------------- HELPERS: SAVE PNG
    private:
      template<typename TIndexAccessible>
      [[maybe_unused]] bool _save_png(std::string_view filepath, int d, TIndexAccessible&& gid) const
      {
          if (filepath.empty())
          { return false; }

          if constexpr (bk::is_matrix_v<value_type>)
          {
              static_assert(bk::is_dynamic_matrix_v<value_type> || bk::is_static_vector_of_size_v<value_type, 1> || bk::is_static_vector_of_size_v<value_type, 2> || bk::is_static_vector_of_size_v<value_type, 3> || bk::is_static_vector_of_size_v<value_type, 4>, "value_type must be 1-4d vector");
              assert((bk::matrix_traits<value_type>(operator[](0)).is_vector_of_size(1) || bk::matrix_traits<value_type>(operator[](0)).is_vector_of_size(2) || bk::matrix_traits<value_type>(operator[](0)).is_vector_of_size(3) || bk::matrix_traits<value_type>(operator[](0)).is_vector_of_size(4)) && "value_type must be 1-4d vector");
          }

          if (d > 1)
          {
              for (unsigned int i = 0; i < size(d); ++i)
              {
                  gid[d] = i;

                  if (!_save_png(filepath, d - 1, gid))
                  { return false; }
              } // for i
          }
          else
          {
              std::stringstream ss;
              ss << filepath;

              for (int k = num_dimensions() - 1; k > 1; --k)
              { ss << "_" << gid[k]; }

              ss << ".png";

              FILE* file = std::fopen(ss.str().c_str(), "wb");

              if (!file)
              {
                  std::fclose(file);
                  std::cerr << "save_png: png file creation error" << std::endl;
                  return false;
              }

              auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

              if (!png_ptr)
              {
                  std::fclose(file);
                  std::cerr << "save_png error at png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)" << std::endl;
                  return false;
              }

              auto info_ptr = png_create_info_struct(png_ptr);

              if (!info_ptr)
              {
                  std::fclose(file);
                  std::cerr << "save_png error at png_create_info_struct(png_ptr);" << std::endl;
                  return false;
              }

              if (setjmp(png_jmpbuf(png_ptr)))
              {
                  png_destroy_write_struct(&png_ptr, &info_ptr);
                  std::fclose(file);
                  std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
                  return false;
              }

              png_init_io(png_ptr, file);

              if (setjmp(png_jmpbuf(png_ptr)))
              {
                  png_destroy_write_struct(&png_ptr, &info_ptr);
                  std::fclose(file);
                  std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
                  return false;
              }

              png_uint_32 width = static_cast<png_uint_32>(size(0));
              png_uint_32 height = static_cast<png_uint_32>(size(1));
              const int bit_depth = 8;

              unsigned int numel = 1;
              if constexpr (bk::is_matrix_v<value_type>)
              { numel = operator[](0).num_elements(); }

              const int color_type = numel == 1 ? PNG_COLOR_TYPE_GRAY : numel == 2 ? PNG_COLOR_TYPE_GA : numel == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA;

              png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
              png_write_info(png_ptr, info_ptr);

              auto minval = allocate_value<double>();
              auto maxval = allocate_value<double>();

              if constexpr (bk::is_matrix_v<value_type>)
              {
                  minval.set_constant(std::numeric_limits<double>::max());
                  maxval.set_constant(std::numeric_limits<double>::lowest());
              }
              else
              {
                  minval = std::numeric_limits<value_type>::max();
                  maxval = std::numeric_limits<value_type>::lowest();
              }

              for (png_uint_32 y = 0; y < height; ++y)
              {
                  gid[1] = y;

                  for (png_uint_32 x = 0; x < width; ++x)
                  {
                      gid[0] = x;

                      if constexpr (bk::is_matrix_v<value_type>)
                      {
                          minval.min_cwise_internal(operator()(gid));
                          maxval.max_cwise_internal(operator()(gid));
                      }
                      else
                      {
                          minval = std::min(minval, static_cast<decltype(minval)>(operator()(gid)));
                          maxval = std::max(maxval, static_cast<decltype(maxval)>(operator()(gid)));
                      }
                  } // for x
              } // for y

              png_bytepp buffer = new png_bytep[height];

              auto range = allocate_value<double>();

              if constexpr (bk::is_matrix_v<value_type>)
              {
                  for (unsigned int k = 0; k < numel; ++k)
                  { range[k] = maxval[k] - minval[k]; }
              }
              else
              { range = maxval - minval; }

              for (png_uint_32 y = 0; y < height; ++y)
              {
                  gid[1] = y;
                  buffer[y] = new png_byte[width * numel];

                  for (png_uint_32 x = 0; x < width; ++x)
                  {
                      gid[0] = x;

                      if constexpr (bk::is_matrix_v<value_type>)
                      {
                          const auto& val = operator()(gid);

                          for (png_uint_32 k = 0; k < numel; ++k)
                          {
                              if (minval[k] != maxval[k])
                              { buffer[y][numel * x + k] = static_cast<png_byte>(255.0 * (val[k] - minval[k]) / range[k]); }
                              else
                              { buffer[y][numel * x + k] = static_cast<png_byte>(255); }
                          } // for k
                      }
                      else
                      {
                          if (minval != maxval)
                          { buffer[y][x] = static_cast<png_byte>(255.0 * (operator()(gid) - minval) / range); }
                          else
                          { buffer[y][x] = static_cast<png_byte>(255); }
                      }
                  } // for x
              } // for y

              png_write_image(png_ptr, buffer);

              auto clear_buffer = [&]()
              {
                  for (png_uint_32 y = 0; y < height; ++y)
                  { delete[] buffer[y]; }

                  delete[] buffer;

                  png_destroy_write_struct(&png_ptr, &info_ptr);
              };

              if (setjmp(png_jmpbuf(png_ptr)))
              {
                  clear_buffer();
                  std::fclose(file);
                  std::cerr << "save_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
                  return false;
              }

              png_write_end(png_ptr, nullptr);
              clear_buffer();
              std::fclose(file);
          }

          return true;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SAVE PNG
      [[maybe_unused]] bool save_png(std::string_view filepath) const
      {
          const std::string _filepath = bk::string_utils::ends_with(filepath, ".png", false) ? bk::string_utils::chop_back(filepath.data(), 4) : filepath.data();
          return _save_png(_filepath, num_dimensions() - 1, MatrixFactory::create<int, NumDimensionsAtCompileTime(), 1>(num_dimensions(), 1));
      }
      /// @}

      /// @{ -------------------------------------------------- LOAD PNG
      [[maybe_unused]] bool load_png(std::string_view filepath)
      {
          if (filepath.empty())
          {
              std::cerr << "load_png: empty file path" << std::endl;
              return false;
          }

          std::string _filepath = filepath.data();

          if (!bk::string_utils::ends_with(_filepath, ".png"))
          { _filepath = bk::string_utils::append(_filepath, ".png"); }

          FILE* file = std::fopen(_filepath.c_str(), "rb");
          if (!file)
          {
              std::cerr << "load_png error: could not open file" << std::endl;
              return false;
          }

          png_byte header[8] = {0, 0, 0, 0, 0, 0, 0, 0};

          [[maybe_unused]] const size_t dummy = std::fread(header, 1, 8, file);

          if (png_sig_cmp(header, 0, 8))
          {
              std::cerr << "load_png_gray(" << _filepath << ") " << "png_sig_cmp(&header, 0, 8)" << std::endl;
              std::fclose(file);
              return false;
          }

          png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

          if (!png_ptr)
          {
              std::cerr << "load_png error at png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)" << std::endl;
              std::fclose(file);
              return false;
          }

          png_infop info_ptr = png_create_info_struct(png_ptr);

          if (!info_ptr)
          {
              std::cerr << "load_png error at png_create_info_struct(png_ptr)" << std::endl;
              std::fclose(file);
              return false;
          }

          if (setjmp(png_jmpbuf(png_ptr)))
          {
              std::cerr << "load_png error at setjmp(png_jmpbuf(png_ptr))" << std::endl;
              png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
              std::fclose(file);
              return false;
          }

          png_init_io(png_ptr, file);
          png_set_sig_bytes(png_ptr, 8);
          png_read_info(png_ptr, info_ptr);
          const png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
          const png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
          //const png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr); // unused
          //const int number_of_passes = png_set_interlace_handling(png_ptr); // unused

          png_read_update_info(png_ptr, info_ptr);

          if (setjmp(png_jmpbuf(png_ptr)))
          {
              std::cerr << "load_png error at " << std::endl;
              png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
              std::fclose(file);
              return false;
          }

          set_size(width, height);

          png_bytepp png_data_raw = new png_bytep[height];

          const png_size_t rowsize = png_get_rowbytes(png_ptr, info_ptr);
          for (png_uint_32 y = 0; y < height; ++y)
          { png_data_raw[y] = new png_byte[rowsize]; }

          png_read_image(png_ptr, png_data_raw);
          const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
          const unsigned int stride = color_type == PNG_COLOR_TYPE_GRAY ? 1 : color_type == PNG_COLOR_TYPE_GA ? 2 : color_type == PNG_COLOR_TYPE_RGB ? 3 : 4;

          for (png_uint_32 y = 0; y < height; ++y)
          {
              const png_bytep row = png_data_raw[y];

              for (png_uint_32 x = 0; x < width; ++x)
              {
                  const png_bytep val = &(row[x * stride]);

                  if constexpr (bk::is_matrix_v<value_type>)
                  {
                      for (unsigned int i = 0; i < stride; ++i)
                      {
                          // potential alpha values of PNG_COLOR_TYPE_RGBA are ignored
                          operator()(x, y)[i] = val[i];
                      } // for i
                  }
                  else
                  {
                      if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                      {
                          // alpha values are ignored for PNG_COLOR_TYPE_GRAY_ALPHA
                          operator()(x, y) = static_cast<value_type>(*val);
                      }
                      else if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA)
                      {
                          // - perform RGB to gray conversion
                          // - potential alpha values are ignored for PNG_COLOR_TYPE_RGBA
                          const double r = 0.2126 * val[0];
                          const double g = 0.7152 * val[1];
                          const double b = 0.0722 * val[2];

                          operator()(x, y) = static_cast<value_type>(r + g + b);
                      }
                  }
              } // for x
          } // for y

          png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
          std::fclose(file);

          for (png_uint_32 y = 0; y < height; ++y)
          { delete[] png_data_raw[y]; }

          delete[] png_data_raw;

          return true;
      }
      /// @}
      #endif // BK_LIB_PNG_AVAILABLE
  }; // class Image

  //====================================================================================================
  //===== TYPE DEFINITIONS
  //====================================================================================================
  template<typename TValue, int TDims> using CartesianImage = Image<TValue, TDims, NoTransformation<TDims>>;
  template<typename TValue, int TDims> using RegularImage = Image<TValue, TDims, ScaleTransformation<TDims>>;
  template<typename TValue, int TDims> using WorldImage = Image<TValue, TDims, WorldMatrixTransformation<TDims>>;
  template<typename TValue, int TDims> using DicomImage = Image<TValue, TDims, DicomTransformation>;

  //====================================================================================================
  //===== OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR +
  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator+(const Image<TValue0, TDims0, TTransformation>& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      Image<common_value_type, std::max(TDims0, TDims1), TTransformation> res = a;
      res += b;

      return res;
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator+(Image<TValue0, TDims0, TTransformation>&& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      {
          a += b;
          return a;
      }
      else
      { return a + b; }
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator+(const Image<TValue0, TDims0, TTransformation>& a, Image<TValue1, TDims1, TTransformation>&& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue1, common_value_type>)
      {
          b += a;
          return b;
      }
      else
      { return a + b; }
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator+(Image<TValue0, TDims0, TTransformation>&& a, Image<TValue1, TDims1, TTransformation>&& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      { return std::move(a) + b; }
      else if constexpr (std::is_same_v<TValue1, common_value_type>)
      { return std::move(b) + a; }
      else
      { return a + b; }
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator+(const Image<TValue, TDims, TTransformation>& img, const T& x)
  {
      Image<TValue, TDims, TTransformation> res = img;
      res += x;
      return res;
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator+(Image<TValue, TDims, TTransformation>&& img, const T& x)
  {
      img += x;
      return std::move(img);
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator+(const T& x, const Image<TValue, TDims, TTransformation>& img)
  { return img + x; }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator+(const T& x, Image<TValue, TDims, TTransformation>&& img)
  { return std::move(img) + x; }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR -
  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator-(const Image<TValue0, TDims0, TTransformation>& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      Image<common_value_type, std::max(TDims0, TDims1), TTransformation> res = a;
      res -= b;

      return res;
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator-(Image<TValue0, TDims0, TTransformation>&& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      {
          a -= b;
          return a;
      }
      else
      { return a - b; }
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator-(const Image<TValue, TDims, TTransformation>& img, const T& x)
  {
      Image<TValue, TDims, TTransformation> res = img;
      res -= x;
      return res;
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator-(Image<TValue, TDims, TTransformation>&& img, const T& x)
  {
      img -= x;
      return std::move(img);
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR *
  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator*(const Image<TValue0, TDims0, TTransformation>& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      Image<common_value_type, std::max(TDims0, TDims1), TTransformation> res = a;
      res *= b;

      return res;
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator*(Image<TValue0, TDims0, TTransformation>&& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      {
          a *= b;
          return a;
      }
      else
      { return a * b; }
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator*(const Image<TValue0, TDims0, TTransformation>& a, Image<TValue1, TDims1, TTransformation>&& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue1, common_value_type>)
      {
          b *= a;
          return b;
      }
      else
      { return a * b; }
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator*(Image<TValue0, TDims0, TTransformation>&& a, Image<TValue1, TDims1, TTransformation>&& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      { return std::move(a) * b; }
      else if constexpr (std::is_same_v<TValue1, common_value_type>)
      { return std::move(b) * a; }
      else
      { return a * b; }
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator*(const Image<TValue, TDims, TTransformation>& img, const T& x)
  {
      Image<TValue, TDims, TTransformation> res = img;
      res *= x;
      return res;
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator*(Image<TValue, TDims, TTransformation>&& img, const T& x)
  {
      img *= x;
      return std::move(img);
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator*(const T& x, const Image<TValue, TDims, TTransformation>& img)
  { return img * x; }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator*(const T& x, Image<TValue, TDims, TTransformation>&& img)
  { return std::move(img) * x; }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR /
  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator/(const Image<TValue0, TDims0, TTransformation>& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      Image<common_value_type, std::max(TDims0, TDims1), TTransformation> res = a;
      res /= b;

      return res;
  }

  template<typename TValue0, typename TValue1, int TDims0, int TDims1, typename TTransformation>
  [[nodiscard]] auto operator/(Image<TValue0, TDims0, TTransformation>&& a, const Image<TValue1, TDims1, TTransformation>& b)
  {
      using common_value_type = std::common_type_t<TValue0, TValue1>;

      if constexpr (std::is_same_v<TValue0, common_value_type>)
      {
          a /= b;
          return a;
      }
      else
      { return a / b; }
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator/(const Image<TValue, TDims, TTransformation>& img, const T& x)
  {
      Image<TValue, TDims, TTransformation> res = img;
      res /= x;
      return res;
  }

  template<typename TValue, int TDims, typename TTransformation, typename T>
  [[nodiscard]] Image<TValue, TDims, TTransformation> operator/(Image<TValue, TDims, TTransformation>&& img, const T& x)
  {
      img /= x;
      return std::move(img);
  }
  /// @}
} // namespace bk

#endif // BK_IMAGE_H  