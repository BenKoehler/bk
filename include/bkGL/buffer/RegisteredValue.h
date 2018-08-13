#pragma once

#ifndef BKGL_REGISTEREDVALUE_H
#define BKGL_REGISTEREDVALUE_H

#include <memory>
#include <string>
#include <string_view>

#include "../gl_definitions.h"
#include "../lib/export.h"

namespace bk
{
  class BK_GL_EXPORT RegisteredValue
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

      std::unique_ptr<Impl> _pdata;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      RegisteredValue();
      RegisteredValue(const self_type&);
      RegisteredValue(self_type&&);
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
      [[maybe_unused]] auto operator=(self_type&&) -> self_type&;
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
