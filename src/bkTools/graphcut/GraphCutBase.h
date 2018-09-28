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

#ifndef BKTOOLS_GRAPHCUTBASE_H
#define BKTOOLS_GRAPHCUTBASE_H

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>
#include <tuple>
#include <type_traits>
#include <vector>

#include <bkTools/graphcut/GraphCutBase_MemberAccess.h>
#include <bkTools/graphcut/gc_definitions.h>

#ifdef BK_EMIT_PROGRESS

    #include <bkTools/progress/Progress.h>
    #include <bkTools/progress/GlobalProgressManager.h>
    #include <bkTools/localization/GlobalLocalizationManager.h>

#endif

namespace bk::gc_details
{
  template<int TDims, typename TDerived> class GraphCutBase : protected GraphCutBase_MemberAccess<TDims, GraphCutBase<TDims, TDerived>>
  {
      //====================================================================================================
      //===== ASSERTIONS
      //====================================================================================================
      static_assert(TDims >= 2, "invalid num dimensions");

      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = GraphCutBase<TDims, TDerived>;
      using derived_type = TDerived;
      using base_type = GraphCutBase_MemberAccess<TDims, GraphCutBase<TDims, TDerived>>;
    protected:
      using gc = gcdef<TDims>;
      using ptNd_type = std::array<double, TDims>;
      using pt2Nd_type = std::array<double, 2 * TDims>;
      template<typename T> using vector_grid_type = wrap_vector_t<T, TDims>;
    public:
      using flag_type = typename gc::flag_type;
      using id_type = typename gc::id_type;

      friend base_type;

      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] static constexpr int NumDimensions() noexcept
      { return TDims; }
      /// @}

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    protected:
      id_type _size;
      vector_grid_type<ptNd_type> _edge_capacity;
      vector_grid_type<pt2Nd_type> _residual;
      vector_grid_type<int> _distance_to_terminal;
      vector_grid_type<int> _timestamp;
      vector_grid_type<flag_type> _flags;
      std::vector<id_type> _connected_to_source;
      std::vector<id_type> _connected_to_sink;
      bool _up2date;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      GraphCutBase()
          : _up2date(false)
      { _size.fill(0); }

      GraphCutBase(const self_type&) = default;
      GraphCutBase(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~GraphCutBase() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPERS: TO DERIVED
    private:
      [[nodiscard]] constexpr derived_type* deriv() noexcept
      { return static_cast<derived_type*>(this); }

      [[nodiscard]] constexpr const derived_type* deriv() const noexcept
      { return static_cast<const derived_type*>(this); }

    protected:
      /// @}

      /// @{ -------------------------------------------------- GET EDGE CAPACITY
      [[nodiscard]] ptNd_type& edge_capacity(const id_type& n)
      { return this->get_from_vector_grid(_edge_capacity, n); }

      [[nodiscard]] const ptNd_type& edge_capacity(const id_type& n) const
      { return this->get_from_vector_grid(_edge_capacity, n); }
      /// @}

      /// @{ -------------------------------------------------- CONNECTED TO SOURCE
      [[nodiscard]] id_type& connected_to_source(const id_type& n)
      { return this->get_from_vector_grid(_connected_to_source, n); }

      [[nodiscard]] const id_type& connected_to_source(const id_type& n) const
      { return this->get_from_vector_grid(_connected_to_source, n); }
      /// @}

      /// @{ -------------------------------------------------- CONNECTED TO SINK
      [[nodiscard]] id_type& connected_to_sink(const id_type& n)
      { return this->get_from_vector_grid(_connected_to_sink, n); }

      [[nodiscard]] const id_type& connected_to_sink(const id_type& n) const
      { return this->get_from_vector_grid(_connected_to_sink, n); }
      /// @}

    protected:
      /// @{ -------------------------------------------------- SETS
      [[nodiscard]] bool is_in_source_set(const id_type& n) const
      { return this->flag(n) & gc::FLAG_SOURCE_SET(); }

      [[nodiscard]] bool is_in_sink_set(const id_type& n) const
      { return this->flag(n) & gc::FLAG_SINK_SET(); }

      [[nodiscard]] bool is_in_free_set(const id_type& n) const
      { return this->flag(n) & gc::FLAG_FREE_SET(); }

      [[nodiscard]] bool are_in_same_set(const id_type& p, const id_type& q) const
      {
          const flag_type flag_p = this->flag(p);
          const flag_type flag_q = this->flag(q);

          return (flag_p & gc::FLAG_SOURCE_SET() && flag_q & gc::FLAG_SOURCE_SET()) || (flag_p & gc::FLAG_SINK_SET() && flag_q & gc::FLAG_SINK_SET());
      }
      /// @}

    public:
      /// @{ -------------------------------------------------- GET NUM CONNECTED SINK/SOURCE NODES
      [[nodiscard]] unsigned int num_nodes_connected_to_source() const
      { return _connected_to_source.size(); }

      [[nodiscard]] unsigned int num_nodes_connected_to_sink() const
      { return _connected_to_sink.size(); }
      /// @}

      /// @{ -------------------------------------------------- GET SINK/SOURCE NODE LIST
      [[nodiscard]] const std::vector<id_type>& nodes_connected_to_source() const
      { return _connected_to_source; }

      [[nodiscard]] const std::vector<id_type>& nodes_connected_to_sink() const
      { return _connected_to_sink; }
      /// @}
    protected:

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET PARENT
      void set_source_as_parent(const id_type& child)
      {
          flag_type& f = this->flag(child);
          f &= ~gc::PARENT_IS_SINK();
          f |= gc::PARENT_IS_SOURCE();

          set_source_set(child);
      }

      void set_sink_as_parent(const id_type& child)
      {
          flag_type& f = this->flag(child);
          f &= ~gc::PARENT_IS_SOURCE();
          f |= gc::PARENT_IS_SINK();

          set_sink_set(child);
      }
      /// @}

      /// @{ -------------------------------------------------- SET SET
      void set_source_set(const id_type& node)
      {
          flag_type& f = this->flag(node);
          f &= ~gc::FLAG_SINK_SET();
          f &= ~gc::FLAG_FREE_SET();
          f |= gc::FLAG_SOURCE_SET();
      }

      void set_sink_set(const id_type& node)
      {
          flag_type& f = this->flag(node);
          f &= ~gc::FLAG_SOURCE_SET();
          f &= ~gc::FLAG_FREE_SET();
          f |= gc::FLAG_SINK_SET();
      }
      /// @}

      /// @{ -------------------------------------------------- EDGE CAPACITY
      void set_edge_capacity(const id_type& node, unsigned char dir, double cap)
      {
          this->residual(node)[dir] = cap;

          if (cap <= 0)
          { this->flag(node) |= (gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << dir); }
          else
          { this->flag(node) &= ~(gc::FLAG_EDGE_PREDECESSOR_IS_FULL(0) << dir); }
      }

      void set_edge_capacity(const id_type& p, const id_type& q, double cap)
      { set_edge_capacity(p, gc::DIFF_TO_EDGE_ID(gc::PAIRWISE_DIFFERENCE(q, p)), cap); }

      void set_edge_capacity_mutual(const id_type& p, unsigned char dir, double cap)
      {
          set_edge_capacity(p, dir, cap);

          for (int dimId = 0; dimId < TDims; ++dimId)
          {
              if (p[dimId] > 0 && dir == gc::ID_EDGE_PREDECESSOR(dimId))
              {
                  set_edge_capacity(gc::NEIGHBOR_PREDECESSOR(p, dimId), gc::ID_EDGE_SUCCESSOR(dimId), cap);
                  break;
              }
              else if (p[dimId] < _size[dimId] - 1 && dir == gc::ID_EDGE_SUCCESSOR(dimId))
              {
                  set_edge_capacity(gc::NEIGHBOR_SUCCESSOR(p, dimId), gc::ID_EDGE_PREDECESSOR(dimId), cap);
                  break;
              }
          }
      }

      void set_edge_capacity_mutual(const id_type& p, const id_type& q, double cap)
      { set_edge_capacity_mutual(p, gc::DIFF_TO_EDGE_ID(gc::PAIRWISE_DIFFERENCE(q, p)), cap); }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- HELPER: RESET
    private:
      template<int I>
      void _reset(id_type& p)
      {
          for (int x = 0; x < _size[I]; ++x)
          {
              p[I] = x;

              if constexpr (I != TDims - 1)
              { _reset<I + 1>(p); }
              else
              {

                  for (int dimId = 0; dimId < TDims; ++dimId)
                  {
                      if (p[dimId] < _size[dimId] - 1)
                      {
                          const double w = edge_capacity(p)[dimId];
                          set_edge_capacity_mutual(p, gc::ID_EDGE_SUCCESSOR(dimId), w);
                      }
                  }

                  this->timestamp(p) = 0;
                  this->flag(p) = gc::FLAG_FREE_SET();
                  this->distance_to_terminal(p) = gc::invalid_distance;
              }
          } // for x
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- RESET
      void reset()
      {
          #pragma omp parallel for
          for (int x = 0; x < _size[0]; ++x)
          {
              id_type p;
              p.fill(0);
              p[0] = x;

              _reset<1>(p);
          }

          _up2date = false;

          deriv()->reset_impl();
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: INIT
    private:
      void _init(const std::array<unsigned int, TDims>& img_size)
      {
          for (int dimId = 0; dimId < TDims; ++dimId)
          { _size[dimId] = static_cast<int>(img_size[dimId]); }

          pt2Nd_type default_residual;
          default_residual.fill(0.0);

          ptNd_type default_edge_capacity;
          default_edge_capacity.fill(0.0);

          resize_wrapped_vector<TDims>(_edge_capacity, _size, default_edge_capacity);
          resize_wrapped_vector<TDims>(_residual, _size, default_residual);
          resize_wrapped_vector<TDims>(_distance_to_terminal, _size, gc::invalid_distance);
          resize_wrapped_vector<TDims>(_timestamp, _size, 0);
          resize_wrapped_vector<TDims>(_flags, _size, gc::FLAG_FREE_SET());

          _up2date = false;
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: INIT FROM INTENSITY IMAGE
    private:
      template<int I, typename TGrayImage, typename TFnPixelAt, typename TFnScale>
      void _init_from_intensity_image(const TGrayImage& img, const std::array<double, TDims>& img_scale, TFnPixelAt function_pixel_at, id_type& id, TFnScale fn_scale, double weight_function_tolerance)
      {
          for (int x = 0; x < _size[I]; ++x)
          {
              id[I] = x;

              if constexpr (I != TDims - 1)
              { _init_from_intensity_image<I + 1>(img, img_scale, function_pixel_at, id, fn_scale, weight_function_tolerance); }
              else
              {
                  const double x0 = fn_scale(function_pixel_at(img, id));
                  ptNd_type& edgecap = this->edge_capacity(id);

                  for (int dimId = 0; dimId < TDims; ++dimId)
                  {
                      if (id[dimId] < _size[dimId] - 1)
                      {
                          const double x1 = function_pixel_at(img, gc::NEIGHBOR_SUCCESSOR(id, dimId));

                          const double diff = (fn_scale(x1) - x0) / img_scale[dimId];
                          const double w = gc::weight_function(diff, weight_function_tolerance);
                          edgecap[dimId] = w;
                      }
                  } // for dimId
              }
          } // for x
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- INIT FROM INTENSITY IMAGE
      template<typename TGrayImage, typename TFnPixelAt>
      void init_from_intensity_image(const TGrayImage& img, const std::array<unsigned int, TDims>& img_size, const std::array<double, TDims>& img_scale, const std::array<double, 2>& minmaxPixelValue, TFnPixelAt function_pixel_at, double weight_function_tolerance = 0.5)
      {
          _init(img_size);

          auto fn_scale = [&](double x) -> double
          { return 255.0 * (x - static_cast<double>(minmaxPixelValue[0])) / (static_cast<double>(minmaxPixelValue[1]) - static_cast<double>(minmaxPixelValue[0])); };

          #pragma omp parallel for
          for (int x = 0; x < _size[0]; ++x)
          {
              id_type id;
              id[0] = x;
              _init_from_intensity_image<1>(img, img_scale, function_pixel_at, id, fn_scale, weight_function_tolerance);
          }
      }
      /// @}

      /// @{ -------------------------------------------------- HELPER: INIT FROM WEIGHT IMAGE
    private:
      template<int I, typename TWeightImage, typename TFnPixelAt>
      void _init_from_weight_image(const TWeightImage& img, TFnPixelAt function_weight_at, id_type& id)
      {
          for (int x = 0; x < _size[I]; ++x)
          {
              id[I] = x;

              if constexpr (I != TDims - 1)
              { _init_from_weight_image<I + 1>(img, function_weight_at, id); }
              else
              {
                  ptNd_type& edgecap = this->edge_capacity(id);

                  for (int dimId = 0; dimId < TDims; ++dimId)
                  {
                      if (id[dimId] < _size[dimId] - 1)
                      { edgecap[dimId] = function_weight_at(img, id, dimId); }
                  } // for dimId
              }
          } // for x
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- INIT FROM WEIGHT IMAGE
      template<typename TWeightImage, typename TFnPixelAt>
      void init_from_weight_image(const TWeightImage& img, const std::array<unsigned int, TDims>& img_size, TFnPixelAt function_weight_at)
      {
          _init(img_size);

          #pragma omp parallel for
          for (int x = 0; x < _size[0]; ++x)
          {
              id_type id;
              id[0] = x;
              _init_from_weight_image<1>(img, function_weight_at, id);
          }
      }
      /// @}

      /// @{ -------------------------------------------------- HELPERS: AUTOMATIC OUTSIDE IDS WITHIN NARROW BAND
    private:
      template<int I>
      void _create_narrow_band(const id_type& source_node, const id_type& band_radius, vector_grid_type<bool>& band, id_type& p)
      {
          if constexpr (I != TDims - 1)
          {
              for (int i = std::max(0, source_node[I] - band_radius[I]); i < std::min(_size[I], source_node[I] + band_radius[I]); ++i)
              {
                  p[I] = i;
                  _create_narrow_band<I + 1>(source_node, band_radius, band, p);
              }
          }
          else
          {
              for (int i = std::max(0, source_node[I] - band_radius[I]); i < std::min(_size[I], source_node[I] + band_radius[I]); ++i)
              {
                  p[I] = i;

                  #pragma omp critical (gc_narrow_band)
                  { this->get_from_vector_grid(band, p) = false; }
              }
          }
      }

      template<int I>
      void _sink_from_narrow_band(const vector_grid_type<bool>& band, id_type& p)
      {
          if constexpr (I != TDims - 1)
          {
              for (int i = 0; i < _size[I]; ++i)
              {
                  p[I] = i;
                  _sink_from_narrow_band<I + 1>(band, p);
              }
          }
          else
          {
              for (int i = 0; i < _size[I]; ++i)
              {
                  p[I] = i;

                  const bool make_sink = this->get_from_vector_grid(band, p);

                  if (make_sink)
                  {
                      #pragma omp critical (gc_narrow_band)
                      { add_sink_node(p); }
                  }
              }
          }
      }

    public:
      /// @}

      /// @{ -------------------------------------------------- AUTOMATIC OUTSIDE IDS WITHIN NARROW BAND
      void narrow_band_sink_ids(double band_width_percent)
      {
          constexpr int boundary_width = 3;

          id_type band_radius;
          for (int i = 0; i < TDims; ++i)
          { band_radius[i] = std::max(boundary_width, static_cast<int>(std::round(static_cast<double>(_size[i]) * band_width_percent))); }

          #ifdef BK_EMIT_PROGRESS
          bk::Progress& prog = bk_progress.emplace_task(_connected_to_source.size() + 1 + _size[0], ___("Creating graph cut narrow band"));
          #endif

          vector_grid_type<bool> band;
          resize_wrapped_vector<TDims>(band, _size, true);

          #ifdef BK_EMIT_PROGRESS
          prog.increment(1);
          #endif

          #pragma omp parallel for
          for (unsigned int i = 0; i < _connected_to_source.size(); ++i)
          {
              id_type p;
              _create_narrow_band<0>(_connected_to_source[i], band_radius, band, p);

              #ifdef BK_EMIT_PROGRESS
              #pragma omp critical
              { prog.increment(1); }
              #endif
          }

          #pragma omp parallel for
          for (int i = 0; i < _size[0]; ++i)
          {
              id_type p;
              p[0] = i;
              _sink_from_narrow_band<1>(band, p);

              #ifdef BK_EMIT_PROGRESS
              #pragma omp critical
              { prog.increment(1); }
              #endif
          }

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif
      }
      /// @}

      /// @{ -------------------------------------------------- ADD SOURCE/SINK NODE
      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      void add_source_node(TIds... ids)
      {
          static_assert(sizeof...(TIds) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TIds>...>, "arithmetic type arguments required");

          _connected_to_source.emplace_back(id_type{static_cast<int>(ids)...});
          _up2date = false;
      }

      void add_source_node(const id_type& p)
      {
          _connected_to_source.emplace_back(p);
          _up2date = false;
      }

      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      void add_sink_node(TIds... ids)
      {
          static_assert(sizeof...(TIds) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TIds>...>, "arithmetic type arguments required");

          _connected_to_sink.emplace_back(id_type{static_cast<int>(ids)...});
          _up2date = false;
      }

      void add_sink_node(const id_type& p)
      {
          _connected_to_sink.emplace_back(p);
          _up2date = false;
      }
      /// @}

      /// @{ -------------------------------------------------- CLEAR SOURCE/SINK NODES
      void clear_source_nodes()
      {
          _connected_to_source.clear();
          _up2date = false;
      }

      void clear_sink_nodes()
      {
          _connected_to_sink.clear();
          _up2date = false;
      }
      /// @}

      /// @{ -------------------------------------------------- IS IN SEGMENTATION
      template<typename... TIds, std::enable_if_t<(sizeof...(TIds) > 1)>* = nullptr>
      [[nodiscard]] bool is_in_segmentation(TIds... ids)
      {
          static_assert(sizeof...(TIds) == TDims, "invalid number of arguments");
          static_assert(std::conjunction_v<std::is_arithmetic<TIds>...>, "arithmetic type arguments required");

          return is_in_segmentation(id_type{static_cast<int>(ids)...});
      }

      [[nodiscard]] bool is_in_segmentation(const id_type& p) const
      { return this->flag(p) & gc::FLAG_SOURCE_SET(); }
      /// @}
  }; // class GraphCutBase
} // namespace bk

#endif //BKTOOLS_GRAPHCUTBASE_H
