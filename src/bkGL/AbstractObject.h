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

#ifndef BKGL_ABSTRACTOBJECT_H
#define BKGL_ABSTRACTOBJECT_H

#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk::details
{
  class BKGL_EXPORT AbstractObject
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AbstractObject;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      #ifdef BK_LIB_QT_AVAILABLE
      qt_gl_functions* _gl;
      #endif

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      #ifndef BK_LIB_QT_AVAILABLE
      AbstractObject();
      #else
      explicit AbstractObject(qt_gl_functions* gl);
      #endif
      AbstractObject(const self_type& other) = delete;
      AbstractObject(self_type&& other) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      virtual ~AbstractObject();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS INITIALIZED
      [[nodiscard]] virtual bool is_initialized() const = 0;

      // Making a virtual "init()" is not feasible since the derived classes may use different
      // function signatures, i.e., parameters are passed to init(...) that are required for
      // the setup.
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type& = delete;
      [[maybe_unused]] auto operator=(self_type&& other) noexcept -> self_type&;
      /// @}
  }; // class AbstractObject
} // namespace bk::details

#endif //BKGL_ABSTRACTOBJECT_H


