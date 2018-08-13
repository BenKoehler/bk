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

#ifndef BK_TRANSLATIONTRANSFORMATION_H
#define BK_TRANSLATIONTRANSFORMATION_H

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>
#include <type_traits>

#include "../../bkTypeTraits/has_index_operator.h"
#include "../../bkTypeTraits/make_constant_index_sequence.h"
#include "../../bkTools/NDContainer"

#include "TransformationBase.h"

namespace bk
{
  template<int TDims = -1> class TranslationTransformation : public details::TransformationBase<TDims, TranslationTransformation<TDims>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims == -1 || TDims > 0);

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TranslationTransformation<TDims>;
      using base_type = details::TransformationBase<TDims, TranslationTransformation<TDims>>;
      using value_type = typename base_type::value_type;
      using translation_vector_type = std::conditional_t<(TDims > 0), std::array<value_type, TDims>, std::vector<value_type>>;

      friend base_type;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      translation_vector_type _translations;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- CTOR HELPERS
      template<int... TScalar>
      constexpr TranslationTransformation(bk::constant_index_sequence<TScalar...>) noexcept
          : _translations{static_cast<value_type>(TScalar)...}
      { /* do nothing */ }
      /// @}

    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr TranslationTransformation()
          : TranslationTransformation(bk::make_constant_index_sequence<0, std::max(TDims, 0)>())
      { /* do nothing */ }

      constexpr TranslationTransformation(const self_type&) = default;
      constexpr TranslationTransformation(self_type&&) = default;

      template<typename... TTranslations>
      constexpr TranslationTransformation(TTranslations... translations)
          : _translations{static_cast<value_type>(translations)...}
      {
          static_assert(sizeof...(TTranslations) != 0, "translation arguments are missing");
          static_assert(TDims == -1 || sizeof...(TTranslations) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TTranslations>...>, "all translation arguments must be arithmetic types");
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TranslationTransformation() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] constexpr unsigned int num_dimensions() const
      { return _translations.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET TRANSLATION
      [[nodiscard]] constexpr translation_vector_type& translation()
      { return _translations; }

      [[nodiscard]] constexpr const translation_vector_type& translation() const
      { return _translations; }

      [[nodiscard]] constexpr value_type translation(unsigned int dimId) const
      {
          assert(dimId < num_dimensions() && "dimId out of bounds");
          return _translations[dimId];
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr self_type& operator=(const self_type&) = default;
      [[maybe_unused]] constexpr self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- SET SCALE
      template<typename TTranslationVec, std::enable_if_t<bk::has_index_operator_v<TTranslationVec>>* = nullptr>
      void set_translation(const TTranslationVec& translation)
      {
          if constexpr (TDims == -1)
          { _translations.assign(translation.begin(), translation.end()); }
          else
          {
              const unsigned int N = std::distance(translation.begin(), translation.end());
              assert(N == TDims && "translationVec has invalid number of arguments");
              std::copy(translation.begin(), translation.end(), _translations.begin());
          }
      }

      template<typename... TTranslations>
      void set_translation(TTranslations ... translations)
      {
          static_assert(sizeof...(TTranslations) != 0, "translation arguments are missing");
          static_assert(TDims == -1 || sizeof...(TTranslations) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TTranslations>...>, "all translation arguments must have arithmetic types");

          _translations = {static_cast<value_type>(translations)...};
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- TO WORLD COORDINATES
      template<typename T>
      constexpr void to_world_coordinates_impl(T& vec) const
      {
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { vec[i] += _translations[i]; }
      }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
      template<typename T>
      constexpr void to_object_coordinates_impl(T& vec) const
      {
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { vec[i] -= _translations[i]; }
      }
      /// @}
  }; // class TranslationTransformation
} // namespace bk

#endif //BK_TRANSLATIONTRANSFORMATION_H
