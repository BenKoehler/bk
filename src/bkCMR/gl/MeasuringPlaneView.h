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

#ifndef BK_MEASURINGPLANEVIEW_H
#define BK_MEASURINGPLANEVIEW_H

#include <string_view>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkCMR/lib/bkCMR_export.h>
#include <bkCMR/dataset/MeasuringPlane.h>

namespace bk
{
  // ------ forward declaration -------
  class ColorRGBA;
  class ColorBarRGBA;
  // ------ forward declaration -------

  inline namespace cmr
  {
    class BKCMR_EXPORT MeasuringPlaneView : public details::AbstractSceneRenderable
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using base_type = details::AbstractSceneRenderable;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      protected:
        class Impl;
        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CONSTRUCTORS
        #ifndef BK_LIB_QT_AVAILABLE

        MeasuringPlaneView();
        #else
        MeasuringPlaneView(bk::qt_gl_functions* gl);
        #endif
        MeasuringPlaneView(const MeasuringPlaneView& ) = delete;
        MeasuringPlaneView(MeasuringPlaneView&& ) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~MeasuringPlaneView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET COLOR
        [[nodiscard]] const ColorRGBA& color() const;
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
        /// @{ -------------------------------------------------- SET LINE WIDTH
        void set_line_width(GLfloat w);
        /// @}

        /// @{ -------------------------------------------------- SET COLOR
      private:
        void _update_ubo_color();
      public:
        void set_color(const ColorRGBA& c);
        void set_color(double r, double g, double b, double a = 1.0);

      private:
        void _set_colorbar(const ColorBarRGBA& colbar);
      public:

        void set_colorbar_heat();
        void set_colorbar_rainbow();
        void set_colorbar_blue_to_red();
        void set_colorbar_magenta();
        void set_colorbar_uniform_yellow();
        void set_colorbar_min_max_value(double vmin, double vmax);
        /// @}

        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] MeasuringPlaneView& operator=(const MeasuringPlaneView& ) = delete;
        [[maybe_unused]] MeasuringPlaneView& operator=(MeasuringPlaneView&& ) noexcept ;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- CLEAR
        void clear_shader();
        void clear_buffers();
        void clear();
        /// @}

        /// @{ -------------------------------------------------- INIT
        void set_plane(const MeasuringPlane& plane);
        void set_color_attribute(const MeasuringPlane& plane, std::string_view color_attribute_name = MeasuringPlane::VelocityThroughPlaneAttributeName());
        void reset_plane();
        void init_plane();
        void init_shader();
        void init_ubo();
        void init(const MeasuringPlane& plane);
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
    }; // class MeasuringPlaneView
  } // inline namespace cmr
} // namespace bk

#endif //BK_MEASURINGPLANEVIEW_H
