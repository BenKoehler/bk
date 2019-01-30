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

#include <bkGL/renderable/AbstractSceneRenderable.h>

#include <iostream>

#include <bk/Signal>

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AbstractSceneRenderable::Impl
  {
      /// @{ -------------------------------------------------- UID
    private:
      static unsigned int _unique_id()
      {
          static std::atomic<GLuint> ID{0};
          return ID++;
      }

    public:
      /// @}

      Signal<> s_scene_changed;

      ~Impl()
      { s_scene_changed.disconnect_all(); }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  AbstractSceneRenderable::AbstractSceneRenderable()
      : base_type()
  #else

  AbstractSceneRenderable::AbstractSceneRenderable(bk::qt_gl_functions* gl) : base_type(gl)
  #endif
  { /* do nothing */ }

  AbstractSceneRenderable::AbstractSceneRenderable(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  AbstractSceneRenderable::~AbstractSceneRenderable() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET SIGNALS
  Signal<>& AbstractSceneRenderable::signal_scene_changed()
  { return _pdata->s_scene_changed; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto AbstractSceneRenderable::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- SIGNALS
  void AbstractSceneRenderable::emit_signal_scene_changed()
  { _pdata->s_scene_changed.emit_signal(); }
  /// @}

  /// @{ -------------------------------------------------- FORWARD CONNECTIONS OF OTHER RENDERABLE
  void AbstractSceneRenderable::_forward_signals(AbstractRenderable* r)
  {
      if (r == nullptr)
      {
          std::cerr << "AbstractSceneRenderable::_forward_signals - parameter is nullptr" << std::endl;
          return;
      }

      static_cast<AbstractSceneRenderable*>(r)->signal_scene_changed().connect([&]()
                                                                               { this->emit_signal_scene_changed(); });
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void AbstractSceneRenderable::draw_opaque()
  {
      if (this->is_initialized() && this->is_visible())
      { draw_opaque_impl(); }
  }

  void AbstractSceneRenderable::draw_transparent()
  {
      if (this->is_initialized() && this->is_visible())
      { draw_transparent_impl(); }
  }

  void AbstractSceneRenderable::draw_impl()
  {
      draw_opaque();
      draw_transparent();
  }
  /// @}
} // namespace bk::details


