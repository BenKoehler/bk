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
 * RGBSliceView isv;
 * - wheel up/down -> isv.nextSlice(); / isv.previousSlice();
 * - window resize (resizeGL) -> isv.set_screen_size(w, h);
 * - mouse move event -> isv.determine_currentIntensity(_mouse.x(), _mouse.y());
 *      -> sends signal_rgbcurrent_changed(value of hovered position)
 * - paint function -> isv.draw()
 * - set image -> isv.set_image(); // performs initialization on success
 *
 * - signals: {xyzt}{max,current}_changed
 */

#pragma once
#ifndef BK_ABSTRACTRGBSLICEVIEW_H
#define BK_ABSTRACTRGBSLICEVIEW_H

#include <memory>
#include <string>

#include <bk/Matrix>

#include <bkGL/renderable/image/AbstractSliceView.h>

namespace bk::details
{
    class BKGL_EXPORT AbstractRGBSliceView : public details::AbstractSliceView
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = AbstractRGBSliceView;
        using base_type = details::AbstractSliceView;
      public:
        using rgb_type = bk::Vec3<GLfloat>;

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      protected:
        class Impl;
        std::unique_ptr<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        /// @{ -------------------------------------------------- CONSTRUCTORS
        #ifndef BK_LIB_QT_AVAILABLE

        AbstractRGBSliceView();
        #else
        AbstractRGBSliceView(bk::qt_gl_functions* gl);
        #endif
        AbstractRGBSliceView(const self_type& other) = delete;
        AbstractRGBSliceView(self_type&& other) noexcept;
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~AbstractRGBSliceView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET INTENSITY
    protected:
        auto rgb_current() -> rgb_type&;
    public:
        auto rgb_current() const -> const rgb_type&;
        /// @}

        /// @{ -------------------------------------------------- GET SIGNAL
        auto signal_rgbcurrent_changed() -> Signal<rgb_type>&;
        /// @}

        /// @{ -------------------------------------------------- GET SHADER FILES
      protected:
        std::string vertex_shader_source() const override;
        std::string fragment_shader_source() const override;
      public:
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        self_type& operator=(const self_type& other) = delete;
        auto operator=(self_type&& other) noexcept -> self_type&;
        /// @}

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        /// @{ -------------------------------------------------- INIT
        GLuint ssbo_intensity_num_elements_per_pixel() const final;
        /// @}

        /// @{ -------------------------------------------------- CLEAR
      protected:
        virtual void clear() override;
      public:
        /// @}

        /// @{ -------------------------------------------------- CURRENT INTENSITY (RGB)
      protected:
        void determine_currentIntensity_impl() final;
        virtual auto rgb_at_xyzt_current() const -> rgb_type = 0;
      public:
        /// @}
    }; // class AbstractRGBSliceView
} // namespace bk::details

#endif //BK_ABSTRACTRGBSLICEVIEW_H


