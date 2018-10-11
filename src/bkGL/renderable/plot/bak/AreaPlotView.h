///*
// * MIT License
// *
// * Copyright (c) 2018 Benjamin Köhler
// *
// * Permission is hereby granted, free of charge, to any person obtaining a copy
// * of this software and associated documentation files (the "Software"), to deal
// * in the Software without restriction, including without limitation the rights
// * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// * copies of the Software, and to permit persons to whom the Software is
// * furnished to do so, subject to the following conditions:
// *
// * The above copyright notice and this permission notice shall be included in all
// * copies or substantial portions of the Software.
// *
// * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// * SOFTWARE.
// */
//
//#pragma once
//
//#ifndef BK_AREAPLOTVIEW_H
//#define BK_AREAPLOTVIEW_H
//
//#include <memory>
//#include <vector>
//
//#include <bk/CopyablePIMPL>
//#include <bkGL/renderable/AbstractRenderable.h>
//
//namespace bk
//{
//  // -------------------- forward declaration
//  class ColorRGBA;
//
//  template<typename... Args> class Signal;
//  // -------------------- forward declaration END
//
//  class BKGL_EXPORT AreaPlotView : public details::AbstractRenderable
//  {
//      //====================================================================================================
//      //===== DEFINITIONS
//      //====================================================================================================
//      using base_type = details::AbstractRenderable;
//
//      //====================================================================================================
//      //===== MEMBERS
//      //====================================================================================================
//      class Impl;
//
//      bk::cpimpl<Impl> _pdata;
//
//      //====================================================================================================
//      //===== CONSTRUCTORS & DESTRUCTOR
//      //====================================================================================================
//    public:
//      /// @{ -------------------------------------------------- CONSTRUCTORS
//      #ifndef BK_LIB_QT_AVAILABLE
//
//      AreaPlotView();
//      #else
//      AreaPlotView(bk::qt_gl_functions* gl);
//      #endif
//      AreaPlotView(const AreaPlotView&) = delete;
//      AreaPlotView(AreaPlotView&&) noexcept;
//      /// @}
//
//      /// @{ -------------------------------------------------- DESTRUCTOR
//      virtual ~AreaPlotView();
//      /// @}
//
//      //====================================================================================================
//      //===== GETTER
//      //====================================================================================================
//      [[nodiscard]] const std::string& xlabel() const;
//      [[nodiscard]] const std::string& ylabel() const;
//      [[nodiscard]] const std::vector<GLfloat>& xvalues() const;
//      [[nodiscard]] const std::vector<GLfloat>& yvalues0() const;
//      [[nodiscard]] const std::vector<GLfloat>& ystdev0() const;
//      [[nodiscard]] const std::vector<GLfloat>& yvalues1() const;
//      [[nodiscard]] const std::vector<GLfloat>& ystdev1() const;
//      [[nodiscard]] GLfloat xmin() const;
//      [[nodiscard]] GLfloat xmax() const;
//      [[nodiscard]] GLfloat ymin() const;
//      [[nodiscard]] GLfloat ymax() const;
//      [[nodiscard]] GLuint numxticks() const;
//      [[nodiscard]] GLuint numyticks() const;
//      [[nodiscard]] const ColorRGBA& fillcolor_above() const;
//      [[nodiscard]] const ColorRGBA& fillcolor_below() const;
//      [[nodiscard]] GLfloat border_width_x_in_percent() const;
//      [[nodiscard]] GLfloat border_width_y_in_percent() const;
//
//      /// @{ -------------------------------------------------- IS INITIALIZED
//      [[nodiscard]] virtual bool is_initialized() const override;
//      /// @}
//
//      //====================================================================================================
//      //===== SETTER
//      //====================================================================================================
//      /// @{ -------------------------------------------------- OPERATOR =
//      [[maybe_unused]] AreaPlotView& operator=(const AreaPlotView&) = delete;
//      [[maybe_unused]] AreaPlotView& operator=(AreaPlotView&&) noexcept;
//      /// @}
//
//      void set_xlabel(const std::string& xlab);
//      void set_ylabel(const std::string& ylab);
//      void set_xvalues(const std::vector<GLfloat>& xv);
//      void set_yvalues0(const std::vector<GLfloat>& yv);
//      void set_ystdev0(const std::vector<GLfloat>& ystd);
//      void set_yvalues1(const std::vector<GLfloat>& yv);
//      void set_ystdev1(const std::vector<GLfloat>& ystd);
//      void set_xmin(GLfloat x);
//      void set_xmax(GLfloat x);
//      void set_xmin_xmax(GLfloat x0, GLfloat x1);
//      void set_ymin(GLfloat y);
//      void set_ymax(GLfloat y);
//      void set_minmax_from_data_values();
//      void set_ymin_ymax(GLfloat y0, GLfloat y1);
//      void set_numxticks(GLuint n);
//      void set_numyticks(GLuint n);
//      void set_fillcolor_above(const ColorRGBA& c);
//      void set_fillcolor_below(const ColorRGBA& c);
//      void set_border_width_x_in_percent(GLfloat b);
//      void set_border_width_y_in_percent(GLfloat b);
//      void set_area_alpha(GLfloat a);
//
//      //====================================================================================================
//      //===== FUNCTIONS
//      //====================================================================================================
//      /// @{ -------------------------------------------------- CLEAR
//    protected:
//      void clear_shader();
//      void clear_buffers();
//      void clear_ubo();
//      void clear_vbo_vao();
//    public:
//      void clear();
//      /// @}
//
//      /// @{ -------------------------------------------------- INIT
//    protected:
//      void init_shader();
//      void init_vbo_vao();
//      void init_ubo();
//    public:
//      void init();
//      /// @}
//
//      /// @{ -------------------------------------------------- EVENTS
//      virtual void on_resize(GLint w, GLint h) override;
//      virtual void on_oit_enabled(bool b) override;
//      virtual void on_animation_enabled(bool b) override;
//      virtual void on_modelview_changed(bool) override;
//      virtual void on_visible_changed(bool b) override;
//      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
//      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
//      virtual void on_mouse_button_released(MouseButton_ btn) override;
//      virtual void on_key_pressed(Key_ k) override;
//      virtual void on_key_released(Key_ k) override;
//      virtual void on_mouse_wheel_up() override;
//      virtual void on_mouse_wheel_down() override;
//      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
//      /// @}
//
//      /// @{ -------------------------------------------------- DRAW
//      virtual void draw_impl() override;
//      /// @}
//  }; // class AreaPlotView
//} // namespace bk
//
//#endif //BK_AREAPLOTVIEW_H
//
//
