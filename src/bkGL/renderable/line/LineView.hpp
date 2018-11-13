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

#ifndef BKGL_LINEVIEW_HPP
#define BKGL_LINEVIEW_HPP

#include <bkGL/renderable/line/LineView.h>
#include <bkAlgorithm/quantile.h>

namespace bk
{
  /// @{ -------------------------------------------------- SET COLOR ATTRIBUTES
  template<typename TLinesIterator>
  void LineView::set_color_attribute(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name, double quantile_low, double quantile_high, std::string_view custom_colorbar_title, const ScalarLineFilter* filter)
  {
      if (!this->is_initialized())
      { return; }

      //------------------------------------------------------------------------------------------------------
      // colors were not yet allocated in the vbo
      //------------------------------------------------------------------------------------------------------
      if (!color_by_attribute_is_enabled())
      {
          init(linesBegin, linesEnd, color_attribute_name);
          return;
      }

      //------------------------------------------------------------------------------------------------------
      // invalid line set or non-existing attribute?
      //------------------------------------------------------------------------------------------------------
      const unsigned int num_lines = std::distance(linesBegin, linesEnd);
      if (num_lines == 0)
      { return; }

      std::vector<unsigned int> cumulative_num_points(num_lines, 0);

      TLinesIterator itLine = linesBegin;

      for (unsigned int i = 1; i < num_lines; ++i, ++itLine)
      { cumulative_num_points[i] = cumulative_num_points[i - 1] + itLine->geometry().num_points(); }

      const unsigned int num_points_total = cumulative_num_points.back() + (linesEnd - 1)->geometry().num_points();

      if (num_points_total == 0)
      { return; }

      _init_set_color_attribute(color_attribute_name);

      /*
       * check lines' time attribute
       */
      for (itLine = linesBegin; itLine != linesEnd; ++itLine)
      {
          if (!_line_has_time_attribute(*itLine))
          { break; }
      }

      /*
       * check lines' color attribute
       */
      for (itLine = linesBegin; itLine != linesEnd; ++itLine)
      {
          if (!_line_has_attribute(*itLine, color_attribute_name))
          { break; }
      }

      //if (!_lines_have_color_attribute())
      if (!this->is_initialized())
      {
          init(linesBegin, linesEnd, color_attribute_name, filter);
          return;
      }

      //------------------------------------------------------------------------------------------------------
      // colors values were allocated in the vbo; map and overwrite
      //------------------------------------------------------------------------------------------------------
      std::vector<GLfloat> attrib_values;
      if (_lines_have_color_attribute())
      { attrib_values.resize(num_points_total); }

      _reset_color_attribute_min_max();

      GLfloat* vbodata = _map_vbo();
      if (vbodata != nullptr)
      {
          const unsigned int floatsPerVertex = _floats_per_vertex();

          #pragma omp parallel for schedule(dynamic, 1)
          for (unsigned int i = 0; i < num_lines; ++i)
          {
              const bk::Line<3>& currentLine = *(linesBegin + i);
              const unsigned int num_points = currentLine.geometry().num_points();
              unsigned int cnt_v = cumulative_num_points[i] * floatsPerVertex;

              for (unsigned int k = 0; k < num_points; ++k)
              {
                  const bool filter_valid = filter != nullptr ? filter->eval(currentLine, k) : true;

                  GLfloat att = !filter_valid ? _invalid_attribute_value() : _lines_have_color_attribute() ? currentLine.point_attribute_value_of_type<double>(color_attribute_name, k) : 0;

                  if (_lines_have_color_attribute())
                  { attrib_values[cumulative_num_points[i] + k] = att; }

                  cnt_v += floatsPerVertex - 1;
                  vbodata[cnt_v++] = att;
              } // for num_points
          } // for num_lines

          _unmap_vbo();

          if (_lines_have_color_attribute())
          {
              std::sort(attrib_values.begin(), attrib_values.end());
              attrib_values.erase(std::remove_if(attrib_values.begin(), attrib_values.end(), [&](GLfloat x)
              { return x == _invalid_attribute_value(); }), attrib_values.end());

              if (quantile_low != 0 && quantile_high != 1)
              {
                  _update_attribute_min_max(bk::quantile(attrib_values.begin(), attrib_values.end(), quantile_low));
                  _update_attribute_min_max(bk::quantile(attrib_values.begin(), attrib_values.end(), quantile_high));
              }
              else
              {
                  _update_attribute_min_max(attrib_values.front());
                  _update_attribute_min_max(attrib_values.back());
              }

              _finalize_set_color_attribute();
              _init_colorbar(color_attribute_name, custom_colorbar_title);
          }

          init_shader();
          init_lineao_shader();

          this->emit_signal_update_required();
      }
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  template<typename TLinesIterator>
  void LineView::init_lines(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name, const ScalarLineFilter* filter)
  {
      //_init_lines(color_attribute_name);

      const unsigned int num_lines = std::distance(linesBegin, linesEnd);
      if (num_lines == 0)
      { return; }

      std::vector<unsigned int> cumulative_num_points(num_lines, 0);

      TLinesIterator itLine = linesBegin;

      for (unsigned int i = 1; i < num_lines; ++i, ++itLine)
      { cumulative_num_points[i] = cumulative_num_points[i - 1] + itLine->geometry().num_points(); }

      const unsigned int num_points_total = cumulative_num_points.back() + (linesEnd - 1)->geometry().num_points();

      if (num_points_total == 0)
      { return; }

      _init_lines(color_attribute_name);

      /*
       * check lines' time attribute
       */
      for (TLinesIterator itLine = linesBegin; itLine != linesEnd; ++itLine)
      {
          if (!_line_has_time_attribute(*itLine))
          { break; }
      }

      /*
       * check lines' color attribute
       */
      for (TLinesIterator itLine = linesBegin; itLine != linesEnd; ++itLine)
      {
          if (!_line_has_attribute(*itLine, color_attribute_name))
          { break; }
      }

      const unsigned int floatPerVertex = _floats_per_vertex();

      std::vector<GLfloat> vertices(floatPerVertex * num_points_total); // xyzt coord (+attrib)
      std::vector<GLuint> indices(num_points_total + (num_lines - 1) + 2 * num_lines); // points + primitive restart value + first/last point twice

      _vao_add_attributes();

      #pragma omp parallel for
      for (unsigned int i = 0; i < num_lines; ++i)
      {
          const bk::Line<3>& currentLine = *(linesBegin + i);
          const unsigned int num_points = currentLine.geometry().num_points();
          unsigned int cnt_v = cumulative_num_points[i] * floatPerVertex;
          unsigned int cnt_i = cumulative_num_points[i] + 3 * i;
          unsigned int vert_id = cumulative_num_points[i];

          for (unsigned int k = 0; k < num_points; ++k)
          {
              const bool filter_valid = filter != nullptr ? filter->eval(currentLine, k) : true;
              const auto& pt = currentLine.geometry().point(k);

              GLfloat att = _lines_have_color_attribute() ? currentLine.point_attribute_value_of_type<double>(color_attribute_name, k) : 0;
              if (!filter_valid)
              { att = _invalid_attribute_value(); }

              if (_lines_have_color_attribute())
              {
                  if (att != _invalid_attribute_value())
                  {
                      #pragma omp critical
                      { _update_attribute_min_max(att); }
                  }
              }

              if (k == 0 || k == num_points - 1)
              {
                  // insert first and last point twice for line adjacency
                  indices[cnt_i++] = vert_id;
              }

              indices[cnt_i++] = vert_id++;

              vertices[cnt_v++] = pt[0];
              vertices[cnt_v++] = pt[1];
              vertices[cnt_v++] = pt[2];

              if (_lines_have_time_attribute())
              { vertices[cnt_v++] = currentLine.point_attribute_value<bk::attribute_info::time()>(k); }

              #pragma omp critical
              { _add_to_center(pt[0], pt[1], pt[2]); }

              vertices[cnt_v++] = att;
          } // for num_points

          if (i < num_lines - 1)
          {
              // primitive restart value not necessary after last line
              indices[cnt_i++] = std::numeric_limits<GLuint>::max();
          }
      } // for num_lines

      _init_line_buffers(vertices, indices, num_points_total);
      _init_colorbar(color_attribute_name);

      this->emit_signal_scene_changed();
  }

  template<typename TLinesIterator>
  void LineView::init(TLinesIterator linesBegin, TLinesIterator linesEnd, std::string_view color_attribute_name, const ScalarLineFilter* filter)
  {
      init_lines(linesBegin, linesEnd, color_attribute_name, filter);
      init_shader();
      init_ubo();

      init_lineao(); // calls init_lineao_shader()

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }
  /// @}
} // namespace bk

#endif //BKGL_LINEVIEW_HPP
