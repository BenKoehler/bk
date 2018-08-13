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
#include <cstdint>
#include <sstream>

#include "../../include/bkTools/Signal"
#include "../../include/bkTools/StringUtils"

#include "../include/bkGL/OrderIndependentTransparency.h"
#include "../include/bkGL/buffer/SSBO.h"
#include "../include/bkGL/renderable/AbstractRenderable.h"
#include "../include/bkGL/renderable/AbstractSceneRenderable.h"
#include "../include/bkGL/renderable/ScreenQuad.h"
#include "../include/bkGL/shader/Shader.h"
#include "../include/bkGL/shader/ShaderLibrary.h"

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  class OrderIndependentTransparency::Impl
  {
    public:
      SSBO ssbo_fragments;
      SSBO ssbo_linkedlist_startids;
      Shader shader_clear_linkedlist_startids;
      Shader shader_render;
      ScreenQuad screenquad;
      GLuint atomic_counter;
      GLuint max_fragments_per_pixel;
      GLuint num_drawn_pixels;
      unsigned int window_width;
      unsigned int window_height;
      bool enabled;
      bk::Signal<bool> s_enabled_changed;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else
      explicit Impl(bk::qt_gl_functions* gl)
          : ssbo_fragments(gl),
            ssbo_linkedlist_startids(gl),
            shader_clear_linkedlist_startids(gl),
            shader_render(gl),
            screenquad(gl),
          #endif
          atomic_counter(0),
          max_fragments_per_pixel(15),
          num_drawn_pixels(0),
          window_width(0),
          window_height(0),
          enabled(true)
      { /* do nothing */ }

      Impl(const Impl&) = delete;
      Impl(Impl&&) = default;

      ~Impl()
      { s_enabled_changed.disconnect_all(); }

      [[maybe_unused]] Impl& operator=(const Impl&) = delete;
      [[maybe_unused]] Impl& operator=(Impl&&) = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  /// @{ -------------------------------------------------- CTOR
  #ifndef BK_LIB_QT_AVAILABLE

  OrderIndependentTransparency::OrderIndependentTransparency()
      : base_type(),
        _pdata(std::make_unique<Impl>())
  #else
  OrderIndependentTransparency::OrderIndependentTransparency(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(std::make_unique<Impl>(gl))
  #endif
  { /* do nothing */ }

  OrderIndependentTransparency::OrderIndependentTransparency(self_type&&) = default;
  /// @}

  /// @{ -------------------------------------------------- DTOR
  OrderIndependentTransparency::~OrderIndependentTransparency() = default;
  /// @}

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- IS ENABLED
  bool OrderIndependentTransparency::is_enabled() const
  { return _pdata->enabled; }
  /// @}

  /// @{ -------------------------------------------------- GET WINDOW SIZE
  unsigned int OrderIndependentTransparency::window_width() const
  { return _pdata->window_width; }

  unsigned int OrderIndependentTransparency::window_height() const
  { return _pdata->window_height; }
  /// @}

  /// @{ -------------------------------------------------- GET MAX FRAGMENTS PER PIXEL
  unsigned int OrderIndependentTransparency::max_fragments_per_pixel() const
  { return _pdata->max_fragments_per_pixel; }
  /// @}

  /// @{ -------------------------------------------------- GET BUFFER
  const SSBO& OrderIndependentTransparency::ssbo_fragments() const
  { return _pdata->ssbo_fragments; }

  const SSBO& OrderIndependentTransparency::ssbo_linked_list() const
  { return _pdata->ssbo_linkedlist_startids; }
  /// @}

  /// @{ -------------------------------------------------- GET SIGNALS
  bk::Signal<bool>& OrderIndependentTransparency::signal_enabled_changed()
  { return _pdata->s_enabled_changed; }

  const bk::Signal<bool>& OrderIndependentTransparency::signal_enabled_changed() const
  { return _pdata->s_enabled_changed; }
  /// @}

  /// @{ -------------------------------------------------- GET SHADER TEXT
  std::string OrderIndependentTransparency::shader_definition_text()
  {
      std::stringstream ss;

      ss << "// atomic counter for pixels" << std::endl;
      ss << "layout(binding = " << bk::string_utils::from_number(buffer_base_atomic_counter()) << ", offset = 0) uniform atomic_uint Counter;" << std::endl << std::endl;

      ss << "// stored attributes incoming per value" << std::endl;
      ss << "struct FragmentVals" << std::endl;
      ss << "{" << std::endl;
      ss << "    uint col;" << std::endl;
      ss << "    uint depth;" << std::endl;
      ss << "    uint next;" << std::endl;
      ss << "};" << std::endl << std::endl;

      ss << "// buffer to store all values (color, depth, next id) of all pixels" << std::endl;
      ss << "layout(binding = " << bk::string_utils::from_number(buffer_base_ssbo_fragments()) << ", std430) buffer _Fragment" << std::endl;
      ss << "{" << std::endl;
      ss << "    FragmentVals Fragment[];" << std::endl;
      ss << "};" << std::endl << std::endl;

      ss << "// buffer with id to latest FragmentVals-entry for each pixel;" << std::endl;
      ss << "// from this entry, all previously stored values can be determined using the next id;" << std::endl;
      ss << "// next id is 0xFFFFFF, if there is no successor" << std::endl;
      ss << "layout(binding = " << bk::string_utils::from_number(buffer_base_ssbo_linkedlist()) << ", std430) buffer _StartID" << std::endl;
      ss << "{" << std::endl;
      ss << "    uint StartID[];" << std::endl;
      ss << "};" << std::endl << std::endl;

      ss << "// transform image coordinate (x,y) to 1D list id" << std::endl;
      ss << "uint gridToListID(ivec2 xy)" << std::endl;
      ss << "{" << std::endl;
      ss << "    return xy.x*window_height + xy.y;" << std::endl;
      ss << "}" << std::endl << std::endl;

      return ss.str();
  }

  std::string OrderIndependentTransparency::shader_main_text()
  {
      std::stringstream ss;

      ss << "if (fragColor.a < " << bk::string_utils::from_number(minimum_valid_opacity()) << "f)" << std::endl;
      ss << "{ discard; }" << std::endl << std::endl;

      ss << "// fragmentCounter; synchronized between all threads" << std::endl;
      ss << "const uint fragmentCount = atomicCounterIncrement(Counter);" << std::endl;
      ss << "if (fragmentCount < oit_max_fragments_per_pixel*window_width*window_height)" << std::endl;
      ss << "{" << std::endl;
      ss << "    // color and depth are stored as unsigned int values" << std::endl;
      ss << "    Fragment[fragmentCount].col = packUnorm4x8(fragColor);" << std::endl;
      ss << "    Fragment[fragmentCount].depth = floatBitsToUint(gl_FragCoord.z);" << std::endl;
      ss << "    Fragment[fragmentCount].next = atomicExchange(StartID[gridToListID(ivec2(gl_FragCoord.xy))], fragmentCount);" << std::endl;
      ss << "}" << std::endl << std::endl;

      return ss.str();
  }
  /// @}

  /// @{ -------------------------------------------------- IS INITIALIZED
  bool OrderIndependentTransparency::is_initialized() const
  { return _pdata->ssbo_fragments.is_initialized() && _pdata->ssbo_linkedlist_startids.is_initialized() && _pdata->shader_clear_linkedlist_startids.is_initialized() && _pdata->shader_render.is_initialized(); }
  /// @}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  /// @{ -------------------------------------------------- OPERATOR =
  auto OrderIndependentTransparency::operator=(self_type&&) -> self_type& = default;
  /// @}

  /// @{ -------------------------------------------------- SET ENABLED
  void OrderIndependentTransparency::set_enabled(bool b)
  {
      if (b != _pdata->enabled)
      {
          _pdata->enabled = b;
          _pdata->screenquad.set_oit_available(_pdata->enabled);

          if (_pdata->enabled)
          { init(); }
          else
          { clear(); }

          _pdata->s_enabled_changed.emit_signal(_pdata->enabled);
      }
  }

  void OrderIndependentTransparency::enable()
  { set_enabled(true); }

  void OrderIndependentTransparency::disable()
  { set_enabled(false); }
  /// @}

  /// @{ -------------------------------------------------- SET WINDOW SIZE
  void OrderIndependentTransparency::set_window_width(unsigned int width)
  { _pdata->window_width = width; }

  void OrderIndependentTransparency::set_window_height(unsigned int height)
  { _pdata->window_height = height; }

  void OrderIndependentTransparency::set_window_size(unsigned int width, unsigned int height)
  {
      set_window_width(width);
      set_window_height(height);
  }
  /// @}

  /// @{ -------------------------------------------------- SET MAX FRAGMENTS PER PIXEL
  void OrderIndependentTransparency::set_max_fragments_per_pixel(unsigned int n)
  { _pdata->max_fragments_per_pixel = std::max(n, 1U); }
  /// @}

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  /// @{ -------------------------------------------------- INIT
  void OrderIndependentTransparency::init()
  {
      clear();

      _pdata->screenquad.init();

      // atomic counter as pixel-count-variable
      BK_QT_GL glGenBuffers(1, &_pdata->atomic_counter);
      BK_QT_GL glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _pdata->atomic_counter);
      BK_QT_GL glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
      BK_QT_GL glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

      const GLuint nPixels = _pdata->window_width * _pdata->window_height;

      const std::uint64_t startids_size_in_byte = nPixels * sizeof(GLuint);
      _pdata->ssbo_linkedlist_startids.set_usage_DYNAMIC_DRAW();
      _pdata->ssbo_linkedlist_startids.init(startids_size_in_byte);

      //const std::uint64_t fragments_size_in_byte = 3 /*col,depth,nextid*/ * _pdata->max_fragments_per_pixel * nPixels * sizeof(GLuint) / (_pdata->ssaa_fac * _pdata->ssaa_fac);
      const std::uint64_t fragments_size_in_byte = 3 /*col,depth,nextid*/ * _pdata->max_fragments_per_pixel * nPixels * sizeof(GLuint);
      _pdata->ssbo_fragments.set_usage_DYNAMIC_DRAW();
      _pdata->ssbo_fragments.init(fragments_size_in_byte);

      using SL = details::ShaderLibrary;
      _pdata->shader_clear_linkedlist_startids.init_from_sources(SL::oit::vert(), SL::oit::frag_clear_linked_list());
      _pdata->shader_render.init_from_sources(SL::oit::vert(), SL::oit::frag_render());
  }

  void OrderIndependentTransparency::init_if_enabled()
  {
      if (_pdata->enabled)
      { init(); }
  }
  /// @}

  /// @{ -------------------------------------------------- CLEAR
  void OrderIndependentTransparency::clear()
  {
      _pdata->screenquad.clear();

      _pdata->ssbo_fragments.clear();
      _pdata->ssbo_linkedlist_startids.clear();

      BK_QT_GL glDeleteBuffers(1, &_pdata->atomic_counter);
      _pdata->atomic_counter = 0;

      _pdata->shader_clear_linkedlist_startids.clear();
      _pdata->shader_render.clear();
  }
  /// @}

  /// @{ -------------------------------------------------- CONNECT SIGNALS
  void OrderIndependentTransparency::connect_signals(std::shared_ptr<details::AbstractRenderable>& r)
  {
      _pdata->s_enabled_changed.connect([=](bool enabled)
                                        { r->set_oit_available(enabled); });
  }

  void OrderIndependentTransparency::connect_signals(std::shared_ptr<details::AbstractSceneRenderable>& r)
  {
      _pdata->s_enabled_changed.connect([=](bool enabled)
                                        { r->set_oit_available(enabled); });
  }
  /// @}

  /// @{ -------------------------------------------------- GL BLEND FUNCTION
  void OrderIndependentTransparency::blendFunc()
  {
      BK_QT_GL glEnable(GL_BLEND);

      if (_pdata->enabled)
      { BK_QT_GL glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); }
      else
      { BK_QT_GL glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
  }
  /// @}

  /// @{ -------------------------------------------------- ON RESIZE
  void OrderIndependentTransparency::on_resize(unsigned int width, unsigned int height)
  {
      set_window_size(width, height);

      if (_pdata->enabled)
      { init(); }
  }
  /// @}

  /// @{ -------------------------------------------------- DRAW
  void OrderIndependentTransparency::draw_init()
  {
      if (_pdata->enabled)
      {
          // reset the atomic counter variable to 0
          static constexpr const GLuint zero = 0;
          BK_QT_GL glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, _pdata->atomic_counter);
          BK_QT_GL glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_DRAW);

          BK_QT_GL glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, buffer_base_atomic_counter(), _pdata->atomic_counter);
          _pdata->ssbo_fragments.bind_to_base(buffer_base_ssbo_fragments());
          _pdata->ssbo_linkedlist_startids.bind_to_base(buffer_base_ssbo_linkedlist());

          // clear the indices to the last values;
          // the shader assigns 0xFFFFFF as pseudo-nullptr to each entry
          BK_QT_GL glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
          BK_QT_GL glDepthMask(GL_FALSE);

          _pdata->shader_clear_linkedlist_startids.bind();
          _pdata->screenquad.draw();
          _pdata->shader_clear_linkedlist_startids.release();

          BK_QT_GL glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
          BK_QT_GL glDepthMask(GL_TRUE);
      }
  }

  void OrderIndependentTransparency::draw_finish()
  {
      if (_pdata->enabled)
      {
          BK_QT_GL glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), static_cast<GLvoid*>(&_pdata->num_drawn_pixels));

          if (_pdata->num_drawn_pixels != 0)
          {
              _pdata->shader_render.bind();
              _pdata->screenquad.draw();
              _pdata->shader_render.release();
          }

          BK_QT_GL glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, buffer_base_atomic_counter(), 0);
          _pdata->ssbo_fragments.release_from_base();
          _pdata->ssbo_linkedlist_startids.release_from_base();
      }
  }

  /// @}
} // namespace bk