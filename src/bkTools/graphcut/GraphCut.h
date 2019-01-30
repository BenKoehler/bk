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

#ifndef BKTOOLS_GRAPHCUT_H
#define BKTOOLS_GRAPHCUT_H

#if defined(__GNUC__) && defined(_OPENMP)

    #include <parallel/algorithm>

#else
    #include <algorithm>
#endif

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>
#include <tuple>
#include <type_traits>
#include <vector>

#include <bkTools/graphcut/GraphCutBase.h>
#include <bkTools/graphcut/GraphCutBase_Run.h>
#include <bkTools/graphcut/gc_definitions.h>

#ifdef BK_EMIT_PROGRESS

    #include <bkTools/progress/GlobalProgressManager.h>
    #include <bkTools/progress/Progress.h>
    #include <bkTools/localization/GlobalLocalizationManager.h>

#endif

namespace bk
{
  template<int TDims> class GraphCut
      : public gc_details::GraphCutBase<TDims, GraphCut<TDims>>,
        public gc_details::GraphCutBase_Run<TDims, GraphCut<TDims>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCut<TDims>;
      using base_type = gc_details::GraphCutBase<TDims, GraphCut<TDims>>;
      using run_base_type = gc_details::GraphCutBase_Run<TDims, GraphCut<TDims>>;
      using gc = typename base_type::gc;

      using pt2Nd_type = typename base_type::pt2Nd_type;
      template<typename T> using vector_grid_type = typename base_type::template vector_grid_type<T>;

    public:
      using flag_type = typename base_type::flag_type;
      using id_type = typename base_type::id_type;

      friend base_type;
      friend gc_details::GraphCutBase_Run<TDims, GraphCut<TDims>>;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      constexpr GraphCut()
          : base_type(),
            run_base_type(this->_current_timestamp)
      { /* do nothing */ }

      constexpr GraphCut(const self_type&) = default;
      constexpr GraphCut(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCut() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- IS VALID
      [[nodiscard]] bool is_valid(const id_type& node) const
      {
          // out of bounds?
          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (node[dimId] < 0 || node[dimId] >= this->_size[dimId])
              { return false; }
          }

          return true;
      }

      [[nodiscard]] static constexpr bool is_valid_extra(const id_type& /*node*/) noexcept
      { return true; }

      [[nodiscard]] static constexpr bool is_valid_lower_bound(int dimId, const id_type& node)
      { return node[dimId] > 0; }

      [[nodiscard]] bool is_valid_upper_bound(int dimId, const id_type& node) const
      { return node[dimId] < this->_size[dimId] - 1; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = delete;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      /// @{ -------------------------------------------------- HELPER: RESET_IMPL
      static constexpr void reset_impl() noexcept
      { /* do nothing */ }
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

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(25, ___("Performing graph cut"));
          #endif

          this->reset();

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          this->_current_timestamp = 1;

          constexpr int timestamp_init = 1;
          const std::chrono::system_clock::time_point clock_start = std::chrono::system_clock::now(); // todo
          unsigned int cnt_iterations = 0;

          for (unsigned int i = 0; i < this->_connected_to_source.size(); ++i)
          {
              const id_type& s = this->_connected_to_source[i];
              this->set_source_as_parent(s);
              this->set_active(s);
              this->distance_to_terminal(s) = 0;
              this->timestamp(s) = timestamp_init;
          }

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          for (unsigned int i = 0; i < this->_connected_to_sink.size(); ++i)
          {
              const id_type& s = this->_connected_to_sink[i];
              this->set_sink_as_parent(s);
              this->set_active(s);
              this->distance_to_terminal(s) = 0;
              this->timestamp(s) = timestamp_init;
          }

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          while (this->grow())
          {
              if (this->augment())
              { this->adopt(); }

              ++cnt_iterations;
          }

          #pragma omp parallel for
          for (unsigned int i = 0; i < this->_connected_to_source.size(); ++i)
          { this->set_source_set(this->_connected_to_source[i]); }

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          #pragma omp parallel for
          for (unsigned int i = 0; i < this->_connected_to_sink.size(); ++i)
          { this->set_sink_set(this->_connected_to_sink[i]); }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          const std::chrono::system_clock::time_point clock_stop = std::chrono::system_clock::now();
          const unsigned int time_in_sec = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(clock_stop - clock_start).count());
          const unsigned int time_in_ms = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(clock_stop - clock_start).count());

          std::cout << "graph cut finished in " << cnt_iterations << " iterations in " << time_in_sec << " s (" << time_in_ms << " ms)" << std::endl;

          this->_up2date = true;
      }
      /// @}
  }; // class GraphCut
} // namespace bk

#endif //BKTOOLS_GRAPHCUT_H
