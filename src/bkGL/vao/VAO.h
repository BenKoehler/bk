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

#pragma once

#ifndef BKGL_VAO_H
#define BKGL_VAO_H

#include <string>

#include <bk/CopyablePIMPL>
#include <bkGL/AbstractBindable.h>

namespace bk
{
  // ------ forward declaration -------
  class IBO;
  class VBO;
  class VertexAttributePointer;
  // ------ forward declaration -------

  class BKGL_EXPORT VAO : public details::AbstractBindable
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = VAO;
      using base_type = details::AbstractBindable;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      VAO();
      #else
      explicit VAO(qt_gl_functions* gl);
      #endif
      VAO(const self_type& other) = delete;
      VAO(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~VAO();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM ATTRIBUTES
      [[nodiscard]] GLuint num_attributes() const;
      /// @}

      /// @{ -------------------------------------------------- GET VERTEX ATTRIB POINTER
      [[nodiscard]] const VertexAttributePointer& attribute(unsigned int i) const;
      /// @}

      /// @{ -------------------------------------------------- GET STRIDE
      [[nodiscard]] GLsizei stride_in_bytes() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- ADD ATTRIBUTES
      void add_attribute(GLenum type, bool values_are_normalized, std::string name);
      void add_default_attribute_position_2xfloat();
      void add_default_attribute_position_3xfloat();
      void add_default_attribute_position_2xfloat_plus_time_1xfloat();
      void add_default_attribute_position_3xfloat_plus_time_1xfloat();
      void add_default_attribute_texture_coordinates_2xfloat();
      void add_default_attribute_texture_coordinates_3xfloat();
      void add_default_attribute_normal_3xfloat();
      void add_default_attribute_colorRGB_3xfloat();
      void add_default_attribute_colorRGBA_4xfloat();
      void add_default_attribute_scalar_1xfloat(std::string name);
      void add_default_attribute_index_1xuint();
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CALC STRIDE
    private:
      void _calc_stride();
    public:
      /// @}

      /// @{ -------------------------------------------------- INIT
      [[maybe_unused]] bool init();
      [[maybe_unused]] bool init(VBO& vbo);
      [[maybe_unused]] bool init(VBO& vbo, IBO& ibo);
      /// @}

      /// @{ -------------------------------------------------- ENABLE/DISABLE ATTRIB POINTERS
    private:
      void _enable_attribute_pointers();
      void _disable_attribute_pointers();
    public:
      /// @}

      /// @{ -------------------------------------------------- SET VBO/IBO
      [[maybe_unused]] bool set_vbo(VBO& vbo);
      [[maybe_unused]] bool set_vbo_and_ibo(VBO& vbo, IBO& ibo);
      /// @}

      /// @{ -------------------------------------------------- CLEAR ATTRIBUTES
      void clear_attributes();
      /// @}

      /// @{ -------------------------------------------------- VIRTUAL
    protected:
      void clear_impl() override;
      void bind_impl() override;
      void release_impl() override;
    public:
      /// @}
  }; // class VAO
} // namespace bk

#endif //BKGL_VAO_H


