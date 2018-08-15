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

#include <bkTools/clock/Clock.h>

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CONSTRUCTORS
  Clock::Clock() = default;
  Clock::Clock(const self_type&) = default;
  Clock::Clock(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DESTRUCTOR
  Clock::~Clock() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TIME IN [UNIT]
  unsigned int Clock::time_in_nano_sec() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::nanoseconds>(_time_diff()).count()); }

  unsigned int Clock::time_in_micro_sec() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::microseconds>(_time_diff()).count()); }

  unsigned int Clock::time_in_milli_sec() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(_time_diff()).count()); }

  unsigned int Clock::time_in_sec() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(_time_diff()).count()); }

  unsigned int Clock::time_in_min() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::minutes>(_time_diff()).count()); }

  unsigned int Clock::time_in_h() const
  { return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::hours>(_time_diff()).count()); }

  std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> Clock::time_in_h_min_s_ms() const
  {
      unsigned int t = time_in_milli_sec();

      unsigned int ms = t % 1'000;
      t /= 1'000;

      unsigned int s = t % 60;
      t /= 60;

      unsigned int min = t % 60;
      t /= 60;

      unsigned int h = t % 60;

      return std::make_tuple(h, min, s, ms);
  }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Clock::operator=(const self_type&) -> self_type& = default;
  auto Clock::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- HELPER: TIME DIFFERENCE
  std::chrono::system_clock::duration Clock::_time_diff() const
  { return _stop - _start; }
  /// @}

  /// @{ -------------------------------------------------- START / STOP
  void Clock::start()
  { _start = clock_type::now(); }

  void Clock::stop()
  { _stop = clock_type::now(); }
  /// @}

  //====================================================================================================
  //===== EXTERNAL OPERATORS
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR <<
  std::ostream& operator<<(std::ostream& o, const bk::Clock& c)
  {
      auto[h, min, s, ms] = c.time_in_h_min_s_ms();

      o << h << " h " << min << " min " << s << " s " << ms << " ms";

      return o;
  }
  /// @}
} // namespace bk


