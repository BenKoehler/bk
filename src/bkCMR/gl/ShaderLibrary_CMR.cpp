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

#include <bkCMR/gl/UBOMeasuringPlanePreview.h>
#include <bkGL/UBOGlobal.h>

namespace bk
{
  inline namespace cmr
  {
    /// @{ -------------------------------------------------- UBO
    std::string ShaderLibrary_CMR::ubo_definition_measuring_plane_preview()
    {
        return ubo_definition(bk::UBOMeasuringPlanePreview(
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

        s << "   vec3 color = vec3(" << bk::UBOMeasuringPlanePreview::name_color_r() << ", " << bk::UBOMeasuringPlanePreview::name_color_g() << ", " << bk::UBOMeasuringPlanePreview::name_color_b() << ");\n\n";

        s << "   // alpha\n";
        s << "   color_out.a = 1;\n\n";

        s << "   // ambient\n";
        s << "   color_out.rgb = 0.1 * color;\n\n";

        s << "   // diffuse\n";
        s << "   const float NdotL = dot(N, L);\n";
        s << "   color_out.rgb += abs(NdotL) * (NdotL >= 0 ? 1.0f : 0.75f) * color;\n\n";

        s << "   // specular\n";
        s << "   const vec3 light_color = vec3(" << bk::UBOMeasuringPlanePreview::name_lightcolor_r() << ", " << bk::UBOMeasuringPlanePreview::name_lightcolor_g() << ", " << bk::UBOMeasuringPlanePreview::name_lightcolor_b() << ");\n";
        s << "   color_out.rgb += light_color * pow(clamp(abs(dot(R, E)), 0.0, 1.0), " << bk::UBOMeasuringPlanePreview::name_shininess() << ");\n";

        s << function_main_end();

        return s.str();
    }
  } // inline namespace cmr
} // namespace bk