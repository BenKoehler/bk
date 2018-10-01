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

#ifndef BKTOOLS_COMPILETIME_RANDOM_H
#define BKTOOLS_COMPILETIME_RANDOM_H

#include <cstdint>

namespace bk
{
    constexpr auto seed_from_buildtime()
    {
        const char inits[] = __TIME__;
        const std::uint32_t seed = (inits[0] - '0') * 100000 + (inits[1] - '0') * 10000 + (inits[3] - '0') * 1000 + (inits[4] - '0') * 100 + (inits[6] - '0') * 10 + inits[7] - '0';
        return seed;
    }

    namespace details
    {
      constexpr std::uint32_t linear_congruential_engine_next_state(std::uint32_t seed, std::uint32_t A = 16807UL, std::uint32_t C = 0UL, std::uint32_t M = (1UL << 31) - 1)
      {
          return (static_cast<std::uint64_t>(seed) * A + C) % M;
      }

      template<std::uint32_t min = 0UL, std::uint32_t max = (1UL << 31) - 1, std::uint32_t seed = seed_from_buildtime(), std::uint32_t A = 16807UL, std::uint32_t C = 0UL, std::uint32_t M = (1UL << 31) - 1>
      struct LinCongGen
      {
        static const std::uint32_t state = (static_cast<std::uint64_t>(seed) * A + C) % M;
        static const std::uint32_t value = min + (state % (max - min + 1));
        using next = LinCongGen<min, max, state>;
      };
    } // namespace details

    constexpr std::uint32_t rand(std::uint32_t min, std::uint32_t max, std::uint32_t seed)
    {
        const auto state = details::linear_congruential_engine_next_state(seed);
        return min + (state % (max - min + 1));
    }

    template<typename TUInt = unsigned int>
    constexpr TUInt rand(TUInt min, TUInt max, TUInt seed)
    {
        return static_cast<TUInt>(static_cast<TUInt>(min), static_cast<TUInt>(max), static_cast<TUInt>(seed));
    }

    template<std::uint32_t min = 0UL, std::uint32_t max = (1UL << 31) - 1, std::uint32_t seed = seed_from_buildtime()> using randi = details::LinCongGen<min, max, seed>;
} // namespace bk

#endif // BKTOOLS_COMPILETIME_RANDOM_H


