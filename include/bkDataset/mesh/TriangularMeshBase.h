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

#ifndef BK_TRIANGULARMESHBASE_H
#define BK_TRIANGULARMESHBASE_H

#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "../../bkTools/StringUtils"

#include "../dataobject/DataObject.h"
#include "../geometry/ExplicitGeometry.h"
#include "../topology/ExplicitTopology.h"

namespace bk::details
{
  template<int TDims = -1> class TriangularMeshBase : public DataObject<ExplicitGeometry<TDims>, ExplicitTopology<3>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = TriangularMeshBase<TDims>;
      using base_type = DataObject<ExplicitGeometry<TDims>, ExplicitTopology<3>>;
    public:
      using point_type = typename ExplicitGeometry<TDims>::point_type;
      using cell_type = typename ExplicitTopology<3>::cell_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
      /// @{ -------------------------------------------------- CTOR
      TriangularMeshBase() = default;
      TriangularMeshBase(const self_type&) = default;
      TriangularMeshBase(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~TriangularMeshBase() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type&) -> self_type& = default;
      [[maybe_unused]] auto operator=(self_type&&) -> self_type& = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      void init()
      {
          this->geometry().construct_kd_tree();
          this->topology().init();
      }
      /// @}

      /// @{ -------------------------------------------------- CLEAR
      void clear()
      {
          this->geometry().clear();
          this->topology().clear();
          this->object_attribute_map().clear();
          this->cell_attribute_map().clear();
          this->point_attribute_map().clear();
      }
      /// @}

      //====================================================================================================
      //===== I/O
      //====================================================================================================
      /// @{ -------------------------------------------------- SAVE
      [[maybe_unused]] bool save(std::string_view filename) const
      {
          /*
           * check filename
           */
          std::string fname(filename);
          const std::string suffix = ".mesh";

          if (fname.empty())
          { fname = "trimesh" + suffix; }
          else if (!string_utils::ends_with(fname, suffix))
          { fname.append(suffix); }

          /*
           * create file
           */
          std::ofstream file(fname, std::ios_base::out | std::ios_base::binary);

          /*
           * save
           */
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
          if (!bk::string_utils::ends_with(filename.data(), ".mesh"))
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
  }; // class TriangularMeshBase
} // namespace bk::details

#endif //BK_TRIANGULARMESHBASE_H


