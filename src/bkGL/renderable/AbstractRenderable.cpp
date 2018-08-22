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

#include <bkGL/renderable/AbstractRenderable.h>

#include <atomic>
#include <iostream>

#include <bk/Signal>

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AbstractRenderable::Impl
  {
      /// @{ -------------------------------------------------- UID
    private:
      [[nodiscard]] static unsigned int _unique_id()
      {
          static std::atomic<GLuint> ID{0};
          return ID++;
      }

    public:
      /// @}

      GLuint uid;
      bool oit_available;
      bool animation_enabled;
      bool modelview_changed;
      bool hidden;
      bk::Signal<> s_update_required;
      bk::Signal<> s_bind_default_fbo;
      bk::Signal<> s_bind_default_fbo_as_read;
      bk::Signal<> s_bind_default_fbo_as_draw;

      Impl()
          : uid(_unique_id()),
            oit_available(true),
            animation_enabled(false),
            modelview_changed(false),
            hidden(false)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;

      ~Impl()
      {
          s_update_required.disconnect_all();
          s_bind_default_fbo.disconnect_all();
          s_bind_default_fbo_as_read.disconnect_all();
          s_bind_default_fbo_as_draw.disconnect_all();
      }

      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  AbstractRenderable::AbstractRenderable()
      : base_type()
  #else

  AbstractRenderable::AbstractRenderable(bk::qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /* do nothing */ }

  AbstractRenderable::AbstractRenderable(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractRenderable::~AbstractRenderable()
  { /* do nothing */ }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET SETTINGS
  bool AbstractRenderable::oit_is_available() const
  { return _pdata->oit_available; }

  bool AbstractRenderable::animation_is_enabled() const
  { return _pdata->animation_enabled; }

  bool AbstractRenderable::modelview_matrix_changed() const
  { return _pdata->modelview_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET VISIBILITY
  bool AbstractRenderable::is_visible() const
  { return !_pdata->hidden; }

  bool AbstractRenderable::is_hidden() const
  { return _pdata->hidden; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<>& AbstractRenderable::signal_update_required()
  { return _pdata->s_update_required; }

  bk::Signal<>& AbstractRenderable::signal_bind_default_fbo()
  { return _pdata->s_bind_default_fbo; }

  bk::Signal<>& AbstractRenderable::signal_bind_default_fbo_as_read()
  { return _pdata->s_bind_default_fbo_as_read; }

  bk::Signal<>& AbstractRenderable::signal_bind_default_fbo_as_draw()
  { return _pdata->s_bind_default_fbo_as_draw; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractRenderable::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET SETTINGS
  void AbstractRenderable::set_oit_available(bool b)
  {
      if (_pdata->oit_available != b)
      {
          _pdata->oit_available = b;
          on_oit_enabled(_pdata->oit_available);
      }
  }

  void AbstractRenderable::set_animation_is_enabled(bool b)
  {
      if (_pdata->animation_enabled != b)
      {
          _pdata->animation_enabled = b;
          on_animation_enabled(_pdata->animation_enabled);
      }
  }

  void AbstractRenderable::set_modelview_matrix_changed(bool b)
  {
      if (_pdata->modelview_changed != b)
      {
          _pdata->modelview_changed = b;
          on_modelview_changed(_pdata->modelview_changed);
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET VISIBILITY
  void AbstractRenderable::set_hidden(bool b)
  {
      if (b != _pdata->hidden)
      {
          _pdata->hidden = b;
          on_visible_changed(!_pdata->hidden);
          emit_signal_update_required();
      }
  }

  void AbstractRenderable::set_visible(bool b)
  { set_hidden(!b); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- UPDATE SIGNAL
  void AbstractRenderable::emit_signal_update_required() const
  {
      if (this->is_initialized())
      { _pdata->s_update_required.emit_signal(); }
  }

  void AbstractRenderable::emit_signal_bind_default_fbo() const
  {
      if (this->is_initialized())
      { _pdata->s_bind_default_fbo.emit_signal(); }
  }

  void AbstractRenderable::emit_signal_bind_default_fbo_as_read() const
  {
      if (this->is_initialized())
      { _pdata->s_bind_default_fbo_as_read.emit_signal(); }
  }

  void AbstractRenderable::emit_signal_bind_default_fbo_as_draw() const
  {
      if (this->is_initialized())
      { _pdata->s_bind_default_fbo_as_draw.emit_signal(); }
  }
  /// @}

  /// @{ -------------------------------------------------- FORWARD CONNECTIONS OF OTHER RENDERABLE
  void AbstractRenderable::forward_signals(AbstractRenderable* r)
  {
      if (r == nullptr)
      {
          std::cerr << "AbstractRenderable::forward_signals - parameter is nullptr" << std::endl;
          return;
      }

      r->signal_update_required().connect([&]()
                                          { this->emit_signal_update_required(); });

      r->signal_bind_default_fbo().connect([&]()
                                           { this->emit_signal_bind_default_fbo(); });

      r->signal_bind_default_fbo_as_read().connect([&]()
                                                   { this->emit_signal_bind_default_fbo_as_read(); });

      r->signal_bind_default_fbo_as_draw().connect([&]()
                                                   { this->emit_signal_bind_default_fbo_as_draw(); });

      _forward_signals(r);
  }

  void AbstractRenderable::_forward_signals(AbstractRenderable*)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- FORWARD SETTINGS TO OTHER RENDERARLE
  void AbstractRenderable::forward_settings(AbstractRenderable* r)
  {
      if (r == nullptr)
      {
          std::cerr << "AbstractRenderable::forward_settings - parameter is nullptr" << std::endl;
          return;
      }

      r->set_oit_available(_pdata->oit_available);
      r->set_animation_is_enabled(_pdata->animation_enabled);

      _forward_settings(r);
  }

  void AbstractRenderable::_forward_settings(AbstractRenderable*)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void AbstractRenderable::draw()
  {
      if (this->is_initialized() && !_pdata->hidden)
      {
          draw_impl();
          _pdata->modelview_changed = false;
      }
  }
  /// @}
} // namespace bk::details


