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

#ifndef BK_SMOOTHPOINTVALUESFILTER_H
#define BK_SMOOTHPOINTVALUESFILTER_H

#include <vector>

#ifdef BK_EMIT_PROGRESS
    #include "../../../bkTools/Progress"
#endif

#include "bkDataset/lib/export.h"

namespace bk
{
  class BK_DATASET_EXPORT SmoothPointValuesFilter
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SmoothPointValuesFilter;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      unsigned int _num_iterations;
      double _lambda;
      double _mu;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      SmoothPointValuesFilter();
      SmoothPointValuesFilter(const self_type& other);
      SmoothPointValuesFilter(self_type&& other);
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~SmoothPointValuesFilter();
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM ITERATIONS
      [[nodiscard]] unsigned int num_iterations() const;
      /// @}

      /// @{ -------------------------------------------------- GET LAMBDA
      [[nodiscard]] double lambda() const;
      /// @}

      /// @{ -------------------------------------------------- GET MU
      [[nodiscard]] double mu() const;
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] auto operator=(const self_type& other) -> self_type&;
      [[maybe_unused]] auto operator=(self_type&& other) -> self_type&;
      /// @}

      /// @{ -------------------------------------------------- SET NUM ITERATIONS
      void set_num_iterations(unsigned int numIterations);
      /// @}

      /// @{ -------------------------------------------------- SET LAMBDA
      void set_lambda(double lambda);
      /// @}

      /// @{ -------------------------------------------------- SET MU
      void set_mu(double mu);
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      /// @{ -------------------------------------------------- APPLY
      template<typename TDataObject, typename T>
      [[nodiscard]] std::vector <T> apply(TDataObject& d, std::vector <T>&& data_vector_copy, const T& zero_val = T()) const
      {
          if (_num_iterations == 0)
          { return std::vector<T>(); }

          const unsigned int numPoints = d.geometry().num_points();

          if (numPoints < 3)
          { return std::vector<T>(); }

          #ifdef BK_EMIT_PROGRESS
          Progress& prog = bk_progress.emplace_task(1 + _num_iterations, "Smoothing");
          #endif

          // determine neighbor ids of each point
          std::vector <std::vector<unsigned int>> points_neighbor_ids(numPoints);

          for (unsigned int pointId = 0; pointId < numPoints; ++pointId)
          { points_neighbor_ids[pointId] = d.topology().neighbors_of_point(pointId); }

          // alternating temp point vectors
          std::vector <T> v0(std::move(data_vector_copy));
          std::vector <T> v1(v0.size());

          // run
          for (unsigned int iteration_cnt = 0; iteration_cnt < _num_iterations; ++iteration_cnt)
          {
              std::vector <T>& read = iteration_cnt % 2 == 0 ? v0 : v1;
              std::vector <T>& write = iteration_cnt % 2 == 0 ? v1 : v0;

              #pragma omp parallel for
              for (unsigned int id = 0; id < numPoints; ++id)
              {
                  const T& val = read[id];
                  const std::vector<unsigned int>& neighbor_ids = points_neighbor_ids[id];
                  const unsigned int num_neighbors = neighbor_ids.size();

                  write[id] = val;

                  if (num_neighbors != 0)
                  {
                      T center = zero_val;

                      for (unsigned int n = 0; n < num_neighbors; ++n)
                      { center += read[neighbor_ids[n]]; }

                      center /= num_neighbors;
                      const T dirToCenter = center - val;

                      write[id] += (iteration_cnt % 2 == 0 ? _lambda : _mu) * dirToCenter;
                  } // if (num_neighbors != 0)
              } // for id : numPoints

              #ifdef BK_EMIT_PROGRESS
              prog.increment(1);
              #endif
          } // for iterations

          /*
           * write result
           */
          std::vector <T> res = std::move(_num_iterations % 2 == 0 ? v0 : v1);

          #ifdef BK_EMIT_PROGRESS
          prog.set_finished();
          #endif

          return res;
      }
      /// @}
  }; // class SmoothPointValuesFilter
} // namespace bk

#endif //BK_SMOOTHPOINTVALUESFILTER_H
