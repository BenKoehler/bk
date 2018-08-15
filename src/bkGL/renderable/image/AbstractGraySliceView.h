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
 * GraySliceView isv;
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

#ifndef BK_ABSTRACTGRAYSLICEVIEW_H
#define BK_ABSTRACTGRAYSLICEVIEW_H

#include <memory>
#include <string>

#include <bkGL/renderable/image/AbstractSliceView.h>

namespace bk
{
  // ------ forward declaration -------
  template<typename... Args> class Signal;
  // ------ forward declaration -------

  namespace details
  {
    class BKGL_EXPORT AbstractGraySliceView : public AbstractSliceView
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================
        using self_type = AbstractGraySliceView;
        using base_type = AbstractSliceView;

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
        AbstractGraySliceView();
        #else
        AbstractGraySliceView(bk::qt_gl_functions* gl);
        #endif
        AbstractGraySliceView(const self_type& other) = delete;
        AbstractGraySliceView(self_type&& other);
        /// @}

        /// @{ -------------------------------------------------- DESTRUCTOR
        virtual ~AbstractGraySliceView();
        /// @}

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- GET INTENSITY
        GLfloat intensity_current();
        /// @}

        /// @{ -------------------------------------------------- GET SIGNAL
        bk::Signal<GLfloat>& signal_intensitycurrent_changed();
        /// @}

        /// @{ -------------------------------------------------- GET SHADER FILES
      protected:
        std::string vertex_shader_source() const override;
        std::string fragment_shader_source() const override;
      public:
        /// @}

        /// @{ -------------------------------------------------- GET MEMBERS
    protected:
        GLfloat& intensitycurrent();
    public:
        /// @}

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        /// @{ -------------------------------------------------- OPERATOR =
        self_type& operator=(const self_type&) = delete;
        auto operator=(self_type&&) -> self_type&;
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

        /// @{ -------------------------------------------------- CURRENT INTENSITY
      protected:
        void determine_currentIntensity_impl() final;
        virtual GLfloat intensity_at_xyzt_current() const = 0;
      public:
        /// @}
    }; // class AbstractGraySliceView
  } // namespace details

} // namespace bk

#endif //BK_ABSTRACTGRAYSLICEVIEW_H


