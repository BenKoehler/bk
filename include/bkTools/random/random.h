/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#pragma once

#ifndef BKTOOLS_RANDOMENGINE_H
#define BKTOOLS_RANDOMENGINE_H

#include <chrono>
#include <cstdint>
#include <random>
#include <thread>
#include <type_traits>

#include "../lib/bkTools_export.h"

namespace bk
{
  class BKTOOLS_EXPORT random
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = random;
    public:
      using generator_type = std::mt19937_64;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      inline static std::hash<std::thread::id> _hash;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr random() = default;
      constexpr random(const self_type&) = default;
      constexpr random(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~random() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] constexpr auto operator=(const self_type&) -> self_type& = default;
      [[maybe_unused]] constexpr auto operator=(self_type&&) -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INT
      [[nodiscard]] static std::int32_t make_int32(std::int32_t min, std::int32_t max)
      {
          static thread_local generator_type generator(std::chrono::system_clock::now().time_since_epoch().count() + _hash(std::this_thread::get_id()));
          std::uniform_int_distribution<std::int32_t> distribution(min, max);

          return distribution(generator);
      }

      [[nodiscard]] static std::int64_t make_int64(std::int64_t min, std::int64_t max)
      {
          static thread_local generator_type generator(std::chrono::system_clock::now().time_since_epoch().count() + _hash(std::this_thread::get_id()));
          std::uniform_int_distribution<std::int64_t> distribution(min, max);

          return distribution(generator);
      }

      [[nodiscard]] static auto make_int(int min, int max)
      {
          if constexpr (std::is_same_v<int, std::int32_t>)
          { return make_int32(min, max); }
          else
          { return make_int64(min, max); }
      }
      /// @}

      /// @{ -------------------------------------------------- FLOATING POINT
      [[nodiscard]] static float make_float(float min, float max)
      {
          static thread_local generator_type generator(std::chrono::system_clock::now().time_since_epoch().count() + _hash(std::this_thread::get_id()));
          std::uniform_real_distribution<float> distribution(min, max);

          return distribution(generator);
      }

      [[nodiscard]] static float make_float()
      { return make_float(0.0f, 1.0f); }

      [[nodiscard]] static double make_double(double min, double max)
      {
          static thread_local generator_type generator(std::chrono::system_clock::now().time_since_epoch().count() + _hash(std::this_thread::get_id()));
          std::uniform_real_distribution<double> distribution(min, max);

          return distribution(generator);
      }

      [[nodiscard]] static double make_double()
      { return make_double(0.0, 1.0); }
      /// @}
  }; // class random
} // namespace bk

#endif //BKTOOLS_RANDOMENGINE_H
