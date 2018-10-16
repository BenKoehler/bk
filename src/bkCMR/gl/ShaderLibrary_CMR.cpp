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

#include <bkCMR/gl/ShaderLibrary_CMR.h>

#include <sstream>

#include <bkCMR/gl/UBOFlowJetView.h>
#include <bkCMR/gl/UBOMeasuringPlanePreview.h>
#include <bkCMR/gl/UBOPressureView.h>
#include <bkGL/UBOGlobal.h>

namespace bk
{
  inline namespace cmr
  {
    /// @{ -------------------------------------------------- UBO
    std::string ShaderLibrary_CMR::ubo_definition_flowjet_view()
    {
        return ubo_definition(bk::UBOFlowJetView(
        #ifdef BK_LIB_QT_AVAILABLE
            nullptr
        #endif
        ));
    }

    std::string ShaderLibrary_CMR::ubo_definition_measuring_plane_preview()
    {
        return ubo_definition(bk::UBOMeasuringPlanePreview(
        #ifdef BK_LIB_QT_AVAILABLE
            nullptr
        #endif
        ));
    }

    std::string ShaderLibrary_CMR::ubo_definition_pressure_view()
    {
        return ubo_definition(bk::UBOPressureView(
        #ifdef BK_LIB_QT_AVAILABLE
            nullptr
        #endif
        ));
    }
    /// @}

    //====================================================================================================
    //===== MEASURING PLANE PREVIEW
    //====================================================================================================
    std::string ShaderLibrary_CMR::measuring_plane_preview::vert()
    {
        std::stringstream s;

        s << comment_tag_vertex_shader("MEASURING PLANE PREVIEW");
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
        s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " vec4(position_in, 1);\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::measuring_plane_preview::frag()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("MEASURING PLANE PREVIEW");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_frag;\n";
        s << "layout(location = 1) in vec3 normal_frag;\n";
        s << ubo_definition_measuring_plane_preview();

        s << comment_region_output();
        s << "layout(location = 0) out vec4 color_out;\n";

        s << comment_region_functions();
        s << function_camera_position();
        s << function_main_begin();
        //s << "   color_out = vec4(" << bk::UBOMeasuringPlanePreview::name_color_r() << ", " << bk::UBOMeasuringPlanePreview::name_color_g() << ", " << bk::UBOMeasuringPlanePreview::name_color_b() << ", " << bk::UBOMeasuringPlanePreview::name_color_a() << ");\n";

        s << "   const vec3 camPos = camera_position();\n";
        s << "   const vec3 E = normalize(position_frag - camPos);\n";
        s << "   const vec3 P = position_frag; // world coordinates\n";
        s << "   const vec3 L = -E; // // headlight\n";
        s << "   const vec3 N = normalize(normal_frag);\n";
        s << "   const vec3 R = normalize(reflect(L, N)); // for specular\n\n";

        s << "   vec3 color = vec3(" << bk::UBOMeasuringPlanePreview::name_color_r() << ", " << bk::UBOMeasuringPlanePreview::name_color_g() << ", " << bk::UBOMeasuringPlanePreview::name_color_b()
          << ");\n\n";

        s << "   // alpha\n";
        s << "   color_out.a = 1;\n\n";

        s << "   // ambient\n";
        s << "   color_out.rgb = 0.1 * color;\n\n";

        s << "   // diffuse\n";
        s << "   const float NdotL = dot(N, L);\n";
        s << "   color_out.rgb += abs(NdotL) * (NdotL >= 0 ? 1.0f : 0.75f) * color;\n\n";

        s << "   // specular\n";
        s << "   const vec3 light_color = vec3(" << bk::UBOMeasuringPlanePreview::name_lightcolor_r() << ", " << bk::UBOMeasuringPlanePreview::name_lightcolor_g() << ", "
          << bk::UBOMeasuringPlanePreview::name_lightcolor_b() << ");\n";
        s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << bk::UBOMeasuringPlanePreview::name_shininess() << ");\n";

        s << function_main_end();

        return s.str();
    }

    //====================================================================================================
    //===== PRESSURE PREVIEW
    //====================================================================================================
    std::string ShaderLibrary_CMR::pressure::vert()
    {
        std::stringstream s;

        s << comment_tag_vertex_shader("PRESSURE VIEW");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_in;\n";
        s << ubo_definition_global();

        s << comment_region_functions();
        s << function_main_begin();
        s << "   gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " vec4(position_in, 1);\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::pressure::frag()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("PRESSURE VIEW");
        s << version();

        s << comment_region_input();
        s << ubo_definition_global();
        s << ubo_definition_pressure_view();
        s << "layout(binding = 1) uniform sampler2D entry_tex;\n";
        s << "layout(binding = 3) uniform sampler2D exit_tex;\n";
        s << "layout(binding = 2) uniform sampler3D image_tex0;\n";
        s << "layout(binding = 4) uniform sampler3D image_tex1;\n\n";

        s << comment_region_output();
        s << "layout(location = 0) out vec4 color_out;\n";

        s << comment_region_functions();
        s << function_main_begin();
        s << "   const vec2 texcoord = vec2(float(gl_FragCoord.x)/float(" << bk::details::UBOGlobal::name_window_width() << "), float(gl_FragCoord.y)/float("
          << bk::details::UBOGlobal::name_window_height() << "));\n";
        s << "   const vec3 entry = texture(entry_tex, texcoord).rgb;\n";
        s << "   const vec3 exit = texture(exit_tex, texcoord).rgb;\n";
        s << "   const vec3 ray_increment = (exit - entry) / (" << UBOPressureView::name_num_ray_samples() << "-1);\n\n";

        s << "   vec3 samplePos = entry;\n";
        s << "   float minDataVal = 10000000;\n";
        s << "   float maxDataVal = -minDataVal;\n\n";

        s << "   const float wimg = (" << bk::details::UBOGlobal::name_animation_current_time() << " / " << UBOPressureView::name_temporal_resolution() << ") - " << UBOPressureView::name_current_t0()
          << ";\n\n";

        s << "   for (int i = 0; i < " << UBOPressureView::name_num_ray_samples() << "; ++i)\n\n";
        s << "   {\n";
        s << "       samplePos += ray_increment;\n";
        s << "       const float imgval = mix(texture(image_tex0, samplePos).r, texture(image_tex1, samplePos).r, wimg);\n";

        s << "       minDataVal = min(minDataVal, imgval);\n";
        s << "       maxDataVal = max(maxDataVal, imgval);\n";
        s << "   }\n\n";

        s << "   if (minDataVal == 0 && maxDataVal == 0)\n";
        s << "   { discard; }\n\n";

        s << "   const float diffmin = abs(" << UBOPressureView::name_referenceVal() << " - minDataVal);\n";
        s << "   const float diffmax = abs(" << UBOPressureView::name_referenceVal() << " - maxDataVal);\n\n";

        s << "   color_out.a = 1;\n\n";

        s << "   if (diffmin >= diffmax)\n";
        s << "   { color_out.rgb = mix(vec3(1), vec3(0,0,1), min(1, diffmin / " << UBOPressureView::name_scaleVal() << ")); }\n";
        s << "   else\n";
        s << "   { color_out.rgb = mix(vec3(1), vec3(1,0,0), min(1, diffmax / " << UBOPressureView::name_scaleVal() << ")); }\n";
        s << function_main_end();

        return s.str();
    }

    //====================================================================================================
    //===== FLOW JET
    //====================================================================================================
    std::string ShaderLibrary_CMR::flow_jet::area::vert()
    {
        std::stringstream s;

        s << comment_tag_vertex_shader("FLOW JET AREA");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in float timeID_in;\n";
        s << "layout(location = 1) in vec3 p0_t0_in; // positions\n";
        s << "layout(location = 2) in vec3 p0_t1_in;\n";
        s << "layout(location = 3) in vec3 n0_t0_in; // normals\n";
        s << "layout(location = 4) in vec3 n0_t1_in;\n";
        s << "layout(location = 5) in float v0_t0_in; // velocities\n";
        s << "layout(location = 6) in float v0_t1_in;\n";
        s << "layout(location = 7) in float halo_percent_in; // halo\n";

        s << comment_region_output();
        s << "layout(location = 0) out float timeID_geom;\n";
        s << "layout(location = 1) out vec3 p0_t0_geom; // positions\n";
        s << "layout(location = 2) out vec3 p0_t1_geom;\n";
        s << "layout(location = 3) out vec3 n0_t0_geom; // normals\n";
        s << "layout(location = 4) out vec3 n0_t1_geom;\n";
        s << "layout(location = 5) out float v0_t0_geom; // velocities\n";
        s << "layout(location = 6) out float v0_t1_geom;\n";
        s << "layout(location = 7) out float halo_percent_geom; // halo\n";

        s << comment_region_functions();
        s << function_main_begin();
        s << "   timeID_geom = timeID_in;\n";
        s << "   p0_t0_geom = p0_t0_in;\n";
        s << "   p0_t1_geom = p0_t1_in;\n";
        s << "   n0_t0_geom = n0_t0_in;\n";
        s << "   n0_t1_geom = n0_t1_in;\n";
        s << "   v0_t0_geom = v0_t0_in;\n";
        s << "   v0_t1_geom = v0_t1_in;\n";
        s << "   halo_percent_geom = halo_percent_in;\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::area::geom()
    {
        std::stringstream s;

        s << comment_tag_geometry_shader("FLOW JET AREA");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in float timeID_geom[3];\n";
        s << "layout(location = 1) in vec3 p0_t0_geom[3]; // positions\n";
        s << "layout(location = 2) in vec3 p0_t1_geom[3];\n";
        s << "layout(location = 3) in vec3 n0_t0_geom[3]; // normals\n";
        s << "layout(location = 4) in vec3 n0_t1_geom[3];\n";
        s << "layout(location = 5) in float v0_t0_geom[3]; // velocities\n";
        s << "layout(location = 6) in float v0_t1_geom[3];\n";
        s << "layout(location = 7) in float halo_percent_geom[3]; // halo_perc\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();
        s << geom_layout_in_triangles();

        s << comment_region_output();
        s << "layout(location = 0) out vec3 position_frag;\n";
        s << "layout(location = 1) out vec3 normal_frag;\n";
        s << "layout(location = 2) out float velocity_frag;\n";
        s << "layout(location = 3) out float halo_percent_frag;\n";
        s << geom_layout_out_triangle_strip(3);

        s << comment_region_functions();
        s << function_main_begin();
        s << "   const float time_scaled = " << bk::details::UBOGlobal::name_animation_current_time() << " / " << UBOFlowJetView::name_temporal_scale() << ";\n";
        s << "   const float t0 = floor(time_scaled);\n";
        s << "   const float t1 = ceil(time_scaled);\n\n";

        s << "   if (timeID_geom[0] >= t0 && timeID_geom[0] < t1)\n";
        s << "   {\n";
        s << "       const float tw = time_scaled - t0;\n\n";

        s << "       for (int i = 0; i < 3; ++i)\n";
        s << "       {\n";
        s << "           position_frag = mix(p0_t0_geom[i], p0_t1_geom[i], tw);\n";
        s << "           normal_frag = mix(n0_t0_geom[i], n0_t1_geom[i], tw);\n";
        s << "           velocity_frag = mix(v0_t0_geom[i], v0_t1_geom[i], tw);\n";
        s << "           halo_percent_frag = halo_percent_geom[i];\n";
        s << "           gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag, 1);\n";
        s << "           EmitVertex();\n";
        s << "       }\n";
        s << "   }\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::area::frag_opaque()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("FLOW JET AREA OPAQUE");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_frag;\n";
        s << "layout(location = 1) in vec3 normal_frag;\n";
        s << "layout(location = 2) in float velocity_frag;\n";
        s << "layout(location = 3) in float halo_percent_frag;\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();

        s << comment_region_output();
        s << "layout(location = 0) out vec4 color_out;\n";

        s << comment_region_functions();
        s << function_camera_position();
        s << function_main_begin();
        s << "   if (velocity_frag < " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ") discard;\n\n";

        s << "   color_out = vec4(0,0,0,1);\n";
        s << "   vec3 color = vec3(0);\n\n";

        s << "   if (abs(halo_percent_frag) >= 1.0f-" << UBOFlowJetView::name_area_halo_width_in_percent() << ")\n";
        s << "   {\n";
        s << "       const float diff = (1 - abs(halo_percent_frag)) / " << UBOFlowJetView::name_area_halo_width_in_percent() << ";\n;";
        s << "       color = mix(vec3(0), vec3(" << UBOFlowJetView::name_area_color_r() << ", " << UBOFlowJetView::name_area_color_g() << ", " << UBOFlowJetView::name_area_color_b()
          << "), diff*diff); // faded halo color\n";
        s << "   }\n";
        s << "   else\n";
        s << "   { color = vec3(" << UBOFlowJetView::name_area_color_r() << ", " << UBOFlowJetView::name_area_color_g() << ", " << UBOFlowJetView::name_area_color_b() << "); }\n\n";

        s << "   const vec3 camPos = camera_position();\n\n";

        s << "   const vec3 E = normalize(position_frag - camPos);\n";
        s << "   const vec3 L = E; // headlight\n\n";

        s << "   const float NdotL = dot(normal_frag, L);\n";
        s << "   const float k = min(0.5 /*ambient*/ + abs(NdotL) /*diffuse*/, 1);\n\n";

        s << "   color_out.rgb = k * color;\n\n";

        s << "   if (NdotL >= 0) // back face\n";
        s << "   { color_out.rgb *= 0.66; }\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::area::frag_transparent()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("FLOW JET AREA OPAQUE");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_frag;\n";
        s << "layout(location = 1) in vec3 normal_frag;\n";
        s << "layout(location = 2) in float velocity_frag;\n";
        s << "layout(location = 3) in float halo_percent_frag;\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();
        s << oit_definition_atomic_counter();
        s << oit_definition_fragments();
        s << oit_definition_linked_list();

        s << comment_region_functions();
        s << function_camera_position();
        s << function_grid_to_list_id();
        s << function_main_begin();
        s << "   if (velocity_frag < " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ") discard;\n\n";

        s << "   float alpha = velocity_frag / " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ";\n";
        s << "   alpha = pow(alpha, " << UBOFlowJetView::name_transparency_exponent() << ");\n\n";

        s << discard_low_alpha();

        s << "   color_out = vec4(0,0,0,alpha);\n";
        s << "   vec3 color = vec3(0);\n\n";

        s << "   if (abs(halo_percent_frag) >= 1.0f-" << UBOFlowJetView::name_area_halo_width_in_percent() << ")\n";
        s << "   {\n";
        s << "       const float diff = (1 - abs(halo_percent_frag)) / " << UBOFlowJetView::name_area_halo_width_in_percent() << ";\n;";
        s << "       color = mix(vec3(0), vec3(" << UBOFlowJetView::name_area_color_r() << ", " << UBOFlowJetView::name_area_color_g() << ", " << UBOFlowJetView::name_area_color_b()
          << "), diff*diff); // faded halo color\n";
        s << "   }\n";
        s << "   else\n";
        s << "   { color = vec3(" << UBOFlowJetView::name_area_color_r() << ", " << UBOFlowJetView::name_area_color_g() << ", " << UBOFlowJetView::name_area_color_b() << "); }\n\n";

        s << "   const vec3 camPos = camera_position();\n\n";

        s << "   const vec3 E = normalize(position_frag - camPos);\n";
        s << "   const vec3 L = E; // headlight\n\n";

        s << "   const float NdotL = dot(normal_frag, L);\n";
        s << "   const float k = min(0.5 /*ambient*/ + abs(NdotL) /*diffuse*/, 1);\n\n";

        s << "   color_out.rgb = k * color;\n\n";

        s << "   if (NdotL >= 0) // back face\n";
        s << "   { color_out.rgb *= 0.66; }\n";

        s << oit_assign_from_color_out();
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::jet::vert()
    {
        std::stringstream s;

        s << comment_tag_vertex_shader("FLOW JET POS");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in float timeID_in;\n";
        s << "layout(location = 1) in vec3 p0_t0_in;\n";
        s << "layout(location = 2) in float p0_t0_velo_in;\n";
        s << "layout(location = 3) in float p0_t0_eccentricity_in;\n";
        s << "layout(location = 4) in vec3 p0_t1_in;\n";
        s << "layout(location = 5) in float p0_t1_velo_in;\n";
        s << "layout(location = 6) in float p0_t1_eccentricity_in;\n";

        s << comment_region_output();
        s << "layout(location = 0) out float timeID_geom;\n";
        s << "layout(location = 1) out vec3 p0_t0_geom;\n";
        s << "layout(location = 2) out float p0_t0_velo_geom;\n";
        s << "layout(location = 3) out float p0_t0_eccentricity_geom;\n";
        s << "layout(location = 4) out vec3 p0_t1_geom;\n";
        s << "layout(location = 5) out float p0_t1_velo_geom;\n";
        s << "layout(location = 6) out float p0_t1_eccentricity_geom;\n";

        s << comment_region_functions();
        s << function_main_begin();
        s << "   timeID_geom = timeID_in;\n";
        s << "   p0_t0_geom = p0_t0_in;\n";
        s << "   p0_t0_velo_geom = p0_t0_velo_in;\n";
        s << "   p0_t0_eccentricity_geom = p0_t0_eccentricity_in;\n";
        s << "   p0_t1_geom = p0_t1_in;\n";
        s << "   p0_t1_velo_geom = p0_t1_velo_in;\n";
        s << "   p0_t1_eccentricity_geom = p0_t1_eccentricity_in;\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::jet::geom()
    {
        std::stringstream s;

        s << comment_tag_geometry_shader("FLOW JET POS");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in float timeID_geom[3];\n";
        s << "layout(location = 1) in vec3 p0_t0_geom[3];\n";
        s << "layout(location = 2) in float p0_t0_velo_geom[3];\n";
        s << "layout(location = 3) in float p0_t0_eccentricity_geom[3];\n";
        s << "layout(location = 4) in vec3 p0_t1_geom[3];\n";
        s << "layout(location = 5) in float p0_t1_velo_geom[3];\n";
        s << "layout(location = 6) in float p0_t1_eccentricity_geom[3];\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();
        s << geom_layout_in_triangles();

        s << comment_region_output();
        s << "layout(location = 0) out vec3 position_frag;\n";
        s << "layout(location = 1) out vec3 normal_frag;\n";
        s << "layout(location = 2) out float velo_frag;\n";
        s << "layout(location = 3) out float eccentricity_frag;\n";
        s << geom_layout_out_triangle_strip(3);

        s << comment_region_functions();
        s << function_main_begin();
        s << "   const float time_scaled = " << bk::details::UBOGlobal::name_animation_current_time() << " / " << UBOFlowJetView::name_temporal_scale() << ";\n";
        s << "   const float t0 = floor(time_scaled);\n";
        s << "   const float t1 = ceil(time_scaled);\n\n";

        s << "   if (timeID_geom[0] >= t0 && timeID_geom[0] < t1)\n";
        s << "   {\n";
        s << "      const float tw = time_scaled - t0;\n\n";

        s << "      const vec3 p[3] = {\n";
        s << "          mix(p0_t0_geom[0], p0_t1_geom[0], tw),\n";
        s << "          mix(p0_t0_geom[1], p0_t1_geom[1], tw),\n";
        s << "          mix(p0_t0_geom[2], p0_t1_geom[2], tw)\n";
        s << "      };\n\n";

        s << "      normal_frag = normalize(cross(p[1]-p[0], p[2]-p[0]));\n";

        s << "      for (int i = 0; i < 3; ++i)\n";
        s << "      {\n";
        s << "          position_frag = p[i];\n";
        s << "          velo_frag = mix(p0_t0_velo_geom[i], p0_t1_velo_geom[i], tw);\n";
        s << "          eccentricity_frag = mix(p0_t0_eccentricity_geom[i], p0_t1_eccentricity_geom[i], tw);\n";
        s << "          gl_Position = " << bk::details::UBOGlobal::name_modelview_projection_matrix() << " * vec4(position_frag, 1);\n";
        s << "          EmitVertex();\n";
        s << "      }\n";
        s << "   }\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::jet::frag_opaque()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("FLOW JET POS OPAQUE");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_frag;\n";
        s << "layout(location = 1) in vec3 normal_frag;\n";
        s << "layout(location = 2) in float velo_frag;\n";
        s << "layout(location = 3) in float eccentricity_frag;\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();

        s << comment_region_output();
        s << "layout(location = 0) out vec4 color_out;\n";

        s << comment_region_functions();
        s << function_camera_position();
        s << function_main_begin();
        s << "   if (velo_frag < " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ") discard;\n\n";

        s << "   color_out = vec4(0, 0, 0, 1);\n";

        s << "   const vec3 camPos = camera_position(); // eye vector\n";
        s << "   const vec3 E = normalize(position_frag - camPos);\n";
        s << "   const vec3 P = position_frag; // position in worldcoordinates\n";
        s << "   const vec3 L = -E; // headlight\n";
        s << "   const vec3 N = normalize(normal_\n";
        s << "   const uint numColors = 6;\n";
        s << "   const vec3 color_scale[numColors] = {\n";
        s << "       vec3(0,1,0), // green  => 0.0\n";
        s << "       vec3(0,1,0), // green  => 0.2\n";
        s << "       vec3(1,1,0), // yellow => 0.4\n";
        s << "       vec3(1,1,0), // yellow => 0.6\n";
        s << "       vec3(1,0,0),  // red    => 0.8\n";
        s << "       vec3(1,0,0)  // red    => 1.0\n";
        s << "   };\n";

        s << "   const float eccentricity = eccentricity_frag * (numColors-1);\n";
        s << "   const uint c0 = min(numColors-1, uint(floor(eccentricity)));\n";
        s << "   const uint c1 = min(numColors-1, uint(ceil(eccentricity)));\n";
        s << "   const float cw = eccentricity - c0;\n";

        s << "   const vec3 color = mix(color_scale[c0], color_scale[c1], cw);\n";

        s << "   color_out.rgb = 0.1 * color; // ambient\n";
        s << "   color_out.rgb += abs(dot(N, L)) * color;  // diffuse\n";
        s << "   const vec3 R = normalize(reflect(L, N)); // specular\n";

        s << "   const vec3 light_color = vec3(" << UBOFlowJetView::name_jet_light_color_r() << ", " << UBOFlowJetView::name_jet_light_color_g() << ", " << UBOFlowJetView::name_jet_light_color_b()
          << ");\n";
        s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << UBOFlowJetView::name_jet_shininess() << ");\n";
        s << function_main_end();

        return s.str();
    }

    std::string ShaderLibrary_CMR::flow_jet::jet::frag_transparent()
    {
        std::stringstream s;

        s << comment_tag_fragment_shader("FLOW JET AREA TRANSPARENT");
        s << version();

        s << comment_region_input();
        s << "layout(location = 0) in vec3 position_frag;\n";
        s << "layout(location = 1) in vec3 normal_frag;\n";
        s << "layout(location = 2) in float velo_frag;\n";
        s << "layout(location = 3) in float eccentricity_frag;\n";
        s << ubo_definition_global();
        s << ubo_definition_flowjet_view();
        s << oit_definition_atomic_counter();
        s << oit_definition_fragments();
        s << oit_definition_linked_list();
        s << early_fragment_test();

        s << comment_region_functions();
        s << function_camera_position();
        s << function_grid_to_list_id();
        s << function_main_begin();
        s << "   if (velo_frag < " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ") discard;\n\n";

        s << "   float alpha = velo_frag / " << UBOFlowJetView::name_relative_velocity_visibility_threshold() << ";\n";
        s << "   alpha = pow(alpha, " << UBOFlowJetView::name_transparency_exponent() << ");\n";

        s << discard_low_alpha();

        s << "   vec4 color_out = vec4(0, 0, 0, 1);\n";

        s << "   const vec3 camPos = camera_position(); // eye vector\n";
        s << "   const vec3 E = normalize(position_frag - camPos);\n";
        s << "   const vec3 P = position_frag; // position in worldcoordinates\n";
        s << "   const vec3 L = -E; // headlight\n";
        s << "   const vec3 N = normalize(normal_\n";
        s << "   const uint numColors = 6;\n";
        s << "   const vec3 color_scale[numColors] = {\n";
        s << "       vec3(0,1,0), // green  => 0.0\n";
        s << "       vec3(0,1,0), // green  => 0.2\n";
        s << "       vec3(1,1,0), // yellow => 0.4\n";
        s << "       vec3(1,1,0), // yellow => 0.6\n";
        s << "       vec3(1,0,0),  // red    => 0.8\n";
        s << "       vec3(1,0,0)  // red    => 1.0\n";
        s << "   };\n";

        s << "   const float eccentricity = eccentricity_frag * (numColors-1);\n";
        s << "   const uint c0 = min(numColors-1, uint(floor(eccentricity)));\n";
        s << "   const uint c1 = min(numColors-1, uint(ceil(eccentricity)));\n";
        s << "   const float cw = eccentricity - c0;\n";

        s << "   const vec3 color = mix(color_scale[c0], color_scale[c1], cw);\n";

        s << "   color_out.rgb = 0.1 * color; // ambient\n";
        s << "   color_out.rgb += abs(dot(N, L)) * color;  // diffuse\n";
        s << "   const vec3 R = normalize(reflect(L, N)); // specular\n";

        s << "   const vec3 light_color = vec3(" << UBOFlowJetView::name_jet_light_color_r() << ", " << UBOFlowJetView::name_jet_light_color_g() << ", " << UBOFlowJetView::name_jet_light_color_b()
          << ");\n";
        s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << UBOFlowJetView::name_jet_shininess() << ");\n";
        s << oit_assign_from_color_out();
        s << function_main_end();

        return s.str();
    }
  } // inline namespace cmr
} // namespace bk