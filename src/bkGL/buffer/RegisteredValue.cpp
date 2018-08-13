#include "../../../include/bkGL/buffer/RegisteredValue.h"
#include "../../../include/bkGL/gl_type_traits.h"

#include "../../../include/bkTools/StringUtils"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct RegisteredValue::Impl
  {
      std::string name;
      GLenum type;
      unsigned int offset_in_bytes;

      Impl(std::string_view _name, GLenum _type, unsigned int _offset_in_bytes)
          : name(_name),
            type(_type),
            offset_in_bytes(_offset_in_bytes)
      { /* do nothing */ }

      Impl() = default;
      Impl(const Impl&) = default;
      Impl(Impl&&) = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = default;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  }; // RegisteredValue::Impl

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //==================================================================================================== 
  /// @{ -------------------------------------------------- CTOR
  RegisteredValue::RegisteredValue()
      : RegisteredValue("", GL_INT, 0)
  { /* do nothing */ }

  RegisteredValue::RegisteredValue(const self_type& other)
      : _pdata(std::make_unique<Impl>(*other._pdata.get()))
  { /* do nothing */ }

  RegisteredValue::RegisteredValue(self_type&&) = default;

  RegisteredValue::RegisteredValue(std::string_view name, GLenum type, unsigned int offset_in_bytes)
      : _pdata(std::make_unique<Impl>(name, type, offset_in_bytes))
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  RegisteredValue::~RegisteredValue() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET NAME
  const std::string& RegisteredValue::name() const
  { return _pdata->name; }
  /// @}

  /// @{ -------------------------------------------------- GET HASH
  [[nodiscard]] unsigned long long RegisteredValue::hash() const
  { return bk::string_utils::hash(_pdata->name); }
  /// @}

  /// @{ -------------------------------------------------- GET SIZE IN BYTES
  unsigned int RegisteredValue::size_in_bytes() const
  { return gl_size_in_bytes(_pdata->type); }
  /// @}

  /// @{ -------------------------------------------------- GET OFFSET IN BYTES
  unsigned int RegisteredValue::offset_in_bytes() const
  { return _pdata->offset_in_bytes; }
  /// @}

  /// @{ -------------------------------------------------- GET TYPE
  GLenum RegisteredValue::type() const
  { return _pdata->type; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //==================================================================================================== 
  /// @{ -------------------------------------------------- OPERATOR =
  auto RegisteredValue::operator=(const self_type& other) -> self_type&
  {
      _pdata.reset(nullptr);
      _pdata = std::make_unique<Impl>(*other._pdata.get());
      return *this;
  }

  auto RegisteredValue::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET NAME
  void RegisteredValue::set_name(std::string_view name)
  { _pdata->name = name; }
  /// @}

  /// @{ -------------------------------------------------- SET OFFSET IN BYTES
  void RegisteredValue::set_offset_in_bytes(unsigned int off)
  { _pdata->offset_in_bytes = off; }
  /// @}

  /// @{ -------------------------------------------------- SET TYPE
  void RegisteredValue::set_type(GLenum t)
  { _pdata->type = t; }
  /// @}
} // namespace bk