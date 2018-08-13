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

#ifndef BKTOOLS_GRAPHCUTPARALLEL_H
#define BKTOOLS_GRAPHCUTPARALLEL_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>

#else
    #include <algorithm>
#endif

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>
#include <tuple>
#include <type_traits>
#include <vector>

#include "GraphCutBase.h"
#include "GraphCutParallelBlock.h"
#include "Edge.h"
#include "gc_definitions.h"

namespace bk
{
  template<int TDims> class GraphCutParallel : public gc_details::GraphCutBase<TDims, GraphCutParallel<TDims>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCutParallel<TDims>;
      using base_type = gc_details::GraphCutBase<TDims, GraphCutParallel<TDims>>;
      using gc = typename base_type::gc;
      using edge_type = gc_details::Edge<TDims>;
      using block_type = gc_details::GraphCutParallelBlock<TDims>;

    public:
      using flag_type = typename base_type::flag_type;
      using id_type = typename base_type::id_type;

      friend base_type;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr GraphCutParallel() = default;
      constexpr GraphCutParallel(const self_type&) = default;
      constexpr GraphCutParallel(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCutParallel() = default;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

    private:
      /// @{ -------------------------------------------------- SET ACTIVE
      void _set_active(const id_type& node)
      {
          flag_type& fc = this->flag(node);

          if (!(fc & gc::FLAG_ORPHAN()) && !(fc & gc::FLAG_FREE_SET()))
          { fc |= gc::FLAG_ACTIVE(); }
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: RESET_IMPL
      static constexpr void reset_impl()
      { block_type::reset_id_counter(); }
      /// @}

      /// @{ -------------------------------------------------- HELPER: INIT BLOCKS
      template<int I>
      void _init_blocks(id_type& p, const id_type& numBlocks, std::vector<block_type>& blocks, std::vector<edge_type>& edges, unsigned int& edgecnt, unsigned int& blockidcnt)
      {
          for (int x = 0; x < numBlocks[I]; ++x)
          {
              p[I] = x;

              if constexpr (I != TDims - 1)
              { _init_blocks<I + 1>(p, numBlocks, blocks, edges, edgecnt, blockidcnt); }
              else
              {
                  id_type size_from;
                  for (int dimId = 0; dimId < TDims; ++dimId)
                  { size_from[dimId] = p[dimId] * gc::BLOCK_SIZE(dimId); }

                  blocks.emplace_back(this->_size, size_from, /*timestamp_init=*/1, this->_residual, this->_distance_to_terminal, this->_timestamp, this->_flags);

                  for (int dimId = 0; dimId < TDims; ++dimId)
                  {
                      if (p[dimId] < numBlocks[dimId] - 1) /// edge to successor
                      {
                          edge_type& e = edges[edgecnt++];

                          e.blockID0 = blockidcnt;
                          e.blockID1 = blockidcnt;

                          unsigned int blockID1offset = 1;
                          for (int k = dimId + 1; k < TDims; ++k)
                          { blockID1offset *= numBlocks[k]; }

                          e.blockID1 += blockID1offset;

                          for (int k = 0; k < TDims; ++k)
                          {
                              if (k == dimId)
                              {
                                  e.size_to[k] = (p[k] + 1) * gc::BLOCK_SIZE(k);
                                  e.size_from[k] = e.size_to[k] - 1;
                              }
                              else
                              {
                                  e.size_from[k] = p[k] * gc::BLOCK_SIZE(k);
                                  e.size_to[k] = std::min(this->_size[k], e.size_from[k] + gc::BLOCK_SIZE(k));
                              }
                          }
                      }
                  }

                  ++blockidcnt;
              }
          } // for x
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: COUNT POTENTIAL AUGMENTATIONS
      template<int I, int skipDim>
      void _count_potential_augmentations(const edge_type& e, id_type& p, unsigned int& score) const
      {
          if constexpr (I == skipDim && I != TDims - 1)
          { _count_potential_augmentations<I + 1, skipDim>(e, p, score); }
          else
          {
              for (int x = e.size_from[I]; x < e.size_to[I]; ++x)
              {
                  p[I] = x;

                  const id_type neigh = gc::NEIGHBOR_SUCCESSOR(p, skipDim);

                  if (!this->are_in_same_set(p, neigh) && !this->is_in_free_set(p) && !this->is_in_free_set(neigh))
                  { ++score; }
              }
          }
      }

      template<int I = 0>
      [[nodiscard]] unsigned int count_potential_augmentations(const edge_type& e) const
      {
          if constexpr (I < TDims)
          {
              if (e.size_to[I] - e.size_from[I] == 1)
              {
                  id_type p;
                  p[I] = e.size_from[I];

                  unsigned int score = 0;

                  _count_potential_augmentations<0, I>(e, p, score);

                  if (score != 0)
                  { return score; }
              }
              else
              { return count_potential_augmentations<I + 1>(e); }
          }

          return 0U;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: ACTIVATE NODES FOR POTENTIAL AUGMENTATION ON EDGE
      template<int I, int skipDim>
      void __activate_nodes_for_potential_augmentation_on_edge(const edge_type& e, id_type& p)
      {
          if constexpr (I != TDims - 1)
          { _activate_nodes_for_potential_augmentation_on_edge<I + 1, skipDim, false>(e, p); }
          else
          {
              const id_type neigh = gc::NEIGHBOR_SUCCESSOR(p, skipDim);

              if (!this->are_in_same_set(p, neigh))
              {
                  _set_active(p);
                  _set_active(neigh);
              }
          }
      }

      template<int I, int skipDim, bool first = true>
      void _activate_nodes_for_potential_augmentation_on_edge(const edge_type& e, id_type& p)
      {
          if constexpr (I == skipDim && I != TDims - 1)
          { _activate_nodes_for_potential_augmentation_on_edge<I + 1, skipDim, first>(e, p); }
          else
          {
              if constexpr (first)
              {
                  #pragma omp parallel for
                  for (int x = e.size_from[I]; x < e.size_to[I]; ++x)
                  {
                      p[I] = x;
                      __activate_nodes_for_potential_augmentation_on_edge<I, skipDim>(e, p);
                  }
              }
              else
              {
                  for (int x = e.size_from[I]; x < e.size_to[I]; ++x)
                  {
                      p[I] = x;
                      __activate_nodes_for_potential_augmentation_on_edge<I, skipDim>(e, p);
                  }
              }
          }
      }

      template<int I = 0>
      void activate_nodes_for_potential_augmentation_on_edge(const edge_type& e)
      {
          if constexpr (I < TDims)
          {
              if (e.size_to[I] - e.size_from[I] == 1)
              {
                  id_type p;
                  p[I] = e.size_from[I];

                  _activate_nodes_for_potential_augmentation_on_edge<0, I>(e, p);
              }
              else
              { activate_nodes_for_potential_augmentation_on_edge<I + 1>(e); }
          }
      }
      /// @}

    public:
      /// @{ -------------------------------------------------- RUN
      void run()
      {
          /*
           * validity check
           */
          if (this->_up2date)
          { return; }

          const bool anySizeIsZero = 0 == std::accumulate(this->_size.begin(), this->_size.end(), 1, [](int x, int y)
          { return x * y; });

          if (anySizeIsZero || this->_connected_to_source.empty() || this->_connected_to_sink.empty())
          {
              std::cerr << "init graph cut first" << std::endl;
              return;
          }

          /*
           * determine num blocks per dimension
           * and num blocks total
           */
          id_type numBlocks;
          int numBlocksTotal = 1;
          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              numBlocks[dimId] = (this->_size[dimId] / gc::BLOCK_SIZE(dimId)) + (this->_size[dimId] % gc::BLOCK_SIZE(dimId) ? 1 : 0);
              numBlocksTotal *= numBlocks[dimId];
          }

          /*
           * determine num edges total
           */
          int numEdgesTotal = 0;
          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              int tempNumEdges = numBlocks[dimId] - 1;

              for (int k = 0; k < dimId; ++k)
              { tempNumEdges *= numBlocks[k]; }

              for (int k = dimId + 1; k < TDims; ++k)
              { tempNumEdges *= numBlocks[k]; }

              numEdgesTotal += tempNumEdges;
          }

          this->reset();

          constexpr int timestamp_init = 1;
          const std::chrono::system_clock::time_point clock_start = std::chrono::system_clock::now(); // todo

          /*
           * divide image into blocks
           */
          block_type::reset_id_counter();

          std::vector<block_type> blocks;
          std::vector<edge_type> edges;

          #pragma omp parallel sections
          {
                  #pragma omp section
              {
                  for (unsigned int i = 0; i < this->_connected_to_source.size(); ++i)
                  {
                      const id_type& s = this->_connected_to_source[i];
                      this->set_source_as_parent(s);
                      _set_active(s);
                      this->distance_to_terminal(s) = 0;
                      this->timestamp(s) = timestamp_init;
                  }

                  for (unsigned int i = 0; i < this->_connected_to_sink.size(); ++i)
                  {
                      const id_type& s = this->_connected_to_sink[i];
                      this->set_sink_as_parent(s);
                      _set_active(s);
                      this->distance_to_terminal(s) = 0;
                      this->timestamp(s) = timestamp_init;
                  }
              } // omp section

                  #pragma omp section
              {
                  blocks.reserve(2 * numBlocksTotal);
                  edges.resize(numEdgesTotal);
                  unsigned int edgecnt = 0;
                  unsigned int blockidcnt = 0;

                  for (int x = 0; x < numBlocks[0]; ++x)
                  {
                      id_type p;
                      p.fill(0);
                      p[0] = x;

                      _init_blocks<1>(p, numBlocks, blocks, edges, edgecnt, blockidcnt);
                  }
              } // omp section
          }; // omp parallel sections

          //------------------------------------------------------------------------------------------------------
          // Phase 1: Uniform Partitioning
          //------------------------------------------------------------------------------------------------------
          /*
           * process blocks
           */
          #pragma omp parallel for schedule(dynamic, 1)
          for (unsigned int blockid = 0; blockid < blocks.size(); ++blockid)
          { blocks[blockid].run(); }

          //------------------------------------------------------------------------------------------------------
          // Phase 2 preparation
          //------------------------------------------------------------------------------------------------------

          // this can happen in rare cases where a block consists only of one rows/cols/slices
          edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const edge_type& e)
          {
              for (int dimId = 0; dimId < TDims; ++dimId)
              {
                  if (e.size_from[dimId] >= this->_size[dimId] - 1)
                  { return true; }
              }

              return false;
          }), edges.end());

          /*
           * count potential augmentations on edges
           */
          #pragma omp parallel for
          for (unsigned int i = 0; i < edges.size(); ++i)
          {
              edge_type& e = edges[i];
              e.score = count_potential_augmentations<0>(e);
          }

          /*
           * sort edge scores (descending)
           */
          #if defined(__GNUC__) && defined(_OPENMP)
          __gnu_parallel
          #else
          std
          #endif
          ::sort(edges.begin(), edges.end(), [](const edge_type& a, const edge_type& b) -> bool
          { return b.score < a.score; });

          //------------------------------------------------------------------------------------------------------
          // Phase 2: Adaptive Merging
          //------------------------------------------------------------------------------------------------------
              #pragma omp parallel for num_threads(gc::NUM_THREADS())
          for (unsigned int t = 0; t < gc::NUM_THREADS(); ++t)
          {
              block_type* block0 = nullptr;
              block_type* block1 = nullptr;
              block_type* newblock = nullptr;
              unsigned int block0id = 0;
              unsigned int block1id = 0;
              std::list<edge_type> current_edges;
              unsigned int newBlockid = 0;

              while (true)
              {
                  current_edges.clear();

                  #pragma omp critical (edge_lock)
                  {
                      for (auto it = edges.begin(); it != edges.end(); ++it)
                      {
                          block0 = &blocks[it->blockID0];
                          block1 = &blocks[it->blockID1];

                          if (!block0->_locked && !block1->_locked)
                          {
                              block0id = block0->_id;
                              block1id = block1->_id;

                              #pragma omp parallel sections
                              {
                                  #pragma omp section
                                  {
                                      /*
                                       * searching further edges that combine the same blocks
                                       * e.g.:  --------------------
                                       *        |            |     |
                                       *               (----)-------
                                       *        |     (|)    |     |
                                       *        --------------------
                                       *        |      |     |     |
                                       *        --------------------
                                       */
                                      current_edges.push_back(*it);
                                      it = edges.erase(it);

                                      while (it != edges.end())
                                      {
                                          if ((it->blockID0 == block0id && it->blockID1 == block1id) || (it->blockID0 == block1id && it->blockID1 == block0id))
                                          {
                                              current_edges.push_back(*it);
                                              it = edges.erase(it);
                                          }
                                          else
                                          { ++it; }
                                      }
                                  } // omp section

                                  #pragma omp section
                                  {
                                      blocks.emplace_back(std::move(block0->join(*block1)));
                                      newblock = &blocks.back();
                                      newBlockid = newblock->_id;
                                      newblock->_locked = true;
                                  } // omp section
                              }; // omp parallel sections

                              #pragma omp parallel for
                              for (unsigned int i = 0; i < edges.size(); ++i)
                              {
                                  edge_type& e = edges[i];

                                  if (e.blockID0 == block0id || e.blockID0 == block1id)
                                  { e.blockID0 = newBlockid; }
                                  else if (e.blockID1 == block0id || e.blockID1 == block1id)
                                  { e.blockID1 = newBlockid; }
                              }

                              break; // for edges
                          } // if blocks not locked
                      } // for edges
                  } // lock

                  if (current_edges.empty())
                  { break; }

                  /*
                   * active nodes for potential augmentation on edge
                   */
                  for (const auto& e: current_edges)
                  { activate_nodes_for_potential_augmentation_on_edge(e); }

                  newblock->run();

                  #pragma omp critical (edge_lock)
                  { newblock->_locked = false; }
              } // while true
          } // for threads

              #pragma omp parallel for
          for (unsigned int i = 0; i < this->_connected_to_source.size(); ++i)
          { this->set_source_set(this->_connected_to_source[i]); }

              #pragma omp parallel for
          for (unsigned int i = 0; i < this->_connected_to_sink.size(); ++i)
          { this->set_sink_set(this->_connected_to_sink[i]); }

          const std::chrono::system_clock::time_point clock_stop = std::chrono::system_clock::now();
          const unsigned int time_in_sec = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(clock_stop - clock_start).count());
          const unsigned int time_in_ms = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(clock_stop - clock_start).count());

          std::cout << "graph cut finished in " << time_in_sec << " s (" << time_in_ms << " ms)" << std::endl;

          this->_up2date = true;
      }
      /// @}
  }; // class GraphCutParallel
} // namespace bk

#endif //BKTOOLS_GRAPHCUTPARALLEL_H
