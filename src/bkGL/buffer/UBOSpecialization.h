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

#ifndef BKGL_UBOSPECIALIZATION_H
#define BKGL_UBOSPECIALIZATION_H

#include <string>
#include <string_view>

#include <bk/CopyablePIMPL>
#include <bkGL/buffer/UBO.h>
#include <bkGL/gl_definitions.h>

namespace bk::details
{
  class UBOSpecialization : public UBO
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = UBOSpecialization;
      using base_type = UBO;
    public:

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      class Impl;
      bk::cpimpl<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      UBOSpecialization();
      UBOSpecialization(std::string_view bufferName, unsigned int bufferBaseId);
      #else
      UBOSpecialization(qt_gl_functions* gl);
      UBOSpecialization(qt_gl_functions* gl, std::string_view bufferName, unsigned int bufferBaseId);
      #endif
      UBOSpecialization(const self_type&) = delete;
      UBOSpecialization(self_type&&) noexcept;

      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~UBOSpecialization();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET BUFFER NAME
      [[nodiscard]] std::string buffer_name() const;
      /// @}

      /// @{ -------------------------------------------------- GET BUFFER BASE
      [[nodiscard]] unsigned int buffer_base() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&&) noexcept -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET BUFFER NAME
      void set_buffer_name(std::string_view bufferName);
      /// @}

      /// @{ -------------------------------------------------- SET BUFFER BASE
      void set_buffer_base(unsigned int bufferBaseId);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- BIND TO DEFAULT BUFFER BASE
      [[maybe_unused]] bool bind_to_default_base();
      /// @}

  }; // class UBOSpecialization::details
} // namespace bk

#endif //BKGL_UBOSPECIALIZATION_H
