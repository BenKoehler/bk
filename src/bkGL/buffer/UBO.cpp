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
#include <unordered_map>

#include "../../../include/bkTools/StringUtils"

#include "../../../include/bkGL/buffer/RegisteredValue.h"
#include "../../../include/bkGL/buffer/UBO.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct UBO::Impl
  {
      std::unordered_map<std::string/*name*/, RegisteredValue, string_utils::hash_obj> registered_values;

      Impl()
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  UBO::UBO()
      : base_type(),
  #else
  UBO::UBO(bk::qt_gl_functions* gl)
      : base_type(gl),
  #endif
        _pdata(std::make_unique<Impl>())
  { set_default_config_uniform_buffer_object(); }

  UBO::UBO(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBO::~UBO() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NUM REGISTERED VALUES
  unsigned int UBO::num_registered_values() const
  { return _pdata->registered_values.size(); }
  /// @}

  /// @{ -------------------------------------------------- REGISTERED VALUES SIZE IN BYTES
  unsigned int UBO::registered_values_size_in_bytes() const
  {
      GLuint s = 0;

      for (const auto& r: _pdata->registered_values)
      {
          // accumulate sizes of all currently registered value
          s += r.second.size_in_bytes();
      }

      return s;
  }
  /// @}

  /// @{ -------------------------------------------------- GET REGISTERED VALUES ITERATORS
  auto UBO::registered_values_begin()
  { return _pdata->registered_values.begin(); }

  auto UBO::registered_values_begin() const
  { return _pdata->registered_values.begin(); }

  auto UBO::registered_values_end()
  { return _pdata->registered_values.end(); }

  auto UBO::registered_values_end() const
  { return _pdata->registered_values.end(); }
  /// @}

  /// @{ -------------------------------------------------- REGISTERED VALUES
  std::vector<RegisteredValue> UBO::registered_values() const
  {
      std::vector<RegisteredValue> r;
      r.reserve(num_registered_values());

      for (const std::pair<std::string, RegisteredValue>& p: _pdata->registered_values)
      { r.push_back(p.second); }

      std::sort(r.begin(), r.end(), [](const RegisteredValue& a, const RegisteredValue& b)
      { return a.offset_in_bytes() < b.offset_in_bytes(); });

      return r;
  }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBO::operator=(self_type&& other) -> self_type&
  {
      _pdata.reset(nullptr);
      _pdata = std::make_unique<Impl>(*other._pdata);
      return *this;
  }
  /// @}

  /// @{ -------------------------------------------------- REGISTER VALUE
  void UBO::_register_value(std::string_view name, GLenum type)
  { _pdata->registered_values.insert(std::make_pair(name, RegisteredValue(name,type,registered_values_size_in_bytes()))); }

  void UBO::register_value(std::string_view name, bool)
  { register_GL_INT(name); }

  void UBO::register_value(std::string_view name, std::uint8_t)
  { register_GL_UNSIGNED_INT(name); }

  void UBO::register_value(std::string_view name, std::uint16_t)
  { register_GL_UNSIGNED_INT(name); }

  void UBO::register_value(std::string_view name, std::uint32_t)
  { register_GL_UNSIGNED_INT(name); }

  void UBO::register_value(std::string_view name, std::uint64_t)
  { register_GL_UNSIGNED_INT(name); }

  void UBO::register_value(std::string_view name, std::int8_t)
  { register_GL_INT(name); }

  void UBO::register_value(std::string_view name, std::int16_t)
  { register_GL_INT(name); }

  void UBO::register_value(std::string_view name, std::int32_t)
  { register_GL_INT(name); }

  void UBO::register_value(std::string_view name, std::int64_t)
  { register_GL_INT(name); }

  void UBO::register_value(std::string_view name, float)
  { register_GL_FLOAT(name); }

  void UBO::register_value(std::string_view name, double)
  { register_GL_FLOAT(name); }

  void UBO::register_GL_INT(std::string_view name)
  { _register_value(name, GL_INT); }

  void UBO::register_GL_UNSIGNED_INT(std::string_view name)
  { _register_value(name, GL_UNSIGNED_INT); }

  void UBO::register_GL_FLOAT(std::string_view name)
  { _register_value(name, GL_FLOAT); }

  void UBO::register_GL_DOUBLE(std::string_view name)
  { _register_value(name, GL_DOUBLE); }

  void UBO::register_GL_INT_VEC2(std::string_view name)
  { _register_value(name, GL_INT_VEC2); }

  void UBO::register_GL_INT_VEC3(std::string_view name)
  { _register_value(name, GL_INT_VEC3); }

  void UBO::register_GL_INT_VEC4(std::string_view name)
  { _register_value(name, GL_INT_VEC4); }

  void UBO::register_GL_UNSIGNED_INT_VEC2(std::string_view name)
  { _register_value(name, GL_UNSIGNED_INT_VEC2); }

  void UBO::register_GL_UNSIGNED_INT_VEC3(std::string_view name)
  { _register_value(name, GL_UNSIGNED_INT_VEC3); }

  void UBO::register_GL_UNSIGNED_INT_VEC4(std::string_view name)
  { _register_value(name, GL_UNSIGNED_INT_VEC4); }

  void UBO::register_GL_FLOAT_VEC2(std::string_view name)
  { _register_value(name, GL_FLOAT_VEC2); }

  void UBO::register_GL_FLOAT_VEC3(std::string_view name)
  { _register_value(name, GL_FLOAT_VEC3); }

  void UBO::register_GL_FLOAT_VEC4(std::string_view name)
  { _register_value(name, GL_FLOAT_VEC4); }

  void UBO::register_GL_DOUBLE_VEC2(std::string_view name)
  { _register_value(name, GL_DOUBLE_VEC2); }

  void UBO::register_GL_DOUBLE_VEC3(std::string_view name)
  { _register_value(name, GL_DOUBLE_VEC3); }

  void UBO::register_GL_DOUBLE_VEC4(std::string_view name)
  { _register_value(name, GL_DOUBLE_VEC4); }

  void UBO::register_GL_FLOAT_MAT2(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT2); }

  void UBO::register_GL_FLOAT_MAT3(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT3); }

  void UBO::register_GL_FLOAT_MAT4(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT4); }

  void UBO::register_GL_DOUBLE_MAT2(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT2); }

  void UBO::register_GL_DOUBLE_MAT3(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT3); }

  void UBO::register_GL_DOUBLE_MAT4(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT4); }

  void UBO::register_GL_FLOAT_MAT2x3(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT2x3); }

  void UBO::register_GL_DOUBLE_MAT2x3(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT2x3); }

  void UBO::register_GL_FLOAT_MAT2x4(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT2x4); }

  void UBO::register_GL_DOUBLE_MAT2x4(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT2x4); }

  void UBO::register_GL_FLOAT_MAT3x2(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT3x2); }

  void UBO::register_GL_DOUBLE_MAT3x2(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT3x2); }

  void UBO::register_GL_FLOAT_MAT3x4(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT3x4); }

  void UBO::register_GL_DOUBLE_MAT3x4(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT3x4); }

  void UBO::register_GL_FLOAT_MAT4x2(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT4x2); }

  void UBO::register_GL_DOUBLE_MAT4x2(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT4x2); }

  void UBO::register_GL_FLOAT_MAT4x3(std::string_view name)
  { _register_value(name, GL_FLOAT_MAT4x3); }

  void UBO::register_GL_DOUBLE_MAT4x3(std::string_view name)
  { _register_value(name, GL_DOUBLE_MAT4x3); }
  /// @}

  /// @{ -------------------------------------------------- UNREGISTER VALUE
  void UBO::unregister_value(std::string_view name)
  { _pdata->registered_values.erase(name.data()); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  bool UBO::init_from_registered_values_size()
  { return init(registered_values_size_in_bytes()); }
  /// @}

  /// @{ -------------------------------------------------- CLEAR
  void UBO::clear_registered_values()
  { _pdata->registered_values.clear(); }
  /// @}

  /// @{ -------------------------------------------------- WRITE REGISTERED VALUES
  bool UBO::write_registered_value(const std::string& name, const GLvoid* data)
  {
      auto it = _pdata->registered_values.find(name);

      if (it == _pdata->registered_values.end())
      { return false; }

      const RegisteredValue& r = it->second;
      return write_values(r.offset_in_bytes(), r.size_in_bytes(), data);
  }
  /// @}
} // namespace bk


