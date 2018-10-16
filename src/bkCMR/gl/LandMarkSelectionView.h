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

#ifndef BK_LANDMARKSELECTIONVIEW_H
#define BK_LANDMARKSELECTIONVIEW_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  // ------ forward declaration -------
  class ColorRGBA;
  class TriangularMesh3DView;
  class LineView;

  inline namespace cmr
  {
    class MeasuringPlanePreviewView;
    // ------ forward declaration -------

    class BKCMR_EXPORT LandMarkSelectionView : public bk::details::AbstractSceneRenderable
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = LandMarkSelectionView;
        using base_type = bk::details::AbstractSceneRenderable;

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
        LandMarkSelectionView();
        #else
        LandMarkSelectionView(bk::qt_gl_functions* gl);
        #endif
        LandMarkSelectionView(const self_type&) = delete;
        LandMarkSelectionView(self_type&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DTOR
        virtual ~LandMarkSelectionView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET VIEWS
        [[nodiscard]] TriangularMesh3DView& mesh_view();
        [[nodiscard]] const TriangularMesh3DView& mesh_view() const;

        [[nodiscard]] LineView& centerline_view();
        [[nodiscard]] const LineView& centerline_view() const;

        [[nodiscard]] MeasuringPlanePreviewView& measuring_plane_view();
        [[nodiscard]] const MeasuringPlanePreviewView& measuring_plane_view() const;
        /// @}

        /// @{ -------------------------------------------------- GET CENTER
        [[nodiscard]] virtual Vec3<GLfloat> center() const override;
        /// @}

        /// @{ -------------------------------------------------- IS INITIALIZED
        [[nodiscard]] virtual bool is_initialized() const override;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
        [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- CLEAR

        void clear();
        /// @}

        /// @{ -------------------------------------------------- INIT
        void init();
        /// @}

        /// @{ -------------------------------------------------- EVENTS
        virtual void on_resize(GLint w, GLint h) override;
        virtual void on_oit_enabled(bool b) override;
        virtual void on_animation_enabled(bool b) override;
        virtual void on_modelview_changed(bool) override;
        virtual void on_visible_changed(bool b) override;
        virtual void on_mouse_pos_changed(GLint x, GLint y) override;
        virtual void on_mouse_button_pressed(MouseButton_ btn) override;
        virtual void on_mouse_button_released(MouseButton_ btn) override;
        virtual void on_key_pressed(Key_ k) override;
        virtual void on_key_released(Key_ k) override;
        virtual void on_mouse_wheel_up() override;
        virtual void on_mouse_wheel_down() override;
        virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
        /// @}

        /// @{ -------------------------------------------------- DRAW
      protected:
        virtual void draw_opaque_impl() override;
        virtual void draw_transparent_impl() override;
      public:
        /// @}
    }; // class LandMarkSelectionView
  } // inline namespace cmr
} // namespace bk

#endif //BK_LANDMARKSELECTIONVIEW_H
