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

#include <algorithm>
#include <cassert>
#include <utility>

#include <bkTools/bitvector/BitVectorDynamic.h>

namespace bk::details
{
  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  BitVectorDynamic::BitVectorDynamic(unsigned int size)
      : _size(size),
        _data((_size / 8) + (_size % 8 == 0 ? 0 : 1), 0)
  { /* do nothing */ }

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  unsigned int BitVectorDynamic::size() const
  { return _size; }

  auto BitVectorDynamic::operator[](unsigned int i) -> BitRef<value_type>
  {
      assert(static_cast<decltype(size())>(i) < size() && "BitVectorDynamic operator[]: invalid id");
      return BitRef<value_type>(_data[i / 8], static_cast<value_type>(i % 8));
  }

  auto BitVectorDynamic::operator[](unsigned int i) const -> BitRef<const value_type>
  {
      assert(static_cast<decltype(size())>(i) < size() && "BitVectorDynamic operator[]: invalid id");
      return BitRef<const value_type>(_data[i / 8], static_cast<value_type>(i % 8));
  }

  auto BitVectorDynamic::data() -> typename container_type::value_type*
  { return _data.data(); }

  auto BitVectorDynamic::data() const -> const container_type::value_type*
  { return _data.data(); }

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void BitVectorDynamic::set_size(unsigned int size)
  {
      _size = size;
      _data.resize((_size / 8) + (_size % 8 == 0 ? 0 : 1), 0);
  }

  //====================================================================================================
  //===== I/O
  //====================================================================================================
  bool BitVectorDynamic::load(std::string_view filepath)
  {
      if (filepath.empty())
      { return false; }

      std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

      if (file.good())
      {
          load(file);
          file.close();
          return true;
      }

      return false;
  }

  void BitVectorDynamic::load(std::ifstream& file)
  {
      using file_size_type = std::uint64_t;
      file_size_type N = 0;
      file.read(reinterpret_cast<char*>(&N), sizeof(file_size_type));

      _data.resize(N);
      _size = N * 8;

      file.read(reinterpret_cast<char*>(_data.data()), N * sizeof(value_type));
  }
} // namespace bk::details


