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

#ifndef BKTOOLS_NDARRAY_H
#define BKTOOLS_NDARRAY_H

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <bkMath/functions/list_grid_id_conversion.h>
#include <bkTypeTraits/make_constant_index_sequence.h>
#include <bkTools/ndcontainer/NDIterator.h>
#include <bkTools/ndcontainer/NDReverseIterator.h>

namespace bk
{
  template<typename TValue, unsigned int... TSizes> class NDArray
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(sizeof...(TSizes) > 0, "template sizes are missing");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr unsigned int NumDimensionsAtCompileTime() noexcept
      { return sizeof...(TSizes); }

      [[nodiscard]] static constexpr unsigned int NumElementsAtCompileTime() noexcept
      { return (TSizes * ...); }
      /// @}

    private:
      using self_type = NDArray<TValue, TSizes...>;
    public:
      using value_type = TValue;
      using size_type = unsigned int;
      using difference_type = int;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using iterator = NDIterator<self_type>;
      using const_iterator = NDIterator<const self_type>;
      using reverse_iterator = NDReverseIterator<self_type>;
      using const_reverse_iterator = NDReverseIterator<const self_type>;

      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(NumDimensionsAtCompileTime() > 0, "template sizes are missing");
      static_assert(NumElementsAtCompileTime() > 0 || ((TSizes == 0) && ...), "all sizes must be != 0 _OR_ all sizes must be == 0");

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      std::array<value_type, NumElementsAtCompileTime()> _values;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR HELPERS
    private:
      template<typename TIndexAccessible, std::size_t... TIndices>
      constexpr NDArray(const TIndexAccessible& a, std::index_sequence<TIndices...>) noexcept
          : _values{static_cast<value_type>(a[TIndices])...}
      { /* do nothing */ }

      template<int... TScalar>
      constexpr NDArray(std::integer_sequence<int, TScalar...>) noexcept
          : _values{static_cast<value_type>(TScalar)...}
      { /* do nothing */ }

    public:
      /// @}

    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr NDArray() noexcept
          : NDArray(bk::make_constant_index_sequence<0, NumElementsAtCompileTime()>())
      { /* do nothing */ }

      constexpr NDArray(const self_type& other) noexcept
          : NDArray(other, std::make_index_sequence<NumElementsAtCompileTime()>())
      { /* do nothing */ }

      constexpr NDArray(self_type&& other) noexcept
          : NDArray(other, std::make_index_sequence<NumElementsAtCompileTime()>())
      { /* do nothing */ }

      template<typename TIndexAccessible, std::enable_if_t<std::is_class_v<TIndexAccessible> && !std::is_same_v<TIndexAccessible, value_type>>* = nullptr>
      constexpr NDArray(const TIndexAccessible& other) noexcept
          : NDArray(other, std::make_index_sequence<NumElementsAtCompileTime()>())
      { /* do nothing */ }

      template<typename... TValues, std::enable_if_t<!std::conjunction_v<std::is_class<TValues>...> || std::conjunction_v<std::is_same<TValues, value_type>...>>* = nullptr>
      constexpr NDArray(TValues ... values) noexcept
          : _values{static_cast<value_type>(values)...}
      { static_assert(sizeof...(TValues) == NumElementsAtCompileTime(), "invalid number of arguments"); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~NDArray() noexcept = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] static constexpr size_type num_dimensions() noexcept
      { return NumDimensionsAtCompileTime(); }
      /// @}

      /// @{ -------------------------------------------------- GET DATA
      [[nodiscard]] constexpr std::array<value_type, NumElementsAtCompileTime()>& data() noexcept
      { return _values; }

      [[nodiscard]] constexpr const std::array<value_type, NumElementsAtCompileTime()>& data() const noexcept
      { return _values; }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] static constexpr std::array<size_type, NumDimensionsAtCompileTime()> size() noexcept
      { return std::array<size_type, NumDimensionsAtCompileTime()>{{TSizes...}}; }

      [[nodiscard]] static constexpr size_type size(size_type dimId) noexcept
      { return size()[dimId]; }

      [[nodiscard]] static constexpr auto max_size() noexcept -> decltype(size())
      { return size(); }

      [[nodiscard]] static constexpr auto max_size(size_type dimId) noexcept -> decltype(size(dimId))
      { return size(dimId); }
      /// @}

      /// @{ -------------------------------------------------- IS EMPTY
      [[nodiscard]] static constexpr bool empty() noexcept
      { return num_values() == 0; }
      /// @}

      /// @{ -------------------------------------------------- HELPER: GET STRIDE
    private:
      template<std::size_t... TIndices>
      [[nodiscard]] static constexpr auto _stride(std::index_sequence<TIndices...>) noexcept
      { return std::array<size_type, NumDimensionsAtCompileTime()>{{bk::stride_of_dim(size(), TIndices, num_dimensions())...}}; }

    public:
      /// @}

      /// @{ -------------------------------------------------- GET STRIDE
      [[nodiscard]] static constexpr std::array<size_type, NumDimensionsAtCompileTime()> stride() noexcept
      { return _stride(std::make_index_sequence<NumDimensionsAtCompileTime()>()); }

      [[nodiscard]] static constexpr size_type stride(size_type dimId) noexcept
      { return stride()[dimId]; }
      /// @}

      /// @{ -------------------------------------------------- GET NUM VALUES
      [[nodiscard]] static constexpr size_type num_values() noexcept
      { return NumElementsAtCompileTime(); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      /*!
       * Returns a reference to the element at specified location pos, _WITHOUT_ bounds checking.
       */
      [[nodiscard]] constexpr reference operator[](size_type i)
      {
          assert(i < num_values() && "id out of bounds");
          return _values[i];
      }

      [[nodiscard]] constexpr const_reference operator[](size_type i) const
      {
          assert(i < num_values() && "id out of bounds");
          return _values[i];
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: IDS ARE WITHIN BOUNDS
    private:
      template<size_type TDimSize, size_type... TDimSizes, typename... TIds>
      [[nodiscard]] static constexpr bool _are_valid_ids(size_type id0, TIds... ids) noexcept
      {
          if (sizeof...(TDimSizes) != sizeof...(TIds))
          { return false; }

          const bool valid = id0 < TDimSize;

          if constexpr (sizeof...(TDimSizes) == 0)
          { return valid; }
          else
          { return valid && _are_valid_ids<TDimSizes...>(ids...); }
      }

      template<size_type TDimSize, size_type... TDimSizes, typename T, std::enable_if_t<std::is_class_v<T>>* = nullptr>
      [[nodiscard]] static bool _are_valid_ids(size_type dimId, const T& ids)
      {
          if (dimId == 0 && std::distance(ids.begin(), ids.end()) != num_dimensions())
          { return false; }

          const bool valid = ids[dimId] >= 0 && static_cast<size_type>(ids[dimId]) < TDimSize;

          if constexpr (sizeof...(TDimSizes) == 0)
          { return valid; }
          else
          { return valid && _are_valid_ids<TDimSizes...>(dimId + 1, ids); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR()
      template<typename... T>
      [[nodiscard]] constexpr reference operator()(const T& ... ids)
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              assert(_are_valid_ids<TSizes...>(0, ids...) && "one or more ids are out of bound OR invalid number of arguments");
              return operator[](bk::grid_to_list_id(size(), ids..., NumDimensionsAtCompileTime()));
          }
          else
          {
              static_assert(nArgs == NumDimensionsAtCompileTime(), "invalid number of arguments");
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");
              assert(_are_valid_ids<TSizes...>(ids...) && "one or more ids are out of bound");

              return operator[](bk::grid_to_list_id(size(), ids...));
          }
      }

      template<typename... T>
      [[nodiscard]] constexpr const_reference operator()(const T& ... ids) const
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              assert(_are_valid_ids<TSizes...>(0, ids...) && "one or more ids are out of bound OR invalid number of arguments");
              return operator[](bk::grid_to_list_id(size(), ids..., NumDimensionsAtCompileTime()));
          }
          else
          {
              static_assert(nArgs == NumDimensionsAtCompileTime(), "invalid number of arguments");
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");
              assert(_are_valid_ids<TSizes...>(ids...) && "one or more ids are out of bound OR invalid number of arguments");

              return operator[](bk::grid_to_list_id(size(), ids...));
          }
      }
      /// @}

      /// @{ -------------------------------------------------- AT
      /*!
       * Returns a reference to the element at specified location pos, _WITH_ bounds checking.
       * If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
       */
      template<typename... T>
      [[nodiscard]] constexpr reference at(const T& ... ids)
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              const unsigned int lid = bk::grid_to_list_id(size(), ids..., NumDimensionsAtCompileTime());
              if (lid >= num_values())
              { throw std::out_of_range("one or more ids are out of bound (i.e., >= size(dimId))"); }

              return operator[](lid);
          }
          else
          {
              static_assert(nArgs == NumDimensionsAtCompileTime(), "invalid number of arguments");
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");

              const unsigned int lid = bk::grid_to_list_id(size(), ids...);
              if (lid >= num_values())
              { throw std::out_of_range("one or more ids are out of bound (i.e., >= size(dimId))"); }

              return operator[](lid);
          }
      }

      template<typename... T>
      [[nodiscard]] constexpr const_reference at(const T& ... ids) const
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              const unsigned int lid = bk::grid_to_list_id(size(), ids..., NumDimensionsAtCompileTime());
              if (lid >= num_values())
              { throw std::out_of_range("one or more ids are out of bound (i.e., >= size(dimId))"); }

              return operator[](lid);
          }
          else
          {
              static_assert(nArgs == NumDimensionsAtCompileTime(), "invalid number of arguments");
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");

              const unsigned int lid = bk::grid_to_list_id(size(), ids...);
              if (lid >= num_values())
              { throw std::out_of_range("one or more ids are out of bound (i.e., >= size(dimId))"); }

              return operator[](bk::grid_to_list_id(size(), ids...));
          }
      }
      /// @}

      /// @{ -------------------------------------------------- GET FRONT
      [[nodiscard]] constexpr reference front()
      { return operator[](0); }

      [[nodiscard]] constexpr const_reference front() const
      { return operator[](0); }
      /// @}

      /// @{ -------------------------------------------------- GET BACK
      [[nodiscard]] constexpr reference back()
      { return operator[](num_values() - 1); }

      [[nodiscard]] constexpr const_reference back() const
      { return operator[](num_values() - 1); }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] constexpr iterator begin() noexcept
      { return iterator(*this); }

      [[nodiscard]] constexpr const_iterator begin() const noexcept
      { return const_iterator(*this); }

      [[nodiscard]] constexpr const_iterator cbegin() const noexcept
      { return const_iterator(*this); }

      [[nodiscard]] constexpr iterator end() noexcept
      { return iterator(*this, num_values()); }

      [[nodiscard]] constexpr const_iterator end() const noexcept
      { return const_iterator(*this, num_values()); }

      [[nodiscard]] constexpr const_iterator cend() const noexcept
      { return const_iterator(*this, num_values()); }

      [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
      { return reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
      { return const_reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
      { return const_reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] constexpr reverse_iterator rend() noexcept
      { return reverse_iterator(*this, -1); }

      [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
      { return const_reverse_iterator(*this, -1); }

      [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
      { return const_reverse_iterator(*this, -1); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) noexcept = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET
      template<typename... TValues>
      constexpr void set(TValues&& ... values)
      {
          static_assert(sizeof...(TValues) == NumElementsAtCompileTime(), "invalid number of arguments");

          _values = {static_cast<value_type>(std::forward<TValues>(values))...};
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- FILL
      constexpr void fill(const_reference value)
      {
          for (reference x: _values)
          { x = value; }
      }
      /// @}

      /// @{ -------------------------------------------------- SWAP
      void swap(self_type& other)
      {
          auto it_this = begin();
          auto it_other = other.begin();

          for (; it_this != end(); ++it_this, ++it_other)
          { std::swap(*it_this, *it_other); }
      }

      void swap(self_type&& other)
      {
          auto it_this = begin();
          auto it_other = other.cbegin();

          for (; it_this != end(); ++it_this, ++it_other)
          { *it_this = *it_other; }
      }
      /// @}

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: COMPARE SIZE TEMPLATE PARAMETERS
    private:
      template<unsigned int I, typename TTupleThis, typename TTupleOther>
      [[nodiscard]] static constexpr bool _size_template_parameters_match(const TTupleThis& t0, const TTupleOther& t1)
      { // expects that num_dimensions() matches
          constexpr unsigned int N = sizeof...(TSizes);

          const bool match = std::get<I>(t0) == std::get<I>(t1);

          if constexpr (I == N - 1)
          { return match; }
          else
          { return match && _size_template_parameters_match<I + 1>(t0, t1); }
      }

      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] static constexpr bool _size_template_parameters_match(const NDArray<TValueOther, TSizesOther...>& other)
      {
          if constexpr (num_dimensions() != other.num_dimensions() || num_values() != other.num_values())
          { return false; }
          else
          { return _size_template_parameters_match<0>(std::make_tuple(TSizes...), std::make_tuple(TSizesOther...)); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- HELPER: COMPARE VALUES WITH BINARY OPERATION
    private:
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool _compare_data_arrays(const NDArray<TValueOther, TSizesOther...>& other, std::function<bool(const_reference, typename NDArray<TValueOther, TSizesOther...>::const_reference)> comp) const
      {
          if constexpr (!_size_template_parameters_match(other))
          { return false; }
          else
          {
              bool valid = true;

              for (unsigned int i = 0; i < num_values(); ++i)
              { valid &= comp(operator[](i), other[i]); }

              return valid;
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR==
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool operator==(const NDArray<TValueOther, TSizesOther...>& other) const
      {
          constexpr auto comp = [](const_reference x, typename NDArray<TValueOther, TSizesOther...>::const_reference y) -> bool
          { return x == y; };

          return _compare_data_arrays(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR!=
      [[nodiscard]] constexpr bool operator!=(const self_type& other) const
      { return !operator==(other); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR<
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool operator<(const NDArray<TValueOther, TSizesOther...>& other) const
      {
          if (num_values() != other.num_values())
          { return num_values() < other.num_values(); }

          constexpr auto comp = [](const_reference x, typename NDArray<TValueOther, TSizesOther...>::const_reference y) -> bool
          { return x < y; };

          return _compare_data_arrays(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR<=
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool operator<=(const NDArray<TValueOther, TSizesOther...>& other) const
      {
          if (num_values() != other.num_values())
          { return num_values() <= other.num_values(); }

          constexpr auto comp = [](const_reference x, typename NDArray<TValueOther, TSizesOther...>::const_reference y) -> bool
          { return x <= y; };

          return _compare_data_arrays(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR>
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool operator>(const NDArray<TValueOther, TSizesOther...>& other) const
      { return !operator<=(other); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR>=
      template<typename TValueOther, unsigned int... TSizesOther>
      [[nodiscard]] constexpr bool operator>=(const NDArray<TValueOther, TSizesOther...>& other) const
      { return !operator<(other); }
      /// @}
  }; // class NDArray

  //====================================================================================================
  //===== EXTERNAL OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR <<
  template<typename TValue, unsigned int... TSizes>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& o, const NDArray<TValue, TSizes...>& v)
  {
      constexpr unsigned int nDims = sizeof...(TSizes);

      if constexpr (nDims == 1)
      {
          const unsigned int N = v.num_values();

          o << "[";

          for (unsigned int i = 0; i < N; ++i)
          {
              o << v[i];

              if (i < N - 1)
              { o << ", "; }
          }

          o << "]";
      }
      else if constexpr (nDims == 2)
      {
          const auto size = v.size();

          o << "[";

          for (unsigned int y = 0; y < size[1]; ++y)
          {
              if (y != 0)
              { o << " "; }

              o << "[";

              for (unsigned int x = 0; x < size[0]; ++x)
              {
                  o << v(x, y);

                  if (x < size[0] - 1)
                  { o << ", "; }
              }

              o << "]";

              if (y < size[1] - 1)
              { o << std::endl; }
          }

          o << "]";
      }
      else
      {
          const unsigned int N = v.num_values();

          o << "[";

          for (unsigned int i = 0; i < N; ++i)
          {
              const std::array<unsigned int, nDims> gid = bk::list_to_grid_id(v.size(), i);
              o << "(";
              for (unsigned int k = 0; k < gid.size(); ++k)
              {
                  o << gid[k];
                  if (k < gid.size() - 1)
                  { o << ","; }
              }
              o << ")=";

              o << v[i];

              if (i < v.num_values() - 1)
              { o << ", "; }
          }

          o << "]";
      }

      return o;
  }
  /// @}
} // namespace bk

#endif //BKTOOLS_NDARRAY_H
