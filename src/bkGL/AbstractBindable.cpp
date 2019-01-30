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

#include <bkGL/AbstractBindable.h>

#include <utility>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  AbstractBindable::AbstractBindable()
      : base_type(),
  #else

  AbstractBindable::AbstractBindable(bk::qt_gl_functions* gl)
      : base_type(gl),
  #endif
        _is_bound(false),
        _id(0)
  { /* do nothing */ }

  AbstractBindable::AbstractBindable(self_type&& other) noexcept
      : base_type(std::move(other)),
        _is_bound(other._is_bound),
        _id(other._id)
  {
      other._is_bound = false;
      other._id = 0; // prevent delete on destroy, which is probably implemented in 'virtual void clear_impl();'
  }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractBindable::~AbstractBindable() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS BOUND
  bool AbstractBindable::is_bound() const
  { return _is_bound; }
  /// @}

  /// @{ -------------------------------------------------- GET ID
  GLuint AbstractBindable::id() const
  { return _id; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool AbstractBindable::is_initialized() const
  { return _id != 0; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractBindable::operator=(self_type&& other) noexcept -> self_type&
  {
      _id = other._id;
      _is_bound = other._is_bound;
      other._id = 0; // prevent delete on destroy, which is probably implemented in 'virtual void clear_impl();'
      other._is_bound = false;

      return *this;
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void AbstractBindable::clear()
  {
      if (!this->is_initialized())
      { return; }

      if (_is_bound)
      { release(); }

      clear_impl();
      _id = 0;
  }
  /// @}

  /// @{ -------------------------------------------------- BIND
  void AbstractBindable::bind()
  {
      _is_bound = _id != 0;

      if (_is_bound)
      { bind_impl(); }
  }
  /// @}

  /// @{ -------------------------------------------------- RELEASE
  void AbstractBindable::release()
  {
      _is_bound = false;
      release_impl();
  }
  /// @}
} // namespace bk::details


