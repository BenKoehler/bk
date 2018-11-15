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

#include <bkGL/shader/ShaderLibrary.h>

#include <sstream>
#include <vector>

#include <bkGL/buffer/RegisteredValue.h>
#include <bkGL/buffer/UBO.h>
#include <bkGL/buffer/UBOSpecialization.h>
#include <bkGL/OrderIndependentTransparency.h>
#include <bkGL/UBODVR.h>
#include <bkGL/UBOGlobal.h>
#include <bkGL/UBOLine.h>
#include <bkGL/UBOPlotArea.h>
#include <bkGL/UBOPlot.h>
#include <bkGL/UBOPlotLine.h>
#include <bkGL/UBOPhong.h>
#include <bkGL/UBOSelectionSphere.h>
#include <bkGL/UBOSliceView.h>
#include <bkGL/UBOText.h>

namespace bk::details
{
  /// @{ -------------------------------------------------- COMMENTS
  std::string ShaderLibrary::comment_tag_shader(std::string_view type, std::string_view name)
  {
      std::stringstream s;
      s << "// " << type << " SHADER (" << name << ")\n";
      s << "//---------------------------------------------------\n";
      return s.str();
  }

  std::string ShaderLibrary::comment_tag_vertex_shader(std::string_view name)
  { return comment_tag_shader("VERTEX", name); }

  std::string ShaderLibrary::comment_tag_fragment_shader(std::string_view name)
  { return comment_tag_shader("FRAGMENT", name); }

  std::string ShaderLibrary::comment_tag_geometry_shader(std::string_view name)
  { return comment_tag_shader("GEOMETRY", name); }

  std::string ShaderLibrary::comment_tag_compute_shader(std::string_view name)
  { return comment_tag_shader("COMPUTE", name); }

  std::string ShaderLibrary::comment_region(std::string_view name)
  {
      std::stringstream s;

      s << "\n//---------------------------------------------------\n";
      s << "// " << name;
      s << "\n//---------------------------------------------------\n";

      return s.str();
  }

  std::string ShaderLibrary::comment_region_input()
  { return comment_region("input"); }

  std::string ShaderLibrary::comment_region_output()
  { return comment_region("output"); }

  std::string ShaderLibrary::comment_region_functions()
  { return comment_region("functions"); }
  /// @}

  /// @{ -------------------------------------------------- VERSION
  std::string ShaderLibrary::version()
  { return "#version 430\n"; }
  /// @}

  /// @{ -------------------------------------------------- DISCARD ALPHA
  std::string ShaderLibrary::alpha_discard_thresh()
  { return "0.025f"; }

  std::string ShaderLibrary::discard_low_alpha()
  {
      std::stringstream s;

      s << "\nif (color_out.a < " << alpha_discard_thresh() << ")\n";
      s << "{ discard; }\n\n";

      return s.str();
  }

  std::string ShaderLibrary::discard_zero_alpha()
  {
      std::stringstream s;

      s << "if (color_out.a == 0)\n";
      s << "{ discard; }\n\n";

      return s.str();
  }

  std::string ShaderLibrary::early_fragment_test()
  {
      std::stringstream s;

      s << "// early depth test -> make sure that only visible fragments are stored\n";
      s << "layout(early_fragment_tests) in;\n\n";

      return s.str();
  }
  /// @}

  /// @{ -------------------------------------------------- FUNCTION MAIN
  std::string ShaderLibrary::function_main_begin()
  { return "void main()\n{\n"; }

  std::string ShaderLibrary::function_main_end()
  { return "} // main()\n"; }
  /// @}

  /// @{ -------------------------------------------------- SPECIAL FUNCTIONS
  std::string ShaderLibrary::function_grid_to_list_id()
  {
      std::stringstream s;

      s << "// transform image coordinate (x,y) to list id\n";
      s << "uint grid_to_list_id(ivec2 xy)\n";
      s << "{ return xy.x * " << bk::details::UBOGlobal::name_window_height() << " + xy.y; }\n\n";

      return s.str();
  }

  std::string ShaderLibrary::function_camera_position()
  {
      std::stringstream s;

      s << "vec3 camera_position()\n";
      s << "{\n";
      s << "   const vec4 temp = inverse(" << bk::details::UBOGlobal::name_modelview_matrix() << ") * vec4(0,0,0,1);\n";
      s << "   return temp.xyz / temp.w;\n";
      s << "}\n\n";

      //s << "{ return xy.x * " << bk::details::UBOGlobal::name_window_width() << " + xy.y; }\n\n";

      return s.str();
  }

  std::string ShaderLibrary::function_linearize_depth()
  {
      const std::string znear = UBOGlobal::name_cam_znear();
      const std::string zfar = UBOGlobal::name_cam_zfar();

      std::stringstream s;

      s << "float linearize_depth(float depth)\n";
      s << "{ return (" << znear << " * " << zfar << ") / (" << zfar << " - depth*(" << zfar << " + " << znear << ")); }\n\n";

      s << "float linearize_depth()\n";
      s << "{ return linearize_depth(gl_FragCoord.z); }\n\n";

      return s.str();
  }

  std::string ShaderLibrary::function_plot_data_to_screen()
  {
      const std::string hb = std::string("2 * ") + UBOPlot::name_border_width_x_in_percent();
      const std::string vb = std::string("2 * ") + UBOPlot::name_border_width_y_in_percent();
      const std::string xmin = UBOPlot::name_xmin();
      const std::string xmax = UBOPlot::name_xmax();
      const std::string ymin = UBOPlot::name_ymin();
      const std::string ymax = UBOPlot::name_ymax();

      std::stringstream s;

      s << "vec2 plot_data_to_screen(vec2 xy)\n";
      s << "{\n";
      s << "   return vec2(\n";
      s << "      (-1 + " << hb << ") + (2 - " << hb << " - " << hb << "/4) * (xy[0] - " << xmin << ") / (" << xmax << " - " << xmin << "),\n";
      s << "      (-1 + " << vb << ") + (2 - " << vb << " - " << vb << "/4) * (xy[1] - " << ymin << ") / (" << ymax << " - " << ymin << ")\n";
      s << "   );\n";
      s << "}\n\n";

      return s.str();
  }
  /// @}

  /// @{ -------------------------------------------------- UBO
  std::string ShaderLibrary::ubo_definition(const UBO& ubo, std::string_view bufferName, unsigned int bufferBase)
  {
      const std::vector<RegisteredValue> rv = ubo.registered_values();

      std::stringstream s;

      s << "layout(binding = " << bufferBase << ", std140) uniform " << bufferName << std::endl;
      s << "{" << std::endl;

      for (const RegisteredValue& r: rv)
      { s << "   " << details::glenum_to_glsl_type_name(r.type()) << " " << r.name() << ";" << std::endl; }

      s << "};" << std::endl;
      s << std::endl;

      return s.str();
  }

  std::string ShaderLibrary::ubo_definition(const UBOSpecialization& ubo)
  { return ubo_definition(ubo, ubo.buffer_name(), ubo.buffer_base()); }

  std::string ShaderLibrary::ubo_definition_dvr()
  {
      return ubo_definition(bk::details::UBODVR(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_global()
  {
      return ubo_definition(bk::details::UBOGlobal(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_plot()
  {
      return ubo_definition(bk::details::UBOPlot(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_plot_area()
  {
      return ubo_definition(bk::details::UBOPlotArea(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_line()
  {
      return ubo_definition(bk::details::UBOLine(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_plot_line()
  {
      return ubo_definition(bk::details::UBOPlotLine(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_phong()
  {
      return ubo_definition(bk::details::UBOPhong(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_selection_sphere()
  {
      return ubo_definition(bk::details::UBOSelectionSphere(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_sliceview()
  {
      return ubo_definition(bk::details::UBOSliceView(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }

  std::string ShaderLibrary::ubo_definition_text()
  {
      return ubo_definition(bk::details::UBOText(
      #ifdef BK_LIB_QT_AVAILABLE
          nullptr
      #endif
      ));
  }
  /// @}

  /// @{ -------------------------------------------------- OIT
  std::string ShaderLibrary::oit_definition_atomic_counter()
  {
      std::stringstream s;
      s << "// atomic pixel counter\n";
      s << "layout(binding = " << OrderIndependentTransparency::buffer_base_atomic_counter() << ", offset = 0) uniform atomic_uint " << OrderIndependentTransparency::buffer_name_atomic_counter() << ";\n\n";
      return s.str();
  }

  std::string ShaderLibrary::oit_definition_fragments()
  {
      std::stringstream s;

      s << "// incoming values per pixel\n";
      s << "struct FragmentVals\n";
      s << "{\n";
      s << "   uint col;\n";
      s << "   uint depth;\n";
      s << "   uint next;\n";
      s << "};\n\n";

      s << "// buffer to store all values (color, depth, next id) of all pixels\n";
      s << "layout(binding = " << OrderIndependentTransparency::buffer_base_ssbo_fragments() << ", std430) buffer _" << OrderIndependentTransparency::buffer_name_ssbo_fragments() << "\n";
      s << "{ FragmentVals " << OrderIndependentTransparency::buffer_name_ssbo_fragments() << "[]; };\n\n";

      return s.str();
  }

  std::string ShaderLibrary::oit_definition_linked_list()
  {
      std::stringstream s;

      s << "// - ids of latest FragmentVals per pixel\n";
      s << "// - from this, all previously stored values can be determined using the next id\n";
      s << "// - next id is 0xFFFFFF if there is no successor\n";
      s << "layout(binding = " << OrderIndependentTransparency::buffer_base_ssbo_linkedlist() << ", std430) buffer _" << OrderIndependentTransparency::buffer_name_ssbo_linkedlist() << "\n";
      s << "{ uint " << OrderIndependentTransparency::buffer_name_ssbo_linkedlist() << "[]; };\n\n";

      return s.str();
  }

  std::string ShaderLibrary::oit_definition_max_fragments()
  {
      std::stringstream s;

      s << "#define " << oit_var_max_fragments() << " 15\n";

      return s.str();
  }

  std::string ShaderLibrary::oit_definition()
  {
      std::stringstream s;

      s << oit_definition_atomic_counter();
      s << oit_definition_fragments();
      s << oit_definition_linked_list();

      return s.str();
  }

  std::string ShaderLibrary::oit_var_max_fragments()
  { return "oit_max_fragments_per_pixel"; }

  std::string ShaderLibrary::oit_assign_from_color_out()
  {
      std::stringstream s;

      s << "\n";
      s << "   // fragmentCounter; synchronized between all threads\n";
      s << "   const uint fragmentCount = atomicCounterIncrement(" << OrderIndependentTransparency::buffer_name_atomic_counter() << ");\n";
      s << "   if (fragmentCount < " << oit_var_max_fragments() << "*" << bk::details::UBOGlobal::name_window_width() << "*" << bk::details::UBOGlobal::name_window_height() << ")\n";
      s << "   {\n";
      s << "       // color and depth are stored as unsigned int values\n";
      s << "       " << OrderIndependentTransparency::buffer_name_ssbo_fragments() << "[fragmentCount].col = packUnorm4x8(color_out);\n";
      s << "       " << OrderIndependentTransparency::buffer_name_ssbo_fragments() << "[fragmentCount].depth = floatBitsToUint(gl_FragCoord.z);\n";
      s << "       " << OrderIndependentTransparency::buffer_name_ssbo_fragments() << "[fragmentCount].next = atomicExchange(" << OrderIndependentTransparency::buffer_name_ssbo_linkedlist() << "   [grid_to_list_id(ivec2(gl_FragCoord.xy))], fragmentCount);\n";
      s << "   }\n\n";

      return s.str();
  }
  /// @}

  /// @{ -------------------------------------------------- GEOM LAYOUTS IN
  std::string ShaderLibrary::geom_layout_in_points()
  { return "\nlayout(points) in;\n"; }

  std::string ShaderLibrary::geom_layout_in_lines()
  { return "\nlayout(lines) in;\n"; }

  std::string ShaderLibrary::geom_layout_in_lines_adjacency()
  { return "\nlayout(lines_adjacency) in;\n"; }

  std::string ShaderLibrary::geom_layout_in_triangles()
  { return "\nlayout(triangles) in;\n"; }

  std::string ShaderLibrary::geom_layout_in_triangles_adjacency()
  { return "\nlayout(triangles_adjacency) in;\n"; }
  /// @}

  /// @{ -------------------------------------------------- GEOM LAYOUTS OUT
  std::string ShaderLibrary::geom_layout_out_points(unsigned int maxNumVertices)
  {
      std::stringstream s;

      s << "\nlayout(points, max_vertices = " << maxNumVertices << ") out;\n";

      return s.str();
  }

  std::string ShaderLibrary::geom_layout_out_line_strip(unsigned int maxNumVertices)
  {
      std::stringstream s;

      s << "\nlayout(line_strip, max_vertices = " << maxNumVertices << ") out;\n";

      return s.str();
  }

  std::string ShaderLibrary::geom_layout_out_triangle_strip(unsigned int maxNumVertices)
  {
      std::stringstream s;

      s << "\nlayout(triangle_strip, max_vertices = " << maxNumVertices << ") out;\n";

      return s.str();
  }
  /// @}

  /// @{ -------------------------------------------------- SEGMENTATION
  std::string ShaderLibrary::segmentation_bit_macros()
  {
      std::stringstream s;

      s << "#define " << segmentation_name_segmentation_bit() << " uint(1)\n";
      s << "#define " << segmentation_name_inside_bit() << " uint(2)\n";
      s << "#define " << segmentation_name_outside_bit() << " uint(4)\n\n";

      return s.str();
  }

  std::string ShaderLibrary::segmentation_name_inside_bit()
  { return "InsideBit"; }

  std::string ShaderLibrary::segmentation_name_outside_bit()
  { return "OutsideBit"; }

  std::string ShaderLibrary::segmentation_name_segmentation_bit()
  { return "SegmentationBit"; }
  /// @}

  //====================================================================================================
  //===== TEXTURE
  //====================================================================================================
  std::string ShaderLibrary::render_texture::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("TEXTURE");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec2 texcoord_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec2 texcoord_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   texcoord_frag = texcoord_in;\n";
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::render_texture::frag(unsigned int texture_unit_id, bool useAlpha)
  {
      std::stringstream s;

      {
          std::stringstream stype;
          stype << "TEXTURE";
          stype << "useAlpha:" << (useAlpha ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 texcoord_frag;\n";
      s << "layout(binding = " << texture_unit_id << ") uniform sampler2D tex;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      if (useAlpha)
      { s << "   color_out = texture(tex, texcoord_frag);\n"; }
      else
      {
          s << "   color_out.rgb = texture(tex, texcoord_frag).rgb;\n";
          s << "   color_out.a = 1;\n";
      }

      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== BACKGROUND
  //====================================================================================================
  std::string ShaderLibrary::background::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("BACKGROUND");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec4 color_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_frag = color_in;\n";
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::background::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("BACKGROUND");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec4 color_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = color_frag;\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== OIT
  //====================================================================================================
  std::string ShaderLibrary::oit::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("OIT");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec2 texcoord_in; //unused\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::oit::frag_clear_linked_list()
  {
      const std::string oitlist = OrderIndependentTransparency::buffer_name_ssbo_linkedlist();

      std::stringstream s;

      s << comment_tag_fragment_shader("OIT CLEAR LINKED LIST");
      s << version();

      s << comment_region_input();
      s << ubo_definition_global();

      s << comment_region_output();
      s << oit_definition_linked_list();

      s << comment_region_functions();
      s << function_grid_to_list_id();
      s << function_main_begin();
      s << "   // reset id of the last stored values;\n";
      s << "   // 0xFFFFFF serves as pseudo-nullptr\n";
      s << "   " << oitlist << "[grid_to_list_id(ivec2(gl_FragCoord.xy))] = 0xFFFFFF;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::oit::frag_render()
  {
      const std::string oitlist = OrderIndependentTransparency::buffer_name_ssbo_linkedlist();
      const std::string oitfrag = OrderIndependentTransparency::buffer_name_ssbo_fragments();

      std::stringstream s;

      s << comment_tag_fragment_shader("OIT RENDER");
      s << version();
      s << oit_definition_max_fragments();

      s << comment_region_input();
      s << ubo_definition_global();
      s << oit_definition_fragments();
      s << oit_definition_linked_list();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_grid_to_list_id();
      s << function_main_begin();
      s << "   color_out = vec4(0,0,0,0);\n\n";

      s << "   // are fragment values saved in current pixel?\n";
      s << "   uint id = " << oitlist << "[grid_to_list_id(ivec2(gl_FragCoord.xy))];\n\n";

      s << "   if (id == 0xFFFFFF) // empty pixel?\n";
      s << "   { discard; }\n\n";

      s << "   //fetch stored fragment values from buffer\n";
      s << "   uint numFragments = 0; // count entries\n";
      s << "   FragmentVals currentFragVals[" << oit_var_max_fragments() << "];\n\n";

      s << "   // while hasNext && space available\n";
      s << "   while (id != 0xFFFFFF && numFragments < " << oit_var_max_fragments() << ")\n";
      s << "   {\n";
      s << "      currentFragVals[numFragments] = " << oitfrag << "[id];\n";
      s << "      id = currentFragVals[numFragments].next;\n";
      s << "      ++numFragments;\n";
      s << "   }\n\n";

      s << "   //sort fragments by depth via insertion sort\n";
      s << "   for (int i = 1; i < numFragments; ++i)\n";
      s << "   {\n";
      s << "      const FragmentVals temp = currentFragVals[i];\n";
      s << "      int j = i-1;\n\n";

      s << "      while (j >= 0 && currentFragVals[j].depth < temp.depth)\n";
      s << "      {\n";
      s << "         currentFragVals[j+1] = currentFragVals[j];\n";
      s << "         --j;\n";
      s << "      }\n\n";

      s << "      currentFragVals[j+1] = temp;\n";
      s << "   }\n\n";

      s << "   //render the fragments\n\n";

      s << "   color_out.a = 1;\n\n";

      s << "   // back to front (max to min depth)\n";
      s << "   for (uint i = 0; i < numFragments; ++i)\n";
      s << "   {\n";
      s << "      // restore rgba\n";
      s << "      const vec4 col = unpackUnorm4x8(currentFragVals[i].col);\n\n";

      s << "      // blend colors\n";
      s << "      color_out.rgb = mix(color_out.rgb, col.rgb, col.a);\n\n";

      s << "      // sum up total alpha\n";
      s << "      color_out.a *= (1-col.a);\n";
      s << "   }\n\n";

      s << "   color_out.a = 1 - color_out.a;\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== SSAA
  //====================================================================================================
  std::string ShaderLibrary::ssaa::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("SSAA");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec2 texcoord_in; //unused\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::ssaa::frag()
  {
      constexpr const char* ssaa_fac = bk::details::UBOGlobal::name_ssaa_factor();

      std::stringstream s;

      s << comment_tag_fragment_shader("SSAA");
      s << version();

      s << comment_region_input();
      s << ubo_definition_global();
      s << "layout(binding = 1) uniform sampler2D color_tex;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_grid_to_list_id();
      s << function_main_begin();
      s << "   const ivec2 screenpos = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));\n\n";

      s << "   color_out = vec4(0);\n\n";

      s << "   for (int dx = " << ssaa_fac << "*screenpos.x; dx < " << ssaa_fac << "*screenpos.x + " << ssaa_fac << "; ++dx)\n";
      s << "   {\n";
      s << "      for (int dy = " << ssaa_fac << "*screenpos.y; dy < " << ssaa_fac << "*screenpos.y + " << ssaa_fac << "; ++dy)\n";
      s << "      { color_out += texelFetch(color_tex, ivec2(dx,dy), 0); }\n";
      s << "   }\n\n";

      s << "    color_out /= (" << ssaa_fac << "*" << ssaa_fac << ");";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== MESH
  //====================================================================================================
  //------------------------------------------------------------------------------------------------------
  // phong
  //------------------------------------------------------------------------------------------------------
  std::string ShaderLibrary::mesh::phong::details::default_frag(bool assignAmbient, bool assignDiffuse, bool assignSpecular, bool defineColorVec3)
  {
      std::stringstream s;

      s << "   const vec3 camPos = camera_position();\n";
      s << "   const vec3 E = normalize(position_frag - camPos);\n";
      s << "   const vec3 P = position_frag; // world coordinates\n";
      s << "   const vec3 L = -E; // // headlight\n";
      s << "   const vec3 N = normalize(normal_frag);\n";
      s << "   const vec3 R = normalize(reflect(L, N)); // for specular\n\n";

      if (defineColorVec3)
      { s << "   vec3 color = vec3(" << bk::details::UBOPhong::name_meshcol_r() << ", " << bk::details::UBOPhong::name_meshcol_g() << ", " << bk::details::UBOPhong::name_meshcol_b() << ");\n\n"; }

      s << "   // alpha\n";
      s << "   color_out.a = 1;\n\n";

      if (assignAmbient)
      {
          s << "   // ambient\n";
          s << "   color_out.rgb = 0.1 * color;\n\n";
      }

      if (assignDiffuse)
      {
          s << "   // diffuse\n";
          s << "   const float NdotL = dot(N, L);\n";
          s << "   color_out.rgb += abs(NdotL) * (NdotL >= 0 ? 1.0f : 0.75f) * color;\n\n";
      }

      if (assignSpecular)
      {
          s << "   // specular\n";
          s << "   const vec3 light_color = vec3(" << bk::details::UBOPhong::name_lightcol_r() << ", " << bk::details::UBOPhong::name_lightcol_g() << ", " << bk::details::UBOPhong::name_lightcol_b() << ");\n";
          s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << bk::details::UBOPhong::name_shininess() << ");\n";
      }

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::details::default_frag_ghosted()
  {
      std::stringstream s;

      s << "   if (NdotL >= 0) // front side -> ghosted view\n";
      s << "   {\n";
      s << "       const float cosE = abs(dot(N,E));\n\n";

      s << "       color_out.a = max(0, color_out.a - pow(cosE, " << bk::details::UBOPhong::name_ghostFalloff() << "));\n\n";

      s << "       if (" << bk::details::UBOPhong::name_ghostCutoff() << " < 1)\n";
      s << "       { color_out.a = max(0, color_out.a - " << bk::details::UBOPhong::name_ghostCutoff() << ") / (1 - " << bk::details::UBOPhong::name_ghostCutoff() << "); }\n\n";

      s << "   }\n";

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::details::default_frag_get_color_from_attribute(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << "   vec3 color = vec3(0);\n\n";

      s << "   if (color_enabled != 0)\n";
      s << "   {\n";
      if (!timeDependentAttribute)
      { s << "       const float attribVal = attrib_t0_frag;\n\n"; }
      else
      {
          s << "       const float t = " << bk::details::UBOGlobal::name_animation_current_time() << " / " << bk::details::UBOPhong::name_temporal_resolution() << ";\n";
          s << "       const float t0 = floor(t);\n";
          s << "       const float tw = t - t0;\n";
          s << "       const float attribVal = mix(attrib_t0_frag, attrib_t1_frag, tw);\n\n";
      }

      s << "       if (attribVal <= " << bk::details::UBOPhong::name_min_attribute_value() << ")\n";
      s << "       { color = ColorBar[0]; }\n";
      s << "       else if (attribVal >= " << bk::details::UBOPhong::name_max_attribute_value() << ")\n";
      s << "       { color = ColorBar[" << bk::details::UBOPhong::name_num_colors() << "-1]; }\n";
      s << "       else\n";
      s << "       {\n";
      s << "           const float temp = (" << bk::details::UBOPhong::name_num_colors() << " - 1) *(attribVal - " << bk::details::UBOPhong::name_min_attribute_value() << ") / (" << bk::details::UBOPhong::name_max_attribute_value() << " - " << bk::details::UBOPhong::name_min_attribute_value() << ");\n";
      s << "           const uint colid0 = uint(floor(temp));\n";
      s << "           const uint colid1 = uint(ceil(temp));\n";
      s << "           const float w = temp - colid0;\n\n";

      s << "           color.rgb = mix(ColorBar[colid0], ColorBar[colid1], w);\n";
      s << "       }\n";
      s << "   }\n\n";

      //s << "   color_out.rgb += abs(NdotL) * (NdotL >= 0 ? 1.0f : 0.75f) * color;\n";

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PHONG");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 normal_in;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_frag = position_in;\n";
      s << "   normal_frag = normal_in;\n";
      s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::vert_color(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PHONG");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 normal_in;\n";
      s << "layout(location = 2) in float attrib_t0_in;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_in;\n"; }
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n";
      s << "layout(location = 2) out float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) out float attrib_t1_frag;\n"; }

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_frag = position_in;\n";
      s << "   normal_frag = normal_in;\n";
      s << "   attrib_t0_frag = attrib_t0_in;\n";
      if (timeDependentAttribute)
      { s << "   attrib_t1_frag = attrib_t1_in;\n"; }
      s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_phong();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << details::default_frag(true, true, true, true);
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag_color(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG COLOR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << "layout(location = 2) in float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_frag;\n"; }
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << "layout(binding = 7, std430) buffer _ColorBar\n";
      s << "{ vec3 ColorBar[]; };\n\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << details::default_frag_get_color_from_attribute(timeDependentAttribute);
      s << details::default_frag(true, true, false, false);
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag_ghosted()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG GHOSTED");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_phong();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << details::default_frag(true, true, true, true);
      s << details::default_frag_ghosted();
      s << discard_low_alpha();
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag_ghosted_oit()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG GHOSTED OIT");
      s << version();
      s << oit_definition_max_fragments();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << early_fragment_test();

      s << comment_region_output();
      s << oit_definition();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_grid_to_list_id();
      s << function_main_begin();
      s << details::default_frag(true, true, true, true);
      s << details::default_frag_ghosted();
      s << discard_low_alpha();
      s << oit_assign_from_color_out();
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag_ghosted_color(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG GHOSTED COLOR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << "layout(location = 2) in float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_frag;\n"; }
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << "layout(binding = 7, std430) buffer _ColorBar\n";
      s << "{ vec3 ColorBar[]; };\n";

      s << comment_region_output();
      s << oit_definition();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << details::default_frag_get_color_from_attribute(timeDependentAttribute);
      s << details::default_frag(true, true, false, false);
      s << details::default_frag_ghosted();
      s << discard_low_alpha();
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::phong::frag_ghosted_color_oit(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PHONG GHOSTED COLOR OIT");
      s << version();
      s << oit_definition_max_fragments();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << "layout(location = 2) in float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_frag;\n"; }
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << "layout(binding = 7, std430) buffer _ColorBar\n";
      s << "{ vec3 ColorBar[]; };\n";
      s << early_fragment_test();

      s << comment_region_output();
      s << oit_definition();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_grid_to_list_id();
      s << function_main_begin();
      s << details::default_frag_get_color_from_attribute(timeDependentAttribute);
      s << details::default_frag(true, true, false, false);
      s << details::default_frag_ghosted();

      s << "   const float color_based_alpha_t0 = (attrib_t0_frag - " << bk::details::UBOPhong::name_min_attribute_value() << ") / (" << bk::details::UBOPhong::name_max_attribute_value() << " - " << bk::details::UBOPhong::name_min_attribute_value() << ");\n";

      if (!timeDependentAttribute)
      { s << "   const float attribVal = color_based_alpha_t0;\n\n"; }
      else
      {
          s << "   const float color_based_alpha_t1 = (attrib_t1_frag - " << bk::details::UBOPhong::name_min_attribute_value() << ") / (" << bk::details::UBOPhong::name_max_attribute_value() << " - " << bk::details::UBOPhong::name_min_attribute_value() << ");\n";

          s << "   const float t = " << bk::details::UBOGlobal::name_animation_current_time() << " / " << bk::details::UBOPhong::name_temporal_resolution() << ";\n";
          s << "   const float t0 = floor(t);\n";
          s << "   const float tw = t - t0;\n";
          s << "   const float attribVal = mix(color_based_alpha_t0, color_based_alpha_t1, tw);\n\n";
      }

      s << "   color_out.a = max(color_out.a, attribVal * attribVal);\n";

      s << discard_low_alpha();
      s << oit_assign_from_color_out();
      s << function_main_end();

      return s.str();
  }

  //------------------------------------------------------------------------------------------------------
  // silhouette
  //------------------------------------------------------------------------------------------------------
  std::string ShaderLibrary::mesh::silhouette::vert()
  { return phong::vert(); }

  std::string ShaderLibrary::mesh::silhouette::vert_color(bool timeDependentAttribute)
  { return phong::vert_color(timeDependentAttribute); }

  std::string ShaderLibrary::mesh::silhouette::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("SILHOUETTE");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_phong();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << "   const vec3 camPos = camera_position();\n\n";

      s << "   if (abs(dot(normal_frag, normalize(position_frag - camPos))) < " << bk::details::UBOPhong::name_silhouette_width() << " * 0.707106781 /*cos 45°*/)\n";
      s << "   {\n";
      s << "       color_out.r = " << bk::details::UBOPhong::name_meshcol_r() << ";\n";
      s << "       color_out.g = " << bk::details::UBOPhong::name_meshcol_g() << ";\n";
      s << "       color_out.b = " << bk::details::UBOPhong::name_meshcol_b() << ";\n";
      s << "       color_out.a = 1;\n";
      s << "   }\n";
      s << "   else\n";
      s << "   { discard; }\n";

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::silhouette::frag_color(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("SILHOUETTE COLOR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << "layout(location = 2) in float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_frag;\n"; }
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << "layout(binding = 7, std430) buffer _ColorBar\n";
      s << "{ vec3 ColorBar[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << "const vec3 camPos = camera_position();\n\n";

      s << "if (abs(dot(normal_frag, normalize(position_frag - camPos))) < " << bk::details::UBOPhong::name_silhouette_width() << " * 0.707106781 /*cos 45°*/)\n";
      s << "{\n";
      s << "    color_out.r = " << bk::details::UBOPhong::name_meshcol_r() << ";\n";
      s << "    color_out.g = " << bk::details::UBOPhong::name_meshcol_g() << ";\n";
      s << "    color_out.b = " << bk::details::UBOPhong::name_meshcol_b() << ";\n";
      s << "    color_out.a = 1;\n";

      s << "    if (color_enabled != 0)\n";
      s << "    {\n";
      if (!timeDependentAttribute)
      { s << "       const float attribVal = attrib_t0_frag;\n\n"; }
      else
      {
          s << "       const float t = " << bk::details::UBOGlobal::name_animation_current_time() << " / " << bk::details::UBOPhong::name_temporal_resolution() << ";\n";
          s << "       const float t0 = floor(t);\n";
          s << "       const float tw = t - t0;\n";
          s << "       const float attribVal = mix(attrib_t0_frag, attrib_t1_frag, tw);\n\n";
      }

      s << "        if (attribVal <= " << bk::details::UBOPhong::name_min_attribute_value() << ")\n";
      s << "        { color_out.rgb = ColorBar[0]; }\n";
      s << "        else if (attribVal >= " << bk::details::UBOPhong::name_max_attribute_value() << ")\n";
      s << "        { color_out.rgb = ColorBar[" << bk::details::UBOPhong::name_num_colors() << "-1]; }\n";
      s << "        else\n";
      s << "        {\n";
      s << "            const float temp = (" << bk::details::UBOPhong::name_num_colors() << " - 1) * (attribVal - " << bk::details::UBOPhong::name_min_attribute_value() << ") / (" << bk::details::UBOPhong::name_max_attribute_value() << " - " << bk::details::UBOPhong::name_min_attribute_value() << ");\n";
      s << "            const uint colid0 = uint(floor(temp));\n";
      s << "            const uint colid1 = uint(ceil(temp));\n";
      s << "            const float w = temp - colid0;\n\n";

      s << "            color_out.rgb = mix(ColorBar[colid0], ColorBar[colid1], w);\n";
      s << "        }\n";
      s << "    }\n";
      s << "}\n";
      s << "else\n";
      s << "{ discard; }\n";

      s << function_main_end();

      return s.str();
  }

  //------------------------------------------------------------------------------------------------------
  // wireframe
  //------------------------------------------------------------------------------------------------------
  std::string ShaderLibrary::mesh::wireframe::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("WIREFRAME");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 normal_in;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_geom;\n";
      s << "layout(location = 1) out vec3 normal_geom;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_geom = position_in;\n";
      s << "   normal_geom = normal_in;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::wireframe::geom()
  {
      std::stringstream s;

      s << comment_tag_geometry_shader("WIREFRAME");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_geom[3];\n";
      s << "layout(location = 1) in vec3 normal_geom[3];\n";
      s << ubo_definition_global();
      s << "layout(triangles) in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n\n";
      s << "layout (line_strip, max_vertices = 3) out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   for (int i = 0; i < 3; ++i)\n";
      s << "   {\n";
      s << "       position_frag = position_geom[i];\n";
      s << "       normal_frag = normal_geom[i];\n";
      s << "       gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[i], 1);\n";
      s << "       EmitVertex();\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::wireframe::geom_color(bool timeDependentAttribute)
  {
      std::stringstream s;

      s << comment_tag_geometry_shader("WIREFRAME COLOR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_geom[3];\n";
      s << "layout(location = 1) in vec3 normal_geom[3];\n";
      s << "layout(location = 2) in float attrib_t0_geom[3];\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) in float attrib_t1_geom[3];\n"; }
      s << ubo_definition_global();
      s << "layout(triangles) in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n";
      s << "layout(location = 2) out float attrib_t0_frag;\n";
      if (timeDependentAttribute)
      { s << "layout(location = 3) out float attrib_t1_frag;\n"; }
      s << "\n";

      s << "layout (line_strip, max_vertices = 3) out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   for (int i = 0; i < 3; ++i)\n";
      s << "   {\n";
      s << "       position_frag = position_geom[i];\n";
      s << "       normal_frag = normal_geom[i];\n";
      s << "       attrib_t0_frag = attrib_t0_geom[i];\n";
      if (timeDependentAttribute)
      { s << "       attrib_t1_frag = attrib_t1_geom[i];\n"; }
      s << "       gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[i], 1);\n";
      s << "       EmitVertex();\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::wireframe::frag()
  { return phong::frag(); }

  std::string ShaderLibrary::mesh::wireframe::frag_color(bool timeDependentAttribute)
  { return phong::frag_color(timeDependentAttribute); }

  //------------------------------------------------------------------------------------------------------
  // picking
  //------------------------------------------------------------------------------------------------------
  std::string ShaderLibrary::mesh::picking::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PICKING");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 normal_in; //unused\n";
      s << "layout(location = 2) in float vertID_in;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) flat out float vertID_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_frag = position_in;\n";
      s << "   vertID_frag = vertID_in;\n";
      s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::picking::vert_phong()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PICKING (PHONG)");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 normal_in;\n";
      s << "layout(location = 2) in float vertID_in; //unused\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_frag = position_in;\n";
      s << "   normal_frag = normal_in;\n";
      s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::picking::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PICKING");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) flat in float vertID_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(vertID_frag, vertID_frag, vertID_frag, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::picking::vert_current_selection()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PICKING (CURRENT SELECTION)");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_geom;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_geom = position_in;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::picking::geom_current_selection()
  {
      std::stringstream s;

      s << comment_tag_geometry_shader("PICKING (CURRENT SELECTION)");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_geom[3];\n";
      s << ubo_definition_global();
      s << ubo_definition_selection_sphere();
      s << "layout (triangles) in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 position_frag;\n";
      s << "layout(location = 1) out vec3 normal_frag;\n\n";

      s << "layout (triangle_strip, max_vertices = 3) out;\n";

      s << comment_region_functions();
      s << function_main_begin();

      s << "   for (int i = 0; i < 3; ++i)\n";
      s << "   {\n";
      s << "       position_frag.x = position_geom[i].x + " << bk::details::UBOSelectionSphere::name_center_x() << ";\n";
      s << "       position_frag.y = position_geom[i].y + " << bk::details::UBOSelectionSphere::name_center_y() << ";\n";
      s << "       position_frag.z = position_geom[i].z + " << bk::details::UBOSelectionSphere::name_center_z() << ";\n";
      s << "       normal_frag = normalize(position_geom[i]);\n";
      s << "       gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag, 1);\n";
      s << "       EmitVertex();\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::mesh::picking::frag_current_selection()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("PICKING (CURRENT SELECTION)");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_frag;\n";
      s << "layout(location = 1) in vec3 normal_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_phong();
      s << ubo_definition_selection_sphere();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();
      s << "   const vec3 camPos = camera_position();\n";
      s << "   const vec3 E = normalize(position_frag - camPos);\n";
      s << "   const vec3 P = position_frag; // world coordinates\n";
      s << "   const vec3 L = -E; // // headlight\n";
      s << "   const vec3 N = normalize(normal_frag);\n";
      s << "   const vec3 R = normalize(reflect(L, N)); // for specular\n\n";

      s << "   vec3 color = vec3(" << bk::details::UBOSelectionSphere::name_color_r() << ", " << bk::details::UBOSelectionSphere::name_color_g() << ", " << bk::details::UBOSelectionSphere::name_color_b() << ");\n";

      s << "   // alpha\n";
      s << "   color_out.a = 1;\n\n";

      s << "   // ambient\n";
      s << "   color_out.rgb = 0.1 * color;\n\n";

      s << "   // diffuse\n";
      s << "   const float NdotL = dot(N, L);\n";
      s << "   color_out.rgb += abs(NdotL) * (NdotL >= 0 ? 1.0f : 0.75f) * color;\n\n";

      s << "   // specular\n";
      s << "   const vec3 light_color = vec3(" << bk::details::UBOPhong::name_lightcol_r() << ", " << bk::details::UBOPhong::name_lightcol_g() << ", " << bk::details::UBOPhong::name_lightcol_b() << ");\n";
      s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << bk::details::UBOPhong::name_shininess() << ");\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== LINES
  //====================================================================================================
  std::string ShaderLibrary::lines::details::set_color_out_rgb_to_line_color()
  {
      std::stringstream s;
      s << "   color_out.rgb = vec3(" << bk::details::UBOLine::name_linecol_r() << ", " << bk::details::UBOLine::name_linecol_g() << ", " << bk::details::UBOLine::name_linecol_b() << ");\n";
      return s.str();
  }

  std::string ShaderLibrary::lines::debug::vert_passthrough(bool linesHaveTime, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES DEBUG linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << "colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_vertex_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_in;\n";
      if (colorEnabled)
      { s << "layout(location = 1) in float attrib_in;\n"; }
      s << "\n";
      s << ubo_definition_global();

      s << comment_region_functions();
      s << function_main_begin();
      s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in.xyz, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::debug::frag_passthrough(bool linesHaveTime, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES DEBUG linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << "colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(1, 0, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::vert(bool linesHaveTime, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_vertex_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_in;\n";
      s << "layout(location = 1) in float attrib_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out " << posType << " position_geom;\n";
      s << "layout(location = 1) out float attrib_geom;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_geom = position_in;\n";
      s << "   attrib_geom = attrib_in;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::geom(bool linesHaveTime, bool animationEnabled, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " animationEnabled:";
          stype << (animationEnabled ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_geometry_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "// line strip with adjacency\n";
      s << "layout(location = 0) in " << posType << " position_geom[4];\n";
      s << "layout(location = 1) in float attrib_geom[4];\n";
      s << ubo_definition_global();
      s << ubo_definition_line();
      s << geom_layout_in_lines_adjacency();

      s << comment_region_output();
      s << "layout(location = 0) out " << posType << " position_frag;\n";
      s << "layout(location = 1) out vec3 tangent_frag;\n";
      s << "layout(location = 2) out float halo_percent_frag;\n";
      if (colorEnabled && linesHaveColor)
      { s << "layout(location = 3) out float attrib_frag;\n"; }
      s << geom_layout_out_triangle_strip(4);

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();

      s << "   if (abs(attrib_geom[1] - " << UBOLine::name_invalid_attrib_value() << ") > 1e-2 && abs(attrib_geom[2] - " << UBOLine::name_invalid_attrib_value() << ") > 1e-2) {\n";

      if (linesHaveTime && animationEnabled)
      {
          s << "   const float dt0 = abs(position_geom[1][3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n";
          s << "   const float dt1 = abs(position_geom[2][3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n\n";

          s << "   if (" << bk::details::UBOGlobal::name_animation_enabled() << " == 0 || dt0 < " << bk::details::UBOLine::name_trail_length_in_ms() << " || dt1 < " << bk::details::UBOLine::name_trail_length_in_ms() << ")\n";
          s << "   {\n";
      }

      const std::string indent = animationEnabled && linesHaveTime ? "   " : "";

      s << indent << "   const vec3 camPos = camera_position();\n\n";

      s << indent << "   const vec3 tangent[2] = {\n";
      s << indent << "      normalize(position_geom[2].xyz - position_geom[0].xyz),\n";
      s << indent << "      normalize(position_geom[3].xyz - position_geom[1].xyz)\n";
      s << indent << "   };\n\n";

      s << indent << "   const vec3 camVec[2] = {\n";
      s << indent << "      normalize(position_geom[1].xyz - camPos),\n";
      s << indent << "      normalize(position_geom[2].xyz - camPos)\n";
      s << indent << "   };\n\n";

      s << indent << "   const vec3 ortho[2] = {\n";
      s << indent << "      normalize(cross(camVec[0], tangent[0])),\n";
      s << indent << "      normalize(cross(camVec[1], tangent[1]))\n";
      s << indent << "   };\n\n";

      if (animationEnabled && linesHaveTime)
      {
          s << indent << "   const float w0 = clamp(1.25 - dt0 / " << bk::details::UBOLine::name_trail_length_in_ms() << ", 0, 1);\n";
          s << indent << "   const float w1 = clamp(1.25 - dt1 / " << bk::details::UBOLine::name_trail_length_in_ms() << ", 0, 1);\n";
      }

      // vertex 0
      s << indent << "   halo_percent_frag = -1;\n";
      if (animationEnabled && linesHaveTime)
      { s << indent << "   position_frag = vec4(position_geom[1].xyz + w0*" << bk::details::UBOLine::name_line_width() << "*ortho[0], position_geom[1][3]);\n"; }
      else
      {
          //s << indent << "   position_frag = position_geom[1] + " << bk::details::UBOLine::name_line_width() << "*ortho[0];\n";

          s << indent << "   position_frag";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " = position_geom[1]";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " + " << bk::details::UBOLine::name_line_width() << "*ortho[0];\n";
      }
      s << indent << "   tangent_frag = tangent[0];\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[1];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag.xyz, 1);\n";
      s << indent << "   EmitVertex();\n\n";

      // vertex 1
      if (animationEnabled && linesHaveTime)
      { s << indent << "   position_frag = vec4(position_geom[2].xyz + w1*" << bk::details::UBOLine::name_line_width() << "*ortho[1], position_geom[2][3]);\n"; }
      else
      {
          //s << indent << "   position_frag = position_geom[2] + " << bk::details::UBOLine::name_line_width() << "*ortho[1];\n";

          s << indent << "   position_frag";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " = position_geom[2]";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " + " << bk::details::UBOLine::name_line_width() << "*ortho[1];\n";
      }
      s << indent << "   tangent_frag = tangent[1];\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[2];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag.xyz, 1);\n";
      s << indent << "   EmitVertex();\n\n";

      // vertex 2
      s << indent << "   halo_percent_frag = +1;\n";
      if (animationEnabled && linesHaveTime)
      { s << indent << "   position_frag = vec4(position_geom[1].xyz - w0*" << bk::details::UBOLine::name_line_width() << "*ortho[0], position_geom[1][3]);\n"; }
      else
      {
          //s << indent << "   position_frag = position_geom[1] - " << bk::details::UBOLine::name_line_width() << "*ortho[0];\n";

          s << indent << "   position_frag";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " = position_geom[1]";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " - " << bk::details::UBOLine::name_line_width() << "*ortho[0];\n";
      }
      s << indent << "   tangent_frag = tangent[0];\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[1];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag.xyz, 1);\n";
      s << indent << "   EmitVertex();\n\n";

      // vertex 3
      if (animationEnabled && linesHaveTime)
      { s << indent << "   position_frag = vec4(position_geom[2].xyz - w1*" << bk::details::UBOLine::name_line_width() << "*ortho[1], position_geom[2][3]);\n"; }
      else
      {
          //s << indent << "   position_frag = position_geom[2] - " << bk::details::UBOLine::name_line_width() << "*ortho[1];\n";

          s << indent << "   position_frag";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " = position_geom[2]";
          if (!animationEnabled && linesHaveTime)
          { s << ".xyz"; }

          s << " - " << bk::details::UBOLine::name_line_width() << "*ortho[1];\n";
      }
      s << indent << "   tangent_frag = tangent[1];\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[2];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag.xyz, 1);\n";
      s << indent << "   EmitVertex();\n\n";

      if (animationEnabled && linesHaveTime)
      { s << "   }\n"; }

      s << "   }\n"; // invalid attribute

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::frag_transparent(bool linesHaveTime, bool animationEnabled, bool linesHaveColor, bool colorEnabled, bool oitEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES TRANSPARENT linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " animationEnabled:";
          stype << (animationEnabled ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");
          stype << " oitEnabled:";
          stype << (oitEnabled ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();
      if (oitEnabled)
      { s << oit_definition_max_fragments(); }

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_frag;\n";
      s << "layout(location = 1) in vec3 tangent_frag;\n";
      s << "layout(location = 2) in float halo_percent_frag;\n";
      if (colorEnabled && linesHaveColor)
      { s << "layout(location = 3) in float attrib_frag;\n"; }
      s << ubo_definition_global();
      s << ubo_definition_line();
      if (colorEnabled && linesHaveColor)
      {
          s << "layout(binding = 7, std430) buffer _ColorBar\n";
          s << "{ vec3 ColorBar[]; };\n";
      }
      if (oitEnabled)
      { s << "\n" << early_fragment_test(); }

      s << comment_region_output();
      if (oitEnabled)
      {
          s << oit_definition_atomic_counter();
          s << oit_definition_fragments();
          s << oit_definition_linked_list();
      }
      else
      { s << "layout(location = 0) out vec4 color_out;\n"; }

      s << comment_region_functions();
      s << function_camera_position();
      s << function_grid_to_list_id();
      s << function_main_begin();
      if (linesHaveTime && animationEnabled)
      {
          s << "    if (" << bk::details::UBOGlobal::name_animation_enabled() << " != 0 && abs(position_frag[3] - " << bk::details::UBOGlobal::name_animation_current_time() << ") > " << bk::details::UBOLine::name_trail_length_in_ms() << ")\n";
          s << "    { discard; }\n\n";
      }

      s << "   ";
      if (oitEnabled)
      { s << "vec4 "; }

      s << "color_out = vec4(" << bk::details::UBOLine::name_linecol_r() << ", " << bk::details::UBOLine::name_linecol_g() << ", " << bk::details::UBOLine::name_linecol_b() << ", 1);\n\n";

      if (linesHaveTime && animationEnabled)
      {
          s << "   const float dt = abs(position_frag[3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n";
          s << "   const float topaque = " << bk::details::UBOLine::name_trail_length_in_ms() << " * " << bk::details::UBOLine::name_trail_opaque_part_in_percent() << ";\n";
          s << "   if (dt <= " << bk::details::UBOLine::name_trail_length_in_ms() << " && dt > topaque)\n";
          s << "   {\n";
          s << "       color_out.a = 1.0f - ((dt - topaque) / (" << bk::details::UBOLine::name_trail_length_in_ms() << " - topaque));\n";
          s << "       color_out.a *= color_out.a;\n";
          s << "   }\n";

          s << discard_low_alpha();
      }
      else
      { s << "   color_out.a = 1;\n\n"; }

      if (colorEnabled && linesHaveColor)
      {
          s << "   if (" << bk::details::UBOLine::name_color_enabled() << " == 0)\n";
          s << "   { " << details::set_color_out_rgb_to_line_color() << "}\n";
          s << "   else\n";
          s << "   {\n";
          s << "      if (attrib_frag <= " << bk::details::UBOLine::name_min_value() << ")\n";
          s << "      {\n";
          s << "          color_out.rgb = ColorBar[0];\n\n";

          s << "          if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0)\n";
          s << "          { color_out.a = 0; }\n";
          s << "      }\n";
          s << "      else if (attrib_frag >= " << bk::details::UBOLine::name_max_value() << ")\n";
          s << "      { color_out.rgb = ColorBar[" << bk::details::UBOLine::name_num_colors() << "-1]; }\n";
          s << "      else\n";
          s << "      {\n";
          s << "          const float temp = (" << bk::details::UBOLine::name_num_colors() << " - 1) * (attrib_frag - " << bk::details::UBOLine::name_min_value() << ") / (" << bk::details::UBOLine::name_max_value() << " - " << bk::details::UBOLine::name_min_value() << ");\n\n";

          s << "          if (" << bk::details::UBOLine::name_scale_attrib_to_colorbar() << " != 0)\n";
          s << "          {\n";
          s << "              const uint colid0 = uint(floor(temp));\n";
          s << "              const uint colid1 = uint(ceil(temp));\n";
          s << "              const float w = temp - colid0;\n\n";

          s << "              color_out.rgb = mix(ColorBar[colid0], ColorBar[colid1], w);\n";
          s << "          }\n";
          s << "          else\n";
          s << "          { color_out.rgb = ColorBar[int(round(attrib_frag))]; }\n\n";

          s << "          if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0)\n";
          s << "          {\n";
          s << "             const float alpha = temp / (" << bk::details::UBOLine::name_num_colors() << " - 1);\n";
          if (!oitEnabled)
          { s << "            color_out.a = alpha;\n"; }
          else
          {
              s << "            if (alpha < " << bk::details::UBOLine::name_color_alpha_correction() << ")\n";
              s << "            { color_out.a *= alpha / " << bk::details::UBOLine::name_color_alpha_correction() << "; }\n";
          }
          s << "          }\n";
          s << "      }\n";
          s << "   }\n";
      }

      s << "   if (" << bk::details::UBOLine::name_halo_enabled() << " != 0 && abs(halo_percent_frag) >= 1.0f-" << bk::details::UBOLine::name_halo_width_in_percent() << ")\n";
      s << "   {\n";
      s << "       const float diff = (1 - abs(halo_percent_frag)) / " << bk::details::UBOLine::name_halo_width_in_percent() << ";\n";
      s << "       color_out.rgb = mix(vec3(0), vec3(color_out.r, color_out.g, color_out.b), diff*diff); // faded halo color\n";
      s << "   }\n";
      if (!colorEnabled)
      {
          s << "   else\n";
          s << "   { " << details::set_color_out_rgb_to_line_color() << " }\n\n";
      }
      else
      { s << discard_low_alpha(); }

      if (!oitEnabled)
      { s << "   color_out.a = 1; // no oit -> no transparency"; }

      s << "   if (" << bk::details::UBOLine::name_isl_enabled() << " != 0)\n";
      s << "   {\n";
      s << "      const vec3 camPos = camera_position();\n";
      s << "      const vec3 E = normalize(position_frag.xyz - camPos);\n";
      s << "      const vec3 T = normalize(tangent_frag);\n";
      s << "      const vec3 L = E; // headlight\n";
      s << "      const vec3 ISLNormal = normalize(cross(normalize(cross(T, L)), T));\n";
      s << "      const vec3 R = normalize(reflect(-L, ISLNormal));\n\n";

      s << "      const vec3 lightColor = vec3(" << bk::details::UBOLine::name_lightcol_r() << ", " << bk::details::UBOLine::name_lightcol_g() << ", " << bk::details::UBOLine::name_lightcol_b() << ");\n\n";

      s << "      color_out.rgb *= clamp(abs(dot(ISLNormal, L)), 0.0, 1.0); //diffuse\n";
      s << "      color_out.rgb += clamp(pow(abs(dot(R, L)), " << bk::details::UBOLine::name_shininess() << "), 0.0, 1.0) * lightColor; // specular\n";

      s << "   }\n";

      if (oitEnabled)
      { s << oit_assign_from_color_out(); }

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::frag_opaque(bool linesHaveTime, bool animationEnabled, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINES OPAQUE linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " animationEnabled:";
          stype << (animationEnabled ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_frag;\n";
      s << "layout(location = 1) in vec3 tangent_frag;\n";
      s << "layout(location = 2) in float halo_percent_frag;\n";
      if (colorEnabled && linesHaveColor)
      { s << "layout(location = 3) in float attrib_frag;\n"; }
      s << "\n";
      s << ubo_definition_global();
      s << ubo_definition_line();

      if (colorEnabled && linesHaveColor)
      {
          s << "layout(binding = 7, std430) buffer _ColorBar\n";
          s << "{ vec3 ColorBar[]; };\n";
      }

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();

      if (linesHaveTime && animationEnabled)
      {
          s << "    if (" << bk::details::UBOGlobal::name_animation_enabled() << " != 0 && abs(position_frag[3] - " << bk::details::UBOGlobal::name_animation_current_time() << ") > " << bk::details::UBOLine::name_trail_length_in_ms() << ")\n";
          s << "    { discard; }\n\n";
      }

      s << details::set_color_out_rgb_to_line_color();
      s << "   color_out.a = 1;\n\n";

      if (linesHaveTime && animationEnabled)
      {
          s << "   const float dt = abs(position_frag[3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n";
          s << "   if (dt <= " << bk::details::UBOLine::name_trail_length_in_ms() << " * " << bk::details::UBOLine::name_trail_opaque_part_in_percent() << ")\n";
          s << "   { color_out.a = 1; }\n";
          s << "   else\n";
          s << "   { discard; }\n\n";
      }

      if (colorEnabled && linesHaveColor)
      {
          s << "   if (" << bk::details::UBOLine::name_color_enabled() << " != 0)\n";
          s << "   {\n";
          s << "      if (attrib_frag <= " << bk::details::UBOLine::name_min_value() << ")\n";
          s << "      {\n";
          s << "          color_out.rgb = ColorBar[0];\n\n";

          s << "          if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0)\n";
          s << "          { color_out.a = 0; }\n";
          s << "      }\n";
          s << "      else if (attrib_frag >= " << bk::details::UBOLine::name_max_value() << ")\n";
          s << "      { color_out.rgb = ColorBar[" << bk::details::UBOLine::name_num_colors() << "-1]; }\n";
          s << "      else\n";
          s << "      {\n";
          s << "          const float temp = (" << bk::details::UBOLine::name_num_colors() << " - 1) * (attrib_frag - " << bk::details::UBOLine::name_min_value() << ") / (" << bk::details::UBOLine::name_max_value() << " - " << bk::details::UBOLine::name_min_value() << ");\n\n";

          s << "          if (" << bk::details::UBOLine::name_scale_attrib_to_colorbar() << " != 0)\n";
          s << "          {\n";
          s << "              const uint colid0 = uint(floor(temp));\n";
          s << "              const uint colid1 = uint(ceil(temp));\n";
          s << "              const float w = temp - colid0;\n\n";

          s << "              color_out.rgb = mix(ColorBar[colid0], ColorBar[colid1], w);\n";
          s << "          }\n";
          s << "          else\n";
          s << "          { color_out.rgb = ColorBar[int(round(attrib_frag))]; }\n\n";

          s << "          if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0)\n";
          s << "          {\n";
          s << "             const float alpha = temp / (" << bk::details::UBOLine::name_num_colors() << " - 1);\n";
          s << "            if (alpha >= " << bk::details::UBOLine::name_color_alpha_correction() << ")\n";
          s << "            { color_out.a = 1; }\n";
          s << "            else\n";
          s << "            { discard; }\n";
          s << "          }\n";
          s << "      }\n";
          s << "   }\n";
      }

      s << "   if (" << bk::details::UBOLine::name_halo_enabled() << " != 0 && abs(halo_percent_frag) >= 1.0f-" << bk::details::UBOLine::name_halo_width_in_percent() << ")\n";
      s << "   {\n";
      s << "       const float diff = (1 - abs(halo_percent_frag)) / " << bk::details::UBOLine::name_halo_width_in_percent() << ";\n";
      s << "       color_out.rgb = mix(vec3(0), color_out.rgb, diff*diff); // faded halo color\n";
      s << "   }\n";
      if (!colorEnabled)
      {
          s << "   else\n";
          s << "   { " << details::set_color_out_rgb_to_line_color() << " }\n\n";
      }
      else
      {
          s << "\n   if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0 && color_out.a < 1)\n";
          s << "   { discard; }\n\n";
      }

      s << "   if (" << bk::details::UBOLine::name_isl_enabled() << " != 0)\n";
      s << "   {\n";
      s << "      const vec3 camPos = camera_position();\n";
      s << "      const vec3 E = normalize(position_frag.xyz - camPos);\n";
      s << "      const vec3 T = normalize(tangent_frag);\n";
      s << "      const vec3 L = E; // headlight\n";
      s << "      const vec3 ISLNormal = normalize(cross(normalize(cross(T, L)), T));\n";
      s << "      const vec3 R = normalize(reflect(-L, ISLNormal));\n\n";

      s << "      const vec3 lightColor = vec3(" << bk::details::UBOLine::name_lightcol_r() << ", " << bk::details::UBOLine::name_lightcol_g() << ", " << bk::details::UBOLine::name_lightcol_b() << ");\n\n";

      s << "      color_out.rgb *= clamp(abs(dot(ISLNormal, L)), 0.0, 1.0); //diffuse\n";
      s << "      color_out.rgb += clamp(pow(abs(dot(R, L)), " << bk::details::UBOLine::name_shininess() << "), 0.0, 1.0) * lightColor; // specular\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("LINEAO");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec2 texcoord_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec2 texcoord_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << "   texcoord_frag = texcoord_in;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("LINEAO");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 texcoord_frag;\n\n";

      s << "layout(binding =  7) uniform sampler2D td_map;     // rgb = normalized tangent, a = linearized depth\n";
      s << "layout(binding =  8) uniform sampler2D pa_map;     // rgb = pos, a = screen-space angle\n";
      s << "layout(binding =  9) uniform sampler2D color_map;  // rgb = color, a = zoom\n";
      s << "layout(binding = 11) uniform sampler2D td_mipmap0; // rgb = normalized tangent, a = linearized depth\n";
      s << "layout(binding = 12) uniform sampler2D pa_mipmap0; // rgb = pos, a = screen-space angle\n";
      s << "layout(binding = 13) uniform sampler2D td_mipmap1; // rgb = normalized tangent, a = linearized depth\n";
      s << "layout(binding = 14) uniform sampler2D pa_mipmap1; // rgb = pos, a = screen-space angle\n";
      s << "layout(binding = 15) uniform sampler2D noise_tex;  // rg = normalized vector € [-1,+1]\n";

      s << ubo_definition_global();
      s << ubo_definition_line();

      s << "// line ao compile time parameters\n";
      s << "const int sh = 32; // maximum number of samples on the hemisphere\n";
      s << "const int sr = 3; // number of radii to evaluate (=^ number of iterations)\n";
      s << "const float r0 = 1.5 * texture(color_map, texcoord_frag).a * " << bk::details::UBOLine::name_line_width() << "; // minimum radius (Eq. 16)\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << "vec3 get_hemisphereVector(float s, int i, int j) // 3.4. \"Implementation\": random noise texture R\n";
      s << "{ return reflect(/*randSphereNormal*/texture(noise_tex, (vec2(float(i) / s, float(j) / (sr-1)))).rgb, /*randNormal*/texture(noise_tex, texcoord_frag).rgb); }\n\n";

      s << "float visibility(float depth_diff) // Vl(w,P), Eq. 8\n";
      s << "{ return depth_diff < 0 ? 1 : 0; }\n\n";

      s << "float hermitePolynom(float x) // Eq. 13\n";
      s << "{ return 3*x*x - 2*x*x*x; }\n\n";

      s << "float falloff(float l) // delta(l), Eq. 11\n";
      s << "{\n";
      s << "    const float f = 1 - l/sr;\n";
      s << "    return f*f;\n";
      s << "}\n\n";

      s << "float depth_weight(float depth_diff, float falloffl) // Eq. 12\n";
      s << "{\n";
      s << "    const float falloff0 = 1e-4; // threshold delta 0\n";
      s << "    return depth_diff > falloffl ? 0 : depth_diff < falloff0 ? 1 : 1 - hermitePolynom((depth_diff - falloff0) / (falloffl - falloff0));\n";
      s << "}\n\n";

      s << "float alignment_weight(float alignment_diff)\n";
      s << "{ return " << bk::details::UBOLine::name_lineao_anisotropic() << " != 0 ? clamp(15 * hermitePolynom(alignment_diff), 0.1f, 1.0f) : 1; }\n\n";

      s << "float light_weight(float Ll_diff) // Eq. 15\n";
      s << "{\n";
      s << "    //return 1 - min(Ll_diff, 1);\n";
      s << "    return 1.5 - Ll_diff;\n";
      s << "}\n\n";

      s << function_camera_position();

      s << "vec3 isl_normal(vec2 texcoord, int j)\n";
      s << "{\n";
      s << "    const vec3 pos = j == 0 ? texture(pa_map, texcoord).rgb : j == 1 ? texture(pa_mipmap0, texcoord).rgb : texture(pa_mipmap1, texcoord).rgb;\n";
      s << "    const vec3 T = j == 0 ? texture(td_map, texcoord).rgb : j == 1 ? texture(td_mipmap0, texcoord).rgb : texture(td_mipmap1, texcoord).rgb; // already normalized\n\n";

      s << "    const vec3 camPos = camera_position();\n";
      s << "    const vec3 E = normalize(pos - camPos);\n";
      s << "    const vec3 L = E; // headlight\n";
      s << "    return normalize(cross(normalize(cross(T, L)), T));\n";
      s << "}\n\n";

      s << "vec3 isl_normal(vec2 texcoord)\n";
      s << "{ return isl_normal(texcoord, 0); }\n\n";

      s << "float calc_normalDifference(vec3 camPos, vec3 ep, vec3 hemisphereVector, int j)\n";
      s << "{\n";
      s << "    const vec3 pos = j == 0 ? texture(pa_map, texcoord_frag).rgb : j == 1 ? texture(pa_mipmap0, texcoord_frag).rgb : texture(pa_mipmap1, texcoord_frag).rgb;\n";
      s << "    const vec3 E = pos - camPos;\n\n";

      s << "    const float pointDiffuse = max( dot( hemisphereVector, isl_normal(texcoord_frag, j) ), 0.0 );\n";
      s << "    const vec3 hemispherePoint = hemisphereVector + ep;\n";
      s << "    const vec3 t = texture(td_map, hemispherePoint.rg).rgb;\n";
      s << "    const vec3 newnorm = normalize( cross( normalize( cross( t, normalize( hemisphereVector ) ) ), t ) );\n";
      s << "    const float occluderDiffuse = max( dot( newnorm, E ), 0.0 );\n";
      s << "    const vec3 H = normalize( E + normalize( hemisphereVector ) );\n";
      s << "    const vec3 occluderNormal = isl_normal(hemispherePoint.rg, j);\n";
      s << "    const float occluderSpecular = pow( max( dot( H, occluderNormal ), 0.0 ), " << bk::details::UBOLine::name_shininess() << " );\n";
      s << "    return pointDiffuse * ( occluderSpecular + occluderDiffuse );\n";
      s << "}\n\n";

      s << function_main_begin();
      s << "    const float zoom = texture(color_map, texcoord_frag).a;\n";
      s << "    if (zoom == 0)\n";
      s << "    { discard; }\n\n";

      s << "    float ao = 0;\n\n";

      s << "    // parameters\n";
      s << "    const float falloff0 = 1e-4; // threshold delta 0\n\n";

      s << "    const vec3 camPos = camera_position();\n";
      s << "    const vec3 ep = vec3(texcoord_frag, texture(td_map, texcoord_frag).a); // tex-position of current pixel\n";
      s << "    const vec3 ISLNormal = isl_normal(texcoord_frag); // normal at current pixel\n\n";

      s << "    /* --- radius 0 --- */\n";
      s << "    const int l0 = 0; // mipmap level\n";
      s << "    const int s0 = sh;\n";
      s << "    const float falloffl0 = falloff(l0); // Eq. 11\n\n";

      s << "    for(int i = 1; i <= s0; ++i) // Eq. 6: evaluate for each sample\n";
      s << "    {\n";
      s << "        // random vector from noise texture [3.4. \"Implementation\": random noise texture R]\n";
      s << "        vec3 hemisphereVector = r0 * get_hemisphereVector(s0, i, l0);\n";
      s << "        hemisphereVector = sign(dot(hemisphereVector, ISLNormal)) * hemisphereVector;\n\n";

      s << "        // depth difference [Delta dl(w,P), Eq. 10, parts of Eq. 8]\n";
      s << "        const float depth_diff = texture(td_map, texcoord_frag).a - texture(td_map, texcoord_frag + hemisphereVector.rg).a;\n\n";

      s << "        // alignment difference\n";
      s << "        const float alignment_diff = abs(texture(pa_map, texcoord_frag).a - texture(pa_map, texcoord_frag + hemisphereVector.rg).a);\n\n";

      s << "        // light\n";
      s << "        const float normalDifference = " << bk::details::UBOLine::name_isl_enabled() << " == 0 ? 0 : calc_normalDifference(camPos, ep, hemisphereVector, l0);\n\n";

      s << "        // ao [weight = Eq. 9]\n";
      s << "        ao += (1 - visibility(depth_diff)) * depth_weight(depth_diff, falloffl0) * alignment_weight(alignment_diff) * light_weight(normalDifference);\n";
      s << "    }\n\n";

      s << "    /* --- radius 1 --- */\n";
      s << "    const int l1 = 1; // mipmap level\n";
      s << "    const int s1 = sh / (l1+1); // integer division on purpose!\n";
      s << "    const float r1 = r0 + l1*zoom;\n";
      s << "    const float falloffl1 = falloff(l1); // Eq. 11\n\n";

      s << "    for(int i = 1; i <= s1; ++i) // Eq. 6: evaluate for each sample\n";
      s << "    {\n";
      s << "        // random vector from noise texture [3.4. \"Implementation\": random noise texture R]\n";
      s << "        vec3 hemisphereVector = r1 * get_hemisphereVector(s1, i, l1);\n";
      s << "        hemisphereVector = sign(dot(hemisphereVector, ISLNormal)) * hemisphereVector;\n\n";

      s << "        // depth difference [Delta dl(w,P), Eq. 10, parts of  Eq. 8]\n";
      s << "        const float depth_diff = texture(td_mipmap0, texcoord_frag).a - texture(td_mipmap0, texcoord_frag + hemisphereVector.rg).a;\n\n";

      s << "        // alignment difference\n";
      s << "        const float alignment_diff = abs(texture(pa_mipmap0, texcoord_frag).a - texture(pa_mipmap0, texcoord_frag + hemisphereVector.rg).a);\n\n";

      s << "        // light\n";
      s << "        const float normalDifference = " << bk::details::UBOLine::name_isl_enabled() << " == 0 ? 0 : calc_normalDifference(camPos, ep, hemisphereVector, l0);\n\n";

      s << "        // ao [weight = Eq. 9]\n";
      s << "        ao += (1 - visibility(depth_diff)) * depth_weight(depth_diff, falloffl1) * alignment_weight(alignment_diff) * light_weight(normalDifference);\n";
      s << "    }\n\n";

      s << "    /* --- radius 2 --- */\n";
      s << "    const int l2 = 2; // mipmap level\n";
      s << "    const int s2 = sh / (l2+1); // integer division on purpose!\n";
      s << "    const float r2 = r0 + l2*zoom;\n";
      s << "    const float falloffl2 = falloff(l2); // Eq. 11\n\n";

      s << "    for(int i = 1; i <= s2; ++i) // Eq. 6: evaluate for each sample\n";
      s << "    {\n";
      s << "        // random vector from noise texture [3.4. \"Implementation\": random noise texture R]\n";
      s << "        vec3 hemisphereVector = r2 * get_hemisphereVector(s2, i, l2);\n";
      s << "        hemisphereVector = sign(dot(hemisphereVector, ISLNormal)) * hemisphereVector;\n\n";

      s << "        // depth difference [Delta dl(w,P), Eq. 10, parts of Eq. 8]\n";
      s << "        const float depth_diff = texture(td_mipmap1, texcoord_frag).a - texture(td_mipmap1, texcoord_frag + hemisphereVector.rg).a;\n\n";

      s << "        // alignment difference\n";
      s << "        const float alignment_diff = abs(texture(pa_mipmap1, texcoord_frag).a - texture(pa_mipmap1, texcoord_frag + hemisphereVector.rg).a);\n\n";

      s << "        // light\n";
      s << "        const float normalDifference = " << bk::details::UBOLine::name_isl_enabled() << " == 0 ? 0 : calc_normalDifference(camPos, ep, hemisphereVector, l0);\n\n";

      s << "        // ao [weight = Eq. 9]\n";
      s << "        ao += (1 - visibility(depth_diff)) * depth_weight(depth_diff, falloffl2) * alignment_weight(alignment_diff) * light_weight(normalDifference);\n";
      s << "    }\n\n";

      s << "    ao /= (s0 + s1 + s2);\n\n\n";

      s << "    const vec3 line_col = texture(color_map, texcoord_frag).rgb;\n";
      s << "    color_out.rgb = line_col;\n";
      s << "    color_out.a = 1;\n\n";

      s << "    if (" << bk::details::UBOLine::name_isl_enabled() << " != 0)\n";
      s << "    {\n";
      s << "        const vec3 P = texture(pa_map, texcoord_frag).rgb;\n";
      s << "        const vec3 L = normalize(P - camPos); // headlight\n";
      s << "        const vec3 R = normalize(reflect(-L, ISLNormal));\n\n";

      s << "        const float k_ambient = 0.1;\n\n";

      s << "        const float k_diffuse = clamp(abs(dot(ISLNormal, L)), 0.0, 1.0);\n\n";

      s << "        const vec3 lightColor = vec3(" << bk::details::UBOLine::name_lightcol_r() << ", " << bk::details::UBOLine::name_lightcol_g() << ", " << bk::details::UBOLine::name_lightcol_b() << ");\n";
      s << "        const float k_specular = clamp(pow(abs(dot(R, L)), " << bk::details::UBOLine::name_shininess() << "), 0.0, 1.0);\n\n";

      s << "        if (lineao_anisotropic != 0)\n";
      s << "        { color_out.rgb *= k_ambient + k_diffuse; }\n";
      s << "        color_out.rgb += k_specular * lightColor;\n";
      s << "    }\n\n";

      s << "    const float aoweigth = " << bk::details::UBOLine::name_lineao_anisotropic() << " != 0 ? 1.5 : 1;\n";
      s << "    color_out.rgb *= max(1 - aoweigth*ao, 0);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::gbuffer::vert(bool linesHaveTime, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINEAO GBUFFER linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_vertex_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_in;\n";
      s << "layout(location = 1) in float attrib_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out " << posType << " position_geom;\n";
      s << "layout(location = 1) out float attrib_geom;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_geom = position_in;\n";
      s << "   attrib_geom = attrib_in;;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::gbuffer::geom(bool linesHaveTime, bool animationEnabled, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINEAO GBUFFER linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " animationEnabled:";
          stype << (animationEnabled ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_geometry_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_geom[4]; // line strip with adjacency\n";
      s << "layout(location = 1) in float attrib_geom[4];\n";
      s << ubo_definition_global();
      s << ubo_definition_line();
      s << geom_layout_in_lines_adjacency();

      s << comment_region_output();
      s << "layout(location = 0) out " << posType << " position_frag;\n";
      s << "layout(location = 1) out vec3 tangent_frag;\n";
      s << "layout(location = 2) out float halo_percent_frag;\n";
      s << "layout(location = 3) out float zoom_frag;\n";
      s << "layout(location = 4) out float angle_frag;\n";
      if (colorEnabled && linesHaveColor)
      { s << "layout(location = 5) out float attrib_frag;\n"; }
      s << geom_layout_out_triangle_strip(4);

      s << comment_region_functions();
      s << function_camera_position();
      s << function_main_begin();

      s << "   if (abs(attrib_geom[1] - " << UBOLine::name_invalid_attrib_value() << ") > 1e-2 && abs(attrib_geom[2] - " << UBOLine::name_invalid_attrib_value() << ") > 1e-2) {\n";

      const std::string indent = linesHaveTime && animationEnabled ? "   " : "";

      if (linesHaveTime && animationEnabled)
      {
          s << "   const float dt0 = abs(position_geom[1][3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n";
          s << "   const float dt1 = abs(position_geom[2][3] - " << bk::details::UBOGlobal::name_animation_current_time() << ");\n\n";

          s << "   if (" << bk::details::UBOGlobal::name_animation_enabled() << " == 0 || dt0 < " << bk::details::UBOLine::name_trail_length_in_ms() << " || dt1 < " << bk::details::UBOLine::name_trail_length_in_ms() << ")\n";
          s << "   {\n";
      }

      s << indent << "   const vec3 camPos = camera_position();\n\n";

      s << indent << "   const vec3 tangent[2] = {\n";
      s << indent << "       normalize(position_geom[2].xyz - position_geom[0].xyz),\n";
      s << indent << "       normalize(position_geom[3].xyz - position_geom[1].xyz)\n";
      s << indent << "   };\n\n";

      s << indent << "   const vec3 camVec[2] = {\n";
      s << indent << "       normalize(position_geom[1].xyz - camPos),\n";
      s << indent << "       normalize(position_geom[2].xyz - camPos)\n";
      s << indent << "   };\n\n";

      s << indent << "   const vec3 ortho[2] = {\n";
      s << indent << "       normalize(cross(camVec[0], tangent[0])),\n";
      s << indent << "       normalize(cross(camVec[1], tangent[1]))\n";
      s << indent << "   };\n\n";

      s << indent << "   vec4 p0temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[0].xyz, 1);\n";
      s << indent << "   vec4 p1temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[1].xyz, 1);\n";
      s << indent << "   vec4 p2temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[2].xyz, 1);\n";
      s << indent << "   vec4 p3temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[3].xyz, 1);\n";
      s << indent << "   p0temp.xyz /= p0temp.w;\n";
      s << indent << "   p1temp.xyz /= p1temp.w;\n";
      s << indent << "   p2temp.xyz /= p2temp.w;\n";
      s << indent << "   p3temp.xyz /= p3temp.w;\n\n";

      s << indent << "   const vec4 zoom0temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[1].xyz + ortho[0], 1);\n";
      s << indent << "   const vec4 zoom1temp = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[2].xyz + ortho[1], 1);\n";
      s << indent << "   const float zoom0 = distance(p1temp.xyz, zoom0temp.xyz / zoom0temp.w);\n";
      s << indent << "   const float zoom1 = distance(p2temp.xyz, zoom1temp.xyz / zoom1temp.w);\n\n";

      s << indent << "   const float angle0 = abs(normalize(p2temp.xy - p0temp.xy).x); // x-component of screen-space tangent\n";
      s << indent << "   const float angle1 = abs(normalize(p3temp.xy - p1temp.xy).x); // x-component of screen-space tangent\n\n";

      if (linesHaveTime && animationEnabled)
      {
          s << indent << "   const float w0 = clamp(1.25 - dt0 / " << bk::details::UBOLine::name_trail_length_in_ms() << ", 0, 1);\n";
          s << indent << "   const float w1 = clamp(1.25 - dt1 / " << bk::details::UBOLine::name_trail_length_in_ms() << ", 0, 1);\n\n";
      }

      const std::string w0fac = linesHaveTime && animationEnabled ? "w0*" : "";
      const std::string w1fac = linesHaveTime && animationEnabled ? "w1*" : "";

      s << indent << "   halo_percent_frag = -1;\n";
      s << indent << "   position_frag = position_geom[1];\n";
      s << indent << "   tangent_frag = tangent[0];\n";
      s << indent << "   zoom_frag = zoom0;\n";
      s << indent << "   angle_frag = angle0;\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[1];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[1].xyz + " << w0fac << bk::details::UBOLine::name_line_width() << "*ortho[0], 1);\n";
      s << indent << "   EmitVertex();\n\n";

      s << indent << "   position_frag = position_geom[2];\n";
      s << indent << "   tangent_frag = tangent[1];\n";
      s << indent << "   zoom_frag = zoom1;\n";
      s << indent << "   angle_frag = angle1;\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[2];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[2].xyz + " << w1fac << bk::details::UBOLine::name_line_width() << "*ortho[1], 1);\n";
      s << indent << "   EmitVertex();\n\n";

      s << indent << "   halo_percent_frag = 1;\n";
      s << indent << "   position_frag = position_geom[1];\n";
      s << indent << "   tangent_frag = tangent[0];\n";
      s << indent << "   zoom_frag = zoom0;\n";
      s << indent << "   angle_frag = angle0;\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[1];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[1].xyz - " << w0fac << bk::details::UBOLine::name_line_width() << "*ortho[0], 1);\n";
      s << indent << "   EmitVertex();\n\n";

      s << indent << "   position_frag = position_geom[2];\n";
      s << indent << "   tangent_frag = tangent[1];\n";
      s << indent << "   zoom_frag = zoom1;\n";
      s << indent << "   angle_frag = angle1;\n";
      if (colorEnabled && linesHaveColor)
      { s << indent << "   attrib_frag = attrib_geom[2];\n"; }
      s << indent << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_geom[2].xyz - " << w1fac << bk::details::UBOLine::name_line_width() << "*ortho[1], 1);\n";
      s << indent << "   EmitVertex();\n";
      if (linesHaveTime && animationEnabled)
      { s << "   }\n"; }

      s << "   }\n"; // invalid attribute

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::gbuffer::frag(bool linesHaveTime, bool animationEnabled, bool linesHaveColor, bool colorEnabled)
  {
      const std::string posType = linesHaveTime ? "vec4" : "vec3";

      std::stringstream s;

      {
          std::stringstream stype;
          stype << "LINEAO GBUFFER linesHaveTime:";
          stype << (linesHaveTime ? "yes" : "no");
          stype << " animationEnabled:";
          stype << (animationEnabled ? "yes" : "no");
          stype << "linesHaveColor:";
          stype << (linesHaveColor ? "yes" : "no");
          stype << " colorEnabled:";
          stype << (colorEnabled ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in " << posType << " position_frag;\n";
      s << "layout(location = 1) in vec3 tangent_frag; // already normalized\n";
      s << "layout(location = 2) in float halo_percent_frag;\n";
      s << "layout(location = 3) in float zoom_frag;\n";
      s << "layout(location = 4) in float angle_frag;\n";
      if (colorEnabled && linesHaveColor)
      {
          s << "layout(location = 5) in float attrib_frag;\n\n";

          s << "layout(binding = 7, std430) buffer _ColorBar\n";
          s << "{ vec3 ColorBar[]; };\n\n";
      }
      else
      { s << "\n"; }
      s << ubo_definition_global();
      s << ubo_definition_line();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 td_map; // 3D normalized tangent, 1D linearized depth\n";
      s << "layout(location = 1) out vec4 pa_map; // 3D pos, 1D screen angle\n";
      s << "layout(location = 2) out vec4 color_map; // 3D color, 1D zoom\n";

      s << comment_region_functions();
      s << function_linearize_depth();
      s << function_main_begin();
      s << "   td_map = vec4(0);\n";
      s << "   pa_map = vec4(0);\n";
      s << "   color_map = vec4(" << bk::details::UBOLine::name_linecol_r() << ", " << bk::details::UBOLine::name_linecol_g() << ", " << bk::details::UBOLine::name_linecol_b() << ", 0);\n\n";

      if (linesHaveTime && animationEnabled)
      {
          s << "   if (" << bk::details::UBOGlobal::name_animation_enabled() << " != 0 && abs(position_frag[3] - " << bk::details::UBOGlobal::name_animation_current_time() << ") > " << bk::details::UBOLine::name_trail_length_in_ms() << ")\n";
          s << "   { discard; }\n\n";
      }

      s << "   td_map.rgb = tangent_frag;\n";
      s << "   td_map.a = linearize_depth();\n\n";

      s << "   pa_map.rgb = position_frag.rgb;\n";
      s << "   pa_map.a = angle_frag;\n\n";

      s << "   color_map.a = zoom_frag;\n\n";

      if (colorEnabled && linesHaveColor)
      {
          s << "   if (" << bk::details::UBOLine::name_color_enabled() << " == 1)\n";
          s << "   {\n";
          s << "       if (attrib_frag <= " << bk::details::UBOLine::name_min_value() << ")\n";
          s << "       {\n";
          s << "           color_map.rgb = ColorBar[0];\n\n";

          s << "           if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0)\n";
          s << "           { discard; }\n";
          s << "       }\n";
          s << "       else if (attrib_frag >= " << bk::details::UBOLine::name_max_value() << ")\n";
          s << "       { color_map.rgb = ColorBar[" << bk::details::UBOLine::name_num_colors() << "-1]; }\n";
          s << "       else\n";
          s << "       {\n";
          s << "           const float temp = (" << bk::details::UBOLine::name_num_colors() << " - 1) * (attrib_frag - " << bk::details::UBOLine::name_min_value() << ") / (" << bk::details::UBOLine::name_max_value() << " - " << bk::details::UBOLine::name_min_value() << ");\n\n";

          s << "           if (" << bk::details::UBOLine::name_scale_attrib_to_colorbar() << " != 0)\n";
          s << "           {\n";
          s << "               const uint colid0 = uint(floor(temp));\n";
          s << "               const uint colid1 = uint(ceil(temp));\n";
          s << "               const float w = temp - colid0;\n\n";

          s << "               color_map.rgb = mix(ColorBar[colid0], ColorBar[colid1], w);\n";
          s << "           }\n";
          s << "           else\n";
          s << "           { color_map.rgb = ColorBar[int(round(attrib_frag))]; }\n\n";

          s << "           if (" << bk::details::UBOLine::name_color_transparency_enabled() << " != 0 && temp / (" << bk::details::UBOLine::name_num_colors() << " - 1) <= 0.05)\n";
          s << "           { discard; }\n";
          s << "       }\n";
          s << "   }\n";
      }

      s << "   const float hwp = 2*" << bk::details::UBOLine::name_halo_width_in_percent() << ";\n";
      s << "   gl_FragDepth = gl_FragCoord.z; //required for depth-dependent halo\n";
      s << "   if (" << bk::details::UBOLine::name_halo_enabled() << " != 0 && abs(halo_percent_frag) >= 1.0f-hwp)\n";
      s << "   {\n";
      s << "       //color_map.rgb = vec3(0); // halo color\n";
      s << "       const float diff = (1 - abs(halo_percent_frag)) / hwp;\n";
      s << "       color_map.rgb = mix(vec3(0), color_map.rgb, diff*diff); // faded halo color\n\n";

      s << "       gl_FragDepth += " << bk::details::UBOLine::name_halo_depth_dependent_dmax() << " * sqrt(abs(halo_percent_frag)); // depth-dependent halo\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::lines::lineAO::mipmap::vert()
  { return lineAO::vert(); }

  std::string ShaderLibrary::lines::lineAO::mipmap::frag(bool firstPass)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("LINEAO MIPMAP");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 texcoord_frag;\n\n";

      s << "layout(binding = 7) uniform sampler2D td_map;\n";
      s << "layout(binding = 8) uniform sampler2D pa_map;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 td_mipmap0;\n";
      s << "layout(location = 1) out vec4 pa_mipmap0;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   const int s = " << (firstPass ? "2" : "4") << ";\n\n";

      s << "   const ivec2 screenpos = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));\n\n";

      s << "   if (screenpos.x >= " << bk::details::UBOGlobal::name_window_width() << "/s || screenpos.y >= " << bk::details::UBOGlobal::name_window_height() << "/s)\n";
      s << "   { discard; }\n\n";

      s << "   td_mipmap0 = vec4(0);\n";
      s << "   pa_mipmap0 = vec4(0);\n\n";

      s << "   for (int x = 0; x < s; ++x)\n";
      s << "   {\n";
      s << "       for (int y = 0; y < s; ++y)\n";
      s << "       {\n";
      s << "           td_mipmap0 += texelFetch(td_map, ivec2(s*screenpos.x + x, s*screenpos.y + y), 0).rgba;\n";
      s << "           pa_mipmap0 += texelFetch(pa_map, ivec2(s*screenpos.x + x, s*screenpos.y + y), 0).rgba;\n";
      s << "       }\n";
      s << "   }\n\n";

      s << "   td_mipmap0 /= s*s;\n";
      s << "   pa_mipmap0 /= s*s;\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== COLORBAR
  //====================================================================================================
  std::string ShaderLibrary::colorbar::details::default_vert(bool discrete)
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("COLORBAR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec3 color_in;\n";

      s << comment_region_output();
      s << "layout(location = 0)";
      if (discrete)
      { s << " flat"; }
      s << " out vec3 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_frag = color_in;\n";
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::colorbar::details::default_frag(bool discrete)
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("COLORBAR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0)";
      if (discrete)
      { s << " flat"; }
      s << " in vec3 color_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(color_frag, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::colorbar::vert()
  { return details::default_vert(false); }

  std::string ShaderLibrary::colorbar::vert_discrete()
  { return details::default_vert(true); }

  std::string ShaderLibrary::colorbar::frag()
  { return details::default_frag(false); }

  std::string ShaderLibrary::colorbar::frag_discrete()
  { return details::default_frag(true); }

  //====================================================================================================
  //===== TEXT
  //====================================================================================================
  std::string ShaderLibrary::text::vert_text()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("TEXT");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in vec2 texCoord_in;\n";
      s << ubo_definition_global();
      s << ubo_definition_text();

      s << comment_region_output();
      s << "layout(location = 0) out vec2 texCoord_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   texCoord_frag = texCoord_in;\n\n";

      s << "   float x = 2*position_in.x;\n";
      s << "   float y = 2*position_in.y;\n\n";

      s << "   if (" << bk::details::UBOText::name_pos_is_absolute() << " != 0)\n";
      s << "   {\n";
      s << "      x /= " << bk::details::UBOGlobal::name_window_width() << ";\n";
      s << "      y /= " << bk::details::UBOGlobal::name_window_height() << ";\n";
      s << "   }\n\n";

      s << "   x -= 1;\n";
      s << "   y -= 1;\n";
      s << "   gl_Position = vec4(x, y, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::text::vert_background()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("TEXT BACKGROUND");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_global();
      s << ubo_definition_text();

      s << comment_region_functions();
      s << function_main_begin();
      s << "   float x = 2*position_in.x;\n";
      s << "   float y = 2*position_in.y;\n\n";

      s << "   if (" << bk::details::UBOText::name_pos_is_absolute() << " != 0)\n";
      s << "   {\n";
      s << "      x /= " << bk::details::UBOGlobal::name_window_width() << ";\n";
      s << "      y /= " << bk::details::UBOGlobal::name_window_height() << ";\n";
      s << "   }\n\n";

      s << "   x -= 1;\n";
      s << "   y -= 1;\n";
      s << "   gl_Position = vec4(x, y, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::text::frag_text()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("TEXT");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 texCoord_frag;\n\n";
      s << "layout(binding = 1) uniform sampler2D text_tex;\n";
      s << ubo_definition_text();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(" << bk::details::UBOText::name_color_text_r() << ", " << bk::details::UBOText::name_color_text_g() << ", " << bk::details::UBOText::name_color_text_b() << ", " << bk::details::UBOText::name_color_text_a() << " * texture(text_tex, texCoord_frag).r);\n\n";

      s << discard_zero_alpha();
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::text::frag_background()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("TEXT BACKGROUND");
      s << version();

      s << comment_region_input();
      s << ubo_definition_text();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(" << bk::details::UBOText::name_color_background_r() << ", " << bk::details::UBOText::name_color_background_g() << ", " << bk::details::UBOText::name_color_background_b() << ", " << bk::details::UBOText::name_color_background_a() << ");\n";
      s << discard_zero_alpha();

      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== TRANSFER FUNCTION
  //====================================================================================================
  std::string ShaderLibrary::transfer_function::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("WINDOWING TRANSFER FUNCTION");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   gl_Position = vec4(position_in, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::transfer_function::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("WINDOWING TRANSFER FUNCTION");
      s << version();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(1, 0, 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== DVR
  //====================================================================================================
  std::string ShaderLibrary::dvr::cuboid::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("DVR CUBOID");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 color_in;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 entry_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   entry_frag = color_in;\n";
      s << "   gl_Position =  " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::dvr::cuboid::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("DVR CUBOID");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 entry_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out.rgb = entry_frag;\n";
      s << "   color_out.a = 1;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::dvr::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("DVR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 position_in;\n";
      s << "layout(location = 1) in vec3 color_in;\n";
      s << ubo_definition_global();

      s << comment_region_output();
      s << "layout(location = 0) out vec3 exit_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   exit_frag = color_in;\n";
      s << "   gl_Position =  " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_in, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::dvr::frag(bool fourDimensional)
  {
      std::stringstream s;

      {
          std::stringstream stype;
          stype << "DVR";
          if (fourDimensional)
          { stype << " 4D"; }

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 exit_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_dvr();
      s << "layout(binding = 1) uniform sampler2D entry_tex;\n";
      s << "layout(binding = 2) uniform sampler3D image_tex0;\n";
      if (fourDimensional)
      { s << "layout(binding = 4) uniform sampler3D image_tex1;\n"; }

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();

      s << "vec3 tf_color(const float d)\n";
      s << "{\n";
      s << "    const float w = clamp((d - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1);\n";
      s << "    return mix(vec3(0), vec3(1), w);\n";
      s << "}\n\n";

      s << "float tf_opacity(const float d)\n";
      s << "{ return clamp((d - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1); }\n\n";

      s << function_main_begin();
      s << "   const vec3 entry = texture(entry_tex, vec2(float(gl_FragCoord.x)/float(" << bk::details::UBOGlobal::name_window_width() << "), float(gl_FragCoord.y)/float(" << bk::details::UBOGlobal::name_window_height() << "))).rgb;\n";
      s << "   const vec3 ray_increment = (exit_frag - entry) / (" << bk::details::UBODVR::name_num_ray_samples() << "-1);\n\n";

      s << "   color_out.rgb = vec3(0);\n";
      s << "   color_out.a = 0;\n";
      s << "   vec3 samplePos = entry;\n\n";

      if (fourDimensional)
      {
          s << "   const float wimg = (" << bk::details::UBOGlobal::name_animation_current_time() << " / " << bk::details::UBODVR::name_temporal_resolution() << ") - " << bk::details::UBODVR::name_current_t0() << ";\n\n";
      }

      s << "   for (int i = 0; i < " << bk::details::UBODVR::name_num_ray_samples() << " && color_out.a < 0.98f; ++i)\n";
      s << "   {\n";
      s << "       samplePos += ray_increment;\n";

      if (!fourDimensional)
      { s << "      const float imgval = texture(image_tex0, samplePos).r;\n"; }
      else
      { s << "      const float imgval = mix(texture(image_tex0, samplePos).r, texture(image_tex1, samplePos).r, wimg);\n"; }

      s << "       const float w = clamp((imgval - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1);\n\n";

      s << "       color_out.rgb += mix(vec3(0), vec3(1), w) * w * (1 - color_out.a);\n";
      s << "       color_out.a += w * (1 - color_out.a);\n";
      s << "   }\n\n";

      s << "   color_out.a = 1;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::dvr::frag_mip(bool fourDimensional, bool useMinimum)
  {
      std::stringstream s;

      {
          std::stringstream stype;
          stype << "DVR MIP";
          if (fourDimensional)
          { stype << " 4D"; }
          stype << " useMinimum:";
          stype << (useMinimum ? "yes" : "no");

          s << comment_tag_fragment_shader(stype.str());
      }

      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 exit_frag;\n";
      s << ubo_definition_global();
      s << ubo_definition_dvr();
      s << "layout(binding = 1) uniform sampler2D entry_tex;\n";
      s << "layout(binding = 2) uniform sampler3D image_tex0;\n";
      if (fourDimensional)
      { s << "layout(binding = 4) uniform sampler3D image_tex1;\n"; }

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();

      s << "vec3 tf_color(const float d)\n";
      s << "{\n";
      s << "    const float w = clamp((d - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1);\n";
      s << "    return mix(vec3(0), vec3(1), w);\n";
      s << "}\n\n";

      s << "float tf_opacity(const float d)\n";
      s << "{ return clamp((d - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1); }\n\n";

      s << function_main_begin();
      s << "   const vec3 entry = texture(entry_tex, vec2(float(gl_FragCoord.x)/float(" << bk::details::UBOGlobal::name_window_width() << "), float(gl_FragCoord.y)/float(" << bk::details::UBOGlobal::name_window_height() << "))).rgb;\n";
      s << "   const vec3 ray_increment = (exit_frag - entry) / (" << bk::details::UBODVR::name_num_ray_samples() << "-1);\n\n";

      s << "   vec3 samplePos = entry;\n";
      s << "   float dataVal = 0;\n\n";

      if (fourDimensional)
      {
          s << "   const float wimg = (" << bk::details::UBOGlobal::name_animation_current_time() << " / " << bk::details::UBODVR::name_temporal_resolution() << ") - " << bk::details::UBODVR::name_current_t0() << ";\n\n";
      }

      s << "   for (int i = 0; i < " << bk::details::UBODVR::name_num_ray_samples() << " && color_out.a < 0.98f; ++i)\n";
      s << "   {\n";
      s << "       samplePos += ray_increment;\n";

      if (!fourDimensional)
      { s << "      const float imgval = texture(image_tex0, samplePos).r;\n"; }
      else
      { s << "      const float imgval = mix(texture(image_tex0, samplePos).r, texture(image_tex1, samplePos).r, wimg);\n"; }

      s << "      dataVal = " << (useMinimum ? "min" : "max") << "(dataVal, imgval);\n";
      s << "   }\n\n";

      s << "   color_out.rgb = mix(vec3(0), vec3(1), clamp((dataVal - " << bk::details::UBODVR::name_tf_center() << " + " << bk::details::UBODVR::name_tf_width() << ") / (2*" << bk::details::UBODVR::name_tf_width() << "), 0, 1));\n";
      s << "   color_out.a = 1;\n";
      s << function_main_end();

      return s.str();
  }

  //====================================================================================================
  //===== SLICE VIEW
  //====================================================================================================
  std::string ShaderLibrary::slice_view::details::default_glposition_vert()
  {
      std::stringstream s;

      s << "   const float qqi_ratio = float(" << bk::details::UBOSliceView::name_window_height() << ") / float(" << bk::details::UBOSliceView::name_window_width() << ");\n\n";

      s << "   const float voxelscale_ratio = " << bk::details::UBOSliceView::name_voxel_scale0() << " / " << bk::details::UBOSliceView::name_voxel_scale1() << ";\n";
      s << "   const float xratio = min(1, voxelscale_ratio*qqi_ratio*float(" << bk::details::UBOSliceView::name_xyzt_max0() << ")/float(" << bk::details::UBOSliceView::name_xyzt_max1() << "));\n";
      s << "   const float yratio = min(1, float(" << bk::details::UBOSliceView::name_xyzt_max1() << ")/(voxelscale_ratio*qqi_ratio*float(" << bk::details::UBOSliceView::name_xyzt_max0() << ")));\n\n";

      s << "   gl_Position.x = xratio*2*position_in.x/float(" << bk::details::UBOSliceView::name_xyzt_max0() << "-1) - xratio;\n";
      s << "   gl_Position.y = yratio*2*position_in.y/float(" << bk::details::UBOSliceView::name_xyzt_max1() << "-1) - yratio;\n";
      s << "   gl_Position.z = 0;\n";
      s << "   gl_Position.w = 1;\n\n";

      return s.str();
  }

  std::string ShaderLibrary::slice_view::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("SLICE VIEW");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_sliceview();

      s << "layout(binding = 2, std430) readonly buffer _Intensity\n";
      s << "{ float intensity[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << details::default_glposition_vert();
      s << "   const uint id = uint(position_in.y*(" << bk::details::UBOSliceView::name_xyzt_max0() << "+1) + position_in.x);\n";
      s << "   const float w = clamp((intensity[id] - " << bk::details::UBOSliceView::name_tf_center() << " + " << bk::details::UBOSliceView::name_tf_width() << ") / (2*" << bk::details::UBOSliceView::name_tf_width() << "), 0, 1);\n";
      s << "   color_frag = mix(vec3(0,0,0), vec3(1,1,1), w);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::slice_view::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("SLICE VIEW");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec3 color_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(color_frag, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::slice_view::rgb::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("SLICE VIEW (RGB)");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_sliceview();

      s << "layout(binding = 2, std430) readonly buffer _Intensity\n";
      s << "{ float intensity[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec3 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << details::default_glposition_vert();
      s << "   const uint id = uint(3*(position_in.y*(" << bk::details::UBOSliceView::name_xyzt_max0() << "+1) + position_in.x));\n\n";

      s << "   for (int i = 0; i < 3; ++i)\n";
      s << "   {\n";
      s << "       const float w = clamp((intensity[id+i] - " << bk::details::UBOSliceView::name_tf_center() << " + " << bk::details::UBOSliceView::name_tf_width() << ") / (2*" << bk::details::UBOSliceView::name_tf_width() << "), 0, 1);\n";
      s << "       color_frag[i] = mix(0.0f, 1.0f, w);\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::slice_view::rgb::frag()
  { return slice_view::frag(); }

  //====================================================================================================
  //===== SEGMENTATION
  //====================================================================================================
  std::string ShaderLibrary::segmentation::inout_overlay::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("SEGMENTATION INOUT OVERLAY");
      s << version();
      s << segmentation_bit_macros();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_sliceview();

      s << "layout(binding = 2, std430) readonly buffer _InOutSeg\n";
      s << "{ uint inoutseg[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << slice_view::details::default_glposition_vert();

      s << "   const uint id = uint(position_in.y*(" << UBOSliceView::name_xyzt_max0() << "+1) + position_in.x);\n\n";

      s << "   if (uint(inoutseg[id] & " << segmentation_name_inside_bit() << ") != 0)\n";
      s << "   { color_frag = vec4(0, 1, 0, 0.125); }\n";
      s << "   else if (uint(inoutseg[id] & " << segmentation_name_outside_bit() << ") != 0)\n";
      s << "   { color_frag = vec4(1, 0, 0, 0.125); }\n";
      s << "   else\n";
      s << "   { color_frag = vec4(0, 0, 0, 0); }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::segmentation::inout_overlay::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("SEGMENTATION INOUT OVERLAY");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec4 color_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = color_frag;\n";
      s << discard_low_alpha();
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::segmentation::contour::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("SEGMENTATION CONTOUR");
      s << version();
      s << segmentation_bit_macros();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_sliceview();

      s << "layout(binding = 2, std430) readonly buffer _InOutSeg\n";
      s << "{ uint inoutseg[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << slice_view::details::default_glposition_vert();

      s << "   color_frag = vec4(1,1,0,0);\n\n";

      s << "   const uint id = uint((position_in.y)*(" << bk::details::UBOSliceView::name_xyzt_max0() << "+1) + (position_in.x));\n";
      s << "   if ((inoutseg[id] & " << segmentation_name_segmentation_bit() << ") != 0)\n";
      s << "   {\n";
      s << "       // determine if is boundary voxel\n\n";

      s << "       vec2 temp_pos = position_in;\n";
      s << "       for (int dx = -1; dx <= 1; ++dx)\n";
      s << "       {\n";
      s << "           temp_pos.x = position_in.x + dx;\n";
      s << "           for (int dy = -1; dy <= 1; ++dy)\n";
      s << "           {\n";
      s << "               temp_pos.y = position_in.y + dy;\n\n";

      s << "               if (temp_pos.x >= 0 && temp_pos.x <= " << bk::details::UBOSliceView::name_xyzt_max0() << " &&\n";
      s << "                   temp_pos.y >= 0 && temp_pos.y <= " << bk::details::UBOSliceView::name_xyzt_max1() << ")\n";
      s << "               {\n";
      s << "                   const uint nid = uint(temp_pos.y*(" << bk::details::UBOSliceView::name_xyzt_max0() << "+1) + temp_pos.x);\n";
      s << "                   if ((inoutseg[nid] & " << segmentation_name_segmentation_bit() << ") == 0)\n";
      s << "                   {\n";
      s << "                       color_frag.a = 0.5;\n";
      s << "                       return;\n";
      s << "                   }\n";
      s << "               }\n";
      s << "           }\n";
      s << "       }\n";
      s << "   }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::segmentation::contour::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("SEGMENTATION CONTOUR");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec4 color_frag;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = color_frag;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::segmentation::live_threshold::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("LIVE THRESHOLD");
      s << version();
      s << segmentation_bit_macros();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << ubo_definition_sliceview();

      s << "layout(binding = 2, std430) readonly buffer _Intensity\n";
      s << "{ float intensity[]; };\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_frag;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << slice_view::details::default_glposition_vert();

      s << "   color_frag = vec4(1,1,0,0);\n\n";

      s << "   const uint id = uint((position_in.y)*(" << bk::details::UBOSliceView::name_xyzt_max0() << "+1) + (position_in.x));\n\n";

      s << "   if (intensity[id] >= " << bk::details::UBOSliceView::name_threshold_lower() << " && intensity[id] <= " << bk::details::UBOSliceView::name_threshold_upper() << ")\n";
      s << "   { color_frag.a = 0.5; }\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::segmentation::live_threshold::frag()
  { return contour::frag(); }

  //====================================================================================================
  //===== PLOT
  //====================================================================================================
  std::string ShaderLibrary::plot::area::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("AREA PLOT");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";
      s << "layout(location = 1) in float dy;\n";
      s << ubo_definition_global();
      s << ubo_definition_plot();
      s << ubo_definition_plot_area();

      s << comment_region_output();
      s << "layout(location = 0) out flat int above_frag;\n";

      s << comment_region_functions();
      s << function_plot_data_to_screen();
      s << function_main_begin();
      s << "   above_frag = dy >= 0 ? 1 : 0;\n";
      s << "   gl_Position = vec4(plot_data_to_screen(position_in), 0, 1);\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::plot::area::frag()
  {
      std::stringstream s;

      s << comment_tag_fragment_shader("AREA PLOT");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in flat int above_frag;\n";
      s << ubo_definition_plot_area();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();

      s << "   if (above_frag <= 0)\n";
      s << "   { color_out = vec4(" << details::UBOPlotArea::name_color_area_below_r() << ", " << details::UBOPlotArea::name_color_area_below_g() << ", " << details::UBOPlotArea::name_color_area_below_b() << ", " << details::UBOPlotArea::name_color_area_below_a() << "); }\n";
      s << "   else\n";
      s << "   { color_out = vec4(" << details::UBOPlotArea::name_color_area_above_r() << ", " << details::UBOPlotArea::name_color_area_above_g() << ", " << details::UBOPlotArea::name_color_area_above_b() << ", " << details::UBOPlotArea::name_color_area_above_a() << "); }\n";

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::plot::line::vert()
  {
      std::stringstream s;

      s << comment_tag_vertex_shader("PLOT LINE");
      s << version();

      s << comment_region_input();
      s << "layout(location = 0) in vec2 position_in;\n";

      s << comment_region_output();
      s << "layout(location = 0) out vec2 position_geom;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   position_geom = position_in;\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::plot::line::geom(bool lines_adjacency)
  {
      std::stringstream s;

      s << comment_tag_geometry_shader("PLOT LINE (lines_adjacency:" + std::string(lines_adjacency ? "yes" : "no") + ")");
      s << version();

      s << comment_region_input();

      s << "layout(location = 0) in vec2 position_geom[";
      if (lines_adjacency)
      { s << "4"; }
      else
      { s << "2"; }
      s << "];\n";

      s << ubo_definition_global();
      s << ubo_definition_plot();
      s << ubo_definition_plot_line();

      if (lines_adjacency)
      { s << geom_layout_in_lines_adjacency(); }
      else
      { s << geom_layout_in_lines(); }

      s << comment_region_output();
      s << geom_layout_out_triangle_strip(4);

      s << comment_region_functions();
      s << function_plot_data_to_screen();
      s << function_main_begin();

      const std::string ww = details::UBOGlobal::name_window_width();
      const std::string wh = details::UBOGlobal::name_window_height();
      const std::string ssaa = details::UBOGlobal::name_ssaa_factor();

      s << "   const vec2 l = {\n";
      s << "      float(" << ssaa << ") / " << ww << ",\n";
      s << "      float(" << ssaa << ") / " << wh << "\n";
      s << "   };\n\n";

      if (lines_adjacency)
      {
          s << "   const vec2 tangent[2] = {\n";
          s << "       normalize(plot_data_to_screen(position_geom[2]) - plot_data_to_screen(position_geom[0])),\n";
          s << "       normalize(plot_data_to_screen(position_geom[3]) - plot_data_to_screen(position_geom[1]))\n";
          s << "   };\n";

          s << "   const vec2 ortho[2] = {\n";
          s << "       vec2(-tangent[0].y, tangent[0].x),\n";
          s << "       vec2(-tangent[1].y, tangent[1].x)\n";
          s << "   };\n\n";
      }
      else
      {
          s << "   const vec2 tangent = normalize(plot_data_to_screen(position_geom[1]) - plot_data_to_screen(position_geom[0]));\n";
          s << "   const vec2 ortho = vec2(-tangent.y, tangent.x);\n\n";
      }

      const std::string pos0 = lines_adjacency ? "position_geom[1]" : "position_geom[0]";
      const std::string pos1 = lines_adjacency ? "position_geom[2]" : "position_geom[1]";
      const std::string ortho0 = lines_adjacency ? "ortho[0]" : "ortho";
      const std::string ortho1 = lines_adjacency ? "ortho[1]" : "ortho";
      const std::string linew = details::UBOPlotLine::name_line_width();

      s << "   vec2 p = plot_data_to_screen(" << pos0 << ");\n";
      s << "   gl_Position = vec4(p[0] + l[0]*" << linew << "*" << ortho0 << "[0], p[1] + l[1]*" << linew << "*" << ortho0 << "[1], 0, 1);\n";
      s << "   EmitVertex();\n\n";

      s << "   p = plot_data_to_screen(" << pos1 << ");\n";
      s << "   gl_Position = vec4(p[0] + l[0]*" << linew << "*" << ortho1 << "[0], p[1] + l[1]*" << linew << "*" << ortho1 << "[1], 0, 1);\n";
      s << "   EmitVertex();\n\n";

      s << "   p = plot_data_to_screen(" << pos0 << ");\n";
      s << "   gl_Position = vec4(p[0] - l[0]*" << linew << "*" << ortho0 << "[0], p[1] - l[1]*" << linew << "*" << ortho0 << "[1], 0, 1);\n";
      s << "   EmitVertex();\n\n";

      s << "   p = plot_data_to_screen(" << pos1 << ");\n";
      s << "   gl_Position = vec4(p[0] - l[0]*" << linew << "*" << ortho1 << "[0], p[1] - l[1]*" << linew << "*" << ortho1 << "[1], 0, 1);\n";
      s << "   EmitVertex();\n";

      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::plot::line::frag()
  {
      const std::string cr = details::UBOPlotLine::name_color_r();
      const std::string cg = details::UBOPlotLine::name_color_g();
      const std::string cb = details::UBOPlotLine::name_color_b();
      const std::string ca = details::UBOPlotLine::name_color_a();

      std::stringstream s;

      s << comment_tag_fragment_shader("PLOT LINE");
      s << version();

      s << comment_region_input();
      s << ubo_definition_plot_line();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(" << cr << ", " << cg << ", " << cb << ", " << ca << ");\n";
      s << function_main_end();

      return s.str();
  }

  /*std::string ShaderLibrary::plot::stdev::vert()
  { return line::vert(); }

  std::string ShaderLibrary::plot::stdev::geom()
  { return line::geom(false); }

  std::string ShaderLibrary::plot::stdev::frag()
  {
      const std::string cr = details::UBOPlotArea::name_stdevcolorr();
      const std::string cg = details::UBOPlotArea::name_stdevcolorg();
      const std::string cb = details::UBOPlotArea::name_stdevcolorb();
      const std::string ca = details::UBOPlotArea::name_stdevcolora();

      std::stringstream s;

      s << comment_tag_fragment_shader("PLOT STDEV");
      s << version();

      s << comment_region_input();
      s << ubo_definition_plot_area();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(" << cr << ", " << cg << ", " << cb << ", " << ca << ");\n";
      s << function_main_end();

      return s.str();
  }*/

  std::string ShaderLibrary::plot::axis::vert()
  { return line::vert(); }

  std::string ShaderLibrary::plot::axis::geom()
  { return line::geom(false); }

  std::string ShaderLibrary::plot::axis::frag()
  {
      const std::string cr = details::UBOPlotLine::name_color_r();
      const std::string cg = details::UBOPlotLine::name_color_g();
      const std::string cb = details::UBOPlotLine::name_color_b();
      const std::string ca = details::UBOPlotLine::name_color_a();

      std::stringstream s;

      s << comment_tag_fragment_shader("PLOT AXIS");
      s << version();

      s << comment_region_input();
      s << ubo_definition_plot_line();

      s << comment_region_output();
      s << "layout(location = 0) out vec4 color_out;\n";

      s << comment_region_functions();
      s << function_main_begin();
      s << "   color_out = vec4(" << cr << ", " << cg << ", " << cb << ", " << ca << ");\n";
      s << function_main_end();

      return s.str();
  }

  std::string ShaderLibrary::plot::marker::vert()
  { return line::vert(); }

  std::string ShaderLibrary::plot::marker::geom()
  { return line::geom(false); }

  std::string ShaderLibrary::plot::marker::frag()
  { return line::frag(); }

  std::string ShaderLibrary::plot::ticks::vert()
  { return line::vert(); }

  std::string ShaderLibrary::plot::ticks::geom()
  { return line::geom(false); }

  std::string ShaderLibrary::plot::ticks::frag()
  { return plot::axis::frag(); }

  //====================================================================================================
  //===== VECTOR VIEW
  //====================================================================================================
  struct vector_view
  {
      std::string ShaderLibrary::vert()
      {
          // todo
          return "";
      }

      std::string ShaderLibrary::geom(bool useColor)
      {
          // todo
          return "";
      }

      std::string ShaderLibrary::frag(bool useColor)
      {
          // todo
          return "";
      }
  }; // struct vector_view
} // namespace bk::details
