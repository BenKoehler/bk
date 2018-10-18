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

#ifndef BK_TRANSFORMABLEGEOMETRY_H
#define BK_TRANSFORMABLEGEOMETRY_H

#include <utility>

namespace bk
{
  template<typename TGeometry, typename TTransformation> class TransformableGeometry : public TGeometry
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TransformableGeometry<TGeometry, TTransformation>;
      using base_type = TGeometry;
    public:
      using value_type = typename base_type::value_type;
      using point_type = typename base_type::point_type;
      using transformation_type = TTransformation;

      /// @{ -------------------------------------------------- IS EXPLICIT GEOMETRY
      [[nodiscard]] static constexpr bool IsExplicit() noexcept
      { return TGeometry::IsExplicit(); }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      transformation_type _transformation;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      TransformableGeometry() = default;
      TransformableGeometry(const self_type& other) = default;
      TransformableGeometry(self_type&& other) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TransformableGeometry() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET TRANSFORMATION
      [[nodiscard]] transformation_type& transformation()
      { return _transformation; }

      [[nodiscard]] const transformation_type& transformation() const
      { return _transformation; }
      /// @}

      /// @{ -------------------------------------------------- GET POINT IN WORLD COORDINATES
      template<typename... TIds>
      [[nodiscard]] auto point_in_world_coordinates(TIds&& ... ids) const
      { return _transformation.to_world_coordinates(this->point(std::forward<TIds>(ids)...)); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type& = default;
      /// @}
  }; // class TransformableGeometry
} // namespace bk

#endif //BK_TRANSFORMABLEGEOMETRY_H
