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

#ifndef BK_NOTRANSFORMATION_H
#define BK_NOTRANSFORMATION_H

#include "TransformationBase.h"

namespace bk
{
  template<int TDims = -1> class NoTransformation : public details::TransformationBase<TDims, NoTransformation<TDims>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || TDims > 0);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = NoTransformation<TDims>;
      using base_type = details::TransformationBase<TDims, NoTransformation<TDims>>;

      friend base_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr NoTransformation() = default;
      constexpr NoTransformation(const self_type&) = default;
      constexpr NoTransformation(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~NoTransformation() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      constexpr self_type& operator=(const self_type&) = default;
      constexpr self_type& operator=(self_type&&) = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- TO WORLD COORDINATES
      template<typename T>
      static constexpr void to_world_coordinates_impl(T& /*vec*/)
      { /* do nothing */; }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
      template<typename T>
      static constexpr void to_object_coordinates_impl(T& /*vec*/)
      { /* do nothing */ }
      /// @}
  }; // class NoTransformation
} // namespace bk

#endif //BK_NOTRANSFORMATION_H


