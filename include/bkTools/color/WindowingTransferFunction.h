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

#ifndef BKTOOLS_WINDOWINGTRANSFERFUNCTION_H
#define BKTOOLS_WINDOWINGTRANSFERFUNCTION_H

#include <memory>
#include <type_traits>

#include "../lib/bkTools_export.h"
#include "ITransferFunction.h"

namespace bk
{
  // ------ forward declaration -------
  class ColorRGBA;
  class ColorBarRGBA;
  // ------ forward declaration -------

  class BKTOOLS_EXPORT WindowingTransferFunction : public details::ITransferFunction<ColorRGBA>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = WindowingTransferFunction;

    public:
      using colorbar_type = ColorBarRGBA;
      using color_type = ColorRGBA;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;
      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      WindowingTransferFunction();
      WindowingTransferFunction(const self_type& other);
      WindowingTransferFunction(self_type&& other);
      WindowingTransferFunction(double intensity_min, double intensity_max, double center, double width);
      ~WindowingTransferFunction();

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] double center() const;
      [[nodiscard]] double width() const;
      [[nodiscard]] auto colorbar() const -> const colorbar_type&;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS
    private:
      void _set_color_boundaries();
      void _set_inner_colors();

    public:
      /// @}

      /// @{ -------------------------------------------------- SET MIN/MAX INTENSITY
      void set_intensity_min_max(double imin, double imax);
      /// @}

      /// @{ -------------------------------------------------- SET CENTER
    private:
      void _set_center(double c);

    public:

      void set_center(double c);
      /// @}

      /// @{ -------------------------------------------------- SHIFT CENTER
      //! @param percent between 0 and 100
      void shift_center_left(double percent = 1);
      void shift_center_right(double percent = 1);
      /// @}

      /// @{ -------------------------------------------------- SET WIDTH
    private:
      void _set_width(double w);
    public:
      void set_width(double w);
      /// @}

      /// @{ -------------------------------------------------- GET INTENSITY
      [[nodiscard]] double intensity_min() const;
      [[nodiscard]] double intensity_max() const;
      [[nodiscard]] double intensity_range() const;
      /// @}

      /// @{ -------------------------------------------------- DECREASE/INCREASE WIDTH
      //! @param percent between 0 and 100
      void increase_width(double percent = 1);
      void decrease_width(double percent = 1);
      /// @}

      /// @{ -------------------------------------------------- RESET
      void reset(bool tolerant = false);
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY TRANSFER FUNCTION
      virtual auto apply(double data_value) const -> color_type override;
      /// @}

  }; // class WindowingTransferFunction

} // namespace bk

#endif //BKTOOLS_WINDOWINGTRANSFERFUNCTION_H


