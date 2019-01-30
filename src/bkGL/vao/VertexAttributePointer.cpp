/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Benjamin Köhler
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

#include <algorithm>
#include <string>

#include <bkGL/vao/VertexAttributePointer.h>
#include <bkGL/gl_type_traits.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class VertexAttributePointer::Impl
  {
    public:
      GLuint id;
      GLenum value_type;
      GLboolean normalized; // GL_FALSE / GL_TRUE
      std::string name;

      Impl(GLuint id, GLenum value_type, bool normalized, std::string name)
          : id(id),
            value_type(value_type),
            normalized(normalized ? GL_TRUE : GL_FALSE),
            name(name)
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  VertexAttributePointer::VertexAttributePointer()
      : VertexAttributePointer(0, GL_FLOAT, false, "var")
  { /* do nothing */ }

  VertexAttributePointer::VertexAttributePointer(GLuint id, GLenum value_type, bool normalized, std::string name)
      : _pdata(id, value_type, normalized, name)
  { /* do nothing */ }

  VertexAttributePointer::VertexAttributePointer(const self_type&) = default;

  VertexAttributePointer::VertexAttributePointer(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  VertexAttributePointer::~VertexAttributePointer() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET ID
  GLuint VertexAttributePointer::id() const
  { return _pdata->id; }
  /// @}

  /// @{ -------------------------------------------------- GET NUMEL
  GLint VertexAttributePointer::numel() const
  { return gl_numel(_pdata->value_type); }
  /// @}

  /// @{ -------------------------------------------------- GET VALUE_TYPE
  GLenum VertexAttributePointer::value_type() const
  { return _pdata->value_type; }
  /// @}

  /// @{ -------------------------------------------------- GET SIZE IN BYTES
  GLsizei VertexAttributePointer::size_in_bytes() const
  { return gl_size_in_bytes(_pdata->value_type); }
  /// @}

  /// @{ -------------------------------------------------- GET NORMALIZED
  GLboolean VertexAttributePointer::is_normalized() const
  { return _pdata->normalized; }
  /// @}

  /// @{ -------------------------------------------------- GET NAME
  const std::string& VertexAttributePointer::name() const
  { return _pdata->name; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto VertexAttributePointer::operator=(const self_type& other) -> self_type& = default;
  auto VertexAttributePointer::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET VALUE TYPE
  void VertexAttributePointer::set_value_type(GLenum type)
  { _pdata->value_type = type; }

  void VertexAttributePointer::set_value_type_BYTE()
  { set_value_type(GL_BYTE); }

  void VertexAttributePointer::set_value_type_UNSIGNED_BYTE()
  { set_value_type(GL_UNSIGNED_BYTE); }

  void VertexAttributePointer::set_value_type_SHORT()
  { set_value_type(GL_SHORT); }

  void VertexAttributePointer::set_value_type_UNSIGNED_SHORT()
  { set_value_type(GL_UNSIGNED_SHORT); }

  void VertexAttributePointer::set_value_type_INT_VEC2()
  { set_value_type(GL_INT_VEC2); }

  void VertexAttributePointer::set_value_type_INT_VEC3()
  { set_value_type(GL_INT_VEC3); }

  void VertexAttributePointer::set_value_type_INT_VEC4()
  { set_value_type(GL_INT_VEC4); }

  void VertexAttributePointer::set_value_type_INT()
  { set_value_type(GL_INT); }

  void VertexAttributePointer::set_value_type_UNSIGNED_INT_VEC2()
  { set_value_type(GL_UNSIGNED_INT_VEC2); }

  void VertexAttributePointer::set_value_type_UNSIGNED_INT_VEC3()
  { set_value_type(GL_UNSIGNED_INT_VEC3); }

  void VertexAttributePointer::set_value_type_UNSIGNED_INT_VEC4()
  { set_value_type(GL_UNSIGNED_INT_VEC4); }

  void VertexAttributePointer::set_value_type_UNSIGNED_INT()
  { set_value_type(GL_UNSIGNED_INT); }

  void VertexAttributePointer::set_value_type_FLOAT_VEC2()
  { set_value_type(GL_FLOAT_VEC2); }

  void VertexAttributePointer::set_value_type_FLOAT_VEC3()
  { set_value_type(GL_FLOAT_VEC3); }

  void VertexAttributePointer::set_value_type_FLOAT_VEC4()
  { set_value_type(GL_FLOAT_VEC4); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT2()
  { set_value_type(GL_FLOAT_MAT2); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT2x3()
  { set_value_type(GL_FLOAT_MAT2x3); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT2x4()
  { set_value_type(GL_FLOAT_MAT2x4); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT3()
  { set_value_type(GL_FLOAT_MAT3); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT3x2()
  { set_value_type(GL_FLOAT_MAT3x2); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT3x4()
  { set_value_type(GL_FLOAT_MAT3x4); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT4()
  { set_value_type(GL_FLOAT_MAT4); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT4x2()
  { set_value_type(GL_FLOAT_MAT4x2); }

  void VertexAttributePointer::set_value_type_FLOAT_MAT4x3()
  { set_value_type(GL_FLOAT_MAT4x3); }

  void VertexAttributePointer::set_value_type_FLOAT()
  { set_value_type(GL_FLOAT); }

  void VertexAttributePointer::set_value_type_DOUBLE_VEC2()
  { set_value_type(GL_DOUBLE_VEC2); }

  void VertexAttributePointer::set_value_type_DOUBLE_VEC3()
  { set_value_type(GL_DOUBLE_VEC3); }

  void VertexAttributePointer::set_value_type_DOUBLE_VEC4()
  { set_value_type(GL_DOUBLE_VEC4); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT2()
  { set_value_type(GL_DOUBLE_MAT2); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT2x3()
  { set_value_type(GL_DOUBLE_MAT2x3); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT2x4()
  { set_value_type(GL_DOUBLE_MAT2x4); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT3()
  { set_value_type(GL_DOUBLE_MAT3); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT3x2()
  { set_value_type(GL_DOUBLE_MAT3x2); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT3x4()
  { set_value_type(GL_DOUBLE_MAT3x4); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT4()
  { set_value_type(GL_DOUBLE_MAT4); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT4x2()
  { set_value_type(GL_DOUBLE_MAT4x2); }

  void VertexAttributePointer::set_value_type_DOUBLE_MAT4x3()
  { set_value_type(GL_DOUBLE_MAT4x3); }

  void VertexAttributePointer::set_value_type_DOUBLE()
  { set_value_type(GL_DOUBLE); }
  /// @}

  /// @{ -------------------------------------------------- SET NORMALIZED
  void VertexAttributePointer::set_normalized(bool b)
  { _pdata->normalized = b ? GL_TRUE : GL_FALSE; }
  /// @}

  /// @{ -------------------------------------------------- SET NAME
  void VertexAttributePointer::set_name(std::string name)
  { _pdata->name = name; }
  /// @}
} // namespace bk


