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

#include <bkCMR/gl/MeasuringPlanePreviewView.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include <bk/Color>

#include<bkGL/buffer/VBO.h>
#include<bkGL/buffer/UBO.h>
#include<bkGL/shader/Shader.h>
#include<bkGL/vao/VAO.h>
#include <bkCMR/gl/UBOMeasuringPlanePreview.h>
#include<bkCMR/gl/ShaderLibrary_CMR.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct MeasuringPlanePreviewView::Impl
    {
        VBO vbo;
        VAO vao;
        UBOMeasuringPlanePreview ubo;
        Shader shader;
        GLfloat shininess;
        GLfloat silhouette_width;
        ColorRGBA color;
        Vec3<GLfloat> center;
        //
        Vec3<GLfloat> pos;
        Vec3<GLfloat> nx;
        Vec3<GLfloat> ny;
        Vec3<GLfloat> nz;
        GLfloat radius;
        GLfloat radius_scale;

            #ifndef BK_LIB_QT_AVAILABLE

        Impl()
            :
            #else
        explicit Impl(bk::qt_gl_functions* gl)
          : vbo(gl),
            vao(gl),
            ubo(gl),
            shader(gl),
            #endif
            shininess(50),
            silhouette_width(0.3),
            color(ColorRGBA::Yellow()),
            center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            pos(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            nx(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            ny(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            nz(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            radius(0),
            radius_scale(3)
        { /* do nothing */ }
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    /// @{ -------------------------------------------------- CTOR
    #ifndef BK_LIB_QT_AVAILABLE

    MeasuringPlanePreviewView::MeasuringPlanePreviewView()
        : base_type()
    #else

    MeasuringPlanePreviewView::MeasuringPlanePreviewView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
    #endif
    {
        _pdata->vbo.set_usage_STATIC_DRAW();
        _pdata->vao.add_default_attribute_position_3xfloat();
        _pdata->vao.add_default_attribute_normal_3xfloat();
    }

    MeasuringPlanePreviewView::MeasuringPlanePreviewView(self_type&&) noexcept = default;
    /// @}

    /// @{ -------------------------------------------------- DTOR
    MeasuringPlanePreviewView::~MeasuringPlanePreviewView() = default;
    /// @}

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- GET POSITION
    const Vec3<GLfloat>& MeasuringPlanePreviewView::position() const
    { return _pdata->pos; }

    const Vec3<GLfloat>& MeasuringPlanePreviewView::plane_normal() const
    { return _pdata->nz; }

    const Vec3<GLfloat>& MeasuringPlanePreviewView::plane_nx() const
    { return _pdata->nx; }

    const Vec3<GLfloat>& MeasuringPlanePreviewView::plane_ny() const
    { return _pdata->ny; }

    GLfloat MeasuringPlanePreviewView::vessel_radius() const
    { return _pdata->radius; }
    /// @}

    /// @{ -------------------------------------------------- GET RADIUS SCALE
    [[nodiscard]] GLfloat MeasuringPlanePreviewView::radius_scale() const
    { return _pdata->radius_scale; }
    /// @}

    /// @{ -------------------------------------------------- GET COLOR
    [[nodiscard]] const ColorRGBA& MeasuringPlanePreviewView::color() const
    { return _pdata->color; }
    /// @}

    /// @{ -------------------------------------------------- GET CENTER
    auto MeasuringPlanePreviewView::center() const -> Vec3<GLfloat>
    { return _pdata->center; }
    /// @}

    /// @{ -------------------------------------------------- IS INITIALIZED
    bool MeasuringPlanePreviewView::is_initialized() const
    { return _pdata->vao.is_initialized(); }
    /// @}

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    /// @{ -------------------------------------------------- SET POSITION
    void MeasuringPlanePreviewView::set_position(const Vec3<GLfloat>& pos, const Vec3<GLfloat>& nz, const Vec3<GLfloat>& nx, const Vec3<GLfloat>& ny, GLfloat vessel_radius)
    {
        _pdata->nx = nx;
        _pdata->ny = ny;
        _pdata->nz = nz;
        _pdata->radius = vessel_radius;
        _pdata->pos = pos;

        if (this->is_initialized())
        {
            init_buffer();
            this->emit_signal_update_required();
        }
    }
    /// @}

    /// @{ -------------------------------------------------- SET RADIUS SCALE
    void MeasuringPlanePreviewView::set_radius_scale(GLfloat s)
    {
        _pdata->radius_scale = s;

        if (this->is_initialized())
        {
            init_buffer();
            this->emit_signal_update_required();
        }
    }
    /// @}

    /// @{ -------------------------------------------------- SET COLOR
    void MeasuringPlanePreviewView::set_color(const ColorRGBA& c)
    { set_color(c[0], c[1], c[2], c[3]); }

    void MeasuringPlanePreviewView::set_color(double r, double g, double b, double a)
    {
        _pdata->color.set(r, g, b, a);

        if (this->is_initialized())
        {
            _pdata->ubo.set_color_r(_pdata->color[0]);
            _pdata->ubo.set_color_g(_pdata->color[1]);
            _pdata->ubo.set_color_b(_pdata->color[2]);
            _pdata->ubo.release();
            this->emit_signal_update_required();
        }
    }
    /// @}

    /// @{ -------------------------------------------------- OPERATOR =
    auto MeasuringPlanePreviewView::operator=(self_type&&) noexcept -> self_type& = default;
    /// @}

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    /// @{ -------------------------------------------------- CLEAR
    void MeasuringPlanePreviewView::clear_shader()
    { _pdata->shader.clear(); }

    void MeasuringPlanePreviewView::clear_buffers()
    {
        _pdata->vbo.clear();
        _pdata->vao.clear();
        _pdata->ubo.clear();
    }

    void MeasuringPlanePreviewView::clear()
    {
        clear_shader();
        clear_buffers();
        this->emit_signal_scene_changed();
        this->emit_signal_update_required();
    }
    /// @}

    /// @{ -------------------------------------------------- INIT
    Vec3<GLfloat> MeasuringPlanePreviewView::normal_of_vertex(const Vec3<GLfloat>& v) const
    { return 0.5 * ((v - _pdata->pos).normalize() + _pdata->nz); }

    std::vector<Vec3<GLfloat>> MeasuringPlanePreviewView::vertices_normals_interleaved() const
    {
        const GLfloat r = _pdata->radius * _pdata->radius_scale;
        
        const Vec3<GLfloat> v0 = _pdata->pos + r * _pdata->nx + r * _pdata->ny;
        const Vec3<GLfloat> n0 = normal_of_vertex(v0);

        const Vec3<GLfloat> v1 = _pdata->pos + r * _pdata->nx - r * _pdata->ny;
        const Vec3<GLfloat> n1 = normal_of_vertex(v1);

        const Vec3<GLfloat> v2 = _pdata->pos - r * _pdata->nx - r * _pdata->ny;
        const Vec3<GLfloat> n2 = normal_of_vertex(v2);

        const Vec3<GLfloat> v3 = _pdata->pos - r * _pdata->nx + r * _pdata->ny;
        const Vec3<GLfloat> n3 = normal_of_vertex(v3);

        return {v0, n0, v1, n1, v2, n2, v3, n3};
    }

    void MeasuringPlanePreviewView::init_buffer()
    {
        const std::vector<Vec3<GLfloat>> verticesNormals = vertices_normals_interleaved();

        this->_pdata->center = 0.25 * std::accumulate(verticesNormals.begin(), verticesNormals.end(), Vec3<GLfloat>(0, 0, 0));

        _pdata->vbo.init(verticesNormals);
        _pdata->vao.init(_pdata->vbo);

        this->emit_signal_scene_changed();
    }

    void MeasuringPlanePreviewView::init_shader()
    {
        clear_shader();

        using SL = bk::ShaderLibrary_CMR;
        _pdata->shader.init_from_sources(SL::measuring_plane_preview::vert(), SL::measuring_plane_preview::frag());
    }

    void MeasuringPlanePreviewView::init_ubo()
    {
        _pdata->ubo.clear();
        _pdata->ubo.init_from_registered_values_size();

        _pdata->ubo.set_color_r(_pdata->color[0]);
        _pdata->ubo.set_color_g(_pdata->color[1]);
        _pdata->ubo.set_color_b(_pdata->color[2]);

        const GLfloat lightCol = 0.5;
        _pdata->ubo.set_lightcolor_r(lightCol);
        _pdata->ubo.set_lightcolor_g(lightCol);
        _pdata->ubo.set_lightcolor_b(lightCol);

        const GLfloat shininess = 50;
        _pdata->ubo.set_shininess(shininess);

        _pdata->ubo.release();
    }

    void MeasuringPlanePreviewView::init(const Vec3<GLfloat>& pos, const Vec3<GLfloat>& nz, const Vec3<GLfloat>& nx, const Vec3<GLfloat>& ny, GLfloat vessel_radius)
    {
        _pdata->nx = nx;
        _pdata->ny = ny;
        _pdata->nz = nz;
        _pdata->radius = vessel_radius;
        _pdata->pos = pos;

        init_buffer();
        init_shader();
        init_ubo();

        this->emit_signal_scene_changed();
        this->emit_signal_update_required();
    }
    /// @}

    /// @{ -------------------------------------------------- EVENTS
    void MeasuringPlanePreviewView::on_resize(GLint /*w*/, GLint /*h*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_oit_enabled(bool /*b*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_animation_enabled(bool /*b*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_modelview_changed(bool /*b*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_visible_changed(bool /*b*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_mouse_button_pressed(MouseButton_ /*btn*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_mouse_button_released(MouseButton_ /*btn*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_key_pressed(Key_ /*k*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_key_released(Key_ /*k*/)
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_mouse_wheel_up()
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_mouse_wheel_down()
    { /* do nothing */ }

    void MeasuringPlanePreviewView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
    { /* do nothing */ }
    /// @}

    /// @{ -------------------------------------------------- DRAW
    void MeasuringPlanePreviewView::draw_opaque_impl()
    {
        // ubo 0 must be global ubo with modelview/projection matrices
        _pdata->ubo.bind_to_default_base();

        _pdata->vao.bind();
        _pdata->shader.bind();
        BK_QT_GL glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, nullptr);
        _pdata->shader.release();
        _pdata->vao.release();

        _pdata->ubo.release_from_base();
    }

    void MeasuringPlanePreviewView::draw_transparent_impl()
    { /* do nothing */ }
    /// @}
  } // inline namespace cmr
} // namespace bk