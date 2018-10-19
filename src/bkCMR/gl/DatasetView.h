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

#ifndef BK_DATASETVIEW_H
#define BK_DATASETVIEW_H

#include <string_view>

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkCMR/lib/bkCMR_export.h>
#include <bkCMR/gl/VesselView.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorBarView;

  inline namespace cmr
  {
    class Dataset;
    class Vessel;
    class VesselView;
    // -------------------- forward declaration END

    class BKCMR_EXPORT DatasetView : public details::AbstractSceneRenderable
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
        DatasetView();
        #else
        explicit DatasetView(bk::qt_gl_functions* gl);
        #endif
        DatasetView(const DatasetView& ) = delete;
        DatasetView(DatasetView&& ) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        ~DatasetView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] unsigned int num_vessels() const;

        /// @{ -------------------------------------------------- GET VESSELVIEW
        [[nodiscard]] VesselView* vesselview(unsigned int i);
        [[nodiscard]] const VesselView* vesselview(unsigned int i) const;
        [[nodiscard]] VesselView* vesselview(std::string_view name);
        [[nodiscard]] const VesselView* vesselview(std::string_view name) const;
        /// @}

        /// @{ -------------------------------------------------- GET COLORBARVIEW
        [[nodiscard]] ColorBarView* colorbarview_pathlines();
        [[nodiscard]] const ColorBarView* colorbarview_pathlines() const;
        [[nodiscard]] ColorBarView* colorbarview_measuringplanes();
        [[nodiscard]] const ColorBarView* colorbarview_measuringplanes() const;
        [[nodiscard]] ColorBarView* colorbarview_surface();
        [[nodiscard]] const ColorBarView* colorbarview_surface() const;
        [[nodiscard]] ColorBarView* colorbarview_flowjet();
        [[nodiscard]] const ColorBarView* colorbarview_flowjet() const;
        [[nodiscard]] ColorBarView* colorbarview_pressure();
        [[nodiscard]] const ColorBarView* colorbarview_pressure() const;
        /// @}

        [[nodiscard]] virtual Vec3<GLfloat> center() const override;

        [[nodiscard]] virtual bool is_initialized() const override;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] DatasetView& operator=(const DatasetView& )  = delete;
        [[maybe_unused]] DatasetView& operator=(DatasetView&& ) noexcept ;
        /// @}

        /// @{ -------------------------------------------------- ADD VESSEL
        [[maybe_unused]] VesselView* add_vessel(const Vessel& v, Dataset& ds, GLuint window_width, GLuint window_height, VesselViewComponent flags = VesselViewComponent_All);
        [[maybe_unused]] VesselView* add_vessel(std::string_view name);
        void add_vessels_from_dataset(Dataset& ds, GLuint window_width, GLuint window_height, VesselViewComponent flags = VesselViewComponent_All);
        /// @}

        /// @{ -------------------------------------------------- SET PATHLINE COLOR
        void set_line_color_attribute(const Dataset& ds, std::string_view color_attribute_name);
        void set_colorbar_pathlines_heat();
        void set_colorbar_pathlines_rainbow();
        void set_colorbar_pathlines_blue_to_red();
        void set_colorbar_pathlines_magenta();
        void set_colorbar_pathlines_uniform_yellow();
        void set_colorbar_pathlines_cluster();
        void show_colorbar_pathlines();
        void hide_colorbar_pathlines();
        /// @}

        /// @{ -------------------------------------------------- SET MEASURING PLANE COLOR
        void set_measuringplanes_color_attribute(const Dataset& ds, std::string_view color_attribute_name);
        void set_colorbar_measuringplanes_heat();
        void set_colorbar_measuringplanes_rainbow();
        void set_colorbar_measuringplanes_blue_to_red();
        void set_colorbar_measuringplanes_magenta();
        void set_colorbar_measuringplanes_uniform_yellow();
        void show_colorbar_measuringplanes();
        void hide_colorbar_measuringplanes();
        /// @}

        /// @{ -------------------------------------------------- SET SURFACE COLOR
        void set_surface_color_attribute(const Dataset& ds, std::string_view color_attribute_name);
        void set_colorbar_surface_heat();
        void set_colorbar_surface_rainbow();
        void set_colorbar_surface_blue_to_red();
        void set_colorbar_surface_magenta();
        void show_colorbar_surface();
        void hide_colorbar_surface();
        /// @}

        /// @{ -------------------------------------------------- SET FLOWJET COLOR
        void show_colorbar_flowjet();
        void hide_colorbar_flowjet();
        /// @}

        /// @{ -------------------------------------------------- SET PRESSURE COLOR
        void set_colorbar_pressure_blue_to_red();
        void update_colorbar_pressure();
        void show_colorbar_pressure();
        void hide_colorbar_pressure();
        /// @}
        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
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
        virtual void draw_opaque_impl() override;
        virtual void draw_transparent_impl() override;
        /// @}
    }; // class DatasetView
  } // inline namespace cmr
} // namespace bk

#endif //BK_DATASETVIEW_H
