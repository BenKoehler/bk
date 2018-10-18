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

#include <bkCMR/gl/VesselView.h>

#include <vector>

#include <bk/CMR>
#include <bk/Signal>
#include <bk/StringUtils>
#include <bk/Color>

#include <bkDataset/attributes/attribute_info.h>
#include <bkGL/renderable/mesh/TriangularMesh3DView.h>
#include <bkGL/renderable/line/LineView.h>
#include <bkGL/EKey.h>
#include <bkGL/EMouseButton.h>
#include <bkCMR/gl/MeasuringPlaneView.h>
#include <bkCMR/gl/FlowJetView.h>
#include <bkCMR/gl/PressureView.h>

namespace bk
{
  struct VesselView::Impl
  {
      TriangularMesh3DView meshview;
      LineView pathlineview;
      LineView centerlineview;
      FlowJetView flowjetview;
      PressureView pressureview;
      std::string name;
      std::vector<MeasuringPlaneView> measuringplaneviews;

          #ifndef BK_LIB_QT_AVAILABLE

      Impl()
          :
          #else

      explicit Impl(bk::qt_gl_functions* gl)
          : meshview(gl),
            pathlineview(gl),
            centerlineview(gl),
            flowjetview(gl),
            pressureview(gl),
          #endif
          name("vessel")
      { /* do nothing */ }

      Impl(const Impl&) = default;
      Impl(Impl&&) noexcept = default;
      ~Impl() = default;
      Impl& operator=(const Impl&) = default;
      Impl& operator=(Impl&&) noexcept = default;
  };

  //====================================================================================================
  //===== CONSTRUCTORS & DESTRUCTOR
  //====================================================================================================
  #ifndef BK_LIB_QT_AVAILABLE
  VesselView::VesselView()
  #else
  VesselView::VesselView(bk::qt_gl_functions* gl)
      : _pdata(gl)
  #endif
  {
      _pdata->centerlineview.set_line_width(_pdata->pathlineview.line_width() * 2);
      _pdata->centerlineview.set_color(ColorRGBA::White());

      _pdata->pathlineview.enable_color_by_attribute();
  }

  VesselView::VesselView(VesselView&&) noexcept= default;
  VesselView::~VesselView() = default;

  //====================================================================================================
  //===== GETTER
  //====================================================================================================
  const std::string& VesselView::name() const
  { return _pdata->name; }

  TriangularMesh3DView& VesselView::meshview()
  { return _pdata->meshview; }

  const TriangularMesh3DView& VesselView::meshview() const
  { return _pdata->meshview; }

  LineView& VesselView::pathlineview()
  { return _pdata->pathlineview; }

  const LineView& VesselView::pathlineview() const
  { return _pdata->pathlineview; }

  LineView& VesselView::centerlineview()
  { return _pdata->centerlineview; }

  const LineView& VesselView::centerlineview() const
  { return _pdata->centerlineview; }

  FlowJetView& VesselView::flowjetview()
  { return _pdata->flowjetview; }

  const FlowJetView& VesselView::flowjetview() const
  { return _pdata->flowjetview; }

  PressureView& VesselView::pressureview()
  { return _pdata->pressureview; }

  const PressureView& VesselView::pressureview() const
  { return _pdata->pressureview; }

  std::vector<MeasuringPlaneView>& VesselView::measuringplaneviews()
  { return _pdata->measuringplaneviews; }

  const std::vector<MeasuringPlaneView>& VesselView::measuringplaneviews() const
  { return _pdata->measuringplaneviews; }

  MeasuringPlaneView* VesselView::measuringplaneview(unsigned int i)
  { return i < _pdata->measuringplaneviews.size() ? &_pdata->measuringplaneviews[i] : nullptr; }

  const MeasuringPlaneView* VesselView::measuringplaneview(unsigned int i) const
  { return i < _pdata->measuringplaneviews.size() ? &_pdata->measuringplaneviews[i] : nullptr; }

  bool VesselView::is_initialized() const
{return _pdata->meshview.is_initialized();}

  //====================================================================================================
  //===== SETTER
  //====================================================================================================
  void VesselView::set_vessel(const Vessel& v, Dataset& ds, std::string_view line_color_attribute_name)
  {
      this->set_name(v.name());

      this->set_visible();

      /*
       * mesh
       */
      if (v.has_mesh())
      { _pdata->meshview.init(v.mesh()); }

      _pdata->meshview.set_mode_front_face_culling_with_ghosted_view();

      this->forward_signals(&_pdata->meshview);
      this->forward_settings(&_pdata->meshview);

      /*
       * pathlines
       */
      if (!v.has_pathlines())
      { _pdata->pathlineview.init(v.pathlines().begin(), v.pathlines().end(), line_color_attribute_name); }

      this->forward_signals(&_pdata->pathlineview);
      this->forward_settings(&_pdata->pathlineview);

      /*
       * centerlines
       */
      if (v.has_centerlines())
      { _pdata->centerlineview.init(v.centerlines().begin(), v.centerlines().end()); }

      this->forward_signals(&_pdata->centerlineview);
      this->forward_settings(&_pdata->centerlineview);

      /*
       * flow jet
       */
      _pdata->flowjetview.init(v.flowjets(), ds.flow_image_3dt()->geometry().transformation().scale(3));

      this->forward_signals(&_pdata->flowjetview);
      this->forward_settings(&_pdata->flowjetview);

      /*
       * pressure map
       */
      //_pdata->pressureview.init(ds.pressure_map(), *v.segmentation3D()); // todo: segmentation in flow field size

      this->forward_signals(&_pdata->pressureview);
      this->forward_settings(&_pdata->pressureview);

      /*
       * measuring planes
       */
      init_measuringplanes(v);

      //determine_visualization_mode();

      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void VesselView::init_measuringplanes(const Vessel& v)
  {
      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.clear(); }

      std::vector<bool> current_visibility(_pdata->measuringplaneviews.size());
      for (unsigned int i = 0; i < _pdata->measuringplaneviews.size(); ++i)
      { current_visibility[i] = _pdata->measuringplaneviews[i].is_visible(); }

      _pdata->measuringplaneviews.clear();

      //for (auto& mp: v.measuring_planes())
      for (unsigned int i = 0; i < v.measuring_planes().size(); ++i)
      {
          auto& mp = v.measuring_planes()[i];

          #ifndef BK_LIB_QT_AVAILABLE
          //_pdata->measuringplaneviews.push_back(MeasuringPlaneView());
          _pdata->measuringplaneviews.emplace_back();
          #else
          //_pdata->measuringplaneviews.push_back(MeasuringPlaneView(this->_gl));
          _pdata->measuringplaneviews.emplace_back(this->_gl);
          #endif

          auto& mpv = _pdata->measuringplaneviews.back();
          mpv.init(mp);
          mpv.set_color_attribute(mp, MeasuringPlane::VelocityThroughPlaneAttributeName());
          mpv.set_colorbar_blue_to_red();

          this->forward_signals(&mpv);
          this->forward_settings(&mpv);

          if (i < current_visibility.size())
          { mpv.set_visible(current_visibility[i]); }
      }

      this->emit_signal_update_required();
  }

  void VesselView::set_name(std::string_view n)
  {
      // todo: use vessel semantics

      _pdata->name = n;

      if (string_utils::contains(_pdata->name, "aort", false) || string_utils::contains(_pdata->name, "aao", false) || string_utils::contains(_pdata->name, "dao", false) || string_utils::contains(_pdata->name, "left", false) || string_utils::contains(_pdata->name, "lv", false))
      { _pdata->meshview.set_color(ColorRGBA::Red()); }
      else if (string_utils::contains(_pdata->name, "pulm", false) || string_utils::contains(_pdata->name, "pa", false) || string_utils::contains(_pdata->name, "rv", false))
      { _pdata->meshview.set_color(ColorRGBA::Blue()); }
      else
      { _pdata->meshview.set_color(ColorRGBA::Green()); }
  }

  void VesselView::set_line_color_attribute(const Vessel& v, std::string_view color_attribute_name)
  {
      _pdata->pathlineview.set_color_attribute(v.pathlines().begin(), v.pathlines().end(), color_attribute_name);
      //determine_visualization_mode();
  }

  void VesselView::set_measuringplane_color_attribute(const Vessel& v, std::string_view color_attribute_name)
  {
      for (unsigned int i = 0; i < _pdata->measuringplaneviews.size(); ++i)
      {
          auto& mp = v.measuring_planes()[i];

          auto mpv = measuringplaneview(i);
          if (mpv == nullptr)
          { break; }

          mpv->set_color_attribute(*mp, color_attribute_name);
      }
  }

  VesselView& VesselView::operator=(VesselView&& ) noexcept = default;

  //====================================================================================================
  //===== FUNCTIONS
  //====================================================================================================
  Vec3<GLfloat> VesselView::center() const 
  {
      Vec3<GLfloat> c(0, 0, 0);
      unsigned int cnt = 0;

      if (_pdata->meshview.is_initialized())
      {
          c += _pdata->meshview.center();
          ++cnt;
      }

      if (_pdata->pathlineview.is_initialized())
      {
          c += _pdata->pathlineview.center();
          ++cnt;
      }

      if (cnt > 1)
      { c /= cnt; }

      return c;
  }

  /*void VesselView::determine_visualization_mode()
  {
      if (_pdata->pathlineview.has_color_attribute())
      { set_visualization_mode_line_quantification(); }
      else
      { set_visualization_mode_default(); }
  }

  void VesselView::set_visualization_mode_default()
  {
      //_pdata->meshview.set_mode_front_face_culling_with_ghosted_view();
      //_pdata->meshview.set_mode_silhouette();
      _pdata->pathlineview.set_isl_enabled(true);
      _pdata->pathlineview.set_halo_enabled(true);

      this->emit_signal_update_required();
  }

  void VesselView::set_visualization_mode_line_quantification()
  {
      //_pdata->meshview.set_mode_silhouette();
      _pdata->pathlineview.set_isl_enabled(false);
      _pdata->pathlineview.set_halo_enabled(false);

      this->emit_signal_update_required();
  }*/

  void VesselView::clear()
  {
      _pdata->pathlineview.clear();
      _pdata->centerlineview.clear();
      _pdata->meshview.clear();
      _pdata->flowjetview.clear();
      _pdata->measuringplaneviews.clear();
      _pdata->pressureview.clear();
      set_name("vessel");
      this->emit_signal_scene_changed();
      this->emit_signal_update_required();
  }

  void VesselView::on_resize(GLint w, GLint h)
  {
      _pdata->pathlineview.on_resize(w, h);
      _pdata->centerlineview.on_resize(w, h);
      _pdata->meshview.on_resize(w, h);
      _pdata->flowjetview.on_resize(w, h);
      _pdata->pressureview.on_resize(w, h);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_resize(w, h); }
  }

  void VesselView::on_oit_enabled(bool b)
  {
      _pdata->pathlineview.set_oit_available(b);
      _pdata->centerlineview.set_oit_available(b);
      _pdata->meshview.set_oit_available(b);
      _pdata->flowjetview.set_oit_available(b);
      _pdata->pressureview.set_oit_available(b);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.set_oit_available(b); }
  }

  void VesselView::on_animation_enabled(bool b)
  {
      _pdata->pathlineview.set_animation_is_enabled(b);
      _pdata->centerlineview.set_animation_is_enabled(b);
      _pdata->meshview.set_animation_is_enabled(b);
      _pdata->flowjetview.set_animation_is_enabled(b);
      _pdata->pressureview.set_animation_is_enabled(b);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.set_animation_is_enabled(b); }
  }

  void VesselView::on_modelview_changed(bool b)
  {
      _pdata->pathlineview.set_modelview_matrix_changed(b);
      _pdata->centerlineview.set_modelview_matrix_changed(b);
      _pdata->meshview.set_modelview_matrix_changed(b);
      _pdata->flowjetview.set_modelview_matrix_changed(b);
      _pdata->pressureview.set_modelview_matrix_changed(b);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.set_modelview_matrix_changed(b); }
  }

  void VesselView::on_visible_changed(bool b)
  {
      _pdata->pathlineview.set_visible(b);
      _pdata->centerlineview.set_visible(b);
      _pdata->meshview.set_visible(b);
      _pdata->flowjetview.set_visible(b);
      _pdata->pressureview.set_visible(b);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.set_visible(b); }
  }

  void VesselView::on_mouse_pos_changed(GLint x, GLint y) {
      _pdata->pathlineview.on_mouse_pos_changed(x,y);
      _pdata->centerlineview.on_mouse_pos_changed(x,y);
      _pdata->meshview.on_mouse_pos_changed(x,y);
      _pdata->flowjetview.on_mouse_pos_changed(x,y);
      _pdata->pressureview.on_mouse_pos_changed(x,y);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_mouse_pos_changed(x,y); }
  }
  void VesselView::on_mouse_button_pressed(MouseButton_ btn) {
      _pdata->pathlineview.on_mouse_button_pressed(btn);
      _pdata->centerlineview.on_mouse_button_pressed(btn);
      _pdata->meshview.on_mouse_button_pressed(btn);
      _pdata->flowjetview.on_mouse_button_pressed(btn);
      _pdata->pressureview.on_mouse_button_pressed(btn);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_mouse_button_pressed(btn); }
  }
  void VesselView::on_mouse_button_released(MouseButton_ btn) {
      _pdata->pathlineview.on_mouse_button_released(btn);
      _pdata->centerlineview.on_mouse_button_released(btn);
      _pdata->meshview.on_mouse_button_released(btn);
      _pdata->flowjetview.on_mouse_button_released(btn);
      _pdata->pressureview.on_mouse_button_released(btn);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_mouse_button_released(btn); }
  }
  void VesselView::on_key_pressed(Key_ k) {
      _pdata->pathlineview.on_key_pressed(k);
      _pdata->centerlineview.on_key_pressed(k);
      _pdata->meshview.on_key_pressed(k);
      _pdata->flowjetview.on_key_pressed(k);
      _pdata->pressureview.on_key_pressed(k);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_key_pressed(k); }
  }
  void VesselView::on_key_released(Key_ k) {
      _pdata->pathlineview.on_key_released(k);
      _pdata->centerlineview.on_key_released(k);
      _pdata->meshview.on_key_released(k);
      _pdata->flowjetview.on_key_released(k);
      _pdata->pressureview.on_key_released(k);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_key_released(k); }
  }
  void VesselView::on_mouse_wheel_up() {
      _pdata->pathlineview.on_mouse_wheel_up();
      _pdata->centerlineview.on_mouse_wheel_up();
      _pdata->meshview.on_mouse_wheel_up();
      _pdata->flowjetview.on_mouse_wheel_up();
      _pdata->pressureview.on_mouse_wheel_up();

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_mouse_wheel_up(); }
  }
  void VesselView::on_mouse_wheel_down() {
      _pdata->pathlineview.on_mouse_wheel_down();
      _pdata->centerlineview.on_mouse_wheel_down();
      _pdata->meshview.on_mouse_wheel_down();
      _pdata->flowjetview.on_mouse_wheel_down();
      _pdata->pressureview.on_mouse_wheel_down();

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_mouse_wheel_down(); }
  }
  void VesselView::on_ssaa_factor_changed(GLint ssaa_factor) {
      _pdata->pathlineview.on_ssaa_factor_changed(ssaa_factor);
      _pdata->centerlineview.on_ssaa_factor_changed(ssaa_factor);
      _pdata->meshview.on_ssaa_factor_changed(ssaa_factor);
      _pdata->flowjetview.on_ssaa_factor_changed(ssaa_factor);
      _pdata->pressureview.on_ssaa_factor_changed(ssaa_factor);

      for (auto& mpv: _pdata->measuringplaneviews)
      { mpv.on_ssaa_factor_changed(ssaa_factor); }
  }
  
  void VesselView::on_animation_time_changed(double d)
  { _pdata->pressureview.on_animation_time_changed(d);}

  void VesselView::draw_opaque_impl()
  {
      _pdata->pressureview.draw_opaque();
      _pdata->meshview.draw_opaque();
      _pdata->pathlineview.draw_opaque();
      _pdata->centerlineview.draw_opaque();
      _pdata->flowjetview.draw_opaque();

      for (auto& mp: _pdata->measuringplaneviews)
      { mp.draw_opaque(); }
  }

  void VesselView::draw_transparent_impl()
  {
      _pdata->pressureview.draw_transparent();
      _pdata->meshview.draw_transparent();
      _pdata->pathlineview.draw_transparent();
      _pdata->centerlineview.draw_transparent();
      _pdata->flowjetview.draw_transparent();

      for (auto& mp: _pdata->measuringplaneviews)
      { mp.draw_transparent(); }
  }
} // namespace bk


