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

#ifndef BK_GRADIENTIMAGEFILTER_H
#define BK_GRADIENTIMAGEFILTER_H

#include <type_traits>

#include <bkMath/functions/list_grid_id_conversion.h>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Progress>
    #include <bk/Localization>

#endif


namespace bk
{
  class GradientImageFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GradientImageFilter;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr GradientImageFilter() = default;
      constexpr GradientImageFilter(const self_type&) = default;
      constexpr GradientImageFilter(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GradientImageFilter() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type& other) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&& other) noexcept -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TImage>
      [[nodiscard]] static auto apply(const TImage& img) -> typename TImage::template self_template_type<std::decay_t<decltype(img.jacobian(bk::list_to_grid_id(img.size(), 0)))>>
      {
          using gradient_type = std::decay_t<decltype(img.jacobian(bk::list_to_grid_id(img.size(), 0)))>;

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(10 + img.num_values(), ___("Gradient image filter"));
          #endif

          typename TImage::template self_template_type<gradient_type> res;
          res.set_size(img.size());

          #ifdef BK_EMIT_PROGRESS
          prog.increment(10);
          #endif

          #pragma omp parallel for
          for (unsigned int i = 0; i < img.num_values(); ++i)
          {
              res[i] = img.jacobian(bk::list_to_grid_id(img.size(), i));

              #ifdef BK_EMIT_PROGRESS
              #pragma omp critical(filter_gradient_strength)
              { prog.increment(1); }
              #endif
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return res;
      }
      /// @}
  }; // class GradientImageFilter
} // namespace bk

#endif //BK_GRADIENTIMAGEFILTER_H
