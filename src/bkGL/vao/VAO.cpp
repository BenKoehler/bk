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

#include <bkGL/vao/VAO.h>

#include <iostream>
#include <sstream>
#include <vector>

#include <bkGL/vao/VertexAttributePointer.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/gl_type_traits.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct VAO::Impl
  {
      std::vector<VertexAttributePointer> attributes;
      GLsizei stride_in_bytes;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  VAO::VAO()
      : base_type()
  #else
  VAO::VAO(bk::qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /* do nothing */ }

  VAO::VAO(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  VAO::~VAO()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM ATTRIBUTES
  GLuint VAO::num_attributes() const
  { return _pdata->attributes.size(); }
  /// @}

  /// @{ -------------------------------------------------- GET VERTEX ATTRIB POINTER
  const VertexAttributePointer& VAO::attribute(unsigned int i) const
  { return _pdata->attributes[i]; }
  /// @}

  /// @{ -------------------------------------------------- GET STRIDE
  GLsizei VAO::stride_in_bytes() const
  { return _pdata->stride_in_bytes; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto VAO::operator=(self_type&& other) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- ADD ATTRIBUTES
  void VAO::add_attribute(GLenum type, bool values_are_normalized, std::string name)
  {
      _pdata->attributes.emplace_back(_pdata->attributes.size(), type, values_are_normalized, std::move(name));
      _calc_stride();

      if (_pdata->attributes.size() >= 10)
      { std::cerr << "VAO::add_attribute - Warning! You have 10+ attributes in this VAO. Is this on purpose or did you forget to call clear_attributes() anywhere?" << std::endl; }
  }

  void VAO::add_default_attribute_position_2xfloat()
  { add_attribute(GL_FLOAT_VEC2, false, "position"); }

  void VAO::add_default_attribute_position_3xfloat()
  { add_attribute(GL_FLOAT_VEC3, false, "position"); }

  void VAO::add_default_attribute_position_2xfloat_plus_time_1xfloat()
  { add_attribute(GL_FLOAT_VEC3, false, "position"); }

  void VAO::add_default_attribute_position_3xfloat_plus_time_1xfloat()
  { add_attribute(GL_FLOAT_VEC4, false, "position"); }

  void VAO::add_default_attribute_texture_coordinates_2xfloat()
  { add_attribute(GL_FLOAT_VEC2, true, "texcoord"); }

  void VAO::add_default_attribute_texture_coordinates_3xfloat()
  { add_attribute(GL_FLOAT_VEC3, true, "texcoord"); }

  void VAO::add_default_attribute_normal_3xfloat()
  { add_attribute(GL_FLOAT_VEC3, true, "normal"); }

  void VAO::add_default_attribute_colorRGB_3xfloat()
  { add_attribute(GL_FLOAT_VEC3, false, "color"); }

  void VAO::add_default_attribute_colorRGBA_4xfloat()
  { add_attribute(GL_FLOAT_VEC4, false, "color"); }

  void VAO::add_default_attribute_scalar_1xfloat(std::string name)
  { add_attribute(GL_FLOAT, false, std::move(name)); }

  void VAO::add_default_attribute_index_1xuint()
  { add_attribute(GL_UNSIGNED_INT, false, "id"); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CALC STRIDE
  void VAO::_calc_stride()
  {
      _pdata->stride_in_bytes = 0;

      for (GLuint i = 0; i < num_attributes(); ++i)
      {
          const VertexAttributePointer& attrib = _pdata->attributes[i];
          _pdata->stride_in_bytes += attrib.size_in_bytes();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool VAO::init()
  {
      clear();
      BK_QT_GL glGenVertexArrays(1, &_id);

      return true;
  }

  bool VAO::init(VBO& vbo)
  { return init() && set_vbo(vbo); }

  bool VAO::init(VBO& vbo, IBO& ibo)
  { return init() && set_vbo_and_ibo(vbo, ibo); }
  /// @}

  /// @{ -------------------------------------------------- ENABLE/DISABLE ATTRIB POINTERS
  void VAO::_enable_attribute_pointers()
  {
      GLint offset_accum = 0;

      for (GLuint i = 0; i < num_attributes(); ++i)
      {
          const VertexAttributePointer& attrib = _pdata->attributes[i];
          BK_QT_GL glVertexAttribPointer(i, attrib.numel(), gl_underlying_glenum(attrib.value_type()), attrib.is_normalized(), _pdata->stride_in_bytes, reinterpret_cast<GLvoid*>(offset_accum));
          BK_QT_GL glEnableVertexAttribArray(i);
          offset_accum += attrib.size_in_bytes();
      }
  }

  void VAO::_disable_attribute_pointers()
  {
      for (GLuint i = 0; i < num_attributes(); ++i)
      { BK_QT_GL glDisableVertexAttribArray(i); }
  }
  /// @}

  /// @{ -------------------------------------------------- SET VBO/IBO
  bool VAO::set_vbo(VBO& vbo)
  {
      bind();
      vbo.bind();
      _enable_attribute_pointers();
      release();
      vbo.release();
      _disable_attribute_pointers();

      return true;
  }

  bool VAO::set_vbo_and_ibo(VBO& vbo, IBO& ibo)
  {
      bind();
      vbo.bind();
      ibo.bind();
      _enable_attribute_pointers();
      release();
      vbo.release();
      ibo.release();
      _disable_attribute_pointers();

      return true;
  }
  /// @}

  /// @{ -------------------------------------------------- CLEAR ATTRIBUTES
  void VAO::clear_attributes()
  { _pdata->attributes.clear(); }
  /// @}

  /// @{ -------------------------------------------------- VIRTUAL
  void VAO::clear_impl()
  { BK_QT_GL glDeleteVertexArrays(1, &_id); }

  void VAO::bind_impl()
  { BK_QT_GL glBindVertexArray(_id); }

  void VAO::release_impl()
  { BK_QT_GL glBindVertexArray(0); }
  /// @}
} // namespace bk