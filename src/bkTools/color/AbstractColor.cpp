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

#include "../../../include/bkTools/color/AbstractColor.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  AbstractColor::AbstractColor()
      : AbstractColor(0, 0, 0, 1)
  { /* do nothing */ }

  AbstractColor::AbstractColor(const self_type&) = default;

  AbstractColor::AbstractColor(self_type&&) = default;

  AbstractColor::AbstractColor(value_type r, value_type g, value_type b, value_type a)
      : _values{r,g,b,a}
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractColor::~AbstractColor()
  { /* do nothing */ }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- DATA
  const double* AbstractColor::data() const
  { return _values; }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR[]
  auto AbstractColor::operator[](unsigned int id) -> value_type&
  { return _values[id]; }

  auto AbstractColor::operator[](unsigned int id) const -> value_type
  { return _values[id]; }
  /// @}

  /// @{ -------------------------------------------------- ITERATORS
  auto AbstractColor::begin() -> value_type*
  { return _values; }

  auto AbstractColor::begin() const -> const value_type*
  { return _values; }

  auto AbstractColor::end() -> value_type*
  { return _values + 3; }

  auto AbstractColor::end() const -> const value_type*
  { return _values + 3; }

  auto AbstractColor::rbegin()
  { return std::reverse_iterator<decltype(end())>(end()); }

  auto AbstractColor::rbegin() const
  { return std::reverse_iterator<decltype(end())>(end()); }

  auto AbstractColor::rend()
  { return std::reverse_iterator<decltype(begin())>(begin()); }

  auto AbstractColor::rend() const
  { return std::reverse_iterator<decltype(begin())>(begin()); }
  /// @}

  /// @{ -------------------------------------------------- GET ALPHA COMPONENT
  auto AbstractColor::alpha() -> value_type&
  { return (*this)[3]; }

  auto AbstractColor::alpha() const -> value_type
  { return (*this)[3]; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR=
  auto AbstractColor::operator=(const self_type& other) -> self_type&
  {
      std::copy(other.begin(), other.end(), this->begin());
      return *this;
  }

  auto AbstractColor::operator=(self_type&& other) -> self_type& = default;

  auto AbstractColor::operator=(std::initializer_list<double> values) -> self_type&
  {
      assert(values.size() == 4);
      std::copy(values.begin(), values.end(), this->begin());
      return *this;
  }
  /// @}

  /// @{ -------------------------------------------------- SET
  void AbstractColor::set(value_type r, value_type g, value_type b, value_type a)
  {
      assert(r >= 0 && r <= 1);
      assert(g >= 0 && g <= 1);
      assert(b >= 0 && b <= 1);
      assert(a >= 0 && a <= 1);

      _values[0] = r;
      _values[1] = g;
      _values[2] = b;
      _values[3] = a;
  }

  void AbstractColor::set(const self_type& other)
  {
      _values[0] = other._values[0];
      _values[1] = other._values[1];
      _values[2] = other._values[2];
      _values[3] = other._values[3];
  }
  /// @}

  //====================================================================================================
  //===== MATH OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR +=
  void AbstractColor::operator+=(double x)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] += x; }
  }

  void AbstractColor::operator+=(const self_type& other)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] += other[i]; }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR -=
  void AbstractColor::operator-=(double x)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] -= x; }
  }

  void AbstractColor::operator-=(const self_type& other)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] -= other[i]; }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR *=
  void AbstractColor::operator*=(double x)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] *= x; }
  }
  /// @}

  /// @{ -------------------------------------------------- OPERATOR /=
  void AbstractColor::operator/=(double x)
  {
      for (unsigned int i = 0; i < 4; ++i)
      { _values[i] /= x; }
  }
  /// @}

  //====================================================================================================
  //===== COMPARE
  //====================================================================================================
  bool AbstractColor::operator==(const self_type& other) const
  {
      for (unsigned int i = 0; i < 4; ++i)
      {
          if (_values[i] != other[i])
          { return false; }
      }

      return true;
  }

  bool AbstractColor::operator!=(const self_type& other) const
  { return !(*this == other); }
} // namespace bk::details


