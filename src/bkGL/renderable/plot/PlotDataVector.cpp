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

#include "PlotDataVector.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct PlotDataVector::Impl
  {
      std::vector<GLfloat> xvalues;
      std::vector<GLfloat> yvalues;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  PlotDataVector::PlotDataVector() = default;
  PlotDataVector::PlotDataVector(const PlotDataVector&) = default;
  PlotDataVector::PlotDataVector(PlotDataVector&&) noexcept = default;
  PlotDataVector::~PlotDataVector() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  bool PlotDataVector::_is_valid_id(unsigned int i) const
  { return i < _pdata->xvalues.size() && i < _pdata->yvalues.size(); }

  unsigned int PlotDataVector::num_values() const
  { return _pdata->xvalues.size(); }

  const GLfloat* PlotDataVector::x_value(unsigned int i) const
  { return _is_valid_id(i) ? &_pdata->xvalues[i] : nullptr; }

  const GLfloat* PlotDataVector::y_value(unsigned int i) const
  { return _is_valid_id(i) ? &_pdata->yvalues[i] : nullptr; }

  const std::vector<GLfloat>& PlotDataVector::x_value_vector() const
  { return _pdata->xvalues; }

  const std::vector<GLfloat>& PlotDataVector::y_value_vector() const
  { return _pdata->yvalues; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  PlotDataVector& PlotDataVector::operator=(const PlotDataVector&) = default;
  PlotDataVector& PlotDataVector::operator=(PlotDataVector&&) noexcept = default;

  void PlotDataVector::set_num_values(unsigned int N)
  {
      _pdata->xvalues.resize(N);
      _pdata->yvalues.resize(N);
  }

  bool PlotDataVector::set_x_value(unsigned int i, GLfloat x)
  {
      if (_is_valid_id(i))
      {
          _pdata->xvalues[i] = x;
          return true;
      }

      return false;
  }

  bool PlotDataVector::set_y_value(unsigned int i, GLfloat y)
  {
      if (_is_valid_id(i))
      {
          _pdata->yvalues[i] = y;
          return true;
      }

      return false;
  }

  bool PlotDataVector::set_value(unsigned int i, GLfloat x, GLfloat y)
  {
      if (_is_valid_id(i))
      {
          _pdata->xvalues[i] = x;
          _pdata->yvalues[i] = y;
          return true;
      }

      return false;
  }
} // namespace bk


