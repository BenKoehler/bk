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

#ifndef BKGL_ABSTRACTSCENERENDERABLE_H
#define BKGL_ABSTRACTSCENERENDERABLE_H

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/renderable/AbstractRenderable.h>

namespace bk::details
{
  class BKGL_EXPORT AbstractSceneRenderable : public AbstractRenderable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractSceneRenderable;
      using base_type = AbstractRenderable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      AbstractSceneRenderable();
      #else
      AbstractSceneRenderable(bk::qt_gl_functions* gl);
      #endif
      AbstractSceneRenderable(const self_type&) = delete;
      AbstractSceneRenderable(self_type&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractSceneRenderable();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      /// @{ -------------------------------------------------- GET CENTER
      [[nodiscard]] virtual bk::Vec3<GLfloat> center() const = 0;
      /// @}

      /// @{ -------------------------------------------------- GET SIGNALS
      [[nodiscard]] Signal<>& signal_scene_changed();
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- SIGNALS
    protected:
      void emit_signal_scene_changed();
    public:
      /// @}

      /// @{ -------------------------------------------------- FORWARD CONNECTIONS OF OTHER RENDERABLE
    protected:
      virtual void _forward_signals(AbstractRenderable* r) override;
    public:
      /// @}

      /// @{ -------------------------------------------------- DRAW
      void draw_opaque();
      void draw_transparent();
    protected:
      virtual void draw_impl() final;
      virtual void draw_opaque_impl()=0;
      virtual void draw_transparent_impl()=0;
    public:
      /// @}
  }; // class AbstractSceneRenderable  
} // namespace bk::details

#endif //BKGL_ABSTRACTSCENERENDERABLE_H


