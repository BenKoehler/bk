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

#ifndef BKGL_VERTEXATTRIBUTEPOINTER_H
#define BKGL_VERTEXATTRIBUTEPOINTER_H

#include <memory>

#include "../gl_definitions.h"

namespace bk
{
  class BKGL_EXPORT VertexAttributePointer
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = VertexAttributePointer;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      VertexAttributePointer();
      VertexAttributePointer(GLuint id, GLenum value_type, bool normalized, std::string name);
      VertexAttributePointer(const self_type& other);
      VertexAttributePointer(self_type&&);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~VertexAttributePointer();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET ID
      [[nodiscard]] GLuint id() const;
      /// @}

      /// @{ -------------------------------------------------- GET NUMEL
      [[nodiscard]] GLint numel() const;
      /// @}

      /// @{ -------------------------------------------------- GET VALUE_TYPE
      [[nodiscard]] GLenum value_type() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE IN BYTES
      [[nodiscard]] GLsizei size_in_bytes() const;
      /// @}

      /// @{ -------------------------------------------------- GET NORMALIZED
      [[nodiscard]] GLboolean is_normalized() const;
      /// @}

      /// @{ -------------------------------------------------- GET NAME
      [[nodiscard]] const std::string& name() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET VALUE TYPE
      void set_value_type(GLenum type);

      void set_value_type_BYTE();
      void set_value_type_UNSIGNED_BYTE();
      void set_value_type_SHORT();
      void set_value_type_UNSIGNED_SHORT();
      void set_value_type_INT_VEC2();
      void set_value_type_INT_VEC3();
      void set_value_type_INT_VEC4();
      void set_value_type_INT();
      void set_value_type_UNSIGNED_INT_VEC2();
      void set_value_type_UNSIGNED_INT_VEC3();
      void set_value_type_UNSIGNED_INT_VEC4();
      void set_value_type_UNSIGNED_INT();
      void set_value_type_FLOAT_VEC2();
      void set_value_type_FLOAT_VEC3();
      void set_value_type_FLOAT_VEC4();
      void set_value_type_FLOAT_MAT2();
      void set_value_type_FLOAT_MAT2x3();
      void set_value_type_FLOAT_MAT2x4();
      void set_value_type_FLOAT_MAT3();
      void set_value_type_FLOAT_MAT3x2();
      void set_value_type_FLOAT_MAT3x4();
      void set_value_type_FLOAT_MAT4();
      void set_value_type_FLOAT_MAT4x2();
      void set_value_type_FLOAT_MAT4x3();
      void set_value_type_FLOAT();
      void set_value_type_DOUBLE_VEC2();
      void set_value_type_DOUBLE_VEC3();
      void set_value_type_DOUBLE_VEC4();
      void set_value_type_DOUBLE_MAT2();
      void set_value_type_DOUBLE_MAT2x3();
      void set_value_type_DOUBLE_MAT2x4();
      void set_value_type_DOUBLE_MAT3();
      void set_value_type_DOUBLE_MAT3x2();
      void set_value_type_DOUBLE_MAT3x4();
      void set_value_type_DOUBLE_MAT4();
      void set_value_type_DOUBLE_MAT4x2();
      void set_value_type_DOUBLE_MAT4x3();
      void set_value_type_DOUBLE();
      /// @}

      /// @{ -------------------------------------------------- SET NORMALIZED
      void set_normalized(bool b);
      /// @}

      /// @{ -------------------------------------------------- SET NAME
      void set_name(std::string name);
      /// @}
  }; // class VertexAttributePointer
} // namespace bk

#endif //BKGL_VERTEXATTRIBUTEPOINTER_H