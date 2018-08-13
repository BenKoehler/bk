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

#include "../../../include/bkGL/buffer/UBOSpecialization.h"

namespace bk::details
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct UBOSpecialization::Impl
  {
      std::string bufferName;
      unsigned int bufferBaseId;

      Impl() = default;
      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;

      Impl(std::string_view _bufferName, unsigned int _bufferBaseId)
          : bufferName(_bufferName),
            bufferBaseId(_bufferBaseId)
      { /* do nothing */ }

      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  }; // UBOSpecialization::Impl

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE
  UBOSpecialization::UBOSpecialization()
      : UBOSpecialization("ubo", 0)
  #else
  UBOSpecialization::UBOSpecialization(bk::qt_gl_functions* gl)
      : UBOSpecialization(gl, "ubo", 0)
  #endif
  { /* do nothing */ }

  UBOSpecialization::UBOSpecialization(self_type&&) = default;

  #ifndef BK_LIB_QT_AVAILABLE
  UBOSpecialization::UBOSpecialization(std::string_view bufferName, unsigned int bufferBaseId)
      : base_type(),
  #else
  UBOSpecialization::UBOSpecialization(bk::qt_gl_functions* gl, std::string_view bufferName, unsigned int bufferBaseId)
      : base_type(gl),
  #endif
        _pdata(std::make_unique<Impl>(bufferName, bufferBaseId))
  { /* do nothing */ }
  /// @}

  /// @{ -------------------------------------------------- DTOR
  UBOSpecialization::~UBOSpecialization() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- GET BUFFER NAME
  std::string UBOSpecialization::buffer_name() const
  { return _pdata->bufferName; }
  /// @}

  /// @{ -------------------------------------------------- GET BUFFER BASE
  unsigned int UBOSpecialization::buffer_base() const
  { return _pdata->bufferBaseId; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto UBOSpecialization::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET BUFFER NAME
  void UBOSpecialization::set_buffer_name(std::string_view bufferName)
  { _pdata->bufferName = bufferName; }
  /// @}

  /// @{ -------------------------------------------------- SET BUFFER BASE
  void UBOSpecialization::set_buffer_base(unsigned int bufferBaseId) const
  { _pdata->bufferBaseId = bufferBaseId; }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- BIND TO DEFAULT BASE
  bool UBOSpecialization::bind_to_default_base()
  { return this->bind_to_base(buffer_base()); }
  /// @}
} // namespace bk::details