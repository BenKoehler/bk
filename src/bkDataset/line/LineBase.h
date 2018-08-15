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

#ifndef BK_LINEBASE_H
#define BK_LINEBASE_H

#include <bkDataset/dataobject/DataObject.h>
#include <bkDataset/geometry/ExplicitGeometry.h>
#include <bkDataset/topology/GridTopology.h>

namespace bk::details
{
  template<int TDims = -1> class LineBase : public DataObject<ExplicitGeometry<TDims>, GridTopology<1>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = LineBase<TDims>;
      using base_type = DataObject<ExplicitGeometry<TDims>, GridTopology<1>>;
    public:
      using point_type = typename ExplicitGeometry<TDims>::point_type;
      using cell_type = typename GridTopology<1>::cell_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      LineBase() = default;
      LineBase(const self_type&) = default;
      LineBase(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~LineBase() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      auto operator=(const self_type& other) -> self_type& = default;
      auto operator=(self_type&& other) noexcept -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INIT
      void init()
      { this->geometry().construct_kd_tree(); }
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      void clear()
      {
          this->geometry().clear();
          //this->topology().clear(); // not existing in GridTopology
          this->object_attributes().clear();
          this->cell_attributes().clear();
          this->point_attributes().clear();

          //_initialized = false;
      }
      /// @}

      /// @{ -------------------------------------------------- TANGENT AT POINT
      Vec<TDims, double> tangent_at_point(unsigned int pointId) const
      {
          const unsigned int nPoints = this->geometry().num_points();

          if (nPoints > 1)
          {
              if (pointId > 0 && pointId < nPoints - 1)
              { return (this->geometry().point(pointId + 1) - this->geometry().point(pointId - 1)) * 0.5; }
              else if (pointId == 0)
              { return this->geometry().point(1) - this->geometry().point(0); }
              else if (pointId == nPoints - 1)
              { return this->geometry().point(nPoints - 1) - this->geometry().point(nPoints - 2); }
          }

          return point_type();
      }
      /// @}

      /// @{ -------------------------------------------------- LENGTH
      double length() const
      {
          double len = 0.0;
          const unsigned int nPoints = this->geometry().num_points();

          if (nPoints >= 2)
          {
              for (unsigned int i = 0; i < nPoints - 1; ++i)
              { len += this->geometry().point(i).distance(this->geometry().point(i + 1)); }
          }

          return len;
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(std::string_view filename) const
      {
          //------------------------------------------------------------------------------------------------------
          // check filename
          //------------------------------------------------------------------------------------------------------
          std::string fname(filename);
          const std::string suffix = ".line";

          if (fname.empty())
          { fname = "line" + suffix; }
          else if (!string_utils::ends_with(fname, suffix))
          { fname.append(suffix); }

          //------------------------------------------------------------------------------------------------------
          std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);

          const bool success = save(file);

          file.close();

          return success;
      }

      [[maybe_unused]] bool save(std::ofstream& file) const
      {
          if (!file.is_open() || !file.good())
          { return false; }

          const bool success_geometry = this->geometry().save(file);
          const bool success_topology = this->topology().save(file);

          return success_geometry && success_topology;
      }
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filename)
      {
          /*
           * check filename
           */
          if (!string_utils::ends_with(filename.data(), ".line"))
          { return false; }

          /*
           * open file
           */
          std::ifstream file(filename.data(), std::ios_base::in | std::ios_base::binary);

          /*
           * load
           */
          const bool success = load(file);

          file.close();

          return success;
      }

      [[maybe_unused]] bool load(std::ifstream& file)
      {
          if (!file.is_open() || !file.good())
          { return false; }

          const bool success_geometry = this->geometry().load(file);
          const bool success_topology = this->topology().load(file);

          return success_geometry && success_topology;
      }
      /// @}
  }; // class LineBase
} // namespace bk::details

#endif //BK_LINEBASE_H


