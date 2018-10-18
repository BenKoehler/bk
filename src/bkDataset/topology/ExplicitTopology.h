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

#ifndef BK_EXPLICITTOPOLOGY_H
#define BK_EXPLICITTOPOLOGY_H

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <map>
#include <string_view>
#include <utility>
#include <vector>

#include <bkDataset/topology/Cell.h>
#include <bk/StringUtils>

namespace bk
{
  template<int TCellSize = -1> class ExplicitTopology
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = ExplicitTopology<TCellSize>;
    public:
      using cell_type = Cell<TCellSize>;

      /// @{ -------------------------------------------------- IS EXPLICIT TOPOLOGY
      [[nodiscard]] static constexpr bool IsExplicit() noexcept
      { return true; }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      std::vector<cell_type> _cells;
      std::map<unsigned int, std::vector<unsigned int>> _neighbors_of_point;
      std::map<unsigned int, std::vector<unsigned int>> _cells_of_point;
      bool _up2date;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      ExplicitTopology()
          : _up2date(false)
      { /* do nothing */ }

      ExplicitTopology(const self_type&) = default;
      ExplicitTopology(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~ExplicitTopology() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM CELLS
      [[nodiscard]] unsigned int num_cells() const
      { return _cells.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET CELL
      [[nodiscard]] cell_type& cell(unsigned int id)
      { return _cells[id]; }

      [[nodiscard]] const cell_type& cell(unsigned int id) const
      { return _cells[id]; }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] typename std::vector<cell_type>::iterator begin()
      { return _cells.begin(); }

      [[nodiscard]] typename std::vector<cell_type>::const_iterator begin() const
      { return _cells.begin(); }

      [[nodiscard]] typename std::vector<cell_type>::iterator end()
      { return _cells.end(); }

      [[nodiscard]] typename std::vector<cell_type>::const_iterator end() const
      { return _cells.end(); }

      [[nodiscard]] typename std::vector<cell_type>::reverse_iterator rbegin()
      { return _cells.rbegin(); }

      [[nodiscard]] typename std::vector<cell_type>::const_reverse_iterator rbegin() const
      { return _cells.rbegin(); }

      [[nodiscard]] typename std::vector<cell_type>::reverse_iterator rend()
      { return _cells.rend(); }

      [[nodiscard]] typename std::vector<cell_type>::const_reverse_iterator rend() const
      { return _cells.rend(); }
      /// @}

      //====================================================================================================
      //===== ADD / REMOVE
      //====================================================================================================
      /// @{ -------------------------------------------------- RESERVE NUM CELLS
      void reserve_num_cells(unsigned int N)
      { _cells.reserve(N); }
      /// @}

      /// @{ -------------------------------------------------- PUSH BACK
      [[maybe_unused]] cell_type& push_back(const cell_type& c)
      {
          _cells.push_back(c);
          _up2date = false;
          return _cells.back();
      }

      [[maybe_unused]] cell_type& push_back(cell_type&& c)
      {
          _cells.push_back(std::move(c));
          _up2date = false;
          return _cells.back();
      }
      /// @}

      /// @{ -------------------------------------------------- EMPLACE BACK
      template<typename... TArgs>
      [[maybe_unused]] cell_type& emplace_back(TArgs&& ... args)
      { return _cells.emplace_back(std::forward<TArgs>(args)...); }
      /// @}

      /// @{ -------------------------------------------------- REMOVE
      [[maybe_unused]] bool remove(unsigned int i)
      {
          if (i < num_cells())
          {
              _cells.erase(begin() + i);
              _up2date = false;
              return true;
          }

          return false;
      }

      [[maybe_unused]] bool remove(const cell_type& c)
      {
          if (auto it = std::find(begin(), end(), c); it != end())
          {
              _cells.erase(it);
              _up2date = false;
              return true;
          }

          return false;
      }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET NUM CELLS
      void set_num_cells(unsigned int N)
      { _cells.resize(N); }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- INITIALIZATION
      void init()
      {
          if (_up2date)
          { return; }

          init_neighbors_of_points();
          init_cells_of_points();

          _up2date = true;
      }

      void clear()
      {
          _cells.clear();
          _up2date = false;
      }
      /// @}

      /// @{ -------------------------------------------------- INVERT CELL IDS
      void invert_cell_ordering()
      {
          #pragma omp parallel for
          for (unsigned int cellId = 0; cellId < num_cells(); ++cellId)
          { _cells[cellId].invert_order(); }
      }
      /// @}

      //====================================================================================================
      //===== CONNECTIVITY
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS: UPDATE NEIGHBORS OF POINTS
    private:
      void add_point_neighbor_ids_of_all_cells()
      {
          for (unsigned int cellId = 0; cellId < num_cells(); ++cellId)
          {
              const cell_type& c = cell(cellId);
              unsigned int numPointIdsInCell = c.size();

              for (unsigned int pointId = 0; pointId < numPointIdsInCell; ++pointId)
              {
                  auto it = _neighbors_of_point.insert(std::make_pair(c[pointId], std::vector<unsigned int>())).first;

                  for (unsigned int otherPointId = 0; otherPointId < numPointIdsInCell; ++otherPointId)
                  {
                      if (pointId == otherPointId)
                      { continue; }

                      it->second.push_back(c[otherPointId]);
                  }
              }
          }
      }

      void remove_duplicate_neighbor_ids()
      {
          for (auto& pair: _neighbors_of_point)
          { pair.second.erase(std::unique(pair.second.begin(), pair.second.end()), pair.second.end()); }
      }

      void sort_neighbor_ids_by_index()
      {
          for (auto& pair: _neighbors_of_point)
          { std::sort(pair.second.begin(), pair.second.end()); }
      }

      void init_neighbors_of_points()
      {
          _neighbors_of_point.clear();
          add_point_neighbor_ids_of_all_cells();
          remove_duplicate_neighbor_ids();
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- NEIGHBORS OF POINT
      [[nodiscard]] const std::vector<unsigned int>& neighbors_of_point(unsigned int pointId) const
      {
          assert(_up2date && "call init() first");

          auto it = _neighbors_of_point.find(pointId);

          assert(it != _neighbors_of_point.end() && "invalid pointId");

          return it->second;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: UPDATE CELLS OF POINT
    private:
      void add_cell_ids_of_all_points()
      {
          for (unsigned int cellId = 0; cellId < num_cells(); ++cellId)
          {
              const cell_type& c = cell(cellId);
              unsigned int numPointIdsInCell = c.size();

              for (unsigned int pointId = 0; pointId < numPointIdsInCell; ++pointId)
              {
                  auto it = _cells_of_point.insert(std::make_pair(c[pointId], std::vector<unsigned int>())).first;
                  it->second.push_back(cellId);
              }
          }
      }

      void init_cells_of_points()
      {
          _cells_of_point.clear();
          add_cell_ids_of_all_points();

          for (auto& pair: _cells_of_point)
          { std::sort(pair.second.begin(), pair.second.end()); }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- CELLS OF POINT
      [[nodiscard]] const std::vector<unsigned int>& cells_of_point(unsigned int pointId) const
      {
          assert(_up2date && "call init() first");

          auto it = _cells_of_point.find(pointId);

          assert(it != _cells_of_point.end() && "invalid pointId");

          return it->second;
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
          const std::string suffix = ".etopo";

          if (fname.empty())
          { fname = "explicit-topology" + suffix; }
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

          if (success)
          { file.close(); }

          return success;
      }

      [[maybe_unused]] bool save(std::ofstream& file) const
      {
          if (!file.is_open() || !file.good())
          { return false; }

          std::uint32_t numCells = num_cells();
          file.write(reinterpret_cast<char*>(&numCells), sizeof(std::uint32_t));

          if (numCells == 0)
          { return true; }

          bool allCellsHaveSameNumberOfIds = true;
          std::uint32_t referenceNum = static_cast<std::uint32_t>(_cells[0].size());
          std::uint32_t cellIdsTotal = 0;

          for (const cell_type& c: _cells)
          {
              if (static_cast<std::uint32_t>(c.size()) != referenceNum)
              { allCellsHaveSameNumberOfIds = false; }

              cellIdsTotal += c.size();
          }

          std::uint8_t allCellsHaveSameNumberOfIdsui8 = allCellsHaveSameNumberOfIds ? true : false;
          file.write(reinterpret_cast<char*>(&allCellsHaveSameNumberOfIdsui8), sizeof(std::uint8_t));

          if (allCellsHaveSameNumberOfIds)
          { file.write(reinterpret_cast<char*>(&referenceNum), sizeof(std::uint32_t)); }
          else
          { file.write(reinterpret_cast<char*>(&cellIdsTotal), sizeof(std::uint32_t)); }

          for (unsigned int cellId = 0; cellId < numCells; ++cellId)
          {
              const cell_type& c = cell(cellId);

              std::uint32_t numIds = c.size();

              if (!allCellsHaveSameNumberOfIds)
              { file.write(reinterpret_cast<char*>(&numIds), sizeof(std::uint32_t)); }

              for (std::uint32_t k = 0; k < numIds; ++k)
              {
                  std::uint32_t uitemp = static_cast<std::uint32_t>(c[k]);
                  file.write(reinterpret_cast<char*>(&uitemp), sizeof(std::uint32_t));
              }
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- LOAD
      [[maybe_unused]] bool load(std::string_view filename)
      {
          /*
           * check file ending
           */
          if (!string_utils::ends_with(filename.data(), ".etopo"))
          { return false; }

          /*
           * open file
           */
          std::ifstream file(filename.data(), std::ios_base::in | std::ios_base::binary);

          /*
           * load
           */
          const bool success = load(file);

          if (success)
          { file.close(); }

          return success;
      }

      [[maybe_unused]] bool load(std::ifstream& file)
      {
          clear();

          if (!file.is_open() || !file.good())
          { return false; }

          std::uint32_t numCells;
          file.read(reinterpret_cast<char*>(&numCells), sizeof(std::uint32_t));
          set_num_cells(numCells);

          std::uint8_t allCellsHaveSameNumberOfIdsui8;
          file.read(reinterpret_cast<char*>(&allCellsHaveSameNumberOfIdsui8), sizeof(std::uint8_t));
          const bool allCellsHaveSameNumberOfIds = allCellsHaveSameNumberOfIdsui8 == 1;

          std::uint32_t numIdsPerCellOrNumIdsTotal;
          file.read(reinterpret_cast<char*>(&numIdsPerCellOrNumIdsTotal), sizeof(std::uint32_t));
          assert((TCellSize <= 0 || (TCellSize > 0 && TCellSize == static_cast<int>(numIdsPerCellOrNumIdsTotal))) && "ExplicitTopology is loaded from an object (this) with wrong TCellSize template argument");

          std::vector<std::uint32_t> cellBuffer;

          if (allCellsHaveSameNumberOfIds)
          {
              cellBuffer.resize(numIdsPerCellOrNumIdsTotal * numCells);
              file.read(reinterpret_cast<char*>(cellBuffer.data()), cellBuffer.size() * sizeof(std::uint32_t));

              #pragma omp parallel for
              for (unsigned int cellId = 0; cellId < numCells; ++cellId)
              {
                  cell_type& c = cell(cellId);

                  for (std::uint32_t k = 0; k < numIdsPerCellOrNumIdsTotal; ++k)
                  { c[k] = cellBuffer[numIdsPerCellOrNumIdsTotal * cellId + k]; }
              }
          }
          else
          {
              cellBuffer.resize(numIdsPerCellOrNumIdsTotal + numCells);
              file.read(reinterpret_cast<char*>(cellBuffer.data()), cellBuffer.size() * sizeof(std::uint32_t));

              unsigned int bufCnt = 0;

              for (unsigned int cellId = 0; cellId < numCells; ++cellId)
              {
                  const std::uint32_t numIds = cellBuffer[bufCnt++];

                  cell_type& c = cell(cellId);
                  if constexpr (TCellSize <= 0)
                  { c.set_size(numIds); }

                  for (std::uint32_t k = 0; k < numIds; ++k)
                  { c[k] = cellBuffer[bufCnt++]; }
              }
          }

          return num_cells() != 0;
      }
      /// @}
  }; // class ExplicitTopology
} // namespace bk

#endif //BK_EXPLICITTOPOLOGY_H
