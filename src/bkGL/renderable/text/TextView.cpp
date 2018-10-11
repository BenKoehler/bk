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

#include <bkGL/renderable/text/TextView.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>

#include <bk/Matrix>
#include <bkTools/color/ColorRGBA.h>

#include <bkGL/buffer/VBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bkGL/shader/ShaderLibrary.h>
#include <bkGL/UBOText.h>

namespace bk
{
  //====================================================================================================
  //===== ENUMS
  //====================================================================================================
  enum TextOrientation_ : unsigned int
  {
      TextOrientation_Horizontal = 0,//
      TextOrientation_Vertical = 1,//
  };

  using TextOrientation = unsigned int;

  //====================================================================================================
  //===== STRUCTS
  //====================================================================================================
  namespace details
  {
        #ifndef BK_LIB_QT_AVAILABLE

    FreeTypeCharacter::FreeTypeCharacter()
        #else
    FreeTypeCharacter::FreeTypeCharacter(bk::qt_gl_functions* gl) :
    tex(gl)
        #endif
    { /* do nothing */ }

    FreeTypeCharacter::FreeTypeCharacter(FreeTypeCharacter&&) = default;
    FreeTypeCharacter::~FreeTypeCharacter() = default;
    FreeTypeCharacter& FreeTypeCharacter::operator=(FreeTypeCharacter&&) = default;
  } // namespace details

  //====================================================================================================
  //===== STATIC MEMBERS
  //====================================================================================================
  bool TextView::ft_initialized = false;

  FT_Library TextView::ft_lib;

  FT_Face TextView::ft_face;

  std::map<GLchar, details::FreeTypeCharacter> TextView::ft_map;

  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct TextView::Impl
  {
      VBO vbo_text;
      VAO vao_text;
      Shader shader_text;
      VBO vbo_background;
      VAO vao_background;
      Shader shader_background;
      details::UBOText ubo;
      std::string text;
      bk::Vec2<GLfloat> pos;
      bool position_is_absolute;
      bk::ColorRGBA color_text;
      bk::ColorRGBA color_background;
      bool background_enabled;
      bk::Vec2<GLfloat> scale;
      bk::Vec2<GLfloat> windowSize_initial;
      bk::Vec2<GLfloat> windowSize_current;
      bk::Vec2<GLfloat> scaleCorrectionWindowSize;
      TextOrientation orientation;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else
      explicit Impl(bk::qt_gl_functions* gl)
          : vbo_text(gl),
            vao_text(gl),
            shader_text(gl),
            vbo_background(gl),
            vao_background(gl),
            shader_background(gl),
            ubo(gl),
          #endif
          pos(0, 0),
          position_is_absolute(true),
          color_text(bk::ColorRGBA::White()),
          color_background(0, 0, 0, 0.75),
          //color_background(0, 0, 0, 1),
          background_enabled(true),
          scale(1, 1),
          windowSize_initial(-1, -1),
          windowSize_current(-1, -1),
          scaleCorrectionWindowSize(1, 1),
          orientation(TextOrientation_Horizontal)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  }; // TextView::Impl

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  TextView::TextView()
      : base_type()
  #else

  TextView::TextView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      _pdata->vbo_text.set_usage_DYNAMIC_DRAW();
      _pdata->vao_text.add_default_attribute_position_2xfloat();
      _pdata->vao_text.add_default_attribute_texture_coordinates_2xfloat();

      _pdata->vbo_background.set_usage_DYNAMIC_DRAW();
      _pdata->vao_background.add_default_attribute_position_2xfloat();
  }

  TextView::TextView(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  TextView::~TextView() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET TEXT
  const std::string& TextView::text() const
  { return _pdata->text; }
  /// @}

  /// @{ -------------------------------------------------- GET POSITION
  GLfloat TextView::position_x() const
  { return _pdata->pos[0]; }

  GLfloat TextView::position_y() const
  { return _pdata->pos[1]; }
  /// @}

  /// @{ -------------------------------------------------- GET POSITION MODE
  bool TextView::position_mode_is_absolute() const
  { return _pdata->position_is_absolute; }

  bool TextView::position_mode_is_relative() const
  { return !_pdata->position_is_absolute; }
  /// @}

  /// @{ -------------------------------------------------- GET COLOR
  const ColorRGBA& TextView::color_text() const
  { return _pdata->color_text; }

  const ColorRGBA& TextView::color_background() const
  { return _pdata->color_background; }
  /// @}

  /// @{ -------------------------------------------------- BACKGROUND IS ENABLED
  bool TextView::background_is_enabled() const
  { return _pdata->background_enabled; }
  /// @}

  /// @{ -------------------------------------------------- GET SCALE
  GLfloat TextView::scale_x() const
  { return _pdata->scale[0]; }

  GLfloat TextView::scale_y() const
  { return _pdata->scale[1]; }
  /// @}

  /// @{ -------------------------------------------------- GET CURRENT TEXT WIDTH IN PIXEL
  GLfloat TextView::text_pixel_width_absolute() const
  {
      GLfloat w = 0;

      for (const char& c: _pdata->text)
      {
          details::FreeTypeCharacter& character = TextView::ft_map[c];
          w += static_cast<GLfloat>(character.advance >> 6);
      }

      w *= _pdata->scale[0] * _pdata->scaleCorrectionWindowSize[0];

      return w;
  }

  GLfloat TextView::text_pixel_width_relative() const
  { return text_pixel_width_absolute() / _pdata->windowSize_current[0]; }

  GLfloat TextView::text_pixel_width() const
  { return position_mode_is_relative() ? text_pixel_width_relative() : text_pixel_width_absolute(); }

  GLfloat TextView::text_pixel_height_absolute() const
  {
      GLfloat h = 0;

      for (const char& c: _pdata->text)
      {
          details::FreeTypeCharacter& character = TextView::ft_map[c];
          h = std::max(h, static_cast<GLfloat>(character.size_y));
      }

      return h * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1];
  }

  GLfloat TextView::text_pixel_height_relative() const
  { return text_pixel_height_absolute() / _pdata->windowSize_current[1]; }

  GLfloat TextView::text_pixel_height() const
  { return position_mode_is_relative() ? text_pixel_height_relative() : text_pixel_height_absolute(); }

  GLfloat TextView::text_max_bearing_y_absolute() const
  {
      GLfloat by = 0;

      for (const char& c: _pdata->text)
      {
          details::FreeTypeCharacter& character = TextView::ft_map[c];
          by = std::max(by, static_cast<GLfloat>(character.bearing_y));
      }

      return by * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1];
  }

  GLfloat TextView::text_max_bearing_y_relative() const
  { return text_max_bearing_y_absolute() / _pdata->windowSize_current[1]; }

  GLfloat TextView::text_max_bearing_y() const
  { return position_mode_is_relative() ? text_max_bearing_y_relative() : text_max_bearing_y_absolute(); }

  GLfloat TextView::text_max_off_y_absolute() const
  {
      GLfloat by = 0;

      for (const char& c: _pdata->text)
      {
          details::FreeTypeCharacter& character = TextView::ft_map[c];
          by = std::max(by, static_cast<GLfloat>(character.size_y) - static_cast<GLfloat>(character.bearing_y));
      }

      return by * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1];
  }

  GLfloat TextView::text_max_off_y_relative() const
  { return text_max_off_y_absolute() / _pdata->windowSize_current[1]; }

  GLfloat TextView::text_max_off_y() const
  { return position_mode_is_relative() ? text_max_off_y_relative() : text_max_off_y_absolute(); }
  /// @}

  /// @{ -------------------------------------------------- GET ORIENTATION
  bool TextView::orientation_is_horizontal() const
  { return _pdata->orientation == TextOrientation_Horizontal; }

  bool TextView::orientation_is_vertical() const
  { return _pdata->orientation == TextOrientation_Vertical; }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool TextView::is_initialized() const
  { return _pdata->shader_text.is_initialized() && _pdata->vao_text.is_initialized() && _pdata->ubo.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto TextView::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET TEXT
  void TextView::set_text(std::string_view txt)
  {
      _pdata->text = txt;

      update_vbo_background();

      if (this->is_initialized())
      { this->emit_signal_update_required(); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET POSITION
  void TextView::set_position(GLfloat x, GLfloat y)
  {
      if (orientation_is_horizontal())
      {
          _pdata->pos[0] = x;
          _pdata->pos[1] = y;
      }
      else
      {
          _pdata->pos[0] = y;
          _pdata->pos[1] = x;
      }

      update_vbo_background();

      if (this->is_initialized())
      { this->emit_signal_update_required(); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET POSITION MODE
  void TextView::_set_position_mode_absolute(bool b)
  {
      if (_pdata->position_is_absolute != b)
      {
          _pdata->position_is_absolute = b;

          update_vbo_background();

          if (this->is_initialized())
          {
              _pdata->ubo.set_pos_is_absolute(static_cast<GLint>(_pdata->position_is_absolute ? 1 : 0));
              _pdata->ubo.release();

              this->emit_signal_update_required();
          }
      }
  }

  void TextView::set_position_mode_absolute()
  { _set_position_mode_absolute(true); }

  void TextView::set_position_mode_relative()
  { _set_position_mode_absolute(false); }
  /// @}

  /// @{ -------------------------------------------------- SET COLOR
  void TextView::set_color_text(const ColorRGBA& c)
  { set_color_text(c.r(), c.g(), c.b(), c.alpha()); }

  void TextView::set_color_text(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      _pdata->color_text[0] = r;
      _pdata->color_text[1] = g;
      _pdata->color_text[2] = b;
      _pdata->color_text[3] = a;

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_text_r(_pdata->color_text[0]);
          _pdata->ubo.set_color_text_g(_pdata->color_text[1]);
          _pdata->ubo.set_color_text_b(_pdata->color_text[2]);
          _pdata->ubo.set_color_text_a(_pdata->color_text[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }

  void TextView::set_color_background(const ColorRGBA& c)
  { set_color_background(c.r(), c.g(), c.b(), c.alpha()); }

  void TextView::set_color_background(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  {
      _pdata->color_background[0] = r;
      _pdata->color_background[1] = g;
      _pdata->color_background[2] = b;
      _pdata->color_background[3] = a;

      if (this->is_initialized())
      {
          _pdata->ubo.set_color_background_r(_pdata->color_background[0]);
          _pdata->ubo.set_color_background_g(_pdata->color_background[1]);
          _pdata->ubo.set_color_background_b(_pdata->color_background[2]);
          _pdata->ubo.set_color_background_a(_pdata->color_background[3]);
          _pdata->ubo.release();

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- SET BACKGROUND ENABLED
  void TextView::set_background_enabled(bool b)
  {
      if (_pdata->background_enabled != b)
      {
          _pdata->background_enabled = b;

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  void TextView::enable_background()
  { set_background_enabled(true); }

  void TextView::disable_background()
  { set_background_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET SCALE
  void TextView::set_scale(GLfloat sx, GLfloat sy)
  {
      if (sx != _pdata->scale[0] || sy != _pdata->scale[1])
      {
          _pdata->scale[0] = sx;
          _pdata->scale[1] = sy;

          update_vbo_background();

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  void TextView::set_scale(GLfloat sxy)
  { set_scale(sxy, sxy); }
  /// @}

  /// @{ -------------------------------------------------- SET ORIENTATION
  void TextView::set_orientation_horizontal()
  {
      if (orientation_is_vertical())
      { std::swap(_pdata->pos[0], _pdata->pos[1]); }

      _pdata->orientation = TextOrientation_Horizontal;

      if (this->is_initialized())
      { init(); }
  }

  void TextView::set_orientation_vertical()
  {
      if (orientation_is_horizontal())
      { std::swap(_pdata->pos[0], _pdata->pos[1]); }

      _pdata->orientation = TextOrientation_Vertical;

      if (this->is_initialized())
      { init(); }
  }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void TextView::clear_shaders()
  {
      _pdata->shader_text.clear();
      _pdata->shader_background.clear();
  }

  void TextView::clear_buffers()
  {
      _pdata->vbo_text.clear();
      _pdata->vao_text.clear();
      _pdata->ubo.clear();

      _pdata->vbo_background.clear();
      _pdata->vao_background.clear();
  }

  void TextView::clear()
  {
      clear_shaders();
      clear_buffers();
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool TextView::init_freetype(std::string_view path_to_font)
  {
      if (TextView::ft_initialized)
      { return true; }

      if (FT_Init_FreeType(&TextView::ft_lib))
      {
          std::cerr << "freetype: init failed" << std::endl;
          return false;
      }

      if (FT_New_Face(TextView::ft_lib, path_to_font.data(), 0, &TextView::ft_face))
      {
          std::cerr << "freetype: load font failed (path: \"" << path_to_font.data() << "\")" << std::endl;
          return false;
      }

      FT_Set_Pixel_Sizes(TextView::ft_face, 0, 48);

      BK_QT_GL glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      for (GLubyte charId = 0; charId < 128; ++charId)
      {
          if (FT_Load_Char(TextView::ft_face, charId, FT_LOAD_RENDER))
          {
              std::cerr << "freetype: load glyph (id " << static_cast<unsigned int>(charId) << ") failed" << std::endl;
              continue;
          }

          std::pair<GLchar, details::FreeTypeCharacter> currentChar(charId, details::FreeTypeCharacter(
          #ifdef BK_LIB_QT_AVAILABLE
              this->_gl
          #endif
          ));

          Texture2D& tex = currentChar.second.tex;
          tex.set_format_RED();
          tex.set_mipmap_level(0);
          tex.set_border_width(0);
          tex.set_pixel_data_type_UNSIGNED_BYTE();
          tex.set_internal_format_RED();
          tex.set_width(TextView::ft_face->glyph->bitmap.width);
          tex.set_height(TextView::ft_face->glyph->bitmap.rows);
          tex.set_texture_unit_number(1);

          if (!tex.init(TextView::ft_face->glyph->bitmap.buffer))
          { std::cerr << "freetype: tex init failed!" << std::endl; }

          tex.bind();
          tex.set_texture_coordinates_CLAMP_TO_EDGE();
          tex.set_interpolation_LINEAR();
          tex.release();

          currentChar.second.size_x = TextView::ft_face->glyph->bitmap.width;
          currentChar.second.size_y = TextView::ft_face->glyph->bitmap.rows;
          currentChar.second.bearing_x = TextView::ft_face->glyph->bitmap_left;
          currentChar.second.bearing_y = TextView::ft_face->glyph->bitmap_top;
          currentChar.second.advance = TextView::ft_face->glyph->advance.x;

          TextView::ft_map.insert(std::move(currentChar));
      } // for charId

      FT_Done_Face(TextView::ft_face);
      FT_Done_FreeType(TextView::ft_lib);

      TextView::ft_initialized = !TextView::ft_map.empty();

      return TextView::ft_initialized;
  }

  void TextView::init_shader()
  {
      clear_shaders();

      using SL = details::ShaderLibrary::text;

      _pdata->shader_text.init_from_sources(SL::vert_text(), SL::frag_text());
      _pdata->shader_background.init_from_sources(SL::vert_background(), SL::frag_background());
  }

  void TextView::init_buffers()
  {
      /*
       * UBO
       */
      _pdata->ubo.init_from_registered_values_size();
      _pdata->ubo.set_color_text_r(_pdata->color_text[0]);
      _pdata->ubo.set_color_text_g(_pdata->color_text[1]);
      _pdata->ubo.set_color_text_b(_pdata->color_text[2]);
      _pdata->ubo.set_color_text_a(_pdata->color_text[3]);
      _pdata->ubo.set_color_background_r(_pdata->color_background[0]);
      _pdata->ubo.set_color_background_g(_pdata->color_background[1]);
      _pdata->ubo.set_color_background_b(_pdata->color_background[2]);
      _pdata->ubo.set_color_background_a(_pdata->color_background[3]);
      _pdata->ubo.set_pos_is_absolute(static_cast<GLint>(_pdata->position_is_absolute ? 1 : 0));
      _pdata->ubo.release();

      /*
       * VBO, VAO (TEXT)
       */
      /* vertex ordering:

            2 ------ 3
            |\       |
            |  \     |
            |    \   |
            |      \ |
            0 ------ 1
        */
      constexpr unsigned int N_text = 16;

      //constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {
      //    /*vert0*/ -1, -1,
      //    /*texCoord0*/ 0, 0,
      //    /*vert1*/ 1, -1,
      //    /*texCoord1*/ 0, 1,
      //    /*vert2*/ -1, 1,
      //    /*texCoord2*/ 1, 0,
      //    /*vert3*/ 1, 1,
      //    /*texCoord3*/ 1, 1};

      //_pdata->vbo_text.init(vertices_texCoords_interleaved_text, N_text * sizeof(GLfloat));

      if (orientation_is_horizontal())
      {
          constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
                                                                           0, 0, // texCoord0
                                                                           1, -1, // vert1
                                                                           0, 1, // texCoord1
                                                                           -1, 1, // vert2
                                                                           1, 0, // texCoord2
                                                                           1, 1, // vert3
                                                                           1, 1 // texCoord3
          };

          _pdata->vbo_text.init(vertices_texCoords_interleaved_text, N_text * sizeof(GLfloat));
      }
      else
      {
          //constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
          //                                                                 1, 0, // texCoord1
          //                                                                 1, -1, // vert1
          //                                                                 0, 0, // texCoord0
          //                                                                 -1, 1, // vert2
          //                                                                 1, 1, // texCoord3
          //                                                                 1, 1, // vert3
          //                                                                 0, 1, // texCoord1

          //constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
          //                                                                 1, 0, // texCoord1
          //                                                                 1, -1, // vert1
          //                                                                 1, 1, // texCoord3
          //                                                                 -1, 1, // vert2
          //                                                                 0, 0, // texCoord0
          //                                                                 1, 1, // vert3
          //                                                                 0, 1, // texCoord1
          //    };

          //constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
          //                                                                 1, 1, // texCoord1
          //                                                                 1, -1, // vert1
          //                                                                 1, 0, // texCoord0
          //                                                                 -1, 1, // vert2
          //                                                                 0, 1, // texCoord3
          //                                                                 1, 1, // vert3
          //                                                                 1, 0, // texCoord1
          //};

          //constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
          //                                                                 0, 1, // texCoord0
          //                                                                 1, -1, // vert1
          //                                                                 0, 0, // texCoord1
          //                                                                 -1, 1, // vert2
          //                                                                 1, 1, // texCoord2
          //                                                                 1, 1, // vert3
          //                                                                 1, 0 // texCoord3
          //};

          constexpr GLfloat vertices_texCoords_interleaved_text[N_text] = {-1, -1, // vert0
                                                                           0, 0, // texCoord0
                                                                           1, -1, // vert1
                                                                           0, 1, // texCoord1
                                                                           -1, 1, // vert2
                                                                           1, 0, // texCoord2
                                                                           1, 1, // vert3
                                                                           1, 1 // texCoord3
          };

          _pdata->vbo_text.init(vertices_texCoords_interleaved_text, N_text * sizeof(GLfloat));
      }

      _pdata->vao_text.init(_pdata->vbo_text);

      /*
       * VBO, VAO (BACKGROUND)
       */
      constexpr unsigned int N_background = 8;
      constexpr GLfloat vertices_background[N_background] = {
          /*vert0*/ -1, -1,
          /*vert1*/ 1, -1,
          /*vert2*/ -1, 1,
          /*vert3*/ 1, 1};

      _pdata->vbo_background.init(vertices_background, N_background * sizeof(GLfloat));
      _pdata->vao_background.init(_pdata->vbo_background);
  }

  bool TextView::init()
  { return init(_pdata->text, BK_GL_FONT_PATH); }

  bool TextView::init(std::string_view txt, std::string_view path_to_font)
  {
      clear();

      _pdata->text = txt;
      const bool success = TextView::init_freetype(path_to_font);
      init_shader();
      init_buffers();
      update_vbo_background();

      return success;
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE POSITION
  GLfloat TextView::update_vbo_text(GLfloat x, GLfloat y, const details::FreeTypeCharacter& character)
  {
      //if (orientation_is_horizontal())
      //{
      if (!this->is_initialized())
      { return x; }

      const GLfloat offy = text_max_off_y();

      GLfloat bx = character.bearing_x;
      GLfloat by = character.bearing_y;
      GLfloat sx = character.size_x;
      GLfloat sy = character.size_y;
      if (position_mode_is_relative())
      {
          bx /= _pdata->windowSize_current[0];
          by /= _pdata->windowSize_current[1];
          sx /= _pdata->windowSize_current[0];
          sy /= _pdata->windowSize_current[1];
      }

      GLfloat* data = _pdata->vbo_text.map_write_only<GLfloat>();
      if (data != nullptr)
      {
          const GLfloat xpos = x + bx * _pdata->scale[0] * _pdata->scaleCorrectionWindowSize[0];
          const GLfloat ypos = y - (sy - by) * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1] + offy;

          const GLfloat w = sx * _pdata->scale[0] * _pdata->scaleCorrectionWindowSize[0];
          const GLfloat h = sy * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1];

          if (orientation_is_horizontal())
          {
              data[0] = xpos;
              data[1] = ypos + h;

              data[4] = xpos;
              data[5] = ypos;

              data[8] = xpos + w;
              data[9] = ypos + h;

              data[12] = xpos + w;
              data[13] = ypos;
          }
          else
          {
              //data[0] = ypos;
              //data[1] = xpos;
              //
              //data[4] = ypos + h;
              //data[5] = xpos;
              //
              //data[8] = ypos;
              //data[9] = xpos + w;
              //
              //data[12] = ypos + h;
              //data[13] = xpos + w;

              const GLfloat corr = (sy - by) * _pdata->scale[1] * _pdata->scaleCorrectionWindowSize[1];

              data[0] = ypos+corr;
              data[1] = xpos;

              data[4] = ypos + h;
              data[5] = xpos;

              data[8] = ypos+corr;
              data[9] = xpos + w;

              data[12] = ypos + h;
              data[13] = xpos + w;
          }

          _pdata->vbo_text.unmap_and_release();
      }

      return x + static_cast<GLfloat>(character.advance >> 6) / (position_mode_is_relative() ? _pdata->windowSize_current[0] : 1) * _pdata->scale[0] * _pdata->scaleCorrectionWindowSize[0];
  }

  void TextView::update_vbo_background()
  {
      if (!this->is_initialized())
      { return; }

      const GLfloat offy = text_max_off_y();

      GLfloat* data = _pdata->vbo_background.map_write_only<GLfloat>();
      if (data != nullptr)
      {
          const GLfloat x0 = _pdata->pos[0];
          const GLfloat x1 = x0 + text_pixel_width();

          const GLfloat y0 = _pdata->pos[1];
          const GLfloat y1 = y0 + text_pixel_height() + offy;

          if (orientation_is_horizontal())
          {
              data[0] = x0;
              data[1] = y1;

              data[2] = x0;
              data[3] = y0;

              data[4] = x1;
              data[5] = y1;

              data[6] = x1;
              data[7] = y0;
          }
          else
          {
              data[0] = y1;
              data[1] = x0;

              data[2] = y0;
              data[3] = x0;

              data[4] = y1;
              data[5] = x1;

              data[6] = y0;
              data[7] = x1;
          }

          _pdata->vbo_background.unmap_and_release();
      }

      if (_pdata->background_enabled)
      { this->emit_signal_update_required(); }
  }
  /// @}

  /// @{ -------------------------------------------------- EVENTS
  void TextView::on_resize(GLint w, GLint h)
  {
      if (_pdata->windowSize_initial[0] == -1 || _pdata->windowSize_initial[1] == -1)
      {
          _pdata->windowSize_initial[0] = w;
          _pdata->windowSize_initial[1] = h;
      }

      _pdata->windowSize_current[0] = w;
      _pdata->windowSize_current[1] = h;

      _pdata->scaleCorrectionWindowSize[0] = static_cast<GLfloat>(w) / _pdata->windowSize_initial[0];
      _pdata->scaleCorrectionWindowSize[1] = static_cast<GLfloat>(h) / _pdata->windowSize_initial[1];
      //_pdata->scaleCorrectionWindowSize[0] = 1;
      //_pdata->scaleCorrectionWindowSize[1] = 1;

      update_vbo_background();
  }

  void TextView::on_oit_enabled(bool /*b*/)
  { /* do nothing */ }

  void TextView::on_animation_enabled(bool /*b*/)
  { /* do nothing */ }

  void TextView::on_modelview_changed(bool /*b*/)
  { /* do nothing */ }

  void TextView::on_visible_changed(bool /*b*/)
  { /* do nothing */ }

  void TextView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void TextView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void TextView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void TextView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void TextView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void TextView::on_mouse_wheel_up()
  { /* do nothing */ }

  void TextView::on_mouse_wheel_down()
  { /* do nothing */ }

  void TextView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void TextView::draw_impl()
  {
      _pdata->ubo.bind_to_default_base();

      BK_QT_GL glPushAttrib(GL_COLOR_BUFFER_BIT);
      BK_QT_GL glEnable(GL_BLEND);
      BK_QT_GL glDepthFunc(GL_ALWAYS);

      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      BK_QT_GL glMatrixMode(GL_MODELVIEW);
      BK_QT_GL glPushMatrix();
      BK_QT_GL glLoadIdentity();

      if (_pdata->background_enabled)
      {
          _pdata->shader_background.bind();
          _pdata->vao_background.bind();
          BK_QT_GL glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
          _pdata->vao_background.release();
          _pdata->shader_background.release();
      }

      _pdata->shader_text.bind();
      _pdata->vao_text.bind();

      GLfloat currentAdvance = _pdata->pos[0];
      for (const char& c: _pdata->text)
      {
          details::FreeTypeCharacter& character = TextView::ft_map[c];
          currentAdvance = update_vbo_text(currentAdvance, _pdata->pos[1], character);

          character.tex.bind();
          BK_QT_GL glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
          character.tex.release();
      }

      _pdata->vao_text.release();
      _pdata->shader_text.release();

      BK_QT_GL glDepthFunc(GL_LESS);

      BK_QT_GL glPopAttrib();

      BK_QT_GL glPopMatrix();
      BK_QT_GL glMatrixMode(GL_PROJECTION);
      BK_QT_GL glPopMatrix();

      _pdata->ubo.release_from_base();
  }
  /// @}
} // namespace bk