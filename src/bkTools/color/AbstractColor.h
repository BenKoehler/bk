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

#ifndef BKTOOLS_ABSTRACTCOLOR_H
#define BKTOOLS_ABSTRACTCOLOR_H

#include <initializer_list>
#include <memory>

#include <bkTools/lib/bkTools_export.h>

namespace bk::details
{
  class BKTOOLS_EXPORT AbstractColor
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractColor;

    public:
      using value_type = double;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      value_type _values[4];

    public:
      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      AbstractColor();
      AbstractColor(const self_type&);
      AbstractColor(self_type&&);
      AbstractColor(value_type r, value_type g, value_type b, value_type a = 1);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractColor();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET DATA
      [[nodiscard]] const value_type* data() const;
      /// @}

      /// @{ -------------------------------------------------- GET OPERATOR[]
      [[nodiscard]] auto operator[](unsigned int id) -> value_type&;
      [[nodiscard]] auto operator[](unsigned int id) const -> value_type;
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] auto begin() -> value_type*;
      [[nodiscard]] auto begin() const -> const value_type*;
      [[nodiscard]] auto end() -> value_type*;
      [[nodiscard]] auto end() const -> const value_type*;
      [[nodiscard]] auto rbegin();
      [[nodiscard]] auto rbegin() const;
      [[nodiscard]] auto rend();
      [[nodiscard]] auto rend() const;
      /// @}

      /// @{ -------------------------------------------------- GET ALPHA COMPONENT
      [[nodiscard]] auto alpha() -> value_type&;
      [[nodiscard]] auto alpha() const -> value_type;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR=
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      [[maybe_unused]] auto operator=(std::initializer_list<value_type> values) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET
      void set(value_type r, value_type g, value_type b, value_type a = 1);
      void set(const self_type& other);
      /// @}

      /// @{ -------------------------------------------------- SET DEFAULT COLOR
      virtual void set_default_red(bool keep_current_alpha) = 0;
      virtual void set_default_light_red(bool keep_current_alpha) = 0;
      virtual void set_default_dark_red(bool keep_current_alpha) = 0;
      virtual void set_default_green(bool keep_current_alpha) = 0;
      virtual void set_default_light_green(bool keep_current_alpha) = 0;
      virtual void set_default_dark_green(bool keep_current_alpha) = 0;
      virtual void set_default_blue(bool keep_current_alpha) = 0;
      virtual void set_default_light_blue(bool keep_current_alpha) = 0;
      virtual void set_default_dark_blue(bool keep_current_alpha) = 0;
      virtual void set_default_magenta(bool keep_current_alpha) = 0;
      virtual void set_default_pink(bool keep_current_alpha) = 0;
      virtual void set_default_purple(bool keep_current_alpha) = 0;
      virtual void set_default_yellow(bool keep_current_alpha) = 0;
      virtual void set_default_orange(bool keep_current_alpha) = 0;
      virtual void set_default_white(bool keep_current_alpha) = 0;
      virtual void set_default_gray(bool keep_current_alpha) = 0;
      virtual void set_default_light_gray(bool keep_current_alpha) = 0;
      virtual void set_default_dark_gray(bool keep_current_alpha) = 0;
      virtual void set_default_black(bool keep_current_alpha) = 0;
      /// @}

      //====================================================================================================
      //===== MATH OPERATORS
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR +=
      void operator+=(double x);
      void operator+=(const self_type& other);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR -=
      void operator-=(double x);
      void operator-=(const self_type& other);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR *=
      void operator*=(double x);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR /=
      void operator/=(double x);
      /// @}

      //====================================================================================================
      //===== COMPARE
      //====================================================================================================
      [[nodiscard]] bool operator==(const self_type& other) const;
      [[nodiscard]] bool operator!=(const self_type& other) const;
  }; // class AbstractColor
} // namespace bk::details

#endif //BKTOOLS_ABSTRACTCOLOR_H


