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

#include <bkGL/renderable/background/GradientBackground.h>

#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkTools/color/ColorRGBA.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct GradientBackground::Impl
  {
      color_type col_bottom_left;
      color_type col_top_right;
      bool vertical;

      Impl()
          : col_bottom_left(166.0 / 255.0, 197.0 / 255.0, 1),
            col_top_right(ColorRGBA::White()),
            vertical(true)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  GradientBackground::GradientBackground()
      : base_type(),
  #else

      GradientBackground::GradientBackground(bk::qt_gl_functions* gl) : base_type(gl),
  #endif
        _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  GradientBackground::GradientBackground(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  GradientBackground::~GradientBackground() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET COLOR
  auto GradientBackground::color_bottom_or_left() const -> const color_type&
  { return _pdata->col_bottom_left; }

  auto GradientBackground::color_top_or_right() const -> const color_type&
  { return _pdata->col_top_right; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto GradientBackground::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void GradientBackground::set_color_bottom_or_left(const color_type& col)
  { _pdata->col_bottom_left.set(col); }

  void GradientBackground::set_color_bottom_or_left(color_type&& col)
  { _pdata->col_bottom_left.set(std::move(col)); }

  void GradientBackground::set_color_bottom_or_left(double r, double g, double b, double a)
  { _pdata->col_bottom_left.set(r, g, b, a); }

  void GradientBackground::set_color_top_or_right(const color_type& col)
  { _pdata->col_top_right.set(col); }

  void GradientBackground::set_color_top_or_right(color_type&& col)
  { _pdata->col_top_right.set(std::move(col)); }

  void GradientBackground::set_color_top_or_right(double r, double g, double b, double a)
  { _pdata->col_top_right.set(r, g, b, a); }
  /// @}

  /// @{ -------------------------------------------------- SET VERTICAL / HORIZONTAL
  void GradientBackground::set_vertical()
  {
      _pdata->vertical = true;
      update_colors();
  }

  void GradientBackground::set_horizontal()
  {
      _pdata->vertical = false;
      update_colors();
  }
  /// @}

  /// @{ -------------------------------------------------- SET DEFAULT
  void GradientBackground::set_default_bk()
  {
      set_vertical();
      set_color_bottom_or_left(166.0 / 255.0, 197.0 / 255.0, 1);
      set_color_top_or_right(ColorRGBA::White());
      update_colors();
  }

  void GradientBackground::set_default_gray_vertical()
  {
      set_vertical();
      set_color_bottom_or_left(ColorRGBA::Dark_Gray());
      set_color_top_or_right(ColorRGBA::Light_Gray());
      update_colors();
  }

  void GradientBackground::set_default_transparent()
  {
      set_vertical();
      set_color_bottom_or_left(0, 0, 0, 0);
      set_color_top_or_right(0, 0, 0, 0);
      update_colors();
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  void GradientBackground::init_vbo_vao()
  {
      constexpr const unsigned int N = 24;

      /* vertex ordering:

           2 ------ 3
           |\       |
           |  \     |
           |    \   |
           |      \ |
           0 ------ 1
       */

      const GLfloat blr = static_cast<GLfloat>(_pdata->col_bottom_left[0]);
      const GLfloat blg = static_cast<GLfloat>(_pdata->col_bottom_left[1]);
      const GLfloat blb = static_cast<GLfloat>(_pdata->col_bottom_left[2]);
      const GLfloat bla = static_cast<GLfloat>(_pdata->col_bottom_left[3]);

      const GLfloat trr = static_cast<GLfloat>(_pdata->col_top_right[0]);
      const GLfloat trg = static_cast<GLfloat>(_pdata->col_top_right[1]);
      const GLfloat trb = static_cast<GLfloat>(_pdata->col_top_right[2]);
      const GLfloat tra = static_cast<GLfloat>(_pdata->col_top_right[3]);

      if (_pdata->vertical)
      {
          const GLfloat vertices_colors_interleaved[N] = {
              /*vert 0*/ -1, -1,
              /* col 0*/ blr, blg, blb, bla,
              /*vert 1*/ 1, -1,
              /* col 1*/ blr, blg, blb, bla,
              /*vert 2*/ -1, 1,
              /* col 2*/ trr, trg, trb, tra,
              /*vert 3*/ 1, 1,
              /* col 3*/ trr, trg, trb, tra};

          vbo().init(vertices_colors_interleaved, N * sizeof(GLfloat));
      }
      else
      {
          const GLfloat vertices_colors_interleaved[N] = {
              /*vert 0*/ -1, -1,
              /* col 0*/ blr, blg, blb, bla,
              /*vert 1*/ 1, -1,
              /* col 1*/ trr, trg, trb, tra,
              /*vert 2*/ -1, 1,
              /* col 2*/ blr, blg, blb, bla,
              /*vert 3*/ 1, 1,
              /* col 3*/ trr, trg, trb, tra};
          vbo().init(vertices_colors_interleaved, N * sizeof(GLfloat));
      }

      vao().init(vbo());
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE
  void GradientBackground::update_colors()
  {
      bool do_update = false;

      if (this->is_initialized())
      {
          GLfloat* x = vbo().map_write_only<GLfloat>();

          if (x != nullptr)
          {
              constexpr unsigned int floatsPerVertex = 6;

              for (unsigned int k = 0; k < 4; ++k)
              {
                  x[0 * floatsPerVertex + 2 + k] = static_cast<GLfloat>(_pdata->col_bottom_left[k]);
                  x[1 * floatsPerVertex + 2 + k] = static_cast<GLfloat>(_pdata->vertical ? _pdata->col_top_right[k] : _pdata->col_bottom_left[k]);
                  x[2 * floatsPerVertex + 2 + k] = static_cast<GLfloat>(_pdata->vertical ? _pdata->col_bottom_left[k] : _pdata->col_top_right[k]);
                  x[3 * floatsPerVertex + 2 + k] = static_cast<GLfloat>(_pdata->col_top_right[k]);
              }

              vbo().unmap_and_release();
              do_update = true;
          }
      }

      if (do_update)
      { this->emit_signal_update_required(); }
  }
  /// @}
} // namespace bk


