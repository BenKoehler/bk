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

#ifndef BK_VECTORVIEW_H
#define BK_VECTORVIEW_H

#include <string_view>
#include <tuple>

#include <bk/CopyablePIMPL>
#include <bk/Matrix>
#include <bkGL/renderable/AbstractSceneRenderable.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  // -------------------- forward declaration
  class ColorRGBA;

  class ColorBarRGBA;

  class ColorBarView;
  // -------------------- forward declaration END

  class BKGL_EXPORT VectorView : public details::AbstractSceneRenderable
  {
  using base_type = details::AbstractSceneRenderable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl <Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      #ifndef BK_LIB_QT_AVAILABLE
      VectorView();
      #else
      VectorView(bk::qt_gl_functions* gl);
      #endif
      VectorView(const VectorView&) = delete;
      VectorView(VectorView&&) noexcept;

      virtual ~VectorView();

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      [[nodiscard]] GLfloat line_width() const;
      [[nodiscard]] const ColorRGBA& color() const;

      [[nodiscard]] bool color_by_attribute_is_enabled() const;
      [[nodiscard]] GLfloat color_attribute_min() const;
      [[nodiscard]] GLfloat color_attribute_max() const;

      [[nodiscard]] ColorBarView& colorbarview();
      [[nodiscard]] const ColorBarView& colorbarview() const;

      [[nodiscard]] GLfloat shininess() const;
      [[nodiscard]] bool isl_is_enabled() const;

      [[nodiscard]] GLfloat halo_width_in_percent() const;
      [[nodiscard]] bool halo_is_enabled() const;

      [[nodiscard]] virtual bk::Vec3<GLfloat> center() const override;
      [[nodiscard]] virtual bool is_initialized() const override;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      [[maybe_unused]] VectorView& operator=(const VectorView&);
      [[maybe_unused]] VectorView& operator=(VectorView&&) noexcept;

      void set_line_width(GLfloat w);

      void set_color(const ColorRGBA& c);
      void set_color(double r, double g, double b);

      void set_color_by_attribute_enabled(bool b);
      void enable_color_by_attribute();
      void disable_color_by_attribute();

    private:
      void _set_colorbar(const ColorBarRGBA& colbar);
      void _set_colorbar_manual_num_colors(const ColorBarRGBA& colbar, unsigned int nColors);
    public:
      void set_colorbar_heat();
      void set_colorbar_rainbow();
      void set_colorbar_blue_to_red();
      void set_colorbar_green_to_red();
      void set_colorbar_magenta();
      void set_colorbar_uniform_yellow();
      void set_colorbar_traffic_light();
      void set_colorbar_cluster();
      void set_colorbar_light_blue_to_yellow();

      void set_enable_colorbar(bool b);

      void set_color_attribute_min(GLfloat v);
      void set_color_attribute_max(GLfloat v);
      void set_color_attribute_clamp_min(GLfloat v);
      void set_color_attribute_clamp_max(GLfloat v);
      void set_attrib_scale_to_colorbar(bool b);
      void set_color_attribute_transparency_enabled(bool b);

      void set_shininess(GLfloat shininess);
      void set_isl_enabled(bool b);

      void set_halo_width_in_percent(GLfloat p);
      void set_halo_enabled(bool b);

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      void clear_shader();
      void clear_buffers();
    public:
      void clear();

    private:
      void init_shader(); // todo
      void init_buffers(const std::vector<std::vector<std::tuple<Vec3d/*pos*/,Vec3d/*vec*/,double/*attrib*/>>>& vecs, double temporal_resolution, std::string_view color_attribute_name = "");
      void init_ubo();
    public:
      void init(const std::vector<std::vector<std::tuple<Vec3d/*pos*/,Vec3d/*vec*/,double/*attrib*/>>>& vecs, double temporal_resolution = 0, std::string_view color_attribute_name = "");

      virtual void on_resize(GLint w, GLint h) override;
      virtual void on_oit_enabled(bool b) override;
      virtual void on_animation_enabled(bool b) override;
      virtual void on_modelview_matrix_changed(bool) override;
      virtual void on_new_modelview_matrix(const ColMat4<GLfloat>& m) override;
      virtual void on_new_projection_matrix(const ColMat4<GLfloat>& p) override;
      virtual void on_visible_changed(bool b) override;
      virtual void on_mouse_pos_changed(GLint x, GLint y) override;
      virtual void on_mouse_button_pressed(MouseButton_ btn) override;
      virtual void on_mouse_button_released(MouseButton_ btn) override;
      virtual void on_key_pressed(Key_ k) override;
      virtual void on_key_released(Key_ k) override;
      virtual void on_mouse_wheel_up() override;
      virtual void on_mouse_wheel_down() override;
      virtual void on_ssaa_factor_changed(GLint ssaa_factor) override;
      virtual void on_animation_time_changed(GLfloat t) override;

    protected:
      virtual void draw_opaque_impl() override;
      virtual void draw_transparent_impl() override;

  }; // class VectorView
} // namespace bk

#endif //BK_VECTORVIEW_H
