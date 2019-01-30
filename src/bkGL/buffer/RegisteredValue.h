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

#ifndef BKGL_REGISTEREDVALUE_H
#define BKGL_REGISTEREDVALUE_H

#include <string>
#include <string_view>

#include <bk/CopyablePIMPL>
#include <bkGL//gl_definitions.h>
#include <bkGL//lib/bkGL_export.h>

namespace bk
{
  class BKGL_EXPORT RegisteredValue
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = RegisteredValue;
    public:

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
      RegisteredValue();
      RegisteredValue(const self_type&);
      RegisteredValue(self_type&&) noexcept;
      RegisteredValue(std::string_view name, GLenum type, unsigned int offset_in_bytes);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~RegisteredValue();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NAME
      [[nodiscard]] const std::string& name() const;
      /// @}

      /// @{ -------------------------------------------------- GET HASH
      [[nodiscard]] unsigned long long hash() const;
      /// @}

      /// @{ -------------------------------------------------- GET SIZE IN BYTES
      [[nodiscard]] unsigned int size_in_bytes() const;
      /// @}

      /// @{ -------------------------------------------------- GET OFFSET IN BYTES
      [[nodiscard]] unsigned int offset_in_bytes() const;
      /// @}

      /// @{ -------------------------------------------------- GET TYPE
      [[nodiscard]] GLenum type() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET NAME
      void set_name(std::string_view name);
      /// @}

      /// @{ -------------------------------------------------- SET OFFSET IN BYTES
      void set_offset_in_bytes(unsigned int off);
      /// @}

      /// @{ -------------------------------------------------- SET TYPE
      void set_type(GLenum t);
      /// @}
  }; // class RegisteredValue
} // namespace bk

#endif //BKGL_REGISTEREDVALUE_H
