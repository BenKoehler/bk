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

#ifndef BK_FLOWJETVIEW_H
#define BK_FLOWJETVIEW_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractSceneRenderable.h>

namespace bk
{
  inline namespace cmr
  {
    // -------------------- forward declaration
    class FlowJet;
    // -------------------- forward declaration END

    class FlowJetView : public details::AbstractSceneRenderable
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using base_type = details::AbstractSceneRenderable;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
        class Impl;

        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CONSTRUCTORS
        #ifndef BK_LIB_QT_AVAILABLE
        FlowJetView();
        #else
        FlowJetView(bk::qt_gl_functions* gl);
        #endif
        FlowJetView(const FlowJetView&) = delete;
        FlowJetView(FlowJetView&&) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~FlowJetView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET VISIBILITY
        [[nodiscard]] bool flow_jet_is_visible() const;
        [[nodiscard]] bool flow_jet_area_is_visible() const;
        /// @}

        /// @{ -------------------------------------------------- RELATIVE VELOCITY VISIBILITY THRESHOLD
        [[nodiscard]] GLfloat relative_velocity_visibility_threshold() const;
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
        /// @{ -------------------------------------------------- SET RELATIVE VELOCITY VISIBILITY THRESHOLD
        void set_relative_velocity_visibility_threshold(GLfloat p);
        /// @}

        /// @{ -------------------------------------------------- SET VISIBILITY
        void set_show_flow_jet(bool b);
        void show_flow_jet();
        void hide_flow_jet();

        void set_show_flow_jet_area(bool b);
        void show_flow_jet_area();
        void hide_flow_jet_area();
        /// @}

        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] FlowJetView& operator=(const FlowJetView&) = delete;
        [[maybe_unused]] FlowJetView& operator=(FlowJetView&&) noexcept;
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
        void init_flowjet(const std::vector<FlowJet>& fjs);
        void init_shader();
        void init_ubo();
        void init(const std::vector<FlowJet>& fjs, double msPerTime);
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
    }; // class FlowJetView
  } // inline namespace cmr
} // namespace bk

#endif //BK_FLOWJETVIEW_H
