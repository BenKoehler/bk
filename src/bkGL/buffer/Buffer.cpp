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

#include <bkGL/buffer/Buffer.h>

#include <iostream>
#include <iterator>

namespace bk
{
  //====================================================================================================
  //===== ENUMS
  //====================================================================================================
  enum class BufferType : int
  {
      VERTEX_BUFFER_OBJECT = 0, INDEX_BUFFER_OBJECT = 1, SHADER_STORAGE_BUFFER_OBJECT = 2, UNIFORM_BUFFER_OBJECT = 3
  }; // enum BufferType

  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct Buffer::Impl
  {
      GLenum target;
      BufferType type;
      GLuint size_in_bytes;
      GLenum usage;
      GLuint buffer_base;
      bool is_bound_to_buffer_base;

      Impl()
          : target(GL_ARRAY_BUFFER),
            type(BufferType::VERTEX_BUFFER_OBJECT),
            size_in_bytes(0),
            usage(GL_STATIC_DRAW),
            buffer_base(0),
            is_bound_to_buffer_base(false)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  Buffer::Buffer()
      : base_type()
  #else

  Buffer::Buffer(bk::qt_gl_functions* gl)
      : base_type(gl)
  #endif
  { /* do nothing */ }

  Buffer::Buffer(self_type&&) noexcept = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  Buffer::~Buffer()
  { clear(); }
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS BOUND TO BUFFER BASE
  bool Buffer::is_bound_to_buffer_base() const
  { return _pdata->is_bound_to_buffer_base; }
  /// @}

  /// @{ -------------------------------------------------- GET TARGET
  GLenum Buffer::target() const
  { return _pdata->target; }
  /// @}

  /// @{ -------------------------------------------------- GET BUFFER TYPE
  bool Buffer::is_vertex_buffer_object() const
  { return _pdata->type == BufferType::VERTEX_BUFFER_OBJECT && _pdata->target == GL_ARRAY_BUFFER; }

  bool Buffer::is_index_buffer_object() const
  { return _pdata->type == BufferType::INDEX_BUFFER_OBJECT && _pdata->target == GL_ELEMENT_ARRAY_BUFFER; }

  bool Buffer::is_shader_storage_buffer_object() const
  { return _pdata->type == BufferType::SHADER_STORAGE_BUFFER_OBJECT && _pdata->target == GL_SHADER_STORAGE_BUFFER; }

  bool Buffer::is_uniform_buffer_object() const
  { return _pdata->type == BufferType::UNIFORM_BUFFER_OBJECT && _pdata->target == GL_UNIFORM_BUFFER; }
  /// @}

  /// @{ -------------------------------------------------- GET SIZE
  GLuint Buffer::size_in_bytes() const
  { return _pdata->size_in_bytes; }
  /// @}

  /// @{ -------------------------------------------------- GET USAGE
  GLenum Buffer::usage() const
  { return _pdata->usage; }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto Buffer::operator=(self_type&&) noexcept -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET VBO/IBO/UBO/SSBO
  void Buffer::set_default_config_vertex_buffer_object()
  {
      _pdata->type = BufferType::VERTEX_BUFFER_OBJECT;
      _pdata->target = GL_ARRAY_BUFFER;
  }

  void Buffer::set_default_config_index_buffer_object()
  {
      _pdata->type = BufferType::INDEX_BUFFER_OBJECT;
      _pdata->target = GL_ELEMENT_ARRAY_BUFFER;
  }

  void Buffer::set_default_config_shader_storage_buffer_object()
  {
      _pdata->type = BufferType::SHADER_STORAGE_BUFFER_OBJECT;
      _pdata->target = GL_SHADER_STORAGE_BUFFER;
  }

  void Buffer::set_default_config_uniform_buffer_object()
  {
      _pdata->type = BufferType::UNIFORM_BUFFER_OBJECT;
      _pdata->target = GL_UNIFORM_BUFFER;
  }
  /// @}

  /// @{ -------------------------------------------------- SET SIZE
  void Buffer::set_size_in_bytes(GLuint s)
  { _pdata->size_in_bytes = s; }
  /// @}

  /// @{ -------------------------------------------------- SET USAGE
  void Buffer::set_usage(GLenum usg)
  {
      if (usg == GL_STREAM_DRAW || usg == GL_STREAM_READ || usg == GL_STREAM_COPY || usg == GL_STATIC_DRAW || usg == GL_STATIC_READ || usg == GL_STATIC_COPY || usg == GL_DYNAMIC_DRAW || usg == GL_DYNAMIC_READ || usg == GL_DYNAMIC_COPY)
      { _pdata->usage = usg; }
  }

  void Buffer::set_usage_STREAM_DRAW()
  { set_usage(GL_STREAM_DRAW); }

  void Buffer::set_usage_STREAM_READ()
  { set_usage(GL_STREAM_READ); }

  void Buffer::set_usage_STREAM_COPY()
  { set_usage(GL_STREAM_COPY); }

  void Buffer::set_usage_STATIC_DRAW()
  { set_usage(GL_STATIC_DRAW); }

  void Buffer::set_usage_STATIC_READ()
  { set_usage(GL_STATIC_READ); }

  void Buffer::set_usage_STATIC_COPY()
  { set_usage(GL_STATIC_COPY); }

  void Buffer::set_usage_DYNAMIC_DRAW()
  { set_usage(GL_DYNAMIC_DRAW); }

  void Buffer::set_usage_DYNAMIC_READ()
  { set_usage(GL_DYNAMIC_READ); }

  void Buffer::set_usage_DYNAMIC_COPY()
  { set_usage(GL_DYNAMIC_COPY); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- CLEAR
  void Buffer::clear_impl()
  {
      if (_pdata->is_bound_to_buffer_base)
      { release_from_base(); }

      _pdata->buffer_base = 0;
      _pdata->is_bound_to_buffer_base = false;

      if (_id != 0)
      { BK_QT_GL glDeleteBuffers(1, &_id); }

      //clear_registered_values();
  }
  /// @}

  /// @{ -------------------------------------------------- (UN)BIND
  void Buffer::bind_impl()
  { BK_QT_GL glBindBuffer(_pdata->target, _id); }

  void Buffer::release_impl()
  { BK_QT_GL glBindBuffer(_pdata->target, 0); }

  bool Buffer::bind_to_base(GLuint bufferBaseID)
  {
      if (_pdata->is_bound_to_buffer_base)
      { release_from_base(); }

      _pdata->buffer_base = bufferBaseID;
      BK_QT_GL glBindBufferBase(_pdata->target, _pdata->buffer_base, _id);
      _pdata->is_bound_to_buffer_base = _id != 0;

      return _pdata->is_bound_to_buffer_base;
  }

  void Buffer::release_from_base()
  {
      BK_QT_GL glBindBufferBase(_pdata->target, _pdata->buffer_base, 0);
      _pdata->is_bound_to_buffer_base = false;
  }
  /// @}

  /// @{ -------------------------------------------------- INIT
  bool Buffer::init(const GLvoid* data, GLuint size)
  {
      clear();

      if (size != 0)
      { _pdata->size_in_bytes = size; }

      BK_QT_GL glGenBuffers(1, &_id);

      bind();

      GLenum err = BK_QT_GL glGetError(); // may contain old errors from previous operations -> erase them
      BK_QT_GL glBufferData(_pdata->target, static_cast<GLsizeiptr>(_pdata->size_in_bytes), data, _pdata->usage);
      err = BK_QT_GL glGetError();

      const bool success = err != GL_INVALID_OPERATION && err != GL_OUT_OF_MEMORY;

      if (success)
      { release(); }
      else
      { clear(); }

      return success;
  }

  bool Buffer::init(GLuint s)
  {
      if (s == 0)
      { return false; }

      set_size_in_bytes(s);

      return init(nullptr);
  }
  /// @}

  /// @{ -------------------------------------------------- UPDATE
  void Buffer::update_data(GLuint offset_in_bytes, GLuint size_in_bytes, const GLvoid* data)
  {
      bind();
      BK_QT_GL glBufferSubData(_pdata->target, static_cast<GLintptr>(offset_in_bytes), static_cast<GLsizeiptr>(size_in_bytes), data);
      release();
  }
  /// @}

  /// @{ -------------------------------------------------- (UN)MAP
  void Buffer::unmap()
  {
      if (!BK_QT_GL glUnmapBuffer(_pdata->target))
      { std::cerr << "glUnmapBuffer returned false!" << std::endl; }
  }

  void Buffer::unmap_and_release()
  {
      unmap();
      release();
  }
  /// @}

  /// @{ -------------------------------------------------- WRITE REGISTERED VALUES
  bool Buffer::write_values(GLuint offset, GLuint size, const GLvoid* data)
  {
      if (!this->is_initialized())
      { return false; }

      if (data == nullptr || offset + size > this->size_in_bytes())
      { return false; }

      if (!_is_bound)
      { bind(); }

      BK_QT_GL glBufferSubData(_pdata->target, offset, size, data);

      return true;
  }
  /// @}
} // namespace bk


