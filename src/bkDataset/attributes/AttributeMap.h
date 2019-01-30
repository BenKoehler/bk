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

#ifndef BK_ATTRIBUTEMAP_H_
#define BK_ATTRIBUTEMAP_H_

#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>

#include <bk/StringUtils>

namespace bk
{
  template<typename TData> class AttributeMap
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = AttributeMap<TData>;
    public:
      using key_type = unsigned long long;
      using data_type = TData;
      using map_type = std::unordered_map<key_type, data_type>;
      using iterator = typename map_type::iterator;
      using const_iterator = typename map_type::const_iterator;

    private:
      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
      map_type _attrib;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CONSTRUCTORS
      AttributeMap() = default;
      AttributeMap(const self_type&) = default;
      AttributeMap(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- DESTRUCTOR
      ~AttributeMap() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET HASH OF ATTRIBUTE NAME
      [[nodiscard]] static constexpr key_type hash(std::string_view attribute_name)
      {return bk::string_utils::hash(attribute_name.data());}
      /// @}

      /// @{ -------------------------------------------------- GET ATTRIBUTE
      [[nodiscard]] data_type& attribute(std::string_view attribute_name)
      { return attribute(hash(attribute_name)); }

      [[nodiscard]] const data_type& attribute(std::string_view attribute_name) const
      { return attribute(hash(attribute_name)); }

      [[nodiscard]] data_type& attribute(const key_type& attribute_hash)
      {
          assert(has_attribute(attribute_hash) && "attribute does not exist");
          return _attrib.find(attribute_hash)->second;
      }

      [[nodiscard]] const data_type& attribute(const key_type& attribute_hash) const
      {
          assert(has_attribute(attribute_hash) && "attribute does not exist");
          return _attrib.find(attribute_hash)->second;
      }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR[]
      [[nodiscard]] iterator operator[](const key_type& attribute_hash)
      { return attribute(attribute_hash); }

      [[nodiscard]] const_iterator operator[](const key_type& attribute_hash) const
      { return attribute(attribute_hash); }
      /// @}

      /// @{ -------------------------------------------------- GET ITERATORS
      [[nodiscard]] iterator begin()
      { return _attrib.begin(); }

      [[nodiscard]] const_iterator begin() const
      { return _attrib.begin(); }

      [[nodiscard]] const_iterator cbegin() const
      { return _attrib.cbegin(); }

      [[nodiscard]] iterator end()
      { return _attrib.end(); }

      [[nodiscard]] const_iterator end() const
      { return _attrib.end(); }

      [[nodiscard]] const_iterator cend() const
      { return _attrib.cend(); }
      /// @}

      /// @{ -------------------------------------------------- NUM ATTRIBUTES
      [[nodiscard]] unsigned int num_attributes() const
      { return _attrib.size(); }
      /// @}

      /// @{ -------------------------------------------------- HAS ATTRIBUTE(S)
      [[nodiscard]] bool has_attributes() const
      { return num_attributes() != 0; }

      [[nodiscard]] bool has_attribute(std::string_view attribute_name) const
      { return has_attribute(hash(attribute_name)); }

      [[nodiscard]] bool has_attribute(const key_type& attribute_hash) const
      { return _attrib.find(attribute_hash) != end(); }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- CLEAR
      virtual void clear()
      { _attrib.clear(); }
      /// @}

      /// @{ -------------------------------------------------- ADD ATTRIBUTE
      [[maybe_unused]] data_type& add_attribute(std::string_view attribute_name)
      { return add_attribute(hash(attribute_name)); }

      [[maybe_unused]] data_type& add_attribute(std::string_view attribute_name, const data_type& value)
      { return add_attribute(hash(attribute_name), value); }

      [[maybe_unused]] data_type& add_attribute(std::string_view attribute_name, data_type&& value)
      { return add_attribute(hash(attribute_name), std::move(value)); }

      [[maybe_unused]] data_type& add_attribute(const key_type& attribute_hash)
      { return _attrib.try_emplace(attribute_hash, data_type()).first->second; }

      [[maybe_unused]] data_type& add_attribute(const key_type& attribute_hash, const data_type& value)
      {
          if (auto it = _attrib.find(attribute_hash); it != _attrib.end())
          { it->second = value; return it->second;}
          else
          {return _attrib.emplace(attribute_hash, value).first->second;}
      }

      [[maybe_unused]] data_type& add_attribute(const key_type& attribute_hash, data_type&& value)
      {
          if (auto it = _attrib.find(attribute_hash); it != _attrib.end())
          { it->second = std::move(value); return it->second;}
          else
          {return _attrib.emplace(attribute_hash, std::move(value)).first->second;}
      }
      /// @}

      /// @{ -------------------------------------------------- REMOVE ATTRIBUTE
      [[maybe_unused]] unsigned long remove_attribute(std::string_view attribute_name)
      { return _attrib.erase(hash(attribute_name)); }

      [[maybe_unused]] unsigned long remove_attribute(const key_type& attribute_hash)
      { return _attrib.erase(attribute_hash); }

      [[maybe_unused]] iterator remove_attribute(const_iterator pos)
      { return _attrib.erase(pos); }

      [[maybe_unused]] iterator remove_attributes(const_iterator first, const_iterator last)
      { return _attrib.erase(first, last); }
      /// @}

      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}
  }; // class AttributeMap
} // namespace bk

#endif //BK_ATTRIBUTEMAP_H_
