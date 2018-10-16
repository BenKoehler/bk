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

#include <bkCMR/gl/LandMarkSelectionView.h>

//#include <bkCMR/EVesselSemantic.h>
#include <bkCMR/gl/MeasuringPlanePreviewView.h>
#include <bkGL/renderable/mesh/TriangularMesh3DView.h>
#include <bkGL/renderable/line/LineView.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct LandMarkSelectionView::Impl
    {
        TriangularMesh3DView mesh_view;
        LineView cl_view;
        MeasuringPlanePreviewView mp_view;

            #ifndef BK_LIB_QT_AVAILABLE

        Impl()
            #else
        explicit Impl(bk::qt_gl_functions* gl)
        : mesh_view(gl),
          cl_view(gl),
          mp_view(gl)
            #endif
        { /* do nothing */ }
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    LandMarkSelectionView::LandMarkSelectionView()
        : base_type()
    #else

    LandMarkSelectionView::LandMarkSelectionView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
    #endif
    { /* do nothing */ }

    LandMarkSelectionView::LandMarkSelectionView(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    LandMarkSelectionView::~LandMarkSelectionView() = default;
    /// @}

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- GET VIEWS
    TriangularMesh3DView& LandMarkSelectionView::mesh_view()
    { return _pdata->mesh_view; }

    const TriangularMesh3DView& LandMarkSelectionView::mesh_view() const
    { return _pdata->mesh_view; }

    LineView& LandMarkSelectionView::centerline_view()
    { return _pdata->cl_view; }

    const LineView& LandMarkSelectionView::centerline_view() const
    { return _pdata->cl_view; }

    MeasuringPlanePreviewView& LandMarkSelectionView::measuring_plane_view()
    { return _pdata->mp_view; }

    const MeasuringPlanePreviewView& LandMarkSelectionView::measuring_plane_view() const
    { return _pdata->mp_view; }
    /// @}

    /// @{ -------------------------------------------------- GET CENTER
    Vec3<GLfloat> LandMarkSelectionView::center() const
    {return _pdata->mesh_view.center();}
    /// @}

    /// @{ -------------------------------------------------- IS INITIALIZED
    bool LandMarkSelectionView::is_initialized() const
    {return _pdata->mesh_view.is_initialized();}
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- OPERATOR =
    auto LandMarkSelectionView::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    /// @{ -------------------------------------------------- CLEAR
    void LandMarkSelectionView::clear()
    {
        _pdata->mesh_view.clear();
        _pdata->cl_view.clear();
        _pdata->mp_view.clear();

        this->emit_signal_scene_changed();
        this->emit_signal_update_required();
    }
    /// @}

    /// @{ -------------------------------------------------- INIT
    void LandMarkSelectionView::init()
    {
        // todo
    }
    /// @}

    /// @{ -------------------------------------------------- EVENTS
    void LandMarkSelectionView::on_resize(GLint w, GLint h)
    {
        _pdata->mesh_view.on_resize(w,h);
        _pdata->cl_view.on_resize(w,h);
        _pdata->mp_view.on_resize(w,h);
    }

    void LandMarkSelectionView::on_oit_enabled(bool b)
    {
        _pdata->mesh_view.on_oit_enabled(b);
        _pdata->cl_view.on_oit_enabled(b);
        _pdata->mp_view.on_oit_enabled(b);
    }

    void LandMarkSelectionView::on_animation_enabled(bool b)
    {
        _pdata->mesh_view.on_animation_enabled(b);
        _pdata->cl_view.on_animation_enabled(b);
        _pdata->mp_view.on_animation_enabled(b);
    }

    void LandMarkSelectionView::on_modelview_changed(bool b)
    {
        _pdata->mesh_view.on_modelview_changed(b);
        _pdata->cl_view.on_modelview_changed(b);
        _pdata->mp_view.on_modelview_changed(b);
    }

    void LandMarkSelectionView::on_visible_changed(bool b)
    {
        _pdata->mesh_view.on_visible_changed(b);
        _pdata->cl_view.on_visible_changed(b);
        _pdata->mp_view.on_visible_changed(b);
    }

    void LandMarkSelectionView::on_mouse_pos_changed(GLint x, GLint y)
    {
        _pdata->mesh_view.on_mouse_pos_changed(x,y);
        _pdata->cl_view.on_mouse_pos_changed(x,y);
        _pdata->mp_view.on_mouse_pos_changed(x,y);
    }

    void LandMarkSelectionView::on_mouse_button_pressed(MouseButton_ btn)
    {
        _pdata->mesh_view.on_mouse_button_pressed(btn);
        _pdata->cl_view.on_mouse_button_pressed(btn);
        _pdata->mp_view.on_mouse_button_pressed(btn);
    }

    void LandMarkSelectionView::on_mouse_button_released(MouseButton_ btn)
    {
        _pdata->mesh_view.on_mouse_button_released(btn);
        _pdata->cl_view.on_mouse_button_released(btn);
        _pdata->mp_view.on_mouse_button_released(btn);
    }

    void LandMarkSelectionView::on_key_pressed(Key_ k)
    {
        _pdata->mesh_view.on_key_pressed(k);
        _pdata->cl_view.on_key_pressed(k);
        _pdata->mp_view.on_key_pressed(k);
    }

    void LandMarkSelectionView::on_key_released(Key_ k)
    {
        _pdata->mesh_view.on_key_released(k);
        _pdata->cl_view.on_key_released(k);
        _pdata->mp_view.on_key_released(k);
    }

    void LandMarkSelectionView::on_mouse_wheel_up()
    {
        _pdata->mesh_view.on_mouse_wheel_up();
        _pdata->cl_view.on_mouse_wheel_up();
        _pdata->mp_view.on_mouse_wheel_up();
    }

    void LandMarkSelectionView::on_mouse_wheel_down()
    {
        _pdata->mesh_view.on_mouse_wheel_down();
        _pdata->cl_view.on_mouse_wheel_down();
        _pdata->mp_view.on_mouse_wheel_down();
    }

    void LandMarkSelectionView::on_ssaa_factor_changed(GLint ssaa_factor)
    {
        _pdata->mesh_view.on_ssaa_factor_changed(ssaa_factor);
        _pdata->cl_view.on_ssaa_factor_changed(ssaa_factor);
        _pdata->mp_view.on_ssaa_factor_changed(ssaa_factor);
    }
    /// @}

    /// @{ -------------------------------------------------- DRAW
    void LandMarkSelectionView::draw_opaque_impl()
    {
        _pdata->mesh_view.draw_opaque();
        _pdata->cl_view.draw_opaque();
        _pdata->mp_view.draw_opaque();
    }

    void LandMarkSelectionView::draw_transparent_impl()
    {
        _pdata->mesh_view.draw_transparent();
        _pdata->cl_view.draw_transparent();
        _pdata->mp_view.draw_transparent();
    }
    /// @}
  } // inline namespace cmr
} // namespace bk