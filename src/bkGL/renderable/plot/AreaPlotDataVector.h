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

#ifndef BK_AREAPLOTDATAVECTOR_H
#define BK_AREAPLOTDATAVECTOR_H

#include <vector>

#include <bk/CopyablePIMPL>
#include <bkGL/gl_definitions.h>
#include <bkGL/lib/bkGL_export.h>

namespace bk
{
  class BKGL_EXPORT AreaPlotDataVector
  {
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
      /// @{ -------------------------------------------------- CONSTRUCTORS
      AreaPlotDataVector();
      AreaPlotDataVector(const AreaPlotDataVector&);
      AreaPlotDataVector(AreaPlotDataVector&&) noexcept;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~AreaPlotDataVector();
      /// @}

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
    private:
      [[nodiscard]] bool _is_valid_id(unsigned int i) const;
    public:
      [[nodiscard]] unsigned int num_values() const;
      [[nodiscard]] const GLfloat* x_value(unsigned int i) const;
      [[nodiscard]] const GLfloat* y0_value(unsigned int i) const;
      [[nodiscard]] const GLfloat* y1_value(unsigned int i) const;
      [[nodiscard]] const std::vector<GLfloat>& x_value_vector() const;
      [[nodiscard]] const std::vector<GLfloat>& y0_value_vector() const;
      [[nodiscard]] const std::vector<GLfloat>& y1_value_vector() const;

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] AreaPlotDataVector& operator=(const AreaPlotDataVector&);
      [[maybe_unused]] AreaPlotDataVector& operator=(AreaPlotDataVector&&) noexcept;
      /// @}

      void set_num_values(unsigned int N);
      [[maybe_unused]] bool set_x_value(unsigned int i, GLfloat x);
      [[maybe_unused]] bool set_y0_value(unsigned int i, GLfloat y0);
      [[maybe_unused]] bool set_y1_value(unsigned int i, GLfloat y1);
      [[maybe_unused]] bool set_value(unsigned int i, GLfloat x, GLfloat y0, GLfloat y1);
  }; // class AreaPlotDataVector
} // namespace bk

#endif //BK_AREAPLOTDATAVECTOR_H
