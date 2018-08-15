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

#include <bkTools/progress/Progress.h>

#include <algorithm>
#include <atomic>

#include <bk/Signal>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct Progress::Impl
  {
    private:
      static unsigned int _unique_id() noexcept
      {
          static std::atomic_uint ID{1};
          return ID++;
      }

    public:
      unsigned int id;
      std::string description;
      double max;
      double current;
      Signal<std::string> s_description_changed;
      Signal<double> s_max_changed;
      Signal<double> s_current_changed;
      Signal<unsigned int> s_finished;

      Impl()
          : id(_unique_id()),
            description(""),
            max(100),
            current(0.0)
      { /* do nothing */ }

      Impl(double max_, std::string_view description_)
          : id(_unique_id()),
            description(description_),
            max(max_),
            current(0.0)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  Progress::Progress()
      : _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  Progress::Progress(self_type&&) noexcept = default;

  Progress::Progress(double max, std::string_view description)
      : _pdata(new Impl(max, description))
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Progress::~Progress() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET UNIQUE ID
  unsigned int Progress::id() const
  { return _pdata->id; }
  /// @}

  /// @{ -------------------------------------------------- GET DESCRIPTION
  const std::string& Progress::description() const
  { return _pdata->description; }

  Signal<std::string>& Progress::signal_description_changed()
  { return _pdata->s_description_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET MAX
  double Progress::max() const
  { return _pdata->max; }

  Signal<double>& Progress::signal_max_changed()
  { return _pdata->s_max_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET CURRENT
  double Progress::current() const
  { return _pdata->current; }

  Signal<double>& Progress::signal_current_changed()
  { return _pdata->s_current_changed; }
  /// @}

  /// @{ -------------------------------------------------- IS FINISHED
  bool Progress::finished() const
  { return _pdata->current >= _pdata->max; }

  Signal<unsigned int>& Progress::signal_finished()
  { return _pdata->s_finished; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Progress::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET MAX
  void Progress::set_max(double m)
  {
      _pdata->max = m;
      _pdata->s_max_changed.emit_signal(_pdata->max);

      if (finished())
      { _pdata->s_finished.emit_signal(_pdata->id); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET CURRENT
  void Progress::set_current(double c)
  {
      _pdata->current = c;
      _pdata->s_current_changed.emit_signal(_pdata->current);

      if (finished())
      { _pdata->s_finished.emit_signal(_pdata->id); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET FINISHED
  void Progress::set_finished()
  {
      if (_pdata->current != _pdata->max)
      {
          _pdata->current = _pdata->max;
          _pdata->s_finished.emit_signal(_pdata->id);
      }
  }
  /// @}

  /// @{ -------------------------------------------------- ENABLE SIGNALS
  void Progress::set_signals_enabled(bool enable)
  {
      const bool mute_signals = !enable;
      _pdata->s_description_changed.mute(mute_signals);
      _pdata->s_max_changed.mute(mute_signals);
      _pdata->s_current_changed.mute(mute_signals);
      _pdata->s_finished.mute(mute_signals);
  }

  void Progress::enable_signals()
  { set_signals_enabled(true); }

  void Progress::disable_signals()
  { set_signals_enabled(false); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INCREMENT
  void Progress::increment(double step)
  { set_current(current() + step); }
  /// @}

  /// @{ -------------------------------------------------- CLEAR SLOTS
  void Progress::clear_slots()
  {
      _pdata->s_description_changed.disconnect_all();
      _pdata->s_max_changed.disconnect_all();
      _pdata->s_current_changed.disconnect_all();
      _pdata->s_finished.disconnect_all();
  }
  /// @}
} // namespace bk


