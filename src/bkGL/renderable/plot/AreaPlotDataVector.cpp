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

#include <bkGL/renderable/plot/AreaPlotDataVector.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct AreaPlotDataVector::Impl
  {
      std::vector<GLfloat> xvalues;
      std::vector<GLfloat> y0values;
      std::vector<GLfloat> y1values;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  AreaPlotDataVector::AreaPlotDataVector() = default;
  AreaPlotDataVector::AreaPlotDataVector(const AreaPlotDataVector&) = default;
  AreaPlotDataVector::AreaPlotDataVector(AreaPlotDataVector&&) noexcept = default;
  AreaPlotDataVector::~AreaPlotDataVector() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  bool AreaPlotDataVector::_is_valid_id(unsigned int i) const
  { return i < _pdata->xvalues.size() && i < _pdata->y0values.size() && i < _pdata->y1values.size(); }

  unsigned int AreaPlotDataVector::num_values() const
  { return _pdata->xvalues.size(); }

  const GLfloat* AreaPlotDataVector::x_value(unsigned int i) const
  { return _is_valid_id(i) ? &_pdata->xvalues[i] : nullptr; }

  const GLfloat* AreaPlotDataVector::y0_value(unsigned int i) const
  { return _is_valid_id(i) ? &_pdata->y0values[i] : nullptr; }

  const GLfloat* AreaPlotDataVector::y1_value(unsigned int i) const
  { return _is_valid_id(i) ? &_pdata->y1values[i] : nullptr; }

  const std::vector<GLfloat>& AreaPlotDataVector::x_value_vector() const
  { return _pdata->xvalues; }

  const std::vector<GLfloat>& AreaPlotDataVector::y0_value_vector() const
  { return _pdata->y0values; }

  const std::vector<GLfloat>& AreaPlotDataVector::y1_value_vector() const
  { return _pdata->y1values; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  AreaPlotDataVector& AreaPlotDataVector::operator=(const AreaPlotDataVector&) = default;
  AreaPlotDataVector& AreaPlotDataVector::operator=(AreaPlotDataVector&&) noexcept = default;

  void AreaPlotDataVector::set_num_values(unsigned int N)
  {
      _pdata->xvalues.resize(N);
      _pdata->y0values.resize(N);
      _pdata->y1values.resize(N);
  }

  bool AreaPlotDataVector::set_x_value(unsigned int i, GLfloat x)
  {
      if (_is_valid_id(i))
      {
          _pdata->xvalues[i] = x;
          return true;
      }

      return false;
  }

  bool AreaPlotDataVector::set_y0_value(unsigned int i, GLfloat y0)
  {
      if (_is_valid_id(i))
      {
          _pdata->y0values[i] = y0;
          return true;
      }

      return false;
  }

  bool AreaPlotDataVector::set_y1_value(unsigned int i, GLfloat y1)
  {
      if (_is_valid_id(i))
      {
          _pdata->y1values[i] = y1;
          return true;
      }

      return false;
  }

  bool AreaPlotDataVector::set_value(unsigned int i, GLfloat x, GLfloat y0, GLfloat y1)
  {
      if (_is_valid_id(i))
      {
          _pdata->xvalues[i] = x;
          _pdata->y0values[i] = y0;
          _pdata->y1values[i] = y1;

          return true;
      }

      return false;
  }
} // namespace bk


