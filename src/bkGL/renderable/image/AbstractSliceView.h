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


/*
* USAGE:
*
* SliceView isv;
* - wheel up/down -> isv.nextSlice(); / isv.previousSlice();
* - window resize (resizeGL) -> isv.set_screen_size(w, h);
* - mouse move event -> isv.determine_currentIntensity(_mouse.x(), _mouse.y());
*      -> sends signal_intensitycurrent_changed(value of hovered position)
* - paint function -> isv.draw()
* - set image -> isv.set_image(); // performs initialization on success
*
* - signals: {xyzt}{max,current}_changed
*/

#pragma once

#ifndef BKGL_ABSTRACTSLICEVIEW_H
#define BKGL_ABSTRACTSLICEVIEW_H

#include <bk/CopyablePIMPL>
#include <bkGL/renderable/AbstractRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // ------ forward declaration -------
  template<typename... Args> class Signal;

  class VAO;

  class Mouse;

  class Shader;

  class WindowingTransferFunctionView;

  class SSBO;

  namespace details
  {
    class UBOSliceView;
  } // namespace details
  // ------ forward declaration -------

  namespace details
  {
    class BKGL_EXPORT AbstractSliceView : public details::AbstractRenderable
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = AbstractSliceView;
        using base_type = details::AbstractRenderable;

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
        /// @{ -------------------------------------------------- CTOR
        #ifndef BK_LIB_QT_AVAILABLE
        AbstractSliceView();
        #else
        AbstractSliceView(bk::qt_gl_functions* gl);
        #endif
        AbstractSliceView(const self_type& other) = delete;
        AbstractSliceView(self_type&& other) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DTOR
        virtual ~AbstractSliceView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET XYZT MAX
        [[nodiscard]] GLuint xmax() const;
        [[nodiscard]] GLuint ymax() const;
        [[nodiscard]] GLuint zmax() const;
        [[nodiscard]] GLuint tmax() const;
        /// @}

        /// @{ -------------------------------------------------- GET XYZT CURRENT
        [[nodiscard]] GLuint xcurrent() const;
        [[nodiscard]] GLuint ycurrent() const;
        [[nodiscard]] GLuint zcurrent() const;
        [[nodiscard]] GLuint tcurrent() const;
        /// @}

        /// @{ -------------------------------------------------- GET INTENSITY
        [[nodiscard]] GLfloat intensitymin() const;
        [[nodiscard]] GLfloat intensitymax() const;
        /// @}

        /// @{ -------------------------------------------------- GET SIGNAL
        [[nodiscard]] bk::Signal<GLuint>& signal_xmax_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_xcurrent_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_ymax_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_ycurrent_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_zmax_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_zcurrent_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_tmax_changed();
        [[nodiscard]] bk::Signal<GLuint>& signal_tcurrent_changed();
        /// @}

        /// @{ -------------------------------------------------- GET MEMBERS
      protected:
        [[nodiscard]] GLuint& _xmax();
        [[nodiscard]] GLuint& _ymax();
        [[nodiscard]] GLuint& _zmax();
        [[nodiscard]] GLuint& _tmax();
        [[nodiscard]] GLuint& _xcurrent();
        [[nodiscard]] GLuint& _ycurrent();
        [[nodiscard]] GLuint& _zcurrent();
        [[nodiscard]] GLuint& _tcurrent();
        [[nodiscard]] GLfloat& _intensitymin();
        [[nodiscard]] GLfloat& _intensitymax();
        [[nodiscard]] SSBO& _ssbo_intensity();
        [[nodiscard]] VAO& _vao();
        [[nodiscard]] details::UBOSliceView& _ubo();
        [[nodiscard]] GLsizei _sizeInd() const;
        [[nodiscard]] Shader& _shader();
        [[nodiscard]] WindowingTransferFunctionView& _tf_view();
        [[nodiscard]] bool _show_tf() const;
        [[nodiscard]] const Mouse& _mouse() const;
      public:
        /// @}

        /// @{ -------------------------------------------------- IS INITIALIZED
        [[nodiscard]] virtual bool is_initialized() const override;
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
        [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
        /// @}

        /// @{ -------------------------------------------------- SET SCREEN SIZE
        void set_screen_size(GLint w, GLint h);
        /// @}

        /// @{ -------------------------------------------------- SET SCALE
        void set_scale(GLfloat vx, GLfloat vy);
        /// @}

        /// @{ -------------------------------------------------- SET SLICE/TIME
        void set_slice(unsigned int z, GLint lastMouseX = -1, GLint lastMouseY = -1);
      protected:
        virtual void set_slice_impl(unsigned int z);
      public:
        void set_time(unsigned int t, GLint lastMouseX = -1, GLint lastMouseY = -1);
      protected:
        virtual void set_time_impl(unsigned int t);
      public:
        void previousTime(GLint lastMouseX = -1, GLint lastMouseY = -1);
        void nextTime(GLint lastMouseX = -1, GLint lastMouseY = -1);
        void previousSlice(GLint lastMouseX = -1, GLint lastMouseY = -1);
        void nextSlice(GLint lastMouseX = -1, GLint lastMouseY = -1);
        /// @}

        /// @{ -------------------------------------------------- SET TRANSFER FUNCTION
        void reset_transfer_function(bool tolerant = false);
        void transfer_function_shift_center_left(double percent);
        void transfer_function_shift_center_right(double percent);
        void transfer_function_increase_width(double percent);
        void transfer_function_decrease_width(double percent);
        /// @}

        /// @{ -------------------------------------------------- SHOW TRANSFER FUNCTION
        void show_transfer_function();
        void hide_transfer_function();
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- CLEAR
      protected:
        void clear_ubo();
        void clear_imageBuffers();
        void clear_shader();
        virtual void clear_image();
      public:
        virtual void clear();
        /// @}

        /// @{ -------------------------------------------------- INIT
      protected:
        void init_ubo();
        bool init_shader();
        void init_image();
        void update_ssbo_intensity_and_determine_intensity_min_max();
        [[nodiscard]] virtual std::string vertex_shader_source() const = 0;
        [[nodiscard]] virtual std::string fragment_shader_source() const = 0;
        [[nodiscard]] virtual std::string geometry_shader_source() const;
        [[nodiscard]] virtual GLuint ssbo_intensity_num_elements_per_pixel() const = 0;
        virtual void update_ssbo_intensity_and_determine_intensity_min_max_impl(GLuint z, GLuint t) = 0;
        [[nodiscard]] virtual GLuint image_size(GLuint id) const = 0;
        [[nodiscard]] virtual GLfloat image_scale(GLuint id) const = 0;
      public:
        virtual void init();
        /// @}

        /// @{ -------------------------------------------------- CURRENT INTENSITY
      protected:
        void mouseXY_to_imageXY(int mouseX, int mouseY, unsigned int& imgX, unsigned int& imgY) const;
        /// @}

        /// @{ -------------------------------------------------- CURRENT INTENSITY
      public:
        void determine_currentIntensity(int mouseX, int mouseY);
      protected:
        virtual void determine_currentIntensity_impl() = 0;
      public:
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

      protected:
        // required for graph cut
        virtual bool on_mouse_pos_changed_impl(GLint x, GLint y);
        virtual bool on_mouse_button_pressed_impl(MouseButton_ btn);
        virtual bool on_mouse_button_released_impl(MouseButton_ btn);
      public:
        /// @}

        /// @{ -------------------------------------------------- DRAW
        virtual void draw_impl() override;
        /// @}
    }; // class AbstractSliceView
  } // namespace details
} // namespace bk

#endif //BKGL_ABSTRACTSLICEVIEW_H


