#include <bkGL/KeyBoard.h>

#include <unordered_set>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct KeyBoard::Impl
  {
      std::unordered_set<Key_> pressed_keys;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  KeyBoard::KeyBoard() = default;
  KeyBoard::KeyBoard(const KeyBoard&) = default;
  KeyBoard::KeyBoard(KeyBoard&&) noexcept = default;
  KeyBoard::~KeyBoard() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET BUTTON INFORMATION
  bool KeyBoard::key_is_pressed(Key_ k) const
  { return _pdata->pressed_keys.find(k) != _pdata->pressed_keys.end(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  KeyBoard& KeyBoard::operator=(const KeyBoard&) = default;
  KeyBoard& KeyBoard::operator=(KeyBoard&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- SET PRESSED
  void KeyBoard::set_key_pressed(Key_ k, bool pressed)
  {
      if (pressed)
      { _pdata->pressed_keys.insert(k); }
      else
      { _pdata->pressed_keys.erase(k); }
  }
  /// @}
} // namespace bk
