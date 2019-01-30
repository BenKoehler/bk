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

#ifndef BKTOOLS_COLORBAR_H
#define BKTOOLS_COLORBAR_H

#include <vector>

namespace bk
{
  template<typename ColorType> class ColorBar
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ColorBar<ColorType>;

    public:
      using color_type = ColorType;

      //====================================================================================================
      //===== MEMBERS 
      //====================================================================================================
      std::vector<color_type> _colors;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ColorBar() = default;
      ColorBar(const self_type&) = default;
      ColorBar(self_type&&) noexcept = default;

      template<typename... Colors_>
      ColorBar(Colors_&& ... colors)
          : _colors{{std::forward<Colors_>(colors)...}}
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ColorBar() = default;

      /// @}
      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- NUM COLORS
      [[nodiscard]] unsigned int num_colors() const
      { return _colors.size(); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      [[nodiscard]] auto operator[](unsigned int id) -> color_type&
      { return _colors[id]; }

      [[nodiscard]] auto operator[](unsigned int id) const -> const color_type&
      { return _colors[id]; }
      /// @}

      /// @{ -------------------------------------------------- ITERATORS
      [[nodiscard]] auto begin()
      { return _colors.begin(); }

      [[nodiscard]] auto begin() const
      { return _colors.begin(); }

      [[nodiscard]] auto end()
      { return _colors.end(); }

      [[nodiscard]] auto end() const
      { return _colors.end(); }

      [[nodiscard]] auto rbegin()
      { return _colors.rbegin(); }

      [[nodiscard]] auto rbegin() const
      { return _colors.rbegin(); }

      [[nodiscard]] auto rend()
      { return _colors.rend(); }

      [[nodiscard]] auto rend() const
      { return _colors.rend(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- SET DEFAULTS
    private:

      template<int I, typename Col_>
      void _set_default(Col_&& col)
      { (*this)[I] = std::forward<Col_>(col); }

      template<int I, typename Col_, typename... Cols_>
      void _set_default(Col_&& col, Cols_&& ... cols)
      {
          _set_default<I>(std::forward<Col_>(col));
          _set_default<I + 1>(std::forward<Cols_>(cols)...);
      }

      template<typename Col_, typename... Cols_>
      void _set_default(Col_&& col, Cols_&& ... cols)
      {
          _colors.resize(1 + sizeof...(cols));
          _set_default<0>(std::forward<Col_>(col), std::forward<Cols_>(cols)...);
      }

    public:

      void set_default_rainbow()
      { _set_default(color_type::Purple(), color_type::Light_Blue(), color_type::Green(), color_type::Yellow(), color_type::Red()); }

      void set_default_plasma()
      { _set_default(color_type::Dark_Blue(), color_type::Purple(), color_type::Pink(), color_type::Light_Red(), color_type::Orange(), color_type::Yellow()); }

      void set_default_gray()
      { _set_default(color_type::Black(), color_type::White()); }

      void set_default_white_to_red()
      { _set_default(color_type::White(), color_type::Red()); }

      void set_default_white_to_green()
      { _set_default(color_type::White(), color_type::Green()); }

      void set_default_white_to_blue()
      { _set_default(color_type::White(), color_type::Blue()); }

      void set_default_red_to_white_to_blue()
      { _set_default(color_type::Red(), color_type::White(), color_type::Blue()); }

      void set_default_blue_to_white_to_red()
      { _set_default(color_type::Blue(), color_type::White(), color_type::Red()); }

      void set_default_red_to_white_to_green()
      { _set_default(color_type::Red(), color_type::White(), color_type::Green()); }

      void set_default_green_to_white_to_red()
      { _set_default(color_type::Green(), color_type::White(), color_type::Red()); }

      void set_default_heat()
      { _set_default(color_type::Black(), color_type::Red(), color_type::Orange(), color_type::Yellow(), color_type::White()); }

      void set_default_magenta()
      { _set_default(color_type::Black(), color_type::Magenta(), color_type::Pink(), color_type::White()); }

      void set_default_red_green_blue()
      { _set_default(color_type::Red(), color_type::Green(), color_type::Blue()); }

      void set_default_blue_green_red()
      { _set_default(color_type::Blue(), color_type::Green(), color_type::Red()); }

      void set_default_uniform_yellow()
      { _set_default(color_type::Yellow(), color_type::Yellow()); }

      void set_default_traffic_light()
      { _set_default(color_type::Green(), color_type::Green(), color_type::Yellow(), color_type::Yellow(), color_type::Red(), color_type::Red()); }

      void set_default_cluster()
      { _set_default(color_type::Yellow(), color_type::Green(), color_type::Light_Blue(), color_type::Red(), color_type::Light_Gray(), color_type::Pink(), color_type::Orange(), color_type::Blue(), color_type::Purple(), color_type::Gray(), color_type::Light_Green(), color_type::Light_Red(), color_type::Dark_Green(), color_type::Dark_Red(), color_type::Dark_Gray(), color_type::Dark_Blue()); }

      void set_default_light_blue_to_black_to_yellow()
      { _set_default(color_type::Light_Blue(), color_type::Blue(), color_type::Dark_Blue(), color_type::Black(), color_type::Red(), color_type::Orange(), color_type::Yellow()); }

      void set_default_black_to_yellow()
      { _set_default(color_type::Black(), color_type::Red(), color_type::Orange(), color_type::Yellow()); }

      void set_default_black_to_light_blue()
      { _set_default(color_type::Black(), color_type::Dark_Blue(), color_type::Blue(), color_type::Light_Blue()); }
      /// @}

      /// @{ -------------------------------------------------- DEFAULTS (STATIC)
      [[nodiscard]] static self_type Rainbow()
      {
          self_type cb;
          cb.set_default_rainbow();
          return cb;
      }

      [[nodiscard]] static self_type Plasma()
      {
          self_type cb;
          cb.set_default_plasma();
          return cb;
      }

      [[nodiscard]] static self_type Gray()
      {
          self_type cb;
          cb.set_default_gray();
          return cb;
      }

      [[nodiscard]] static self_type Red_White_Blue()
      {
          self_type cb;
          cb.set_default_red_to_white_to_blue();
          return cb;
      }

      [[nodiscard]] static self_type Blue_White_Red()
      {
          self_type cb;
          cb.set_default_blue_to_white_to_red();
          return cb;
      }

      [[nodiscard]] static self_type Green_White_Red()
      {
          self_type cb;
          cb.set_default_green_to_white_to_red();
          return cb;
      }

      [[nodiscard]] static self_type Red_White_Green()
      {
          self_type cb;
          cb.set_default_red_to_white_to_green();
          return cb;
      }

      [[nodiscard]] static self_type Heat()
      {
          self_type cb;
          cb.set_default_heat();
          return cb;
      }

      [[nodiscard]] static self_type Magenta()
      {
          self_type cb;
          cb.set_default_magenta();
          return cb;
      }

      [[nodiscard]] static self_type White_Red()
      {
          self_type cb;
          cb.set_default_white_to_red();
          return cb;
      }

      [[nodiscard]] static self_type White_Green()
      {
          self_type cb;
          cb.set_default_white_to_green();
          return cb;
      }

      [[nodiscard]] static self_type White_Blue()
      {
          self_type cb;
          cb.set_default_white_to_blue();
          return cb;
      }

      [[nodiscard]] static self_type Red_Green_Blue()
      {
          self_type cb;
          cb.set_default_red_green_blue();
          return cb;
      }

      [[nodiscard]] static self_type Blue_Green_Red()
      {
          self_type cb;
          cb.set_default_blue_green_red();
          return cb;
      }

      [[nodiscard]] static self_type Uniform_Yellow()
      {
          self_type cb;
          cb.set_default_uniform_yellow();
          return cb;
      }

      [[nodiscard]] static self_type Traffic_Light()
      {
          self_type cb;
          cb.set_default_traffic_light();
          return cb;
      }

      [[nodiscard]] static self_type Cluster()
      {
          self_type cb;
          cb.set_default_cluster();
          return cb;
      }

      [[nodiscard]] static self_type Light_Blue_Black_Yellow()
      {
          self_type cb;
          cb.set_default_light_blue_to_black_to_yellow();
          return cb;
      }

      [[nodiscard]] static self_type Black_Yellow()
      {
          self_type cb;
          cb.set_default_black_to_yellow();
          return cb;
      }

      [[nodiscard]] static self_type Black_Light_Blue()
      {
          self_type cb;
          cb.set_default_black_to_light_blue();
          return cb;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type& = default;
      /// @}
  }; // class ColorBar<ColorType>
} // namespace bk

#endif //BKTOOLS_COLORBAR_H


