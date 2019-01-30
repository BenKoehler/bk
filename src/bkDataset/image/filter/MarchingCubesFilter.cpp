/*
 * This implementation of the Marching Cubes algorithm is based on
 * http://paulbourke.net/geometry/polygonise/ ,
 * provided by Paul Bourke.
 *
 * --------------------------------------------------------------------------------
 *
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

#include "MarchingCubesFilter.h"

namespace bk
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  MarchingCubesFilter::MarchingCubesFilter()
      : MarchingCubesFilter(0.5)
  { /* do nothing */ }

  MarchingCubesFilter::MarchingCubesFilter(const self_type&) = default;
  MarchingCubesFilter::MarchingCubesFilter(self_type&&) noexcept = default;

  MarchingCubesFilter::MarchingCubesFilter(double iso)
      : _iso(iso)
  { /* do nothing */ }

  MarchingCubesFilter::~MarchingCubesFilter() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  double MarchingCubesFilter::iso_value() const
  { return _iso; }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  auto MarchingCubesFilter::operator=(const self_type&) -> self_type& = default;
  auto MarchingCubesFilter::operator=(self_type&&) noexcept -> self_type& = default;

  void MarchingCubesFilter::set_iso_value(double iso)
  { _iso = iso; }
} // namespace bk