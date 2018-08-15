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

#ifndef BKCMR_SAMPLEVECTORTIMEDEPENDENT_H
#define BKCMR_SAMPLEVECTORTIMEDEPENDENT_H

#include <functional>
#include <algorithm>
#include <utility>
#include <vector>

#include <bk/Algorithm>
#include <bkCMR/sampling/SampleVector.h>

namespace bk
{
  template<typename T_, typename Compare_ = std::less<T_>> class SampleVectorTimeDependent
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SampleVectorTimeDependent<T_>;

    public:
      using sample_vector_type = SampleVector<T_, Compare_>;
      using time_dependent_sample_vector_type = std::vector<sample_vector_type>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      time_dependent_sample_vector_type _samples;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      SampleVectorTimeDependent() = default;

      SampleVectorTimeDependent(const self_type& other)
          : _samples(other._samples.begin(), other._samples.end())
      { /*do nothing*/ }

      SampleVectorTimeDependent(self_type&& other)
          : _samples(std::move(other._samples))
      { /*do nothing*/ }

      ~SampleVectorTimeDependent() = default;

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      unsigned int get_num_times() const
      { return _samples.size(); }

      sample_vector_type& get_sample_vector_at_time(unsigned int t)
      { return _samples[t]; }

      const sample_vector_type& get_sample_vector_at_time(unsigned int t) const
      { return _samples[t]; }

      sample_vector_type& operator[](unsigned int t)
      { return get_sample_vector_at_time(t); }

      const sample_vector_type& operator[](unsigned int t) const
      { return get_sample_vector_at_time(t); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      self_type& operator=(const self_type& other)
      {
          _samples.resize(other.reserve_num_times());
          std::copy(other._samples.begin(), other._samples.end(), _samples.begin());
          return *this;
      }

      self_type& operator=(self_type&& other)
      {
          _samples = std::move(other._samples);
          return *this;
      }

      void set_num_times(unsigned int T)
      { _samples.resize(T); }

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      void reserve_num_times(unsigned int T)
      { _samples.reserve(T); }

      //====================================================================================================
      //===== STATISTICS
      //====================================================================================================
      /*T_ sum() const
      { return bk::sum(_samples.begin(), _samples.end()); }

      T_ mean() const
      { return bk::mean(_samples.begin(), _samples.end()); }

      T_ median() const
      { return _samples[get_num_samples() / 2]; }

      T_ stdev() const
      { return bk::standard_deviation(_samples.begin(), _samples.end()); }

      T_ variance() const
      { return bk::variance(_samples.begin(), _samples.end()); }

      T_ min() const
      { return *std::min_element(_samples.begin(), _samples.end()); }

      T_ max() const
      { return *std::max_element(_samples.begin(), _samples.end()); }

      std::pair<T_, T_> minmax() const
      { std::minmax(_samples.begin(), _samples.end()); }*/
  }; // class SampleVectorTimeDependent
} // namespace bk

#endif //BKCMR_SAMPLEVECTORTIMEDEPENDENT_H


