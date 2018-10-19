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

#include <bkCMR/gl/DatasetView.h>

#include <algorithm>
#include <memory>
#include <vector>

#include <bk/Localization>
#include <bk/StringUtils>
#include <bk/Signal>

#include <bkCMR/dataset/Dataset.h>
#include <bkCMR/dataset/Vessel.h>
#include <bkCMR/gl/VesselView.h>
#include <bkCMR/gl/FlowJetView.h>
#include <bkCMR/gl/PressureView.h>
#include <bkCMR/gl/MeasuringPlaneView.h>
#include <bkGL/renderable/mesh/TriangularMesh3DView.h>
#include <bkGL/renderable/line/LineView.h>
#include <bkGL/renderable/ColorBarView.h>

namespace bk
{
  inline namespace cmr
  {
    struct DatasetView::Impl
    {
        ColorBarView colorbarview_pathlines;
        ColorBarView colorbarview_measuringplanes;
        ColorBarView colorbarview_surface;
        ColorBarView colorbarview_flowjet;
        ColorBarView colorbarview_pressure;
        std::vector<std::unique_ptr<VesselView>> vessels;

            #ifndef BK_LIB_QT_AVAILABLE

        Impl()
            #else

        explicit Impl(bk::qt_gl_functions* gl)
          : colorbarview_pathlines(gl),
            colorbarview_measuringplanes(gl),
            colorbarview_surface(gl),
            colorbarview_flowjet(gl),
            colorbarview_pressure(gl)
            #endif
        { /* do nothing */ }

        Impl(const Impl&) = delete;
        Impl(Impl&&) noexcept = default;
        ~Impl() = default;
        Impl& operator=(const Impl&) = delete;
        Impl& operator=(Impl&&) noexcept = default;
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    #ifndef BK_LIB_QT_AVAILABLE

    DatasetView::DatasetView()
    #else
    DatasetView::DatasetView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
    #endif
    {
        /*
         * left side:
         * [pressure]
         * [measuring planes]
         * [pathlines]
         */
        _pdata->colorbarview_pressure.set_position_vertical(2);
        _pdata->colorbarview_pressure.set_position_horizontal_left();
        _pdata->colorbarview_pressure.set_hidden();

        _pdata->colorbarview_measuringplanes.set_position_vertical(1);
        _pdata->colorbarview_measuringplanes.set_position_horizontal_left();
        _pdata->colorbarview_measuringplanes.set_hidden();

        _pdata->colorbarview_pathlines.set_position_vertical(0);
        _pdata->colorbarview_pathlines.set_position_horizontal_left();
        _pdata->colorbarview_pathlines.set_hidden();

        /*
         * right side:
         * [flow jet]
         * [surface]
         */
        _pdata->colorbarview_flowjet.set_position_vertical(1);
        _pdata->colorbarview_flowjet.set_position_horizontal_right();
        _pdata->colorbarview_flowjet.set_title(___("Displacement [%]"));
        _pdata->colorbarview_flowjet.set_value_range(0, 1, true);
        _pdata->colorbarview_flowjet.set_hidden();

        _pdata->colorbarview_surface.set_position_vertical(0);
        _pdata->colorbarview_surface.set_position_horizontal_right();
        _pdata->colorbarview_surface.set_hidden();
    }

    DatasetView::DatasetView(DatasetView&&) noexcept = default;
    DatasetView::~DatasetView() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    unsigned int DatasetView::num_vessels() const
    { return _pdata->vessels.size(); }

    VesselView* DatasetView::vesselview(unsigned int i)
    { return i < _pdata->vessels.size() ? _pdata->vessels[i].get() : nullptr; }

    const VesselView* DatasetView::vesselview(unsigned int i) const
    { return i < _pdata->vessels.size() ? _pdata->vessels[i].get() : nullptr; }

    VesselView* DatasetView::vesselview(std::string_view name)
    {
        if (_pdata->vessels.empty())
        { return nullptr; }

        auto it = std::find_if(_pdata->vessels.begin(), _pdata->vessels.end(), [&](const std::unique_ptr<VesselView>& vv) -> bool
        { return string_utils::equals(vv->name(), name, false /*case insensitive*/); });

        return it != _pdata->vessels.end() ? it->get() : nullptr;
    }

    const VesselView* DatasetView::vesselview(std::string_view name) const
    {
        if (_pdata->vessels.empty())
        { return nullptr; }

        auto it = std::find_if(_pdata->vessels.begin(), _pdata->vessels.end(), [&](const std::unique_ptr<VesselView>& vv) -> bool
        { return string_utils::equals(vv->name(), name, false /*case insensitive*/); });

        return it != _pdata->vessels.end() ? it->get() : nullptr;
    }

    ColorBarView* DatasetView::colorbarview_pathlines()
    { return &_pdata->colorbarview_pathlines; }

    const ColorBarView* DatasetView::colorbarview_pathlines() const
    { return &_pdata->colorbarview_pathlines; }

    ColorBarView* DatasetView::colorbarview_measuringplanes()
    { return &_pdata->colorbarview_measuringplanes; }

    const ColorBarView* DatasetView::colorbarview_measuringplanes() const
    { return &_pdata->colorbarview_measuringplanes; }

    ColorBarView* DatasetView::colorbarview_surface()
    { return &_pdata->colorbarview_surface; }

    const ColorBarView* DatasetView::colorbarview_surface() const
    { return &_pdata->colorbarview_surface; }

    ColorBarView* DatasetView::colorbarview_flowjet()
    { return &_pdata->colorbarview_flowjet; }

    const ColorBarView* DatasetView::colorbarview_flowjet() const
    { return &_pdata->colorbarview_flowjet; }

    ColorBarView* DatasetView::colorbarview_pressure()
    { return &_pdata->colorbarview_pressure; }

    const ColorBarView* DatasetView::colorbarview_pressure() const
    { return &_pdata->colorbarview_pressure; }

    Vec3<GLfloat> DatasetView::center() const
    {
        Vec3<GLfloat> c(0, 0, 0);
        unsigned int cnt = 0;

        for (auto& v: _pdata->vessels)
        {
            if (v->is_initialized())
            {
                c += v->center();
                ++cnt;
            }

            if (cnt > 1)
            { c /= cnt; }
        }

        return c;
    }

    bool DatasetView::is_initialized() const
    {
        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            if (_pdata->vessels[i]->is_initialized())
            { return true; }
        }

        return false;
    }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    DatasetView& DatasetView::operator=(DatasetView&&) noexcept = default;

    VesselView* DatasetView::add_vessel(const Vessel& v, Dataset& ds, GLuint window_width, GLuint window_height, VesselViewComponent flags)
    {
        auto vv = add_vessel(v.name());
        vv->init(v, ds, window_width, window_height, flags);

        this->emit_signal_update_required();

        return vv;
    }

    VesselView* DatasetView::add_vessel(std::string_view name)
    {
        #ifndef BK_LIB_QT_AVAILABLE
        std::unique_ptr<VesselView> vv = std::make_unique<VesselView>();
        #else
        std::unique_ptr < VesselView > vv = std::make_unique<VesselView>(this->_gl);
        #endif

        _pdata->vessels.emplace_back(std::move(vv));
        _pdata->vessels.back()->set_name(name);

        this->forward_signals(_pdata->vessels.back().get());
        this->forward_settings(_pdata->vessels.back().get());

        this->emit_signal_scene_changed();

        return _pdata->vessels.back().get();
    }

    void DatasetView::add_vessels_from_dataset(Dataset& ds, GLuint window_width, GLuint window_height, VesselViewComponent flags)
    {
        _pdata->vessels.clear();

        for (unsigned int i = 0; i < ds.num_vessels(); ++i)
        { add_vessel(*ds.vessel(i), ds, window_width, window_height, flags); }

        this->emit_signal_scene_changed();
        this->emit_signal_update_required();
    }

    void DatasetView::set_line_color_attribute(const Dataset& ds, std::string_view color_attribute_name)
    {
        double vmin = std::numeric_limits<double>::max();
        double vmax = -vmin;

        for (unsigned int i = 0; i < ds.num_vessels(); ++i)
        {
            auto v = ds.vessel(i);

            for (auto& vv: _pdata->vessels)
            {
                if (string_utils::equals(v->name(), vv->name()))
                { vv->set_line_color_attribute(*v, color_attribute_name); }
            }

            const auto[attribMin, attribMax] = v->min_max_scalar_pathline_point_attribute(color_attribute_name);
            vmin = std::min(attribMin, vmin);
            vmax = std::max(attribMax, vmax);
        }

        _pdata->colorbarview_pathlines.set_title(color_attribute_name);
        _pdata->colorbarview_pathlines.set_value_range(vmin, vmax, true);

        //_pdata->show_colorbar_pathlines = !color_attribute_name.empty();
        _pdata->colorbarview_pathlines.set_visible(!color_attribute_name.empty());

        for (auto& vv: _pdata->vessels)
        {
            vv->pathlineview().set_color_attribute_min(vmin);
            vv->pathlineview().set_color_attribute_max(vmax);
        }
    }

    void DatasetView::set_colorbar_pathlines_heat()
    {
        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(true);
        _pdata->colorbarview_pathlines.init_heat();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_heat();
            pv.set_attrib_scale_to_colorbar(true);
        }
    }

    void DatasetView::set_colorbar_pathlines_rainbow()
    {
        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(true);
        _pdata->colorbarview_pathlines.init_rainbow();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_rainbow();
            pv.set_attrib_scale_to_colorbar(true);
        }
    }

    void DatasetView::set_colorbar_pathlines_blue_to_red()
    {
        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(true);
        _pdata->colorbarview_pathlines.init_blue_to_red();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_blue_to_red();
            pv.set_attrib_scale_to_colorbar(true);
        }
    }

    void DatasetView::set_colorbar_pathlines_magenta()
    {
        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(true);
        _pdata->colorbarview_pathlines.init_magenta();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_magenta();
            pv.set_attrib_scale_to_colorbar(true);
        }
    }

    void DatasetView::set_colorbar_pathlines_uniform_yellow()
    {
        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(true);
        _pdata->colorbarview_pathlines.init_uniform_yellow();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_uniform_yellow();
            pv.set_attrib_scale_to_colorbar(true);
        }
    }

    void DatasetView::set_colorbar_pathlines_cluster()
    {
        double maxNumClusters = 0;

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            auto vv = vesselview(i);
            auto& pv = vv->pathlineview();
            pv.set_colorbar_cluster();
            pv.set_attrib_scale_to_colorbar(false);

            maxNumClusters = std::max(maxNumClusters, static_cast<double>(pv.color_attribute_max()));
        }

        _pdata->colorbarview_pathlines.set_linear_color_interpolation_enabled(false);
        _pdata->colorbarview_pathlines.init_cluster(maxNumClusters + 1);
    }

    void DatasetView::show_colorbar_pathlines()
    { _pdata->colorbarview_pathlines.set_visible(); }

    void DatasetView::hide_colorbar_pathlines()
    { _pdata->colorbarview_pathlines.set_hidden(); }

    void DatasetView::set_measuringplanes_color_attribute(const Dataset& ds, std::string_view color_attribute_name)
    {
        double vmin = std::numeric_limits<double>::max();
        double vmax = -vmin;

        for (unsigned int i = 0; i < ds.num_vessels(); ++i)
        {
            auto v = ds.vessel(i);

            const auto[attribMin, attribMax] = v->min_max_scalar_measuring_plane_point_attribute(color_attribute_name);
            vmin = std::min(vmin, attribMin);
            vmax = std::max(vmax, attribMax);
        }

        const double absvmax = std::max(std::abs(vmin), std::abs(vmax));

        _pdata->colorbarview_measuringplanes.set_title(color_attribute_name);
        _pdata->colorbarview_measuringplanes.set_value_range(-absvmax, +absvmax, true);
        _pdata->colorbarview_measuringplanes.init_blue_to_red();

        _pdata->colorbarview_measuringplanes.set_visible(!color_attribute_name.empty());

        for (auto& vv: _pdata->vessels)
        {
            for (auto& mpv: vv->measuringplaneviews())
            { mpv.set_colorbar_min_max_value(-absvmax, +absvmax); }
        }
    }

    void DatasetView::set_colorbar_measuringplanes_heat()
    {
        _pdata->colorbarview_measuringplanes.init_heat();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            for (auto& mp: vesselview(i)->measuringplaneviews())
            { mp.set_colorbar_heat(); }
        }
    }

    void DatasetView::set_colorbar_measuringplanes_rainbow()
    {
        _pdata->colorbarview_measuringplanes.init_rainbow();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            for (auto& mp: vesselview(i)->measuringplaneviews())
            { mp.set_colorbar_rainbow(); }
        }
    }

    void DatasetView::set_colorbar_measuringplanes_blue_to_red()
    {
        _pdata->colorbarview_measuringplanes.init_blue_to_red();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            for (auto& mp: vesselview(i)->measuringplaneviews())
            { mp.set_colorbar_blue_to_red(); }
        }
    }

    void DatasetView::set_colorbar_measuringplanes_magenta()
    {
        _pdata->colorbarview_measuringplanes.init_magenta();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            for (auto& mp: vesselview(i)->measuringplaneviews())
            { mp.set_colorbar_magenta(); }
        }
    }

    void DatasetView::set_colorbar_measuringplanes_uniform_yellow()
    {
        _pdata->colorbarview_measuringplanes.init_uniform_yellow();

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            for (auto& mp: vesselview(i)->measuringplaneviews())
            { mp.set_colorbar_uniform_yellow(); }
        }
    }

    void DatasetView::show_colorbar_measuringplanes()
    { _pdata->colorbarview_measuringplanes.set_visible(); }

    void DatasetView::hide_colorbar_measuringplanes()
    { _pdata->colorbarview_measuringplanes.set_hidden(); }

    void DatasetView::set_surface_color_attribute(const Dataset& ds, std::string_view color_attribute_name)
    {
        for (unsigned int vid = 0; vid < ds.num_vessels(); ++vid)
        {
            const Vessel* v = ds.vessel(vid);

            if (!v->has_mesh())
            { continue; }

            auto vv = vesselview(vid);
            if (vv == nullptr)
            { continue; }

            vv->meshview().init(v->mesh(), color_attribute_name);
        }

        double vmin = std::numeric_limits<double>::max();
        double vmax = -vmin;

        for (unsigned int i = 0; i < ds.num_vessels(); ++i)
        {
            auto v = ds.vessel(i);

            const auto[attribMin, attribMax] = v->min_max_scalar_mesh_point_attribute(color_attribute_name);
            vmin = std::min(vmin, attribMin);
            vmax = std::max(vmax, attribMax);
        }

        _pdata->colorbarview_surface.set_title(color_attribute_name);
        _pdata->colorbarview_surface.set_value_range(vmin, vmax, true);
        _pdata->colorbarview_surface.init_magenta();

        _pdata->colorbarview_surface.set_visible(!color_attribute_name.empty());

        for (auto& vv: _pdata->vessels)
        { vv->meshview().set_colorbar_min_max_value(vmin, vmax); }
    }

    void DatasetView::set_colorbar_surface_heat()
    {
        _pdata->colorbarview_surface.init_heat();

        for (auto& vv: _pdata->vessels)
        { vv->meshview().set_colorbar_heat(); }
    }

    void DatasetView::set_colorbar_surface_rainbow()
    {
        _pdata->colorbarview_surface.init_rainbow();

        for (auto& vv: _pdata->vessels)
        { vv->meshview().set_colorbar_rainbow(); }
    }

    void DatasetView::set_colorbar_surface_blue_to_red()
    {
        _pdata->colorbarview_surface.init_blue_to_red();

        for (auto& vv: _pdata->vessels)
        { vv->meshview().set_colorbar_blue_to_red(); }
    }

    void DatasetView::set_colorbar_surface_magenta()
    {
        _pdata->colorbarview_surface.init_magenta();

        for (auto& vv: _pdata->vessels)
        { vv->meshview().set_colorbar_magenta(); }
    }

    void DatasetView::show_colorbar_surface()
    { _pdata->colorbarview_surface.set_visible(); }

    void DatasetView::hide_colorbar_surface()
    { _pdata->colorbarview_surface.set_hidden(); }

    void DatasetView::show_colorbar_flowjet()
    {
        if (!_pdata->colorbarview_flowjet.is_initialized())
        { _pdata->colorbarview_flowjet.init_traffic_light(); }

        _pdata->colorbarview_flowjet.set_visible();
    }

    void DatasetView::hide_colorbar_flowjet()
    { _pdata->colorbarview_flowjet.set_hidden(); }

    void DatasetView::set_colorbar_pressure_blue_to_red()
    {
        _pdata->colorbarview_pressure.init_blue_to_red();

        _pdata->colorbarview_pressure.set_title(___("Relative Pressure [mmHg]"));
        update_colorbar_pressure();
    }

    void DatasetView::update_colorbar_pressure()
    {
        GLfloat v = 0;

        for (auto& vv: _pdata->vessels)
        {
            v = std::max(v, std::abs(vv->pressureview().min_value()));
            v = std::max(v, std::abs(vv->pressureview().max_value()));
        }

        for (auto& vv: _pdata->vessels)
        { vv->pressureview().set_scale_val(v); }

        _pdata->colorbarview_pressure.set_value_range(-v, v, true);
    }

    void DatasetView::show_colorbar_pressure()
    { _pdata->colorbarview_pressure.set_visible(); }

    void DatasetView::hide_colorbar_pressure()
    { _pdata->colorbarview_pressure.set_hidden(); }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void DatasetView::clear()
    {
        for (auto& vv: _pdata->vessels)
        { vv->clear(); }

        _pdata->vessels.clear();

        if (this->is_initialized())
        {
            this->emit_signal_scene_changed();
            this->emit_signal_update_required();
        }
    }

    void DatasetView::on_resize(GLint w, GLint h)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_resize(w, h); }
    }

    void DatasetView::on_oit_enabled(bool b)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_oit_enabled(b); }
    }

    void DatasetView::on_animation_enabled(bool b)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_animation_enabled(b); }
    }

    void DatasetView::on_modelview_changed(bool b)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_modelview_changed(b); }
    }

    void DatasetView::on_visible_changed(bool b)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_visible_changed(b); }
    }

    void DatasetView::on_mouse_pos_changed(GLint x, GLint y)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_mouse_pos_changed(x, y); }
    }

    void DatasetView::on_mouse_button_pressed(MouseButton_ btn)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_mouse_button_pressed(btn); }
    }

    void DatasetView::on_mouse_button_released(MouseButton_ btn)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_mouse_button_released(btn); }
    }

    void DatasetView::on_key_pressed(Key_ k)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_key_pressed(k); }
    }

    void DatasetView::on_key_released(Key_ k)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_key_released(k); }
    }

    void DatasetView::on_mouse_wheel_up()
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_mouse_wheel_up(); }
    }

    void DatasetView::on_mouse_wheel_down()
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_mouse_wheel_down(); }
    }

    void DatasetView::on_ssaa_factor_changed(GLint ssaa_factor)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_ssaa_factor_changed(ssaa_factor); }
    }

    void DatasetView::on_animation_time_changed(double d)
    {
        for (auto& vv: _pdata->vessels)
        { vv->on_animation_time_changed(d); }
    }

    void DatasetView::draw_opaque_impl()
    {
        _pdata->colorbarview_pathlines.draw();
        _pdata->colorbarview_measuringplanes.draw();
        _pdata->colorbarview_surface.draw();
        _pdata->colorbarview_flowjet.draw();
        _pdata->colorbarview_pressure.draw();

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->pressureview().draw_opaque(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->meshview().draw_opaque(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->centerlineview().draw_opaque(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->flowjetview().draw_opaque(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            {
                for (auto& mp: vv->measuringplaneviews())
                { mp.draw_opaque(); }
            }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->pathlineview().draw_opaque(); }
        }
    }

    void DatasetView::draw_transparent_impl()
    {

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->pressureview().draw_transparent(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->meshview().draw_transparent(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->centerlineview().draw_transparent(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->flowjetview().draw_transparent(); }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            {
                for (auto& mp: vv->measuringplaneviews())
                { mp.draw_transparent(); }
            }
        }

        for (auto& vv: _pdata->vessels)
        {
            if (vv->is_visible())
            { vv->pathlineview().draw_transparent(); }
        }
    }
  } // inline namespace cmr
} // namespace bk