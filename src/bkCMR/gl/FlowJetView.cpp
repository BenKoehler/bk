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

#include <bkCMR/gl/FlowJetView.h>

#include <vector>

#include <bkCMR/FlowJet.h>
#include <bkCMR/FlowJetPoint.h>
#include <bkCMR/gl/UBOFlowJetView.h>
#include <bkCMR/gl/ShaderLibrary_CMR.h>
#include <bkGL/buffer/IBO.h>
#include <bkGL/buffer/VBO.h>
#include <bkGL/buffer/SSBO.h>
#include <bkGL/vao/VAO.h>
#include <bkGL/shader/Shader.h>
#include <bk/ThreadPool>
#include <bk/Color>
#include <bkMath/constants/pi.h>

namespace bk
{
  //====================================================================================================
  //===== MEMBERS
  //====================================================================================================
  struct FlowJetView::Impl
  {
      UBOFlowJetView ubo;
      // areas
      VBO vbo_area;
      IBO ibo_area;
      VAO vao_area;
      Shader shader_area_opaque;
      Shader shader_area_transparent;
      // glyph
      VBO vbo_jet_glyph;
      IBO ibo_jet_glyph;
      VAO vao_jet_glyph;
      Shader shader_jet_glyph_opaque;
      Shader shader_jet_glyph_transparent;
      // area streamlines
      VBO vbo_jet_area_streamlines;
      IBO ibo_jet_area_streamlines;
      VAO vao_jet_area_streamlines;
      //
      GLsizei sizeInd_area;
      GLsizei sizeInd_jet_glyph;
      GLsizei sizeInd_jet_area_streamlines;
      //GLsizei sizeInd_area_surface;
      Vec3<GLfloat> center;
      GLfloat relative_velocity_visibility_threshold;
      GLfloat transparency_exponent;
      ColorRGBA area_color;
      bool show_jet;
      bool show_jet_area;
      GLfloat area_line_width;
      GLfloat area_halo_width_in_percent;
      GLfloat area_line_shininess;
      //GLfloat area_surface_alpha;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : ubo(gl),
            vbo_area(gl),
            ibo_area(gl),
            vao_area(gl),
            shader_area_opaque(gl),
            shader_area_transparent(gl),
            vbo_jet_glyph(gl),
            ibo_jet_glyph(gl),
            vao_jet_glyph(gl),
            shader_jet_glyph_opaque(gl),
            shader_jet_glyph_transparent(gl),
            vbo_jet_area_streamlines(gl),
            ibo_jet_area_streamlines(gl),
            vao_jet_area_streamlines(gl),
          #endif
          sizeInd_area(0),
          sizeInd_jet_glyph(0),
          sizeInd_jet_area_streamlines(0),
          //sizeInd_area_surface(0),
          center(MatrixFactory::Zero_Vec_3D<GLfloat>()),
          relative_velocity_visibility_threshold(1.0 / 3.0),
          transparency_exponent(5),
          //area_color(ColorRGBA::Orange()),
          //area_color(0.5, 0.8, 1), // light blue
          area_color(1, 0, 1), // pink
          show_jet(true),
          show_jet_area(true),
          area_line_width(0.5),
          area_halo_width_in_percent(0.2),
          area_line_shininess(5)
      { /* do nothing */ }
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE

  FlowJetView::FlowJetView()
      : base_type()
  #else

  FlowJetView::FlowJetView(bk::qt_gl_functions* gl)
      : base_type(gl),
        _pdata(gl)
  #endif
  {
      /*
       * flow jet area (circle)
       */
      _pdata->vbo_area.set_usage_STATIC_DRAW();
      _pdata->ibo_area.set_usage_STATIC_DRAW();

      _pdata->vao_area.add_default_attribute_scalar_1xfloat("time_id"); // time id
      _pdata->vao_area.add_default_attribute_position_3xfloat(); // pos t0
      _pdata->vao_area.add_default_attribute_position_3xfloat(); // pos t1
      _pdata->vao_area.add_default_attribute_position_3xfloat(); // normal t0
      _pdata->vao_area.add_default_attribute_position_3xfloat(); // normal t1
      _pdata->vao_area.add_default_attribute_scalar_1xfloat("velocity_t0"); // velocity t0
      _pdata->vao_area.add_default_attribute_scalar_1xfloat("velocity_t1"); // velocity t1
      _pdata->vao_area.add_default_attribute_scalar_1xfloat("halo_percent"); // halo percent

      /*
       * flow jet glyph
       */
      _pdata->vbo_jet_glyph.set_usage_STATIC_DRAW();
      _pdata->ibo_jet_glyph.set_usage_STATIC_DRAW();

      _pdata->vao_jet_glyph.add_default_attribute_scalar_1xfloat("time_id"); // time id
      _pdata->vao_jet_glyph.add_default_attribute_position_3xfloat(); // pos t0
      _pdata->vao_jet_glyph.add_default_attribute_scalar_1xfloat("velocity_t0"); // velocity t0
      _pdata->vao_jet_glyph.add_default_attribute_scalar_1xfloat("eccentricity_t0"); // eccentricity t0
      _pdata->vao_jet_glyph.add_default_attribute_position_3xfloat(); // pos t1
      _pdata->vao_jet_glyph.add_default_attribute_scalar_1xfloat("velocity_t1"); // velocity t1
      _pdata->vao_jet_glyph.add_default_attribute_scalar_1xfloat("eccentricity_t1"); // eccentricity t1

      /*
       * area streamlines
       */
      _pdata->vbo_jet_area_streamlines.set_usage_STATIC_DRAW();
      _pdata->ibo_jet_area_streamlines.set_usage_STATIC_DRAW();

      _pdata->vao_jet_area_streamlines.add_default_attribute_scalar_1xfloat("time_id"); // time id
      _pdata->vao_jet_area_streamlines.add_default_attribute_position_3xfloat(); // pos t0
      _pdata->vao_jet_area_streamlines.add_default_attribute_position_3xfloat(); // pos t1
      _pdata->vao_jet_area_streamlines.add_default_attribute_position_3xfloat(); // normal t0
      _pdata->vao_jet_area_streamlines.add_default_attribute_position_3xfloat(); // normal t1
      _pdata->vao_jet_area_streamlines.add_default_attribute_scalar_1xfloat("velocity_t0"); // velocity t0
      _pdata->vao_jet_area_streamlines.add_default_attribute_scalar_1xfloat("velocity_t1"); // velocity t1
      _pdata->vao_jet_area_streamlines.add_default_attribute_scalar_1xfloat("halo_percent"); // halo percent
  }

  FlowJetView::FlowJetView(FlowJetView&&) noexcept = default;

  FlowJetView::~FlowJetView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  bool FlowJetView::flow_jet_is_visible() const
  { return _pdata->show_jet; }

  bool FlowJetView::flow_jet_area_is_visible() const
  { return _pdata->show_jet_area; }

  GLfloat FlowJetView::relative_velocity_visibility_threshold() const
  { return _pdata->relative_velocity_visibility_threshold; }

  Vec3<GLfloat> FlowJetView::center() const
  { return _pdata->center; }

  bool FlowJetView::is_initialized() const
  {return _pdata->vao_area.is_initialized() && _pdata->vao_jet_glyph.is_initialized() && _pdata->vao_jet_area_streamlines.is_initialized();}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void FlowJetView::set_relative_velocity_visibility_threshold(GLfloat p)
  { _pdata->relative_velocity_visibility_threshold = std::min(static_cast<GLfloat>(1), std::max(static_cast<GLfloat>(0), p)); }

  void FlowJetView::set_show_flow_jet(bool b)
  {
      if (b != _pdata->show_jet)
      {
          _pdata->show_jet = b;

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  void FlowJetView::show_flow_jet()
  { set_show_flow_jet(true); }

  void FlowJetView::hide_flow_jet()
  { set_show_flow_jet(false); }

  void FlowJetView::set_show_flow_jet_area(bool b)
  {
      if (b != _pdata->show_jet_area)
      {
          _pdata->show_jet_area = b;

          if (this->is_initialized())
          { this->emit_signal_update_required(); }
      }
  }

  void FlowJetView::show_flow_jet_area()
  { set_show_flow_jet_area(true); }

  void FlowJetView::hide_flow_jet_area()
  { set_show_flow_jet_area(false); }

  auto FlowJetView::operator=(FlowJetView&&) noexcept -> FlowJetView& = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  void FlowJetView::clear_shader()
  {
      _pdata->shader_area_opaque.clear();
      _pdata->shader_area_transparent.clear();
      _pdata->shader_jet_glyph_opaque.clear();
      _pdata->shader_jet_glyph_transparent.clear();
  }

  void FlowJetView::clear_buffers()
  {
      _pdata->ubo.clear();

      _pdata->vbo_area.clear();
      _pdata->ibo_area.clear();
      _pdata->vao_area.clear();
      _pdata->sizeInd_area = 0;

      _pdata->vbo_jet_glyph.clear();
      _pdata->ibo_jet_glyph.clear();
      _pdata->vao_jet_glyph.clear();
      _pdata->sizeInd_jet_glyph = 0;

      _pdata->vbo_jet_area_streamlines.clear();
      _pdata->ibo_jet_area_streamlines.clear();
      _pdata->vao_jet_area_streamlines.clear();
      _pdata->sizeInd_jet_area_streamlines = 0;
  }

  void FlowJetView::clear()
  {
      clear_shader();
      clear_buffers();
      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void FlowJetView::init_flowjet(const std::vector<FlowJet>& fjs)
  {
      if (fjs.empty())
      { return; }

      const unsigned int numTimes = fjs[0].num_times();
      std::vector<GLfloat> vbo_area_data;
      std::vector<GLuint> ibo_area_data;
      std::vector<GLfloat> vbo_jet_glyph_data;
      std::vector<GLuint> ibo_jet_glyph_data;
      std::vector<GLfloat> vbo_jet_area_streamlines_data;
      std::vector<GLuint> ibo_jet_area_streamlines_data;
      std::vector<GLfloat> vbo_area_surface_data;
      std::vector<GLuint> ibo_area_surface_data;

      //constexpr const unsigned int theta_steps = 25;
      constexpr const unsigned int theta_steps = 50;
      //constexpr const unsigned int theta_steps = 100;
      constexpr const double theta_delta = 2.0 * pi < GLfloat > / (theta_steps - 1);
      constexpr const double jet_radius_scale = 2; // todo options

      for (const FlowJet& fj: fjs)
      {
          for (unsigned int pid = 0; pid < fj.num_positions() - 1; ++pid)
          {
              for (unsigned int tid = 0; tid < numTimes - 1; ++tid)
              {
                  // (pid, tid), (pid+1, tid), (pid, tid+1), (pid+1, tid+1) are valid
                  //if (!fj.has_center_at_plane_at_time(pid, tid) || !fj.center_has_spatial_successor(pid, tid) || !fj.center_has_temporal_successor(pid, tid) || !fj.center_has_temporal_successor(pid + 1, tid))
                  //{ continue; }

                  const FlowJetPoint& p0_t0 = fj.point(pid,tid);
                  const FlowJetPoint& p0_t1 = fj.point(pid,tid+1);
                  const FlowJetPoint& p1_t0 = fj.point(pid+1,tid);
                  const FlowJetPoint& p1_t1 = fj.point(pid+1,tid+1);

                  /*
                   * flow jet properties
                   */
                  const Vec3d& jet_center0_t0 = p0_t0.peak_velocity_position;
                  const Vec3d& jet_center0_t1 = p0_t1.peak_velocity_position;
                  const Vec3d& jet_center1_t0 = p1_t0.peak_velocity_position;
                  const Vec3d& jet_center1_t1 = p1_t1.peak_velocity_position;
                  const double jet_center0_t0_v = p0_t0.peak_velocity;
                  const double jet_center0_t1_v = p0_t1.peak_velocity;
                  const double jet_center1_t0_v = p1_t0.peak_velocity;
                  const double jet_center1_t1_v = p1_t1.peak_velocity;
                  const std::pair<Vec3d, Vec3d> jet_center0_lcs(p0_t0.local_coord_sys_x, p0_t0.local_coord_sys_y);
                  const std::pair<Vec3d, Vec3d> jet_center1_lcs(p0_t0.local_coord_sys_x, p1_t0.local_coord_sys_y);

                  /*
                   * area properties
                   */
                  const Vec3d& area_center0_t0 = p0_t0.area_center;
                  const Vec3d& area_center0_t1 = p0_t1.area_center;
                  const Vec3d& area_center1_t0 = p1_t0.area_center;
                  const Vec3d& area_center1_t1 = p1_t1.area_center;
                  const std::pair<double, double> area_center0_t0_r(p0_t0.area_radius_x, p0_t0.area_radius_y);
                  const std::pair<double, double> area_center0_t1_r(p0_t1.area_radius_x, p0_t1.area_radius_y);
                  const std::pair<double, double> area_center1_t0_r(p1_t0.area_radius_x, p1_t0.area_radius_y);
                  const std::pair<double, double> area_center1_t1_r(p1_t1.area_radius_x, p1_t1.area_radius_y);
                  const std::pair<Vec3d, Vec3d> area_center0_t0_dir(p0_t0.area_dir_x, p0_t0.area_dir_y);
                  const std::pair<Vec3d, Vec3d> area_center0_t1_dir(p0_t1.area_dir_x, p0_t1.area_dir_y);
                  const std::pair<Vec3d, Vec3d> area_center1_t0_dir(p1_t0.area_dir_x, p1_t0.area_dir_y);
                  const std::pair<Vec3d, Vec3d> area_center1_t1_dir(p1_t1.area_dir_x, p1_t1.area_dir_y);

                  /*
                   * flow displacement properties
                   */
                  const Vec3d& vessel_center0 = p0_t0.vessel_center;
                  const Vec3d& vessel_center1 = p1_t0.vessel_center;
                  const double vessel_radius0 = p0_t0.vessel_radius;
                  const double vessel_radius1 = p1_t0.vessel_radius;

                  /*
                   * flow jet area
                   */
                  auto fut_jet_area = bk_threadpool.enqueue([&]()
                                                            {
                                                                for (unsigned int k = 0; k < theta_steps; ++k)
                                                                {
                                                                    // c0 p0 t0
                                                                    GLfloat theta = k * theta_delta;
                                                                    GLfloat s = std::cos(theta) * area_center0_t0_r.first;
                                                                    GLfloat t = std::sin(theta) * area_center0_t0_r.second;
                                                                    const Vec3d area_c0_p0_t0 = area_center0_t0 + area_center0_t0_dir.first * s + area_center0_t0_dir.second * t;

                                                                    // c0 p0 t1
                                                                    s = std::cos(theta) * area_center0_t1_r.first;
                                                                    t = std::sin(theta) * area_center0_t1_r.second;
                                                                    const Vec3d area_c0_p0_t1 = area_center0_t1 + area_center0_t1_dir.first * s + area_center0_t1_dir.second * t;

                                                                    // c0 p1 t0
                                                                    theta = ((k + 1) % theta_steps) * theta_delta;
                                                                    s = std::cos(theta) * area_center0_t0_r.first;
                                                                    t = std::sin(theta) * area_center0_t0_r.second;
                                                                    const Vec3d area_c0_p1_t0 = area_center0_t0 + area_center0_t0_dir.first * s + area_center0_t0_dir.second * t;

                                                                    // c0 p1 t1
                                                                    s = std::cos(theta) * area_center0_t1_r.first;
                                                                    t = std::sin(theta) * area_center0_t1_r.second;
                                                                    const Vec3d area_c0_p1_t1 = area_center0_t1 + area_center0_t1_dir.first * s + area_center0_t1_dir.second * t;

                                                                    // c1 p0 t0
                                                                    theta = k * theta_delta;
                                                                    s = std::cos(theta) * area_center1_t0_r.first;
                                                                    t = std::sin(theta) * area_center1_t0_r.second;
                                                                    const Vec3d temp_c1_p0_t0 = area_center1_t0 + area_center1_t0_dir.first * s + area_center1_t0_dir.second * t;

                                                                    // c1 p0 t1
                                                                    s = std::cos(theta) * area_center1_t1_r.first;
                                                                    t = std::sin(theta) * area_center1_t1_r.second;
                                                                    const Vec3d temp_c1_p0_t1 = area_center1_t1 + area_center1_t1_dir.first * s + area_center1_t1_dir.second * t;

                                                                    // c1 p1 t0
                                                                    theta = ((k + 1) % theta_steps) * theta_delta;
                                                                    s = std::cos(theta) * area_center1_t0_r.first;
                                                                    t = std::sin(theta) * area_center1_t0_r.second;
                                                                    const Vec3d temp_c1_p1_t0 = area_center1_t0 + area_center1_t0_dir.first * s + area_center1_t0_dir.second * t;

                                                                    // c1 p1 t1
                                                                    s = std::cos(theta) * area_center1_t1_r.first;
                                                                    t = std::sin(theta) * area_center1_t1_r.second;
                                                                    const Vec3d temp_c1_p1_t1 = area_center1_t1 + area_center1_t1_dir.first * s + area_center1_t1_dir.second * t;

                                                                    // --------------

                                                                    const Vec3d dir_p0_t0 = (temp_c1_p0_t0 - area_c0_p0_t0).normalize();
                                                                    const Vec3d dir_p0_t1 = (temp_c1_p0_t1 - area_c0_p0_t1).normalize();
                                                                    const Vec3d dir_p1_t0 = (temp_c1_p1_t0 - area_c0_p1_t0).normalize();
                                                                    const Vec3d dir_p1_t1 = (temp_c1_p1_t1 - area_c0_p1_t1).normalize();
                                                                    const Vec3d area_c1_p0_t0 = area_c0_p0_t0 + dir_p0_t0 * _pdata->area_line_width;
                                                                    const Vec3d area_c1_p1_t0 = area_c0_p1_t0 + dir_p1_t0 * _pdata->area_line_width;
                                                                    const Vec3d area_c1_p0_t1 = area_c0_p0_t1 + dir_p0_t1 * _pdata->area_line_width;
                                                                    const Vec3d area_c1_p1_t1 = area_c0_p1_t1 + dir_p1_t1 * _pdata->area_line_width;

                                                                    // --------------

                                                                    const Vec3d normal_c0_p0_t0 = (area_c0_p0_t0 - area_center0_t0).normalize();
                                                                    const Vec3d normal_c0_p0_t1 = (area_c0_p0_t1 - area_center0_t1).normalize();
                                                                    const Vec3d normal_c0_p1_t0 = (area_c0_p1_t0 - area_center0_t0).normalize();
                                                                    const Vec3d normal_c0_p1_t1 = (area_c0_p1_t1 - area_center0_t1).normalize();
                                                                    const Vec3d normal_c1_p0_t0 = (area_c1_p0_t0 - area_center1_t0).normalize();
                                                                    const Vec3d normal_c1_p0_t1 = (area_c1_p0_t1 - area_center1_t1).normalize();
                                                                    const Vec3d normal_c1_p1_t0 = (area_c1_p1_t0 - area_center1_t0).normalize();
                                                                    const Vec3d normal_c1_p1_t1 = (area_c1_p1_t1 - area_center1_t1).normalize();

                                                                    // --------------

                                                                    /*
                                                                     * c0 p0 t0 / t1
                                                                     */
                                                                    vbo_area_data.push_back(tid); // time id

                                                                    // pos
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c0_p0_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c0_p0_t1[i]); }

                                                                    // normal
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c0_p0_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c0_p0_t1[i]); }

                                                                    // velocity0 t0 / t1
                                                                    vbo_area_data.push_back(jet_center0_t0_v);
                                                                    vbo_area_data.push_back(jet_center0_t1_v);

                                                                    // halo
                                                                    vbo_area_data.push_back(-1);

                                                                    /*
                                                                     * c1 p0 t0 / t1
                                                                     */
                                                                    vbo_area_data.push_back(tid); // time id

                                                                    // pos
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c1_p0_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c1_p0_t1[i]); }

                                                                    // normal
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c1_p0_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c1_p0_t1[i]); }

                                                                    // velocity0 t0 / t1
                                                                    vbo_area_data.push_back(jet_center1_t0_v);
                                                                    vbo_area_data.push_back(jet_center1_t1_v);

                                                                    // halo
                                                                    vbo_area_data.push_back(+1);

                                                                    /*
                                                                     * c0 p1 t0 / t1
                                                                     */
                                                                    vbo_area_data.push_back(tid); // time id

                                                                    // pos
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c0_p1_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c0_p1_t1[i]); }

                                                                    // normal
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c0_p1_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c0_p1_t1[i]); }

                                                                    // velocity0 t0 / t1
                                                                    vbo_area_data.push_back(jet_center0_t0_v);
                                                                    vbo_area_data.push_back(jet_center0_t1_v);

                                                                    // halo
                                                                    vbo_area_data.push_back(-1);

                                                                    /*
                                                                     * c1 p1 t0 / t1
                                                                     */
                                                                    vbo_area_data.push_back(tid); // time id

                                                                    // pos
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c1_p1_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(area_c1_p1_t1[i]); }

                                                                    // normal
                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c1_p1_t0[i]); }

                                                                    for (unsigned int i: {0, 1, 2})
                                                                    { vbo_area_data.push_back(normal_c1_p1_t1[i]); }

                                                                    // velocity0 t0 / t1
                                                                    vbo_area_data.push_back(jet_center1_t0_v);
                                                                    vbo_area_data.push_back(jet_center1_t1_v);

                                                                    // halo
                                                                    vbo_area_data.push_back(+1);

                                                                    ibo_area_data.push_back(_pdata->sizeInd_area);
                                                                    ibo_area_data.push_back(_pdata->sizeInd_area + 1);
                                                                    ibo_area_data.push_back(_pdata->sizeInd_area + 2);

                                                                    ibo_area_data.push_back(_pdata->sizeInd_area + 1);
                                                                    ibo_area_data.push_back(_pdata->sizeInd_area + 2);
                                                                    ibo_area_data.push_back(_pdata->sizeInd_area + 3);

                                                                    _pdata->sizeInd_area += 4;
                                                                }
                                                            });

                  /*
                  * flow jet glyph volumetric
                  */
                  auto fut_jet_glyph = bk_threadpool.enqueue([&]()
                                                             {
                                                                 const double center0_t0_eccentricity = (jet_center0_t0 - vessel_center0).norm() / vessel_radius0;
                                                                 const double center0_t1_eccentricity = (jet_center0_t1 - vessel_center0).norm() / vessel_radius0;
                                                                 const double center1_t0_eccentricity = (jet_center1_t0 - vessel_center1).norm() / vessel_radius1;
                                                                 const double center1_t1_eccentricity = (jet_center1_t1 - vessel_center1).norm() / vessel_radius1;

                                                                 for (unsigned int k = 0; k < theta_steps; ++k)
                                                                 {
                                                                     const GLfloat theta = k * theta_delta;
                                                                     GLfloat s = std::cos(theta) * jet_center0_t0_v * jet_radius_scale;
                                                                     GLfloat t = std::sin(theta) * jet_center0_t0_v * jet_radius_scale;
                                                                     const Vec3d glyph_center0_t0 = jet_center0_t0 + jet_center0_lcs.first * s + jet_center0_lcs.second * t;
                                                                     s = std::cos(theta) * jet_center0_t1_v * jet_radius_scale;
                                                                     t = std::sin(theta) * jet_center0_t1_v * jet_radius_scale;
                                                                     const Vec3d glyph_center0_t1 = jet_center0_t1 + jet_center0_lcs.first * s + jet_center0_lcs.second * t;

                                                                     s = std::cos(theta) * jet_center1_t0_v * jet_radius_scale;
                                                                     t = std::sin(theta) * jet_center1_t0_v * jet_radius_scale;
                                                                     const Vec3d glyph_center1_t0 = jet_center1_t0 + jet_center1_lcs.first * s + jet_center1_lcs.second * t;
                                                                     s = std::cos(theta) * jet_center1_t1_v * jet_radius_scale;
                                                                     t = std::sin(theta) * jet_center1_t1_v * jet_radius_scale;
                                                                     const Vec3d glyph_center1_t1 = jet_center1_t1 + jet_center1_lcs.first * s + jet_center1_lcs.second * t;

                                                                     // time id
                                                                     vbo_jet_glyph_data.push_back(tid);

                                                                     // pos0 t0
                                                                     for (unsigned int i: {0, 1, 2})
                                                                     { vbo_jet_glyph_data.push_back(glyph_center0_t0[i]); }

                                                                     // velocity0 t0
                                                                     vbo_jet_glyph_data.push_back(jet_center0_t0_v);

                                                                     // eccentricity0 t0
                                                                     vbo_jet_glyph_data.push_back(center0_t0_eccentricity);

                                                                     // pos0 t1
                                                                     for (unsigned int i: {0, 1, 2})
                                                                     { vbo_jet_glyph_data.push_back(glyph_center0_t1[i]); }

                                                                     // velocity0 t1
                                                                     vbo_jet_glyph_data.push_back(jet_center0_t1_v);

                                                                     // eccentricity0 t1
                                                                     vbo_jet_glyph_data.push_back(center0_t1_eccentricity);

                                                                     ibo_jet_glyph_data.push_back(_pdata->sizeInd_jet_glyph++);

                                                                     // time id
                                                                     vbo_jet_glyph_data.push_back(tid);

                                                                     // pos1 t0
                                                                     for (unsigned int i: {0, 1, 2})
                                                                     { vbo_jet_glyph_data.push_back(glyph_center1_t0[i]); }

                                                                     // velocity1 t0
                                                                     vbo_jet_glyph_data.push_back(jet_center1_t0_v);

                                                                     // eccentricity1 t0
                                                                     vbo_jet_glyph_data.push_back(center1_t0_eccentricity);

                                                                     // pos1 t1
                                                                     for (unsigned int i: {0, 1, 2})
                                                                     { vbo_jet_glyph_data.push_back(glyph_center1_t1[i]); }

                                                                     // velocity1 t1
                                                                     vbo_jet_glyph_data.push_back(jet_center1_t1_v);

                                                                     // eccentricity1 t1
                                                                     vbo_jet_glyph_data.push_back(center1_t1_eccentricity);

                                                                     ibo_jet_glyph_data.push_back(_pdata->sizeInd_jet_glyph++);
                                                                 }

                                                                 ibo_jet_glyph_data.push_back(std::numeric_limits<GLuint>::max());
                                                             });

                  /*
                  * area streamlines
                  */
                  constexpr const unsigned int theta_steps_scale = 5;

                  auto fut_area_streamlines = bk_threadpool.enqueue([&]()
                                                                    {
                                                                        const int theta_steps_scaled = theta_steps / theta_steps_scale;

                                                                        for (int k = 0; k < static_cast<int>(theta_steps_scaled); ++k)
                                                                        {
                                                                            // temp c0 pm1 t0/1
                                                                            GLfloat theta = (((k - 1) * theta_steps_scale) % theta_steps) * theta_delta;
                                                                            GLfloat s = std::cos(theta) * area_center0_t0_r.first;
                                                                            GLfloat t = std::sin(theta) * area_center0_t0_r.second;
                                                                            const Vec3d temp_c0_pm1_t0 = area_center0_t0 + area_center0_t0_dir.first * s + area_center0_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center0_t1_r.first;
                                                                            t = std::sin(theta) * area_center0_t1_r.second;
                                                                            const Vec3d temp_c0_pm1_t1 = area_center0_t1 + area_center0_t1_dir.first * s + area_center0_t1_dir.second * t;

                                                                            // temp c1 pm1 t0/1
                                                                            s = std::cos(theta) * area_center1_t0_r.first;
                                                                            t = std::sin(theta) * area_center1_t0_r.second;
                                                                            const Vec3d temp_c1_pm1_t0 = area_center1_t0 + area_center1_t0_dir.first * s + area_center1_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center1_t1_r.first;
                                                                            t = std::sin(theta) * area_center1_t1_r.second;
                                                                            const Vec3d temp_c1_pm1_t1 = area_center1_t1 + area_center1_t1_dir.first * s + area_center1_t1_dir.second * t;

                                                                            // temp c0 p0 t0/1
                                                                            theta = k * theta_steps_scale * theta_delta;
                                                                            s = std::cos(theta) * area_center0_t0_r.first;
                                                                            t = std::sin(theta) * area_center0_t0_r.second;
                                                                            const Vec3d temp_c0_p0_t0 = area_center0_t0 + area_center0_t0_dir.first * s + area_center0_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center0_t1_r.first;
                                                                            t = std::sin(theta) * area_center0_t1_r.second;
                                                                            const Vec3d temp_c0_p0_t1 = area_center0_t1 + area_center0_t1_dir.first * s + area_center0_t1_dir.second * t;

                                                                            // temp c1 p0 t0/1
                                                                            s = std::cos(theta) * area_center1_t0_r.first;
                                                                            t = std::sin(theta) * area_center1_t0_r.second;
                                                                            const Vec3d temp_c1_p0_t0 = area_center1_t0 + area_center1_t0_dir.first * s + area_center1_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center1_t1_r.first;
                                                                            t = std::sin(theta) * area_center1_t1_r.second;
                                                                            const Vec3d temp_c1_p0_t1 = area_center1_t1 + area_center1_t1_dir.first * s + area_center1_t1_dir.second * t;

                                                                            // temp c0 p1 t0/1
                                                                            theta = (((k + 1) * theta_steps_scale) % theta_steps) * theta_delta;
                                                                            s = std::cos(theta) * area_center0_t0_r.first;
                                                                            t = std::sin(theta) * area_center0_t0_r.second;
                                                                            const Vec3d temp_c0_p1_t0 = area_center0_t0 + area_center0_t0_dir.first * s + area_center0_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center0_t1_r.first;
                                                                            t = std::sin(theta) * area_center0_t1_r.second;
                                                                            const Vec3d temp_c0_p1_t1 = area_center0_t1 + area_center0_t1_dir.first * s + area_center0_t1_dir.second * t;

                                                                            // temp c1 p1 t0/1
                                                                            s = std::cos(theta) * area_center1_t0_r.first;
                                                                            t = std::sin(theta) * area_center1_t0_r.second;
                                                                            const Vec3d temp_c1_p1_t0 = area_center1_t0 + area_center1_t0_dir.first * s + area_center1_t0_dir.second * t;

                                                                            s = std::cos(theta) * area_center1_t1_r.first;
                                                                            t = std::sin(theta) * area_center1_t1_r.second;
                                                                            const Vec3d temp_c1_p1_t1 = area_center1_t1 + area_center1_t1_dir.first * s + area_center1_t1_dir.second * t;

                                                                            // ----------------

                                                                            const Vec3d ortho_c0_t0 = (temp_c0_p1_t0 - temp_c0_pm1_t0).normalize();
                                                                            const Vec3d ortho_c0_t1 = (temp_c0_p1_t1 - temp_c0_pm1_t1).normalize();
                                                                            const Vec3d ortho_c1_t0 = (temp_c1_p1_t0 - temp_c1_pm1_t0).normalize();
                                                                            const Vec3d ortho_c1_t1 = (temp_c1_p1_t1 - temp_c1_pm1_t1).normalize();
                                                                            const Vec3d vert_c0_p0_t0 = temp_c0_p0_t0 - ortho_c0_t0 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c0_p0_t1 = temp_c0_p0_t1 - ortho_c0_t1 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c1_p0_t0 = temp_c1_p0_t0 - ortho_c1_t0 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c1_p0_t1 = temp_c1_p0_t1 - ortho_c1_t1 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c0_p1_t0 = temp_c0_p0_t0 + ortho_c0_t0 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c0_p1_t1 = temp_c0_p0_t1 + ortho_c0_t1 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c1_p1_t0 = temp_c1_p0_t0 + ortho_c1_t0 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d vert_c1_p1_t1 = temp_c1_p0_t1 + ortho_c1_t1 * (0.5 * _pdata->area_line_width);
                                                                            const Vec3d normal_c0_p0_t0 = (vert_c0_p0_t0 - area_center0_t0).normalize();
                                                                            const Vec3d normal_c0_p0_t1 = (vert_c0_p0_t1 - area_center0_t1).normalize();
                                                                            const Vec3d normal_c1_p0_t0 = (vert_c1_p0_t0 - area_center1_t0).normalize();
                                                                            const Vec3d normal_c1_p0_t1 = (vert_c1_p0_t1 - area_center1_t1).normalize();
                                                                            const Vec3d normal_c0_p1_t0 = (vert_c0_p0_t0 - area_center0_t0).normalize();
                                                                            const Vec3d normal_c0_p1_t1 = (vert_c0_p0_t1 - area_center0_t1).normalize();
                                                                            const Vec3d normal_c1_p1_t0 = (vert_c1_p0_t0 - area_center1_t0).normalize();
                                                                            const Vec3d normal_c1_p1_t1 = (vert_c1_p0_t1 - area_center1_t1).normalize();

                                                                            // ----------------

                                                                            /*
                                                                             * c0 p0 t0 / t1
                                                                             */
                                                                            // time id
                                                                            vbo_jet_area_streamlines_data.push_back(tid);

                                                                            // pos
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c0_p0_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c0_p0_t1[i]); }

                                                                            // normal
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c0_p0_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c0_p0_t1[i]); }

                                                                            // velocity c0 t0 / t1
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center0_t0_v);
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center0_t1_v);

                                                                            // halo
                                                                            vbo_jet_area_streamlines_data.push_back(-1);

                                                                            /*
                                                                             * c1 p0 t0 / t1
                                                                             */
                                                                            // time id
                                                                            vbo_jet_area_streamlines_data.push_back(tid);

                                                                            // pos
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c1_p0_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c1_p0_t1[i]); }

                                                                            // normal
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c1_p0_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c1_p0_t1[i]); }

                                                                            // velocity c1 t0 / t1
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center1_t0_v);
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center1_t1_v);

                                                                            // halo
                                                                            vbo_jet_area_streamlines_data.push_back(-1);

                                                                            /*
                                                                             * c0 p1 t0 / t1
                                                                             */
                                                                            // time id
                                                                            vbo_jet_area_streamlines_data.push_back(tid);

                                                                            // pos
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c0_p1_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c0_p1_t1[i]); }

                                                                            // normal
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c0_p1_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c0_p1_t1[i]); }

                                                                            // velocity c0 t0 / t1
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center0_t0_v);
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center0_t1_v);

                                                                            // halo
                                                                            vbo_jet_area_streamlines_data.push_back(+1);

                                                                            /*
                                                                             * c1 p1 t0 / t1
                                                                             */
                                                                            // time id
                                                                            vbo_jet_area_streamlines_data.push_back(tid);

                                                                            // pos
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c1_p1_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(vert_c1_p1_t1[i]); }

                                                                            // normal
                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c1_p1_t0[i]); }

                                                                            for (unsigned int i: {0, 1, 2})
                                                                            { vbo_jet_area_streamlines_data.push_back(normal_c1_p1_t1[i]); }

                                                                            // velocity c0 t0 / t1
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center1_t0_v);
                                                                            vbo_jet_area_streamlines_data.push_back(jet_center1_t1_v);

                                                                            // halo
                                                                            vbo_jet_area_streamlines_data.push_back(+1);

                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines);
                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines + 1);
                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines + 2);

                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines + 1);
                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines + 2);
                                                                            ibo_jet_area_streamlines_data.push_back(_pdata->sizeInd_jet_area_streamlines + 3);

                                                                            _pdata->sizeInd_jet_area_streamlines += 4;
                                                                        }
                                                                    });

                  fut_jet_area.get();
                  fut_jet_glyph.get();
                  fut_area_streamlines.get();
              } // for tid: numTimes
          } // for pid: numPlanes
      } // for fj: fjs

      _pdata->sizeInd_area = ibo_area_data.size();

      _pdata->vbo_area.init(vbo_area_data);
      _pdata->ibo_area.init(ibo_area_data);
      _pdata->vao_area.init(_pdata->vbo_area, _pdata->ibo_area);

      _pdata->sizeInd_jet_glyph = ibo_jet_glyph_data.size();

      _pdata->vbo_jet_glyph.init(vbo_jet_glyph_data);
      _pdata->ibo_jet_glyph.init(ibo_jet_glyph_data);
      _pdata->vao_jet_glyph.init(_pdata->vbo_jet_glyph, _pdata->ibo_jet_glyph);

      _pdata->sizeInd_jet_area_streamlines = ibo_jet_area_streamlines_data.size();

      _pdata->vbo_jet_area_streamlines.init(vbo_jet_area_streamlines_data);
      _pdata->ibo_jet_area_streamlines.init(ibo_jet_area_streamlines_data);
      _pdata->vao_jet_area_streamlines.init(_pdata->vbo_jet_area_streamlines, _pdata->ibo_jet_area_streamlines);
  }

  void FlowJetView::init_shader()
  {
      using SL = ShaderLibrary_CMR::flow_jet;

      _pdata->shader_area_opaque.init(SL::area::vert(), SL::area::frag_opaque(), SL::area::geom());
      _pdata->shader_area_transparent.init(SL::area::vert(), SL::area::frag_transparent(), SL::area::geom());
      _pdata->shader_area_opaque.init(SL::jet::vert(), SL::jet::frag_opaque(), SL::jet::geom());
      _pdata->shader_area_transparent.init(SL::jet::vert(), SL::jet::frag_transparent(), SL::jet::geom());
  }

  void FlowJetView::init_ubo()
  {
      _pdata->ubo.init_from_registered_values_size();

      _pdata->ubo.set_relative_velocity_visibility_threshold(_pdata->relative_velocity_visibility_threshold);
      _pdata->ubo.set_transparency_exponent(_pdata->transparency_exponent);
      _pdata->ubo.set_area_color_r(_pdata->area_color[0]);
      _pdata->ubo.set_area_color_g(_pdata->area_color[1]);
      _pdata->ubo.set_area_color_b(_pdata->area_color[2]);
      _pdata->ubo.set_area_line_width(_pdata->area_line_width);
      _pdata->ubo.set_area_halo_width_in_percent(_pdata->area_halo_width_in_percent);
      _pdata->ubo.set_area_line_shininess(_pdata->area_line_shininess);
      const GLfloat light = 0.5;
      _pdata->ubo.set_jet_light_color_r(light);
      _pdata->ubo.set_jet_light_color_g(light);
      _pdata->ubo.set_jet_light_color_b(light);
      _pdata->ubo.set_jet_shininess(_pdata->area_line_shininess);

      _pdata->ubo.release();
  }

  void FlowJetView::init(const std::vector<FlowJet>& fjs, double msPerTime)
  {
      if (fjs.empty())
      { return; }

      init_ubo();

      _pdata->ubo.set_num_times(fjs[0].num_times());
      _pdata->ubo.set_temporal_scale(msPerTime);
      _pdata->ubo.release();

      init_flowjet(fjs);
      init_shader();
      //this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void FlowJetView::on_resize(GLint /*w*/, GLint /*h*/)
  { /* do nothing */ }

  void FlowJetView::on_oit_enabled(bool)
  { /* do nothing */ }

  void FlowJetView::on_animation_enabled(bool)
  { /* do nothing */ }

  void FlowJetView::on_modelview_changed(bool)
  { /* do nothing */ }

  void FlowJetView::on_visible_changed(bool)
  { /* do nothing */ }

  void FlowJetView::on_mouse_pos_changed(GLint /*x*/, GLint /*y*/)
  { /* do nothing */ }

  void FlowJetView::on_mouse_button_pressed(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void FlowJetView::on_mouse_button_released(MouseButton_ /*btn*/)
  { /* do nothing */ }

  void FlowJetView::on_key_pressed(Key_ /*k*/)
  { /* do nothing */ }

  void FlowJetView::on_key_released(Key_ /*k*/)
  { /* do nothing */ }

  void FlowJetView::on_mouse_wheel_up()
  { /* do nothing */ }

  void FlowJetView::on_mouse_wheel_down()
  { /* do nothing */ }

  void FlowJetView::on_ssaa_factor_changed(GLint /*ssaa_factor*/)
  { /* do nothing */ }

  void FlowJetView::draw_opaque_impl()
  { if (!_pdata->show_jet && !_pdata->show_jet_area)
      { return; }

      // ubo 0 must be global ubo with modelview/projection matrices
      _pdata->ubo.bind_to_base(1);

      if (_pdata->show_jet_area)
      {
          // area circles
          _pdata->vao_area.bind();
          _pdata->shader_area_opaque.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_area, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_area_opaque.release();
          _pdata->vao_area.release();

          // area streamlines
          _pdata->vao_jet_area_streamlines.bind();
          _pdata->shader_area_opaque.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_jet_area_streamlines, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_area_opaque.release();
          _pdata->vao_jet_area_streamlines.release();
      }

      if (_pdata->show_jet)
      {
          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

          _pdata->vao_jet_glyph.bind();
          _pdata->shader_jet_glyph_opaque.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd_jet_glyph, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_jet_glyph_opaque.release();
          _pdata->vao_jet_glyph.release();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
      }

      _pdata->ubo.release_from_base(); }

  void FlowJetView::draw_transparent_impl()
  {
      if (!this->is_initialized() || this->is_hidden() || !this->oit_is_available())
      { return; }

      if (!_pdata->show_jet && !_pdata->show_jet_area)
      { return; }

      // ubo 0 must be global ubo with modelview/projection matrices
      _pdata->ubo.bind_to_base(1);

      BK_QT_GL glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      BK_QT_GL glDepthMask(GL_FALSE);

      if (_pdata->show_jet_area)
      {
          // area circles
          _pdata->vao_area.bind();
          _pdata->shader_area_transparent.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_area, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_area_transparent.release();
          _pdata->vao_area.release();

          // area streamlines
          _pdata->vao_jet_area_streamlines.bind();
          _pdata->shader_area_transparent.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLES, _pdata->sizeInd_jet_area_streamlines, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_area_transparent.release();
          _pdata->vao_jet_area_streamlines.release();
      }

      if (_pdata->show_jet)
      {
          BK_QT_GL glEnable(GL_PRIMITIVE_RESTART);
          BK_QT_GL glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

          _pdata->vao_jet_glyph.bind();
          _pdata->shader_jet_glyph_transparent.bind();
          BK_QT_GL glDrawElements(GL_TRIANGLE_STRIP, _pdata->sizeInd_jet_glyph, GL_UNSIGNED_INT, nullptr);
          _pdata->shader_jet_glyph_transparent.release();
          _pdata->vao_jet_glyph.release();

          BK_QT_GL glDisable(GL_PRIMITIVE_RESTART);
      }

      BK_QT_GL glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      BK_QT_GL glDepthMask(GL_TRUE);

      _pdata->ubo.release_from_base();
  }
} // namespace bk