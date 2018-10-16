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

#include <bkCMR/gl/PressureView.h>

#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/buffer/FBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/renderable/ScreenQuad.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/texture/Texture3D.h>
#include <bkGL/texture/Texture2D.h>

#include <bkCMR/gl/UBOPressureView.h>
#include <bkCMR/gl/ShaderLibrary_CMR.h>

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct PressureView::Impl
    {
        // mesh
        VBO vbo_mesh;
        IBO ibo_mesh;
        VAO vao_mesh;
        // cuboid faces
        VBO vbo_cuboid;
        IBO ibo_cuboid;
        VAO vao_cuboid;
        UBOPressureView ubo;
        Shader shader_cuboid;
        Shader shader_dvr;
        FBO fbo_front;
        FBO fbo_back;
        Texture3D image_tex[2];
        GLsizei sizeInd_mesh; // mesh
        GLsizei sizeInd_cuboid; // cuboid faces
        Vec3<GLfloat> center;
        GLuint num_ray_samples;
        bool changed;
        RegularImage<double, 4> img;
        mutable bool time_changed;
        GLuint oldt0;
        GLuint oldt1;
        GLfloat scaleVal;
        GLuint window_width;
        GLuint window_height;
        double current_time;
        GLfloat referenceVal;
        GLfloat minVal;
        GLfloat maxVal;
        bool staticMode;

            #ifndef BK_LIB_QT_AVAILABLE

        Impl()
            :
            #else

        explicit Impl(bk::qt_gl_functions* gl)
          : vbo_mesh(gl),
            ibo_mesh(gl),
            vao_mesh(gl),
            vbo_cuboid(gl),
            ibo_cuboid(gl),
            vao_cuboid(gl),
            ubo(gl),
            shader_cuboid(gl),
            shader_dvr(gl),
            fbo_front(gl),
            fbo_back(gl),
            image_tex{Texture3D(gl), Texture3D(gl)},
            #endif
            sizeInd_mesh(0),
            sizeInd_cuboid(0),
            center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
            num_ray_samples(100), // todo options
            changed(true),
            time_changed(true),
            oldt0(0),
            oldt1(0),
            scaleVal(10),
            window_width(1),
            window_height(1),
            current_time(-1),
            referenceVal(0),
            minVal(0),
            maxVal(0),
            staticMode(false)
        { /* do nothing */ }
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    #ifndef BK_LIB_QT_AVAILABLE

    PressureView::PressureView()
        : base_type()
    #else

    PressureView::PressureView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
    #endif
    {
        _pdata->vbo_mesh.set_usage_STATIC_DRAW();
        _pdata->ibo_mesh.set_usage_STATIC_DRAW();
        _pdata->vao_mesh.add_default_attribute_position_3xfloat();

        _pdata->vbo_cuboid.set_usage_STATIC_DRAW();
        _pdata->ibo_cuboid.set_usage_STATIC_DRAW();
        _pdata->vao_cuboid.add_default_attribute_position_3xfloat();
        _pdata->vao_cuboid.add_default_attribute_colorRGB_3xfloat();
    }

    PressureView::PressureView(PressureView&&) noexcept = default;
    PressureView::~PressureView() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    GLuint PressureView::num_ray_samples() const
    { return _pdata->num_ray_samples; }

    GLfloat PressureView::scale_val() const
    { return _pdata->scaleVal; }

    GLfloat PressureView::reference_val() const
    { return _pdata->referenceVal; }

    VBO& PressureView::vbo_mesh()
    { return _pdata->vbo_mesh; }

    IBO& PressureView::ibo_mesh()
    { return _pdata->ibo_mesh; }

    GLsizei& PressureView::sizeInd_mesh()
    { return _pdata->sizeInd_mesh; }

    VAO& PressureView::vao_mesh()
    { return _pdata->vao_mesh; }

    VBO& PressureView::vbo_cuboid()
    { return _pdata->vbo_cuboid; }

    IBO& PressureView::ibo_cuboid()
    { return _pdata->ibo_cuboid; }

    GLsizei& PressureView::sizeInd_cuboid()
    { return _pdata->sizeInd_cuboid; }

    VAO& PressureView::vao_cuboid()
    { return _pdata->vao_cuboid; }

    UBO& PressureView::ubo()
    { return _pdata->ubo; }

    FBO& PressureView::fbo_front()
    { return _pdata->fbo_front; }

    FBO& PressureView::fbo_back()
    { return _pdata->fbo_back; }

    Shader& PressureView::shader_dvr()
    { return _pdata->shader_dvr; }

    Texture3D& PressureView::image_tex(unsigned int i)
    { return _pdata->image_tex[i]; }

    RegularImage<double, 4>& PressureView::img()
    { return _pdata->img; }

    auto PressureView::minVal() -> GLfloat&
    { return _pdata->minVal; }

    auto PressureView::maxVal() -> GLfloat&
    { return _pdata->maxVal; }

    auto PressureView::min_value() const -> GLfloat
    { return _pdata->minVal; }

    auto PressureView::max_value() const -> GLfloat
    { return _pdata->maxVal; }

    bool PressureView::is_up_to_date() const
    {
        const bool b = _pdata->time_changed;
        _pdata->time_changed = false;
        return !b;
    }

    Vec3<GLfloat> PressureView::center() const
    { return _pdata->center; }

    bool PressureView::is_initialized() const
    { return _pdata->vao_mesh.is_initialized() && _pdata->vao_cuboid.is_initialized(); }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    void PressureView::set_num_ray_samples(GLuint n)
    {
        _pdata->num_ray_samples = std::max(n, static_cast<GLuint>(1));

        if (this->is_initialized())
        {
            _pdata->ubo.set_num_ray_samples(_pdata->num_ray_samples);
            _pdata->ubo.release();

            this->emit_signal_update_required();
        }
    }

    void PressureView::set_scale_val(GLfloat s)
    {
        _pdata->changed = true;

        _pdata->scaleVal = s;
        _pdata->ubo.set_scaleVal(_pdata->scaleVal);
        _pdata->ubo.release();
        this->emit_signal_update_required();
    }

    void PressureView::set_reference_val(GLfloat r)
    {
        _pdata->changed = true;

        _pdata->referenceVal = r;
        _pdata->ubo.set_referenceVal(_pdata->referenceVal);
        _pdata->ubo.release();
        this->emit_signal_update_required();
    }

    void PressureView::set_static_mode(bool b)
    {
        if (_pdata->staticMode != b)
        {
            _pdata->staticMode = b;

            if (this->is_initialized())
            {
                if (b)
                {
                    const auto& image_size = _pdata->img.geometry().size();

                    _pdata->time_changed = true;

                    const unsigned int N = image_size[0] * image_size[1] * image_size[2];
                    std::vector<GLfloat> texVals0(N, 0);
                    std::vector<GLfloat> texVals1(N, 0);

                    #pragma omp parallel for
                    for (unsigned int z = 0; z < image_size[2]; ++z)
                    {
                        unsigned int cnt0 = z * image_size[1] * image_size[0];
                        unsigned int cnt1 = cnt0;

                        for (unsigned int y = 0; y < image_size[1]; ++y)
                        {
                            for (unsigned int x = 0; x < image_size[0]; ++x)
                            {
                                GLfloat minval = std::numeric_limits<GLfloat>::max();
                                GLfloat maxval = -minval;

                                for (unsigned int t = 0; t < image_size[3]; ++t)
                                {
                                    minval = std::min(minval, static_cast<GLfloat>(_pdata->img(x, y, z, t)));
                                    maxval = std::max(maxval, static_cast<GLfloat>(_pdata->img(x, y, z, t)));
                                }

                                if (std::abs(minval) > std::abs(maxval))
                                {
                                    texVals0[cnt0++] = minval;
                                    texVals1[cnt1++] = minval;
                                }
                                else
                                {
                                    texVals0[cnt0++] = maxval;
                                    texVals1[cnt1++] = maxval;
                                }
                            }
                        }
                    }

                    _pdata->image_tex[0].init(static_cast<GLvoid*>(texVals0.data()));
                    _pdata->image_tex[1].init(static_cast<GLvoid*>(texVals1.data()));

                    _pdata->oldt0 = 0;
                    _pdata->oldt1 = 0;
                }
                else
                { on_animation_time_changed(_pdata->current_time); }

                this->emit_signal_update_required();
            }
        }
    }

    PressureView& PressureView::operator=(PressureView&&) noexcept = default;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void PressureView::clear_shader()
    {
        _pdata->shader_dvr.clear();
        _pdata->shader_cuboid.clear();
    }

    void PressureView::clear_buffers_and_textures()
    {
        _pdata->vbo_mesh.clear();
        _pdata->ibo_mesh.clear();
        _pdata->vao_mesh.clear();
        _pdata->sizeInd_mesh = 0;

        _pdata->vbo_cuboid.clear();
        _pdata->ibo_cuboid.clear();
        _pdata->vao_cuboid.clear();
        _pdata->sizeInd_cuboid = 0;

        _pdata->ubo.clear();

        _pdata->fbo_front.clear();
        _pdata->fbo_back.clear();

        _pdata->image_tex[0].clear();
        _pdata->image_tex[1].clear();
    }

    void PressureView::clear()
    {
        clear_shader();
        clear_buffers_and_textures();
        this->emit_signal_scene_changed();
        this->emit_signal_update_required();
    }

    void PressureView::init_shader()
    {
        clear_shader();

        using SL = bk::details::ShaderLibrary::dvr::cuboid;
        _pdata->shader_cuboid.init(SL::vert(), SL::frag());

        using SLCMR = ShaderLibrary_CMR::pressure;
        _pdata->shader_dvr.init(SLCMR::vert(), SLCMR::frag());
    }

    void PressureView::init_ubo()
    {
        _pdata->ubo.init_from_registered_values_size();
        _pdata->ubo.set_num_ray_samples(_pdata->num_ray_samples);
        _pdata->ubo.set_num_times(_pdata->img.geometry().size(3));
        _pdata->ubo.set_temporal_resolution(img().geometry().transformation().scale(3));
        _pdata->ubo.set_current_t0(0);
        _pdata->ubo.set_scaleVal(_pdata->scaleVal);
        _pdata->ubo.set_referenceVal(_pdata->referenceVal);
        _pdata->ubo.release();
    }

    void PressureView::init_fbo(GLuint window_width, GLuint window_height)
    {
        _pdata->fbo_front.clear();
        _pdata->fbo_back.clear();

        //------------------------------------------------------------------------------------------------------
        // FBO Front
        //------------------------------------------------------------------------------------------------------
        /*
         * color
         */
        #ifndef BK_LIB_QT_AVAILABLE
        Texture2D entry_tex;
        #else
        Texture2D entry_tex(this->_gl);
        #endif
        entry_tex.set_width(window_width);
        entry_tex.set_height(window_height);
        entry_tex.set_default_config_rgb_tex();
        entry_tex.set_texture_unit_number(1);
        entry_tex.init();

        entry_tex.bind();
        entry_tex.set_interpolation_LINEAR();
        entry_tex.set_texture_coordinates_CLAMP();
        entry_tex.release();

        _pdata->fbo_front.set_texture_COLOR_ATTACHMENT(std::move(entry_tex), 0);
        _pdata->fbo_front.init();

        //------------------------------------------------------------------------------------------------------
        // FBO Back
        //------------------------------------------------------------------------------------------------------
        /*
         * color
         */
        #ifndef BK_LIB_QT_AVAILABLE
        Texture2D exit_tex;
        #else
        Texture2D exit_tex(this->_gl);
        #endif
        exit_tex.set_width(window_width);
        exit_tex.set_height(window_height);
        exit_tex.set_default_config_rgb_tex();
        exit_tex.set_texture_unit_number(3);
        exit_tex.init();

        exit_tex.bind();
        exit_tex.set_interpolation_LINEAR();
        exit_tex.set_texture_coordinates_CLAMP();
        exit_tex.release();

        _pdata->fbo_back.set_texture_COLOR_ATTACHMENT(std::move(exit_tex), 0);
        _pdata->fbo_back.init();
    }

    void PressureView::on_resize(GLint w, GLint h)
    {
        _pdata->window_width = static_cast<GLuint>(w);
        _pdata->window_height = static_cast<GLuint>(h);

        init_fbo(w, h);
        this->set_modelview_matrix_changed(true);
    }

    void PressureView::on_oit_enabled(bool /*b*/)
    { /*do nothing*/ }

    void PressureView::on_animation_enabled(bool /*b*/)
    { /*do nothing*/ }

    void PressureView::on_modelview_changed(bool b)
    { this->set_modelview_matrix_changed(b); }

    void PressureView::on_visible_changed(bool b)
    {
        if (b)
        {
            this->set_modelview_matrix_changed(true);
            on_animation_time_changed(_pdata->current_time);
        }
    }

    void PressureView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
    { /* do nothing */ }

    void PressureView::on_mouse_button_pressed(MouseButton_ /*btn*/)
    { /* do nothing */ }

    void PressureView::on_mouse_button_released(MouseButton_ /*btn*/)
    { /* do nothing */ }

    void PressureView::on_key_pressed(Key_ /*k*/)
    { /* do nothing */ }

    void PressureView::on_key_released(Key_ /*k*/)
    { /* do nothing */ }

    void PressureView::on_mouse_wheel_up()
    { /* do nothing */ }

    void PressureView::on_mouse_wheel_down()
    { /* do nothing */ }

    void PressureView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
    { /* do nothing */ }

    void PressureView::on_animation_time_changed(double d)
    {
        _pdata->current_time = d;

        if (!this->is_visible() || _pdata->staticMode)
        { return; }

        const auto& image_size = _pdata->img.geometry().size();
        const auto& image_scale = _pdata->img.geometry().transformation().scale();
        const GLuint t0 = std::min(static_cast<GLuint>(std::floor(d / image_scale[3])), image_size[3] - 1);
        const GLuint t1 = (t0 + 1) % image_size[3];

        _pdata->time_changed = true;

        if (t0 == _pdata->oldt0 && t1 == _pdata->oldt1) // up to date
        { return; }

        const unsigned int N = image_size[0] * image_size[1] * image_size[2];
        std::vector<GLfloat> texVals0(N, 0);
        std::vector<GLfloat> texVals1(N, 0);

        #pragma omp parallel for
        for (unsigned int z = 0; z < image_size[2]; ++z)
        {
            unsigned int cnt0 = z * image_size[1] * image_size[0];
            unsigned int cnt1 = cnt0;

            for (unsigned int y = 0; y < image_size[1]; ++y)
            {
                for (unsigned int x = 0; x < image_size[0]; ++x)
                {
                    texVals0[cnt0++] = static_cast<GLfloat>(_pdata->img(x, y, z, t0));
                    texVals1[cnt1++] = static_cast<GLfloat>(_pdata->img(x, y, z, t1));
                }
            }
        }

        _pdata->image_tex[0].init(static_cast<GLvoid*>(texVals0.data()));
        _pdata->image_tex[1].init(static_cast<GLvoid*>(texVals1.data()));

        this->ubo().write_registered_value("current_t0", &t0);
        this->ubo().release();

        _pdata->oldt0 = t0;
        _pdata->oldt1 = t1;
    }

    void PressureView::draw_opaque_impl()
    {
        if (this->modelview_matrix_changed() || !is_up_to_date() || _pdata->changed)
        {
            BK_QT_GL glPushAttrib(GL_POLYGON_BIT);

            // ubo 0 must be global ubo with modelview/projection matrices

            BK_QT_GL glDepthMask(GL_FALSE);

            /*
             * first pass: render cuboid front faces with position colors
             */
            BK_QT_GL glEnable(GL_CULL_FACE);
            BK_QT_GL glCullFace(GL_BACK);

            _pdata->ubo.bind_to_default_base();

            _pdata->fbo_front.bind();
            BK_QT_GL glClear(GL_COLOR_BUFFER_BIT);

            _pdata->vao_cuboid.bind();
            _pdata->shader_cuboid.bind();
            BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_cuboid, GL_UNSIGNED_INT, nullptr);
            _pdata->shader_cuboid.release();
            _pdata->vao_cuboid.release();

            _pdata->fbo_front.release();

            /*
             * second pass: render cuboid back faces with position colors
             */
            BK_QT_GL glCullFace(GL_FRONT);

            _pdata->ubo.bind_to_base(1);

            _pdata->fbo_back.bind();
            BK_QT_GL glClear(GL_COLOR_BUFFER_BIT);

            _pdata->vao_cuboid.bind();
            _pdata->shader_cuboid.bind();
            BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_cuboid, GL_UNSIGNED_INT, nullptr);
            _pdata->shader_cuboid.release();
            _pdata->vao_cuboid.release();

            _pdata->fbo_back.release();

            _pdata->ubo.release_from_base();

            BK_QT_GL glDepthMask(GL_TRUE);

            BK_QT_GL glPopAttrib();

            this->emit_signal_bind_default_fbo();

            _pdata->changed = false;
        }

        /*
         * third pass: draw mesh (with front-face culling) as stencil
         *    - determine directions via entry_tex
         *    - perform ray casting
         */
        _pdata->ubo.bind_to_base(1);

        BK_QT_GL glPushAttrib(GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT);

        BK_QT_GL glEnable(GL_DEPTH_TEST);
        BK_QT_GL glDepthFunc(GL_LESS);

        BK_QT_GL glEnable(GL_CULL_FACE);
        BK_QT_GL glFrontFace(GL_CCW);
        BK_QT_GL glCullFace(GL_FRONT);

        _pdata->fbo_front.color_attachment(0)->bind(); // bound to tex unit 1
        _pdata->fbo_back.color_attachment(0)->bind(); // bound to tex unit 3
        _pdata->image_tex[0].bind();
        _pdata->image_tex[1].bind();

        _pdata->vao_mesh.bind();
        _pdata->shader_dvr.bind();
        BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_mesh, GL_UNSIGNED_INT, nullptr);
        _pdata->shader_dvr.release();
        _pdata->vao_mesh.release();

        _pdata->fbo_front.color_attachment(0)->release();
        _pdata->fbo_back.color_attachment(0)->release();
        _pdata->image_tex[0].release();
        _pdata->image_tex[1].release();

        BK_QT_GL glPopAttrib();

        _pdata->ubo.release_from_base();
    }

    void PressureView::draw_transparent_impl()
    { /* do nothing */ }
  } // inline namespace cmr
} // namespace bk