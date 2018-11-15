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

#include <iterator>
#include <limits>
#include <string_view>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bk/Line>
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
  //====================================================================================================
  //===== DEFINITIONS
  //====================================================================================================
  using self_type = VectorView;
  using base_type = details::AbstractSceneRenderable;

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
  VectorView();
  #else
  VectorView(bk::qt_gl_functions* gl);
  #endif
  VectorView(const self_type&) = delete;
  VectorView(self_type&&) noexcept;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  virtual ~VectorView();
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET LINE WIDTH
  [[nodiscard]] GLfloat line_width() const;
  /// @}

  /// @{ -------------------------------------------------- GET COLOR
  [[nodiscard]] const ColorRGBA& color() const;
  /// @}

  /// @{ -------------------------------------------------- GET COLOR ATTRIBUTES
  [[nodiscard]] bool color_by_attribute_is_enabled() const;
  [[nodiscard]] GLfloat color_attribute_min() const;
  [[nodiscard]] GLfloat color_attribute_max() const;

  [[nodiscard]] ColorBarView& colorbarview();
  [[nodiscard]] const ColorBarView& colorbarview() const;
  /// @}

  /// @{ -------------------------------------------------- GET ISL
  [[nodiscard]] GLfloat shininess() const;
  [[nodiscard]] bool isl_is_enabled() const;
  /// @}

  /// @{ -------------------------------------------------- GET HALO
  [[nodiscard]] GLfloat halo_width_in_percent() const;
  [[nodiscard]] bool halo_is_enabled() const;
  /// @}

  /// @{ -------------------------------------------------- GET CENTER
  [[nodiscard]] virtual bk::Vec3<GLfloat> center() const override;
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  [[nodiscard]] bool is_initialized() const;
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
  [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
  /// @}

  /// @{ -------------------------------------------------- SET LINE WIDTH
  void set_line_width(GLfloat w);
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void set_color(const ColorRGBA& c);
  void set_color(double r, double g, double b);
  /// @}

  /// @{ -------------------------------------------------- SET COLOR BY ATTRIBUTE ENABLED
  void set_color_by_attribute_enabled(bool b);
  void enable_color_by_attribute();
  void disable_color_by_attribute();
  /// @}

  /// @{ -------------------------------------------------- SET COLORBAR
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
  /// @}

  /// @{ -------------------------------------------------- HELPERS: SET COLOR ATTRIBUTE
  private:
  void _init_set_color_attribute(std::string_view color_attribute_name);
  [[nodiscard]] GLfloat* _map_vbo();
  void _unmap_vbo();
  void _reset_color_attribute_min_max();
  void _finalize_set_color_attribute();
  public:
  /// @}

  /// @{ -------------------------------------------------- SET COLOR ATTRIBUTES
  void set_color_attribute_min(GLfloat v);
  void set_color_attribute_max(GLfloat v);
  void set_color_attribute_clamp_min(GLfloat v);
  void set_color_attribute_clamp_max(GLfloat v);
  void set_attrib_scale_to_colorbar(bool b);
  void set_color_attribute_transparency_enabled(bool b);

  template<typename TLinesIterator>
  void set_color_attribute(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name, double quantile_low = 0, double quantile_high = 1, std::string_view custom_colorbar_title = "", const ScalarLineFilter* filter = nullptr);
  /// @}

  /// @{ -------------------------------------------------- SET ISL
  void set_shininess(GLfloat shininess);
  void set_isl_enabled(bool b);
  /// @}

  /// @{ -------------------------------------------------- SET HALO
  void set_halo_width_in_percent(GLfloat p);
  void set_halo_enabled(bool b);
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  private:
  void clear_color_attribute();
  void clear_shader();
  void clear_buffers();
  void clear_lineao_buffers_textures();
  void clear_lineao_shader();
  public:
  void clear();
  /// @}

  /// @{ -------------------------------------------------- HELPERS: INIT LINES
  private:
  void _init_lines(std::string_view color_attribute_name);
  [[nodiscard]] bool _line_has_time_attribute(const bk::Line<3>& line) const;
  [[nodiscard]] bool _line_has_attribute(const bk::Line<3>& line, std::string_view name) const;
  [[nodiscard]] unsigned int _floats_per_vertex() const;
  [[nodiscard]] bool _lines_have_time_attribute() const;
  [[nodiscard]] bool _lines_have_color_attribute() const;
  void _add_to_center(double x, double y, double z);
  void _update_attribute_min_max(GLfloat newAttribVal);
  void _vao_add_attributes();
  void _init_line_buffers(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, unsigned int num_points_total);
  void _init_colorbar(std::string_view color_attribute_name, std::string_view custom_colorbar_title = "");
  public:
  /// @}

  /// @{ -------------------------------------------------- INIT
  private:
  template<typename TLinesIterator>
  void init_lines(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name);
  void init_shader();
  void init_ubo();
  public:
  template<typename TLinesIterator>
  void init(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name = "");
  /// @}

  /// @{ -------------------------------------------------- EVENTS
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
  /// @}

  protected:
  /// @{ -------------------------------------------------- DRAW
  virtual void draw_opaque_impl() override;
  virtual void draw_transparent_impl() override;
  /// @}
}; // class VectorView
} // namespace bk

#include <bkGL/renderable/line/VectorView.hpp>

#endif //BK_VECTORVIEW_H
