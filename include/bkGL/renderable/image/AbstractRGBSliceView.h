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

#include "../../../bkMath/Matrix"

#include "AbstractSliceView.h"

namespace bk::details
{
    class BK_GL_EXPORT AbstractRGBSliceView : public details::AbstractSliceView
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
        AbstractRGBSliceView(self_type&& other);
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
        auto operator=(self_type&& other) -> self_type&;
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


