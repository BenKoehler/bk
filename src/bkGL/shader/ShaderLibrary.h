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

#ifndef BKGL_SHADERLIBRARY_H
#define BKGL_SHADERLIBRARY_H

#include <string>
#include <string_view>

// -------------------- forward declaration
namespace bk
{
  class RegisteredValue;

  class UBO;

  namespace details
  {
    class UBOSpecialization;
  } // namespace details
} // namespace bk
// -------------------- forward declaration END

namespace bk::details
{
  struct ShaderLibrary
  {
      /// @{ -------------------------------------------------- COMMENTS
      [[nodiscard]] static std::string comment_tag_shader(std::string_view type, std::string_view name);
      [[nodiscard]] static std::string comment_tag_vertex_shader(std::string_view name);
      [[nodiscard]] static std::string comment_tag_fragment_shader(std::string_view name);
      [[nodiscard]] static std::string comment_tag_geometry_shader(std::string_view name);
      [[nodiscard]] static std::string comment_region(std::string_view name);
      [[nodiscard]] static std::string comment_region_input();
      [[nodiscard]] static std::string comment_region_output();
      [[nodiscard]] static std::string comment_region_functions();
      /// @}

      /// @{ -------------------------------------------------- VERSION
      [[nodiscard]] static std::string version();
      /// @}

      /// @{ -------------------------------------------------- DISCARD ALPHA
      [[nodiscard]] static std::string alpha_discard_thresh();
      [[nodiscard]] static std::string discard_low_alpha();
      [[nodiscard]] static std::string discard_zero_alpha();

      [[nodiscard]] static std::string early_fragment_test();
      /// @}

      /// @{ -------------------------------------------------- FUNCTION MAIN
      [[nodiscard]] static std::string function_main_begin();
      [[nodiscard]] static std::string function_main_end();
      /// @}

      /// @{ -------------------------------------------------- SPECIAL FUNCTIONS
      [[nodiscard]] static std::string function_grid_to_list_id();
      [[nodiscard]] static std::string function_camera_position();
      [[nodiscard]] static std::string function_linearize_depth();
      [[nodiscard]] static std::string function_plot_data_to_screen();
      /// @}

      /// @{ -------------------------------------------------- UBO
      [[nodiscard]] static std::string ubo_definition(const UBO& ubo, std::string_view bufferName, unsigned int bufferBase);
      [[nodiscard]] static std::string ubo_definition(const UBOSpecialization& ubo);
      [[nodiscard]] static std::string ubo_definition_dvr();
      [[nodiscard]] static std::string ubo_definition_global();
      [[nodiscard]] static std::string ubo_definition_line();
      [[nodiscard]] static std::string ubo_definition_area_plot();
      [[nodiscard]] static std::string ubo_definition_plot();
      [[nodiscard]] static std::string ubo_definition_plot_line();
      [[nodiscard]] static std::string ubo_definition_phong();
      [[nodiscard]] static std::string ubo_definition_selection_sphere();
      [[nodiscard]] static std::string ubo_definition_sliceview();
      [[nodiscard]] static std::string ubo_definition_text();
      /// @}

      /// @{ -------------------------------------------------- OIT
      [[nodiscard]] static std::string oit_definition_atomic_counter();
      [[nodiscard]] static std::string oit_definition_fragments();
      [[nodiscard]] static std::string oit_definition_linked_list();
      [[nodiscard]] static std::string oit_definition_max_fragments();
      [[nodiscard]] static std::string oit_definition();
      [[nodiscard]] static std::string oit_var_max_fragments();
      [[nodiscard]] static std::string oit_assign_from_color_out();
      /// @}

      /// @{ -------------------------------------------------- GEOM LAYOUTS IN
      [[nodiscard]] static std::string geom_layout_in_points();
      [[nodiscard]] static std::string geom_layout_in_lines();
      [[nodiscard]] static std::string geom_layout_in_lines_adjacency();
      [[nodiscard]] static std::string geom_layout_in_triangles();
      [[nodiscard]] static std::string geom_layout_in_triangles_adjacency();
      /// @}

      /// @{ -------------------------------------------------- GEOM LAYOUTS OUT
      [[nodiscard]] static std::string geom_layout_out_points(unsigned int maxNumVertices);
      [[nodiscard]] static std::string geom_layout_out_line_strip(unsigned int maxNumVertices);
      [[nodiscard]] static std::string geom_layout_out_triangle_strip(unsigned int maxNumVertices);
      /// @}

      /// @{ -------------------------------------------------- SEGMENTATION
      [[nodiscard]] static std::string segmentation_bit_macros();
      [[nodiscard]] static std::string segmentation_name_inside_bit();
      [[nodiscard]] static std::string segmentation_name_outside_bit();
      [[nodiscard]] static std::string segmentation_name_segmentation_bit();
      /// @}

      //====================================================================================================
      //===== TEXTURE
      //====================================================================================================
      struct render_texture
      {
          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag(unsigned int texture_unit_id, bool useAlpha);
      };

      //====================================================================================================
      //===== BACKGROUND
      //====================================================================================================
      struct background
      {
          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag();
      };

      //====================================================================================================
      //===== OIT
      //====================================================================================================
      struct oit
      {
          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag_clear_linked_list();
          [[nodiscard]] static std::string frag_render();
      };

      //====================================================================================================
      //===== SSAA
      //====================================================================================================
      struct ssaa
      {
          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag();
      };

      //====================================================================================================
      //===== MESH
      //====================================================================================================
      struct mesh
      {
          struct phong
          {
              struct details
              {
                  [[nodiscard]] static std::string default_frag(bool assignAmbient, bool assignDiffuse, bool assignSpecular, bool defineColorVec3);
                  [[nodiscard]] static std::string default_frag_ghosted();
                  [[nodiscard]] static std::string default_frag_get_color_from_attribute();
              };

              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string vert_color();
              [[nodiscard]] static std::string frag();
              [[nodiscard]] static std::string frag_color();
              [[nodiscard]] static std::string frag_ghosted();
              [[nodiscard]] static std::string frag_ghosted_oit();
              [[nodiscard]] static std::string frag_ghosted_color();
              [[nodiscard]] static std::string frag_ghosted_color_oit();
          };

          struct silhouette
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string vert_color();
              [[nodiscard]] static std::string frag();
              [[nodiscard]] static std::string frag_color();
          };

          struct wireframe
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom();
              [[nodiscard]] static std::string geom_color();
              [[nodiscard]] static std::string frag();
              [[nodiscard]] static std::string frag_color();
          };

          struct picking
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string vert_phong();
              [[nodiscard]] static std::string frag();
              [[nodiscard]] static std::string vert_current_selection();
              [[nodiscard]] static std::string geom_current_selection();
              [[nodiscard]] static std::string frag_current_selection();
          };
      }; // mesh

      //====================================================================================================
      //===== LINES
      //====================================================================================================
      struct lines
      {
          struct details
          {
              [[nodiscard]] static std::string set_color_out_rgb_to_line_color();
          };

          struct debug
          {
              [[nodiscard]] static std::string vert_passthrough(bool linesHaveTime, bool colorEnabled);
              [[nodiscard]] static std::string frag_passthrough(bool linesHaveTime, bool colorEnabled);
          };

          [[nodiscard]] static std::string vert(bool linesHaveTime, bool colorEnabled);
          [[nodiscard]] static std::string geom(bool linesHaveTime, bool animationEnabled, bool colorEnabled);
          [[nodiscard]] static std::string frag_transparent(bool linesHaveTime, bool animationEnabled, bool colorEnabled, bool oitEnabled);
          [[nodiscard]] static std::string frag_opaque(bool linesHaveTime, bool animationEnabled, bool colorEnabled);

          struct lineAO
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();

              struct gbuffer
              {
                  [[nodiscard]] static std::string vert(bool linesHaveTime, bool colorEnabled);
                  [[nodiscard]] static std::string geom(bool linesHaveTime, bool animationEnabled, bool colorEnabled);
                  [[nodiscard]] static std::string frag(bool linesHaveTime, bool animationEnabled, bool colorEnabled);
              };

              struct mipmap
              {
                  [[nodiscard]] static std::string vert();
                  [[nodiscard]] static std::string frag(bool firstPass);
              };
          }; // struct lineAO
      }; // struct lines

      //====================================================================================================
      //===== COLORBAR
      //====================================================================================================
      struct colorbar
      {
          struct details
          {
              [[nodiscard]] static std::string default_vert(bool discrete);
              [[nodiscard]] static std::string default_frag(bool discrete);
          };

          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string vert_discrete();
          [[nodiscard]] static std::string frag();
          [[nodiscard]] static std::string frag_discrete();
      }; // struct colorbar

      //====================================================================================================
      //===== TEXT
      //====================================================================================================
      struct text
      {
          [[nodiscard]] static std::string vert_text();
          [[nodiscard]] static std::string vert_background();
          [[nodiscard]] static std::string frag_text();
          [[nodiscard]] static std::string frag_background();
      }; // struct text

      //====================================================================================================
      //===== TRANSFER FUNCTION
      //====================================================================================================
      struct transfer_function
      {
          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag();
      }; // struct transfer_function

      //====================================================================================================
      //===== DVR
      //====================================================================================================
      struct dvr
      {
          struct cuboid
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };

          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag(bool fourDimensional);
          [[nodiscard]] static std::string frag_mip(bool fourDimensional, bool useMinimum);
      }; // struct dvr

      //====================================================================================================
      //===== SLICE VIEW
      //====================================================================================================
      struct slice_view
      {
          struct details
          {
              [[nodiscard]] static std::string default_glposition_vert();
          };

          [[nodiscard]] static std::string vert();
          [[nodiscard]] static std::string frag();

          struct rgb
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };
      }; // struct slice_view

      //====================================================================================================
      //===== SEGMENTATION
      //====================================================================================================
      struct segmentation
      {
          struct inout_overlay
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };

          struct contour
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };

          struct live_threshold
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };

      }; // struct segmentation

      //====================================================================================================
      //===== PLOT
      //====================================================================================================
      struct plot
      {
          struct area
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string frag();
          };

          struct line
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom(bool use_area_ubo);
              [[nodiscard]] static std::string frag(bool use_area_ubo);
          };

          struct stdev
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom();
              [[nodiscard]] static std::string frag();
          };

          struct axis
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom(bool use_area_ubo);
              [[nodiscard]] static std::string frag(bool use_area_ubo);
          };

          struct marker
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom();
              [[nodiscard]] static std::string frag();
          };

          struct ticks
          {
              [[nodiscard]] static std::string vert();
              [[nodiscard]] static std::string geom(bool use_area_ubo);
              [[nodiscard]] static std::string frag(bool use_area_ubo);
          };
      };
  }; // class ShaderLibrary
} // namespace bk::details

#endif //BKGL_SHADERLIBRARY_H
