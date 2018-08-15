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

#include <bkGL/renderable/background/UniformBackground.h>

#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkTools/color/ColorRGBA.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct UniformBackground::Impl
  {
      color_type col;

      Impl()
          : col(ColorRGBA::Light_Gray())
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  UniformBackground::UniformBackground()
      : base_type(),
  #else

  UniformBackground::UniformBackground(bk::qt_gl_functions* gl)
      : base_type(gl),
  #endif
        _pdata(std::make_unique<Impl>())
  { /* do nothing */ }

  UniformBackground::UniformBackground(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UniformBackground::~UniformBackground() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET COLOR
  auto UniformBackground::color() const -> const color_type&
  { return _pdata->col; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UniformBackground::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void UniformBackground::set_color(const color_type& col)
  { _pdata->col.set(col); }

  void UniformBackground::set_color(color_type&& col)
  { _pdata->col.set(std::move(col)); }

  void UniformBackground::set_color(double r, double g, double b, double a)
  { _pdata->col.set(r, g, b, a); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void UniformBackground::init_vbo_vao()
  {
      /* vertex ordering:

           2 ------ 3
           |\       |
           |  \     |
           |    \   |
           |      \ |
           0 ------ 1
       */

      const GLfloat r = static_cast<GLfloat>(_pdata->col[0]);
      const GLfloat g = static_cast<GLfloat>(_pdata->col[1]);
      const GLfloat b = static_cast<GLfloat>(_pdata->col[2]);
      const GLfloat a = static_cast<GLfloat>(_pdata->col[3]);

      constexpr const unsigned int N = 24;

      const GLfloat vertices_colors_interleaved[N] = {
          /*vert 0*/ -1, -1,
          /* col 0*/ r, g, b, a,
          /*vert 1*/ 1, -1,
          /* col 1*/ r, g, b, a,
          /*vert 2*/ -1, 1,
          /* col 2*/ r, g, b, a,
          /*vert 3*/ 1, 1,
          /* col 3*/ r, g, b, a};
      vbo().init(vertices_colors_interleaved, N * sizeof(GLfloat));
      vao().init(vbo());
  }

  void UniformBackground::update_colors()
  {
      bool do_update = false;

      if (this->is_initialized())
      {
          GLfloat* x = vbo().map_write_only<GLfloat>();

          if (x != nullptr)
          {
              constexpr unsigned int floatsPerVertex = 6;

              for (unsigned int i = 0; i < 4 * floatsPerVertex; i += floatsPerVertex)
              {
                  for (unsigned int k = 0; k < 4; ++k)
                  { x[i + 2 + k] = static_cast<GLfloat>(_pdata->col[k]); }
              }

              vbo().unmap_and_release();
              do_update = true;
          }
      }

      if (do_update)
      { this->emit_signal_update_required(); }
  }
} // namespace bk


