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

#ifndef BKTOOLS_GRAPHCUTPARALLELBLOCK_H
#define BKTOOLS_GRAPHCUTPARALLELBLOCK_H

#include <algorithm>
#include <array>
#include <queue>
#include <utility>
#include <vector>

#include <bkTools/graphcut/GraphCutBase_Run.h>
#include <bkTools/graphcut/GraphCutBase_MemberAccess.h>

namespace bk::gc_details
{
  template<int TDims> class GraphCutParallelBlock
      : protected GraphCutBase_Run<TDims, GraphCutParallelBlock<TDims>>,
        protected GraphCutBase_MemberAccess<TDims, GraphCutParallelBlock<TDims>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCutParallelBlock<TDims>;
      using base_type = GraphCutBase_Run<TDims, GraphCutParallelBlock<TDims>>;

      using gc = typename base_type::gc;
      using pt2Nd_type = typename base_type::pt2Nd_type;
      template<typename T> using vector_grid_type = typename base_type::template vector_grid_type<T>;

    public:
      using flag_type = typename base_type::flag_type;
      using id_type = typename base_type::id_type;

      friend base_type;
      friend GraphCutBase_MemberAccess<TDims, GraphCutParallelBlock<TDims>>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      id_type _size;
      vector_grid_type<pt2Nd_type>& _residual;
      vector_grid_type<int>& _distance_to_terminal;
      vector_grid_type<int>& _timestamp;
      vector_grid_type<flag_type>& _flags;
      id_type _blocks_size;
      vector_grid_type<bool> _blocks;
      inline static unsigned int _gid;
    public:
      bool _locked;
      unsigned int _id;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GraphCutParallelBlock() = delete;
      GraphCutParallelBlock(const self_type&) = delete;
      GraphCutParallelBlock(self_type&&) = default;

      GraphCutParallelBlock(const id_type& img_size, id_type size_from, int current_timestamp, vector_grid_type<pt2Nd_type>& residual, vector_grid_type<int>& distance_to_terminal, vector_grid_type<int>& timestamp, vector_grid_type<flag_type>& flags)
          : GraphCutParallelBlock(img_size, current_timestamp, residual, distance_to_terminal, timestamp, flags)
      {
          for (int dimId = 0; dimId < TDims; ++dimId)
          { size_from[dimId] /= gc::BLOCK_SIZE(dimId); }

          this->get_from_vector_grid(_blocks, size_from) = true;
      }

      GraphCutParallelBlock(const id_type& img_size, int current_timestamp, vector_grid_type<pt2Nd_type>& residual, vector_grid_type<int>& distance_to_terminal, vector_grid_type<int>& timestamp, vector_grid_type<flag_type>& flags)
          : base_type(current_timestamp),
            _size(img_size),
            _residual(residual),
            _distance_to_terminal(distance_to_terminal),
            _timestamp(timestamp),
            _flags(flags),
            _locked(false),
            _id(_gid++)
      {
          for (int dimId = 0; dimId < TDims; ++dimId)
          { _blocks_size[dimId] = _size[dimId] / gc::BLOCK_SIZE(dimId) + (_size[dimId] % gc::BLOCK_SIZE(dimId) ? 1 : 0); }

          gc_details::resize_wrapped_vector<TDims>(_blocks, _blocks_size, false);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCutParallelBlock() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- IS VALID
      [[nodiscard]] bool is_valid(const id_type& node) const
      {
          // out of bounds?
          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (node[dimId] < 0 || node[dimId] >= _size[dimId])
              { return false; }
          }

          // block flag
          id_type blockid;

          for (int dimId = 0; dimId < TDims; ++dimId)
          { blockid[dimId] = node[dimId] / gc::BLOCK_SIZE(dimId); }

          return this->get_from_vector_grid(_blocks, blockid);
      }

      [[nodiscard]] bool is_valid_extra(const id_type& node) const
      { return is_valid(node); }

      [[nodiscard]] static constexpr bool is_valid_lower_bound(int /*dimId*/, const id_type& /*node*/)
      { return true; }

      [[nodiscard]] static constexpr bool is_valid_upper_bound(int /*dimId*/, const id_type& /*node*/)
      { return true; }
      /// @}

      /// @{ -------------------------------------------------- TERMINAL CONNECTIONS
      [[nodiscard]] int& distance_to_terminal(const id_type& node)
      { return this->get_from_vector_grid(_distance_to_terminal, node); }

      [[nodiscard]] int distance_to_terminal(const id_type& node) const
      { return this->get_from_vector_grid(_distance_to_terminal, node); }
      /// @}

      /// @{ -------------------------------------------------- TIME STAMP
      [[nodiscard]] int& timestamp(const id_type& node)
      { return this->get_from_vector_grid(_timestamp, node); }

      [[nodiscard]] int timestamp(const id_type& node) const
      { return this->get_from_vector_grid(_timestamp, node); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = delete;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- HELPER: JOIN
      template<int I = 0>
      void _join_blocks(self_type& res, const self_type& b, id_type& id) const
      {
          for (int x = 0; x < _blocks_size[I]; ++x)
          {
              id[I] = x;

              if constexpr (I != TDims - 1)
              { _join_blocks<I + 1>(res, b, id); }
              else
              { this->get_from_vector_grid(res._blocks, id) = this->get_from_vector_grid(_blocks, id) || this->get_from_vector_grid(b._blocks, id); }
          }
      }
      /// @}

    public:
      /// @{ -------------------------------------------------- JOIN
      self_type join(self_type& b)
      //self_type join(const self_type& b)
      {
          const int newTimeStamp = std::max(b._current_timestamp, this->_current_timestamp);
          self_type res(_size, newTimeStamp, this->_residual, this->_distance_to_terminal, this->_timestamp, this->_flags);

          id_type id;
          _join_blocks(res, b, id);

          return res;
      }
      /// @}

      /// @{ -------------------------------------------------- RESET ID COUNTER
      static void reset_id_counter() noexcept
      { _gid = 0; }
      /// @}

      /// @{ -------------------------------------------------- HELPER: RUN
    private:
      template<int I = 0>
      void _add_active_nodes_outer_loop(id_type& outer_loop_id)
      {
          for (int x = 0; x < _blocks_size[I]; ++x)
          {
              outer_loop_id[I] = x;

              if constexpr (I != TDims - 1)
              { _add_active_nodes_outer_loop<I + 1>(outer_loop_id); }
              else
              {
                  if (!this->get_from_vector_grid(_blocks, outer_loop_id))
                  { continue; }

                  id_type inner_loop_id;
                  _add_active_nodes_inner_loop(outer_loop_id, inner_loop_id);
              }
          }
      }

      template<int I = 0>
      void _add_active_nodes_inner_loop(const id_type& outer_loop_id, id_type& inner_loop_id)
      {
          for (int x = outer_loop_id[I] * gc::BLOCK_SIZE(I); x < std::min(_size[I], (outer_loop_id[I] + 1) * gc::BLOCK_SIZE(I)); ++x)
          {
              inner_loop_id[I] = x;

              if constexpr (I != TDims - 1)
              { _add_active_nodes_inner_loop<I + 1>(outer_loop_id, inner_loop_id); }
              else
              {
                  if (this->flag(inner_loop_id) & gc::FLAG_ACTIVE())
                  { this->_active_nodes.emplace_back(inner_loop_id); }
              }
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- RUN
      void run()
      {
          this->_active_nodes.clear();

          std::queue<id_type> o;
          std::swap(this->_orphans, o);

          this->_current_path_source_to_sink.clear();

          id_type outer_loop_id;
          _add_active_nodes_outer_loop(outer_loop_id);

          while (this->grow())
          {
              if (this->augment())
              { this->adopt(); }
          }
      }
      /// @}
  }; // class GraphCutParallelBlock
} // namespace bk::gc_details

#endif //BKTOOLS_GRAPHCUTPARALLELBLOCK_H
