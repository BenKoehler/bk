/*
 * This Signal implementation is based on the one by Simon Schneegans, published in 2015
 * and released into the public domain.
 *
 * https://simmesimme.github.io/tutorials/2015/09/20/signal-slot
 *
 * -----------------------------------------------------
 *
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

#ifndef BKTOOLS_SIGNAL_H
#define BKTOOLS_SIGNAL_H

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <mutex>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace bk
{
  template<typename... Args> class Signal
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = Signal<Args...>;
      //using slot_map_type = std::vector<std::pair<unsigned int /*slot_id*/, std::function<void(Args...)> /*execute_function*/>>;
      using slot_map_type = std::vector<std::tuple<unsigned int /*slot_id*/, bool /*once*/, std::function<void(Args...)> /*execute_function*/>>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      mutable slot_map_type _slots;
      mutable std::atomic_uint _current_id;
      mutable std::mutex _map_mutex;
      bool _mute;

    public:
      //====================================================================================================
      //===== CTOR & DTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CONSTRUCTORS
      Signal()
          : _current_id(0),
            _mute(false)
      { /* do nothing */ }

      Signal(const self_type&) = delete;

      Signal(self_type&& other)
          : _slots(std::move(other._slots)),
            _mute(other._mute)
      { /* do nothing */ }
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~Signal() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- NUM CONNECTIONS
      [[nodiscard]] unsigned int num_connections() const
      { return _slots.size(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR=
      //! assignment creates new Signal
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CONNECT
      //! connects a member function of a given object to this Signal
      template<typename F, typename... A>
      [[maybe_unused]] unsigned int connect_member(F&& f, A&& ... a)
      {
          std::unique_lock<std::mutex> lock(_map_mutex);
          _slots.emplace_back(++_current_id, false, std::bind(std::forward<F>(f), std::forward<A>(a)...));
          return _current_id;
      }

      //! connects a std::function to the signal. The returned value can be used to disconnect the function again
      [[maybe_unused]] unsigned int connect(const std::function<void(Args...)>& slot)
      {
          std::unique_lock<std::mutex> lock(_map_mutex);
          _slots.emplace_back(++_current_id, false, slot);
          return _current_id;
      }

      //! connects a std::function to the signal. The returned value can be used to disconnect the function again.
      [[maybe_unused]] unsigned int connect_front(const std::function<void(Args...)>& slot)
      {
          std::unique_lock<std::mutex> lock(_map_mutex);
          _slots.emplace(_slots.begin(), false, ++_current_id, slot);
          return _current_id;
      }

      //! connects a member function of a given object to this Signal
      template<typename F, typename... A>
      [[maybe_unused]] unsigned int connect_member_once(F&& f, A&& ... a)
      {
          unsigned int id = connect_member(std::forward<F>(f), std::forward<A>(a)...);
          std::get<1>(_slots.back()) = true;
          return id;
      }

      //! connects a std::function to the signal. The returned value can be used to disconnect the function again
      [[maybe_unused]] unsigned int connect_once(const std::function<void(Args...)>& slot)
      {
          unsigned int id = connect(slot);
          std::get<1>(_slots.back()) = true;
          return id;
      }

      //! connects a std::function to the signal. The returned value can be used to disconnect the function again.
      [[maybe_unused]] unsigned int connect_front_once(const std::function<void(Args...)>& slot)
      {
          unsigned int id = connect_front(slot);
          std::get<1>(_slots.front()) = true;
          return id;
      }
      /// @}

      /// @{ -------------------------------------------------- DISCONNECT
      //! disconnects a previously connected function
      void disconnect(unsigned int id)
      {
          std::unique_lock<std::mutex> lock(_map_mutex);

          _slots.erase(std::remove_if(_slots.begin(), _slots.end(), [&](const typename slot_map_type::value_type& x) -> bool
          {
              return std::get<0>(x) == id;
          }), _slots.end());
      }

      //! disconnects all previously connected functions
      void disconnect_all()
      {
          std::unique_lock<std::mutex> lock(_map_mutex);
          _current_id = 0;
          _slots.clear();
      }
      /// @}

      /// @{ -------------------------------------------------- MUTE
      //! muted signals do not emit to slots
      void mute(bool m)
      { _mute = m; }
      /// @}

      /// @{ -------------------------------------------------- EMIT
      //! calls all connected functions
      void emit_signal(Args... p) const
      { // name "emit" conflicts in Qt projects
          if (!_mute)
          {
              std::unique_lock<std::mutex> lock(_map_mutex);
              for (auto it = _slots.begin(); it != _slots.end(); ++it)
              { std::get<2>(*it)(p...); }

              // remove one time executions
              _slots.erase(std::remove_if(_slots.begin(), _slots.end(), [](const typename slot_map_type::value_type& x)
              { return std::get<1>(x); }), _slots.end());
          }
      }
      /// @}
  }; // class Signal
} // namespace bk

#endif //BKTOOLS_SIGNAL_H


