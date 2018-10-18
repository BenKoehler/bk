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

#ifndef BK_VESSELVIEW_H
#define BK_VESSELVIEW_H

#include <string>
#include <string_view>

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bk/Matrix>
#include <bkCMR/lib/bkCMR_export.h>

namespace bk
{
  // -------------------- forward declaration
  class TriangularMesh3DView;
  class LineView;

  inline namespace cmr
  {
    class Vessel;
    class Dataset;
    class FlowJetView;
    class PressureView;
    class MeasuringPlaneView;
    // -------------------- forward declaration END
    
    class BKCMR_EXPORT VesselView : public details::AbstractSceneRenderable
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
        VesselView();
        #else
        explicit VesselView(bk::qt_gl_functions* gl);
        #endif
        VesselView(const VesselView& ) = delete;
        VesselView(VesselView&& ) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~VesselView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET NAME
        [[nodiscard]] const std::string& name() const;
        /// @}

        /// @{ -------------------------------------------------- GET MESHVIEW
        [[nodiscard]] TriangularMesh3DView& meshview();
        [[nodiscard]] const TriangularMesh3DView& meshview() const;
        /// @}

        /// @{ -------------------------------------------------- GET PATHLINEVIEW
        [[nodiscard]] LineView& pathlineview();
        [[nodiscard]] const LineView& pathlineview() const;
        /// @}

        /// @{ -------------------------------------------------- GET CENTERLINEVIEW
        [[nodiscard]] LineView& centerlineview();
        [[nodiscard]] const LineView& centerlineview() const;
        /// @}

        /// @{ -------------------------------------------------- GET FLOWJETVIEW
        [[nodiscard]] FlowJetView& flowjetview();
        [[nodiscard]] const FlowJetView& flowjetview() const;
        /// @}

        /// @{ -------------------------------------------------- GET PRESSUREVIEW
        [[nodiscard]] PressureView& pressureview();
        [[nodiscard]] const PressureView& pressureview() const;
        /// @}

        /// @{ -------------------------------------------------- GET MEASURING PLANES VIEW
        [[nodiscard]] std::vector<MeasuringPlaneView>& measuringplaneviews();
        [[nodiscard]] const std::vector<MeasuringPlaneView>& measuringplaneviews() const;
        [[nodiscard]] MeasuringPlaneView* measuringplaneview(unsigned int i);
        [[nodiscard]] const MeasuringPlaneView* measuringplaneview(unsigned int i) const;
        /// @}

        /// @{ -------------------------------------------------- IS INITIALIZED
        [[nodiscard]] virtual bool is_initialized() const override;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        void set_vessel(const Vessel& v, Dataset& ds, std::string_view line_color_attribute_name = "");
        void init_measuringplanes(const Vessel& v);

        /// @{ -------------------------------------------------- SET NAME
        void set_name(std::string_view n);
        /// @}

        /// @{ -------------------------------------------------- COLOR ATTRIBUTES
        void set_line_color_attribute(const Vessel& v, std::string_view color_attribute_name);
        void set_measuringplane_color_attribute(const Vessel& v, std::string_view color_attribute_name);
        /// @}

        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] VesselView& operator=(const VesselView& )  = delete;
        [[maybe_unused]] VesselView& operator=(VesselView&& ) noexcept ;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- GET CENTER
        [[nodiscard]] virtual Vec3<GLfloat> center() const override;
        /// @}


        /// @{ -------------------------------------------------- MODES
        //void determine_visualization_mode();
        //void set_visualization_mode_default();
        //void set_visualization_mode_line_quantification();
        /// @}

        void clear();

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
        void on_animation_time_changed(double d);
        /// @}

        /// @{ -------------------------------------------------- DRAW
      protected:
        virtual void draw_opaque_impl() override;
        virtual void draw_transparent_impl() override;
      public:
        /// @}
    }; // class VesselView 
  } // inline namespace cmr
} // namespace bk

#endif //BK_VESSELVIEW_H
