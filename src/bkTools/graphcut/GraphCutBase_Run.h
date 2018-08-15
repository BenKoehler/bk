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

#ifndef BKTOOLS_GRAPHCUTBASE_RUN_H
#define BKTOOLS_GRAPHCUTBASE_RUN_H

#include <array>
#include <algorithm>
#include <limits>
#include <list>
#include <queue>
#include <vector>

#include <bkTools/graphcut/gc_definitions.h>

namespace bk::gc_details
{
  template<int TDims, typename TDerived> class GraphCutBase_Run 
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCutBase_Run<TDims, TDerived>;
      using derived_type = TDerived;
    protected:
      using gc = gcdef<TDims>;
      using pt2Nd_type = typename gc::pt2Nd_type;
      template<typename T> using vector_grid_type = typename gc::template vector_grid_type<T>;
    public:
      using flag_type = typename gc::flag_type;
      using id_type = typename gc::id_type;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      int _current_timestamp;
      std::pair<id_type, id_type> _current_path_contact;
      std::queue<id_type> _orphans;
      std::list<id_type> _active_nodes;
      std::list<id_type> _current_path_source_to_sink; // sorted from source to sink

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GraphCutBase_Run() = delete;
      GraphCutBase_Run(const self_type&) = default;
      GraphCutBase_Run(self_type&&) = default;

      GraphCutBase_Run(int current_timestamp)
          : _current_timestamp(current_timestamp)
      {
          _current_path_contact.first.fill(0);
          _current_path_contact.second.fill(0);
      }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCutBase_Run() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- HELPERS: TO DERIVED
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }
      /// @}

    protected:
      /// @{ -------------------------------------------------- PARENTS
      [[nodiscard]] bool is_parent_of(const id_type& parent, const id_type& child) const
      {
          if (!deriv()->is_valid_extra(parent) || !deriv()->is_valid_extra(child))
          { return false; }

          const id_type diff = gc::PAIRWISE_DIFFERENCE(parent, child);

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (diff[dimId] == -1 && deriv()->flag(child) & gc::FLAG_PARENT_IS_PREDECESSOR(dimId))
              { return true; }

              if (diff[dimId] == +1 && deriv()->flag(child) & gc::FLAG_PARENT_IS_SUCCESSOR(dimId))
              { return true; }
          }

          return false;
      }

      [[nodiscard]] bool has_parent(const id_type& node) const
      {
          const flag_type f = deriv()->flag(node);

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (f & gc::FLAG_PARENT_IS_PREDECESSOR(dimId) && deriv()->is_valid_extra(gc::NEIGHBOR_PREDECESSOR(node, dimId)))
              { return true; }

              if (f & gc::FLAG_PARENT_IS_SUCCESSOR(dimId) && deriv()->is_valid_extra(gc::NEIGHBOR_SUCCESSOR(node, dimId)))
              { return true; }
          }

          return f & gc::PARENT_IS_SOURCE() || f & gc::PARENT_IS_SINK();
      }

      [[nodiscard]] id_type parent_of(const id_type& node) const
      {
          const flag_type f = deriv()->flag(node);

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (deriv()->is_valid_lower_bound(dimId, node) && f & gc::FLAG_PARENT_IS_PREDECESSOR(dimId))
              { return gc::NEIGHBOR_PREDECESSOR(node, dimId); }

              if (deriv()->is_valid_upper_bound(dimId, node) && f & gc::FLAG_PARENT_IS_SUCCESSOR(dimId))
              { return gc::NEIGHBOR_SUCCESSOR(node, dimId); }
          }

          id_type invalid;
          invalid.fill(-1);
          return invalid;
      }

      [[nodiscard]] bool is_valid_parent_of(const id_type& parent, const id_type& orphan)
      { return are_in_same_set(parent, orphan) && !edge_is_saturated(parent, orphan) && is_connected_to_terminal(parent); }

      [[nodiscard]] bool source_is_parent_of(const id_type& node) const
      { return deriv()->flag(node) & gc::PARENT_IS_SOURCE(); }

      [[nodiscard]] bool sink_is_parent_of(const id_type& node) const
      { return deriv()->flag(node) & gc::PARENT_IS_SINK(); }

      [[nodiscard]] bool terminal_is_parent_of(const id_type& node) const
      { return source_is_parent_of(node) || sink_is_parent_of(node); }
      /// @}

      /// @{ -------------------------------------------------- SETS
      [[nodiscard]] bool is_in_source_set(const id_type& node) const
      { return deriv()->flag(node) & gc::FLAG_SOURCE_SET(); }

      [[nodiscard]] bool is_in_sink_set(const id_type& node) const
      { return deriv()->flag(node) & gc::FLAG_SINK_SET(); }

      [[nodiscard]] bool is_in_free_set(const id_type& node) const
      { return deriv()->flag(node) & gc::FLAG_FREE_SET(); }

      [[nodiscard]] bool are_in_same_set(const id_type& p, const id_type& q) const
      {
          if (!deriv()->is_valid_extra(p) || !deriv()->is_valid_extra(q))
          { return false; }

          const flag_type fp = deriv()->flag(p);
          const flag_type fq = deriv()->flag(q);

          return (fp & gc::FLAG_SOURCE_SET() && fq & gc::FLAG_SOURCE_SET()) || (fp & gc::FLAG_SINK_SET() && fq & gc::FLAG_SINK_SET());
      }
      /// @}

      /// @{ -------------------------------------------------- EDGES
      [[nodiscard]] bool edge_is_saturated(const id_type& p, const id_type& q) const
      {
          if (!deriv()->is_valid_extra(p) || !deriv()->is_valid_extra(q))
          { return true; }

          if (is_in_source_set(p))
          {
              const id_type diff = gc::PAIRWISE_DIFFERENCE(q, p);
              const std::uint8_t ID_EDGE = gc::DIFF_TO_EDGE_ID(diff);
              return deriv()->flag(p) & (gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << ID_EDGE);
          }
          else if (is_in_sink_set(p))
          {
              const id_type diff = gc::PAIRWISE_DIFFERENCE(p, q);
              const std::uint8_t ID_EDGE = gc::DIFF_TO_EDGE_ID(diff);
              return deriv()->flag(q) & (gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << ID_EDGE);
          }

          return true;
      }

      [[nodiscard]] double edge_capacity(const id_type& p, const id_type& q) const
      {
          if (!deriv()->is_valid_extra(p) || !deriv()->is_valid_extra(q))
          { return 0.0; }

          const id_type diff = gc::PAIRWISE_DIFFERENCE(q, p);
          const std::uint8_t ID_EDGE = gc::DIFF_TO_EDGE_ID(diff);
          return deriv()->residual(p)[ID_EDGE];
      }
      /// @}

      /// @{ -------------------------------------------------- TERMINAL CONNECTIONS
      [[nodiscard]] int calculate_distance_to_terminal(const id_type& node)
      {
          if (!timestamp_of_node_is_up_to_date(node))
          {
              id_type current_node = node;
              int distance = 0;
              bool connected_to_terminal = false;

              if (terminal_is_parent_of(current_node))
              {
                  deriv()->timestamp(node) = _current_timestamp;
                  deriv()->distance_to_terminal(node) = 0;
                  return 0;
              }

              while (has_parent(current_node))
              {
                  ++distance;
                  current_node = parent_of(current_node);

                  if (timestamp_of_node_is_up_to_date(current_node) && deriv()->distance_to_terminal(current_node) != gc::invalid_distance)
                  {
                      const int dist_of_current = deriv()->distance_to_terminal(current_node);
                      deriv()->distance_to_terminal(node) = distance + dist_of_current;

                      connected_to_terminal = true;
                      break;
                  }

                  if (terminal_is_parent_of(current_node))
                  {
                      deriv()->distance_to_terminal(node) = distance;

                      connected_to_terminal = true;
                      break;
                  }
              }

              deriv()->timestamp(node) = _current_timestamp;

              if (connected_to_terminal)
              {
                  current_node = node;
                  distance = deriv()->distance_to_terminal(node);

                  while (has_parent(current_node))
                  {
                      current_node = parent_of(current_node);
                      --distance;

                      deriv()->distance_to_terminal(current_node) = distance;
                      deriv()->timestamp(current_node) = _current_timestamp;

                      if (terminal_is_parent_of(current_node))
                      { break; }
                  }
              }
              else
              { deriv()->distance_to_terminal(node) = gc::invalid_distance; }
          }

          return deriv()->distance_to_terminal(node);
      }

      [[nodiscard]] bool is_closer_to_terminal_no_set_check(const id_type& p, const id_type& q)
      { return deriv()->timestamp(q) <= deriv()->timestamp(p) && deriv()->distance_to_terminal(q) > deriv()->distance_to_terminal(p); }

      [[nodiscard]] bool is_connected_to_source(const id_type& node)
      { return deriv()->flag(node) & gc::FLAG_SOURCE_SET() && calculate_distance_to_terminal(node) != gc::invalid_distance; }

      [[nodiscard]] bool is_connected_to_sink(const id_type& node)
      { return deriv()->flag(node) & gc::FLAG_SINK_SET() && calculate_distance_to_terminal(node) != gc::invalid_distance; }

      [[nodiscard]] bool is_connected_to_terminal(const id_type& node)
      { return is_connected_to_source(node) || is_connected_to_sink(node); }
      /// @}

      /// @{ -------------------------------------------------- TIME STAMP
      [[nodiscard]] bool timestamp_of_node_is_up_to_date(const id_type& node) const
      { return deriv()->timestamp(node) == _current_timestamp; }
      /// @}

      /// @{ -------------------------------------------------- ACTIVE NODES
      [[nodiscard]] bool has_active_nodes() const
      { return !_active_nodes.empty(); }

      [[nodiscard]] const id_type& current_active_node() const
      { return _active_nodes.front(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) = default;
      /// @}

      /// @{ -------------------------------------------------- SET PARENT
      void set_parent(const id_type& child, const id_type& parent)
      {
          flag_type& fc = deriv()->flag(child);
          flag_type& fp = deriv()->flag(parent);

          // remove parent and orphan flags
          fc &= ~gc::FLAG_ORPHAN();

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              fc &= ~gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
              fc &= ~gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
          }

          const id_type diff = gc::PAIRWISE_DIFFERENCE(parent, child);

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (diff[dimId] == -1) // parent is predecessor
              {
                  fc |= gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
                  fp &= ~gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
                  break;
              }
              else if (diff[dimId] == +1) // parent is successor
              {
                  fc |= gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
                  fp &= ~gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
                  break;
              }
          }

          // copy timestamp from parent
          deriv()->timestamp(child) = deriv()->timestamp(parent);

          // copy distance to terminal from parent
          const int pdist = deriv()->distance_to_terminal(parent);
          deriv()->distance_to_terminal(child) = pdist != gc::invalid_distance ? pdist + 1 : gc::invalid_distance;

          // copy set from parent
          if (fp & gc::FLAG_SOURCE_SET())
          {
              fc &= ~gc::FLAG_FREE_SET();
              fc &= ~gc::FLAG_SINK_SET();
              fc |= gc::FLAG_SOURCE_SET();
          }
          else if (fp & gc::FLAG_SINK_SET())
          {
              fc &= ~gc::FLAG_FREE_SET();
              fc &= ~gc::FLAG_SOURCE_SET();
              fc |= gc::FLAG_SINK_SET();
          }
      }
      /// @}

      /// @{ -------------------------------------------------- SET SET
      void set_free_set(const id_type& node)
      {
          // are_in_same_set() performs valid checks

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              const id_type neighPre = gc::NEIGHBOR_PREDECESSOR(node, dimId);
              const id_type neighSuc = gc::NEIGHBOR_SUCCESSOR(node, dimId);

              if (deriv()->is_valid_lower_bound(dimId, node) && are_in_same_set(node, neighPre))
              {
                  if (!edge_is_saturated(node, neighPre))
                  { set_active(neighPre); }

                  if (is_parent_of(node, neighPre))
                  {
                      deriv()->flag(neighPre) &= ~gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
                      deriv()->distance_to_terminal(neighPre) = gc::invalid_distance;
                      deriv()->timestamp(neighPre) = _current_timestamp;
                  }
              }

              if (deriv()->is_valid_upper_bound(dimId, node) && are_in_same_set(node, neighSuc))
              {
                  if (!edge_is_saturated(node, neighSuc))
                  { set_active(neighSuc); }

                  if (is_parent_of(node, neighSuc))
                  {
                      deriv()->flag(neighSuc) &= ~gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
                      deriv()->distance_to_terminal(neighSuc) = gc::invalid_distance;
                      deriv()->timestamp(neighSuc) = _current_timestamp;
                  }
              }
          } // for dimId

          /*
           * remove node
           */
          flag_type& f = deriv()->flag(node);
          f |= gc::FLAG_FREE_SET();
          f &= ~gc::FLAG_SOURCE_SET();
          f &= ~gc::FLAG_SINK_SET();
          f &= ~gc::FLAG_ACTIVE();
          f &= ~gc::FLAG_ORPHAN();
          f &= ~gc::PARENT_IS_SOURCE();
          f &= ~gc::PARENT_IS_SINK();

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              f &= ~gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
              f &= ~gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
          }

          /*
           * remove connectivity
           */
          deriv()->distance_to_terminal(node) = gc::invalid_distance;
          deriv()->timestamp(node) = _current_timestamp;
      }
      /// @}

      /// @{ -------------------------------------------------- SET STATE
      void set_active(const id_type& node)
      {
          flag_type& fc = deriv()->flag(node);

          if (!(fc & gc::FLAG_ORPHAN()) && !(fc & gc::FLAG_FREE_SET()))
          {
              fc |= gc::FLAG_ACTIVE();

              _active_nodes.emplace_back(node);
          }
      }

      void set_passive(const id_type& node)
      {
          set_passive_no_list_remove(node);
          remove_from_active_nodes(node);
      }

      void set_passive_no_list_remove(const id_type& node)
      { deriv()->flag(node) &= ~gc::FLAG_ACTIVE(); }

      void set_orphan(const id_type& node)
      {
          flag_type& f = deriv()->flag(node);
          f |= gc::FLAG_ORPHAN();
          f &= ~gc::PARENT_IS_SOURCE();
          f &= ~gc::PARENT_IS_SINK();

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              f &= ~gc::FLAG_PARENT_IS_PREDECESSOR(dimId);
              f &= ~gc::FLAG_PARENT_IS_SUCCESSOR(dimId);
          }

          /*
           * remove connectivity
           */
          deriv()->distance_to_terminal(node) = gc::invalid_distance;
          deriv()->timestamp(node) = _current_timestamp;

          _orphans.emplace(node);
      }

      void remove_from_active_nodes(const id_type& node)
      {
          const auto it1 = std::find(_active_nodes.begin(), _active_nodes.end(), node);

          if (it1 != _active_nodes.end())
          { _active_nodes.erase(it1); }
      }

      void remove_first_from_active_nodes()
      { _active_nodes.pop_front(); }
      /// @}

      /// @{ -------------------------------------------------- EDGE CAPACITY
      void decrease_edge_capacity(const id_type& p, const id_type& q, double cap)
      {
          id_type diff = gc::PAIRWISE_DIFFERENCE(q, p);
          const std::uint8_t ID_EDGE0 = gc::DIFF_TO_EDGE_ID(diff);

          for (int dimId = 0; dimId < TDims; ++dimId)
          { diff[dimId] = -diff[dimId]; }

          const std::uint8_t ID_EDGE1 = gc::DIFF_TO_EDGE_ID(diff);

          pt2Nd_type& rp = deriv()->residual(p);
          pt2Nd_type& rq = deriv()->residual(q);

          rp[ID_EDGE0] -= cap;
          rq[ID_EDGE1] += cap;

          if (rp[ID_EDGE0] <= 0)
          { deriv()->flag(p) |= (gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << ID_EDGE0); }

          if (rq[ID_EDGE1] > 0)
          { deriv()->flag(q) &= ~(gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << ID_EDGE1); }
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================    
      /// @{ -------------------------------------------------- CREATE CURRENT PATH
      void create_current_path()
      {
          // _current_path_source_to_sink is sorted from source to sink
          _current_path_source_to_sink.clear();

          id_type* node_in_source_set = nullptr;
          id_type* node_in_sink_set = nullptr;

          if (is_in_source_set(_current_path_contact.first))
          {
              node_in_source_set = &_current_path_contact.first;
              node_in_sink_set = &_current_path_contact.second;
          }
          else
          {
              node_in_source_set = &_current_path_contact.second;
              node_in_sink_set = &_current_path_contact.first;
          }

          // adds nodes from source set so that last node is closest to source
          while (has_parent((*node_in_source_set)) && !source_is_parent_of(*node_in_source_set))
          {
              _current_path_source_to_sink.push_front(*node_in_source_set);
              *node_in_source_set = parent_of(*node_in_source_set);
          }

          _current_path_source_to_sink.push_front(*node_in_source_set);

          while (has_parent(*node_in_sink_set) && !sink_is_parent_of(*node_in_sink_set))
          {
              _current_path_source_to_sink.push_back(*node_in_sink_set);
              *node_in_sink_set = parent_of(*node_in_sink_set);
          }

          _current_path_source_to_sink.push_back(*node_in_sink_set);
      }
      /// @}

      /// @{ -------------------------------------------------- FIND MINIMUM CAPACITY OF CURRENT PATH
      [[nodiscard]] double find_minimum_capacity_of_current_path()
      {
          double minCap = std::numeric_limits<double>::max();

          // _current_path_source_to_sink always has at least nodes from the contact
          auto it0 = _current_path_source_to_sink.begin();
          auto it1 = ++_current_path_source_to_sink.begin();
          for (; it1 != _current_path_source_to_sink.end(); ++it0, ++it1)
          { minCap = std::min(minCap, edge_capacity(*it0, *it1)); }

          return minCap;
      }
      /// @}

      /// @{ -------------------------------------------------- GROW
      [[nodiscard]] bool grow()
      {
          while (has_active_nodes())
          {
              const id_type active_node = current_active_node();

              for (int dimId = 0; dimId < TDims; ++dimId)
              {
                  for (unsigned int i = 0; i < 2; ++i)
                  {
                      const id_type neighbor = i == 0 ? gc::NEIGHBOR_PREDECESSOR(active_node, dimId) : gc::NEIGHBOR_SUCCESSOR(active_node, dimId);

                      if (!deriv()->is_valid(neighbor))
                      { continue; }

                      if (!edge_is_saturated(active_node, neighbor))
                      {
                          if (is_in_free_set(neighbor))
                          {
                              set_parent(neighbor, active_node);
                              set_active(neighbor);
                          }

                          if (!is_in_free_set(neighbor) && !are_in_same_set(active_node, neighbor))
                          {
                              _current_path_contact.first = active_node;
                              _current_path_contact.second = neighbor;

                              return true;
                          }
                      }
                  } // for i
              } // for dimId

              set_passive_no_list_remove(active_node);
              remove_first_from_active_nodes();
          } // while has_active_nodes()

          return false;
      }
      /// @}

      /// @{ -------------------------------------------------- AUGMENT
      [[nodiscard]] bool augment()
      {
          ++_current_timestamp;

          create_current_path();

          const double minCap = find_minimum_capacity_of_current_path();
          auto it0 = _current_path_source_to_sink.begin();
          auto it1 = ++_current_path_source_to_sink.begin();
          for (; it1 != _current_path_source_to_sink.end(); ++it0, ++it1)
          {
              decrease_edge_capacity(*it0, *it1, minCap);

              if (edge_capacity(*it0, *it1) == 0)
              {
                  if (is_in_source_set(*it0) && is_in_source_set(*it1))
                  { set_orphan(*it1); }
                  else if (is_in_sink_set(*it0) && is_in_sink_set(*it1))
                  { set_orphan(*it0); }
              }
          }

          return true;
      }
      /// @}

      /// @{ -------------------------------------------------- ADOPT
      void adopt()
      {
          while (!_orphans.empty())
          {
              const id_type orphan = _orphans.front();
              _orphans.pop();

              id_type neighbor[gc::NUM_NEIGHBORS()];

              for (int dimId = 0; dimId < TDims; ++dimId)
              {
                  neighbor[gc::ID_EDGE_PREDECESSOR(dimId)] = gc::NEIGHBOR_PREDECESSOR(orphan, dimId);
                  neighbor[gc::ID_EDGE_SUCCESSOR(dimId)] = gc::NEIGHBOR_SUCCESSOR(orphan, dimId);
              } // for dimId

              std::vector<unsigned int> valid_parents;
              valid_parents.reserve(gc::NUM_NEIGHBORS());

              // valid id check is performed in is_valid_parent_of()

              for (unsigned int i = 0; i < gc::NUM_NEIGHBORS(); ++i)
              {
                  if (is_valid_parent_of(neighbor[i], orphan))
                  { valid_parents.push_back(i); }
              } // for i

              if (valid_parents.empty())
              {
                  // valid id check is performed in are_in_same_set()

                  for (unsigned int i = 0; i < gc::NUM_NEIGHBORS(); ++i)
                  {
                      if (are_in_same_set(orphan, neighbor[i]))
                      {
                          if (is_parent_of(orphan, neighbor[i]))
                          { set_orphan(neighbor[i]); }

                          if (!edge_is_saturated(neighbor[i], orphan))
                          { set_active(neighbor[i]); }
                      }
                  } // for i

                  remove_from_active_nodes(orphan);
                  set_free_set(orphan);
              }
              else
              {
                  int minDist = std::numeric_limits<int>::max();

                  id_type minDistParent;
                  minDistParent.fill(-1);

                  for (unsigned int i: valid_parents)
                  {
                      if (const int d = deriv()->distance_to_terminal(neighbor[i]); d < minDist)
                      {
                          minDist = d;
                          minDistParent = neighbor[i];
                      }
                  }

                  set_parent(orphan, minDistParent);
                  deriv()->timestamp(orphan) = _current_timestamp;
              }
          }
      }
      /// @}
  }; // class GraphCutBase_Run
} // namespace bk

#endif //BKTOOLS_GRAPHCUTBASE_RUN_H
