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

#ifndef BKTOOLS_NDVECTOR_H
#define BKTOOLS_NDVECTOR_H

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <bkMath/functions/list_grid_id_conversion.h>
#include <bkTools/ndcontainer/NDIterator.h>
#include <bkTools/ndcontainer/NDReverseIterator.h>

namespace bk
{
  template<typename TValue> class NDVector
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = NDVector<TValue>;
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
      //===== MEMBERS
      //====================================================================================================
    private:
      std::vector<size_type> _sizes;
      std::vector<value_type> _values;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      NDVector() = default;
      NDVector(const self_type&) = default;
      NDVector(self_type&&) noexcept = default;

      template<typename... TSizes, std::enable_if_t<std::conjunction_v<std::is_integral<TSizes>...>>* = nullptr>
      NDVector(TSizes... sizes)
          : _sizes({static_cast<size_type>(sizes)...}),
            _values(num_values_from_sizes(), static_cast<value_type>(0))
      {
          static_assert(sizeof...(TSizes) != 0, "size arguments are missing");
          static_assert(std::conjunction_v<std::is_integral<TSizes>...>, "size arguments must be integral types");
          assert(num_values_from_sizes() != 0 && "all size arguments must be >= 1");
      }

      template<typename Iter, std::enable_if_t<std::is_class_v<Iter> || std::is_pointer_v<Iter>>* = nullptr>
      NDVector(Iter firstSize, Iter lastSize)
      { resize(firstSize, lastSize); }

      template<typename TArray, std::enable_if_t<std::is_class_v<TArray>>* = nullptr>
      NDVector(TArray arr)
      {
          resize(arr.size());
          std::copy(arr.begin(), arr.end(), begin());
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~NDVector() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] size_type num_dimensions() const
      { return _sizes.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET DATA
      [[nodiscard]] std::vector<value_type>& data()
      { return _values; }

      [[nodiscard]] const std::vector<value_type>& data() const
      { return _values; }
      /// @}

      /// @{ -------------------------------------------------- GET SIZE
      [[nodiscard]] const std::vector<size_type>& size() const
      { return _sizes; }

      [[nodiscard]] size_type size(size_type dimId) const
      {
          assert(dimId < num_dimensions() && "dimId exceeds num_dimensions()");

          return _sizes[dimId];
      }

      [[nodiscard]] auto max_size() const -> decltype(size())
      { return _sizes.max_size(); }
      /// @}

      /// @{ -------------------------------------------------- IS EMPTY
      [[nodiscard]] bool empty() const
      { return _values.empty(); }
      /// @}

      /// @{ -------------------------------------------------- GET STRIDE
      [[nodiscard]] std::vector<size_type> stride() const
      {
          std::vector<size_type> s(num_dimensions());

          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { s[i] = bk::stride_of_dim(_sizes, i, _sizes.size()); }

          return s;
      }

      [[nodiscard]] size_type stride(size_type dimId) const
      { return bk::stride_of_dim(_sizes, dimId, _sizes.size()); }
      /// @}

      /// @{ -------------------------------------------------- GET NUM VALUES
    private:
      [[nodiscard]] size_type num_values_from_sizes() const
      {
          return std::accumulate(_sizes.begin(), _sizes.end(), 1U, [](size_type x, size_type y)
          { return x * y; });
      }

    public:

      [[nodiscard]] size_type num_values() const
      { return _values.size(); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      /*!
      * Returns a reference to the element at specified location pos, _WITHOUT_ bounds checking.
      */
      [[nodiscard]] reference operator[](size_type i)
      {
          assert(i < num_values() && "id out of bounds");
          return _values[i];
      }

      [[nodiscard]] const_reference operator[](size_type i) const
      {
          assert(i < num_values() && "id out of bounds");
          return _values[i];
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: IDS IN BOUND ASSERTION
    private:
      template<typename... TIds>
      [[nodiscard]] bool _are_valid_ids(size_type dimId, size_type id0, TIds... ids) const
      {
          if (dimId == 0 && 1+sizeof...(ids) != num_dimensions())
          {return false;}

          const bool valid = id0 >= 0 && id0 < _sizes[dimId];

          if constexpr (sizeof...(TIds) == 0)
          { return valid; }
          else
          { return valid && _are_valid_ids(dimId + 1, ids...); }
      }

      template<typename T>
      [[nodiscard]] bool _are_valid_ids(const T& ids) const
      {
          const unsigned int N = std::distance(ids.begin(), ids.end());
          if (N != num_dimensions())
          {return false;}

          for (unsigned int i = 0; i < N; ++i)
          {
              if (ids[i] < 0 || static_cast<unsigned int>(ids[i]) >= _sizes[i])
              {return false;}
          }

          return true;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- OPERATOR()
      template<typename... T>
      [[nodiscard]] reference operator()(const T& ... ids)
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              const unsigned int numSizes = std::distance(std::begin(ids...), std::end(ids...));
              const unsigned int lid = numSizes != _sizes.size() ? std::numeric_limits<unsigned int>::max() : bk::grid_to_list_id(_sizes, ids..., _sizes.size());

              if (lid >= num_values())
              {
                  std::vector<unsigned int> newsize(std::begin(ids...), std::end(ids...));

                  for (unsigned int i = 0; i < newsize.size(); ++i)
                  {
                      ++newsize[i];

                      if (i < _sizes.size())
                      { newsize[i] = std::max(newsize[i], _sizes[i]); }
                  }

                  assert(std::all_of(newsize.begin(), newsize.end(), [](unsigned int x)
                  { return x > 0; }) && "one or more ids are < 0");

                  resize(newsize);

                  return operator[](bk::grid_to_list_id(_sizes, ids..., _sizes.size()));
              }
              else
              { return operator[](lid); }

              return operator[](bk::grid_to_list_id(_sizes, ids..., _sizes.size()));
          }
          else
          {
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");
              assert(((ids >= 0) && ...) && "one or more ids are < 0");

              const unsigned int lid = nArgs != _sizes.size() ? std::numeric_limits<unsigned int>::max() : bk::grid_to_list_id(_sizes, ids...);

              if (lid >= num_values())
              {
                  std::array<unsigned int, nArgs> newsize{static_cast<unsigned int>(ids)...};

                  for (unsigned int i = 0; i < nArgs; ++i)
                  {
                      ++newsize[i];

                      if (i < _sizes.size())
                      { newsize[i] = std::max(newsize[i], _sizes[i]); }
                  }

                  resize(newsize);

                  return operator[](bk::grid_to_list_id(_sizes, ids...));
              }
              else
              { return operator[](lid); }
          }
      }

      template<typename... T>
      [[nodiscard]] const_reference operator()(const T& ... ids) const
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              assert(_are_valid_ids(ids...) && "one or more ids are out of bound OR invalid number of arguments");
              return operator[](bk::grid_to_list_id(_sizes, ids..., _sizes.size()));
          }
          else
          {
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");
              assert(sizeof...(T) == num_dimensions() && "invalid number of arguments");
              assert(_are_valid_ids(0, ids...) && "one or more ids are out of bound OR invalid number of arguments");

              return operator[](bk::grid_to_list_id(_sizes, ids...));
          }
      }
      /// @}

      /// @{ -------------------------------------------------- AT
      /*!
       * Returns a reference to the element at specified location pos, _WITH_ bounds checking.
       * If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
       */
      template<typename... T>
      [[nodiscard]] reference at(const T& ... ids)
      { return operator()(ids...); }

      template<typename... T>
      [[nodiscard]] const_reference at(const T& ... ids) const
      {
          constexpr unsigned int nArgs = sizeof...(ids);
          static_assert(nArgs != 0, "arguments are missing");

          if constexpr (nArgs == 1 && std::conjunction_v<std::is_class<T>...>)
          {
              if (!_are_valid_ids(ids...))
              { throw std::out_of_range("one or more ids are out of bound OR invalid number of arguments"); }

              return operator[](bk::grid_to_list_id(_sizes, ids..., _sizes.size()));
          }
          else
          {
              static_assert(std::conjunction_v<std::is_integral<T>...>, "all ids must be integral types");

              if (!_are_valid_ids(0, ids...))
              { throw std::out_of_range("one or more ids are out of bound OR invalid number of arguments"); }

              return operator[](bk::grid_to_list_id(_sizes, ids...));
          }
      }
      /// @}

      /// @{ -------------------------------------------------- GET FRONT
      [[nodiscard]] reference front()
      { return operator[](0); }

      [[nodiscard]] const_reference front() const
      { return operator[](0); }
      /// @}

      /// @{ -------------------------------------------------- GET BACK
      [[nodiscard]] reference back()
      { return operator[](num_values() - 1); }

      [[nodiscard]] const_reference back() const
      { return operator[](num_values() - 1); }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] iterator begin()
      { return iterator(*this); }

      [[nodiscard]] const_iterator begin() const
      { return const_iterator(*this); }

      [[nodiscard]] const_iterator cbegin() const
      { return const_iterator(*this); }

      [[nodiscard]] iterator end()
      { return iterator(*this, num_values()); }

      [[nodiscard]] const_iterator end() const
      { return const_iterator(*this, num_values()); }

      [[nodiscard]] const_iterator cend() const
      { return const_iterator(*this, num_values()); }

      [[nodiscard]] reverse_iterator rbegin()
      { return reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] const_reverse_iterator rbegin() const
      { return const_reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] const_reverse_iterator crbegin() const
      { return const_reverse_iterator(*this, static_cast<int>(num_values())-1); }

      [[nodiscard]] reverse_iterator rend()
      { return reverse_iterator(*this, -1); }

      [[nodiscard]] const_reverse_iterator rend() const
      { return const_reverse_iterator(*this, -1); }

      [[nodiscard]] const_reverse_iterator crend() const
      { return const_reverse_iterator(*this, -1); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- HELPER: VALID SIZES ASSERTION
    private:
      template<typename... TSizes>
      [[nodiscard]] bool are_valid_sizes(size_type size0, TSizes... sizes) const
      {
          const bool valid = size0 > 0;

          if constexpr (sizeof...(TSizes) == 0)
          { return valid; }
          else
          { return valid && are_valid_sizes(sizes...); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- SET SIZE
      template<typename... TSizes, std::enable_if_t<std::conjunction_v<std::is_integral<TSizes>...>>* = nullptr>
      void resize(TSizes... sizes)
      {
          static_assert(sizeof...(TSizes) != 0, "size arguments are missing");
          assert(are_valid_sizes(sizes...) && "all sizes must be > 0");

          _sizes = {static_cast<size_type>(sizes)...};
          _values.resize(num_values_from_sizes());
          _values.shrink_to_fit();
      };

      template<typename TIteratable, std::enable_if_t<std::is_class_v<TIteratable>>* = nullptr>
      void resize(const TIteratable& size)
      { resize(size.begin(), size.end()); }

      template<typename Iter, std::enable_if_t<std::is_class_v<Iter> || std::is_pointer_v<Iter>>* = nullptr>
      void resize(Iter first, Iter last)
      {
          _sizes.assign(first, last);
          assert(std::all_of(_sizes.begin(), _sizes.end(), [](size_type x)
          { return x > 0; }) && "all sizes must be > 0");

          _values.resize(num_values_from_sizes());
          _values.shrink_to_fit();
      };
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void clear()
      {
          _values.clear();
          _values.shrink_to_fit();
          _sizes.clear();
          _sizes.shrink_to_fit();
      }
      /// @}

      /// @{ -------------------------------------------------- RESERVE
      void reserve(size_type N)
      { _values.reserve(N); }

      template<typename... TSizes, std::enable_if_t<(sizeof...(TSizes) > 1)>* = nullptr>
      void reserve(TSizes... sizes)
      { _values.reserve((sizes * ...)); }
      /// @}

      /// @{ -------------------------------------------------- FILL
      void fill(const_reference value)
      {
          for (reference x: _values)
          { x = value; }
      }
      /// @}

      /// @{ -------------------------------------------------- ASSIGN
      void assign(size_type count, const_reference value)
      { _values.assign(count, value); }

      template<class TInputIt>
      void assign(TInputIt first, TInputIt last)
      { _values.assign(first, last); }

      void assign(std::initializer_list<value_type> ilist)
      { _values.assign(ilist); }
      /// @}

      /// @{ -------------------------------------------------- SWAP
      void swap(self_type& other)
      {
          std::swap(_sizes, other._sizes);
          std::swap(_values, other._values);
      }

      void swap(self_type&& other)
      {
          _sizes = std::move(other._sizes);
          _values = std::move(other._values);
      }
      /// @}

      /// @{ -------------------------------------------------- SHRINK TO FIT
      void shrink_to_fit()
      {
          _sizes.shrink_to_fit();
          _values.shrink_to_fit();
      }
      /// @}

      //====================================================================================================
      //===== COMPARISON
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: COMPARE SIZES
    private:
      template<typename TValueOther>
      [[nodiscard]] bool _sizes_match(const NDVector<TValueOther>& other) const
      {
          if (num_dimensions() != other.num_dimensions())
          { return false; }

          for (unsigned int i = 0; i < num_dimensions(); ++i)
          {
              if (size(i) != other.size(i))
              { return false; }
          }

          return true;
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- HELPER: COMPARE VALUES WITH BINARY OPERATION
      template<typename TValueOther>
      [[nodiscard]] bool _compare_data_vectors(const NDVector<TValueOther>& other, std::function<bool(const_reference, typename NDVector<TValueOther>::const_reference)> comp) const
      {
          if (!_sizes_match(other))
          { return false; }

          bool valid = true;

          for (unsigned int i = 0; i < num_values(); ++i)
          { valid &= comp(operator[](i), other[i]); }

          return valid;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR==
      template<typename TValueOther>
      [[nodiscard]] bool operator==(const NDVector<TValueOther>& other) const
      {
          constexpr auto comp = [](const_reference x, typename NDVector<TValueOther>::const_reference y) -> bool
          { return x == y; };

          return _compare_data_vectors(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR!=
      template<typename TValueOther>
      [[nodiscard]] bool operator!=(const NDVector<TValueOther>& other) const
      { return !operator==(other); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR<
      template<typename TValueOther>
      [[nodiscard]] bool operator<(const NDVector<TValueOther>& other) const
      {
          if (num_values() != other.num_values())
          { return num_values() < other.num_values(); }

          constexpr auto comp = [](const_reference x, typename NDVector<TValueOther>::const_reference y) -> bool
          { return x < y; };

          return _compare_data_vectors(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR<=
      template<typename TValueOther>
      [[nodiscard]] bool operator<=(const NDVector<TValueOther>& other) const
      {
          if (num_values() != other.num_values())
          { return num_values() <= other.num_values(); }

          constexpr auto comp = [](const_reference x, typename NDVector<TValueOther>::const_reference y) -> bool
          { return x <= y; };

          return _compare_data_vectors(other, comp);
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR>
      template<typename TValueOther>
      [[nodiscard]] bool operator>(const NDVector<TValueOther>& other) const
      { return !operator<=(other); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR>=
      template<typename TValueOther>
      [[nodiscard]] bool operator>=(const NDVector<TValueOther>& other) const
      { return !operator<(other); }
      /// @}

  }; // class NDVector

  //====================================================================================================
  //===== EXTERNAL OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR <<
  template<typename TValue>
  [[maybe_unused]] std::ostream& operator<<(std::ostream& o, const NDVector<TValue>& v)
  {
      const unsigned int N = v.num_values();
      const auto size = v.size();

      switch (v.num_dimensions())
      {
          case 1:
          {
              o << "[";

              for (unsigned int i = 0; i < N; ++i)
              {
                  o << v[i];

                  if (i < N - 1)
                  { o << ", "; }
              }

              o << "]";
              break;
          }
          case 2:
          {
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
              break;
          }
          default:
          {
              o << "[";

              for (unsigned int i = 0; i < N; ++i)
              {
                  const std::vector<unsigned int> gid = bk::list_to_grid_id(v.size(), i);
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
      }

      return o;
  }
  /// @}
} // namespace bk

#endif //BKTOOLS_NDVECTOR_H
