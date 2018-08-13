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

#ifndef BKCMR_SAMPLEVECTOR_H
#define BKCMR_SAMPLEVECTOR_H

#include <functional>
#include <algorithm>
#include <utility>
#include <vector>

#include <algorithm/bk/Algorithm>

namespace bk
{
  template<typename T_, typename Compare_ = std::less<T_>> class SampleVector
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = SampleVector<T_>;

    public:
      using vector_type = std::vector<T_>;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      vector_type _samples;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR 
      //====================================================================================================
    public:
      SampleVector() = default;

      SampleVector(const self_type& other)
          : _samples(other._samples.begin(), other._samples.end())
      { /*do nothing*/ }

      SampleVector(self_type&& other)
          : _samples(std::move(other._samples))
      { /*do nothing*/ }

      ~SampleVector() = default;

      //====================================================================================================
      //===== GETTER 
      //====================================================================================================
      unsigned int get_num_samples() const
      { return _samples.size(); }

      bool has_samples() const
      { return !_samples.empty(); }

      const T_& get_sample(unsigned int i) const
      { return _samples[i]; }

      const T_& operator[](unsigned int i) const
      { return get_sample(i); }

      const vector_type& get_sample_vector() const
      { return _samples; }

      auto begin()
      { return _samples.begin(); }

      auto begin() const
      { return _samples.begin(); }

      auto end()
      { return _samples.end(); }

      auto end() const
      { return _samples.end(); }

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      self_type& operator=(const self_type& other)
      {
          _samples.resize(other.get_num_samples());
          std::copy(other._samples.begin(), other._samples.end(), _samples.begin());
          return *this;
      }

      self_type& operator=(self_type&& other)
      {
          _samples = std::move(other._samples);
          return *this;
      }

      void push_back(const T_& s)
      {
          _samples.push_back(s);
          _sort_samples();
      }

      void push_back(T_&& s)
      {
          _samples.push_back(std::move(s));
          _sort_samples();
      }

      template<typename... Args_>
      void emplace_back(Args_&& ... args)
      {
          _samples.emplace_back(std::forward<Args_>(args)...);
          _sort_samples();
      }

      auto erase(typename vector_type::const_iterator position)
      { return _samples.erase(position); }

      auto erase(typename vector_type::const_iterator first, typename vector_type::const_iterator last)
      { return _samples.erase(first, last); }

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
      void reserve_num_samples(unsigned int N)
      { _samples.reserve(N); }

    private:
      void _sort_samples()
      { std::sort(_samples.begin(), _samples.end(), Compare_()); }

    public:

      //====================================================================================================
      //===== STATISTICS
      //====================================================================================================
      T_ sum() const
      { return has_samples() ? bk::sum(_samples.begin(), _samples.end()) : T_(); }

      T_ mean() const
      { return has_samples() ? bk::mean(_samples.begin(), _samples.end()) : T_(); }

      T_ median() const
      { return has_samples() ? _samples[get_num_samples() / 2] : T_(); }

      T_ stdev() const
      { return has_samples() ? bk::standard_deviation(_samples.begin(), _samples.end()) : T_(); }

      T_ variance() const
      { return has_samples() ? bk::variance(_samples.begin(), _samples.end()) : T_(); }

      T_ min() const
      { return has_samples() ? *std::min_element(_samples.begin(), _samples.end()) : T_(); }

      T_ max() const
      { return has_samples() ? *std::max_element(_samples.begin(), _samples.end()) : T_(); }

      std::pair<T_, T_> minmax() const
      { has_samples() ? std::minmax(_samples.begin(), _samples.end()) : std::make_pair<T_, T_>(T_(), T_()); }
  }; // class SampleVector
} // namespace bk

#endif //BKCMR_SAMPLEVECTOR_H


