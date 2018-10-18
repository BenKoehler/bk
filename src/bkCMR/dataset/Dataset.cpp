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
#include <cassert>
#include <filesystem>
#include <future>
#include <utility>

#include <bk/Clock>
#include <bkCMR/dataset/Dataset.h>
#include <bkCMR/CardiacCycleDefinition.h>
#include <bkCMR/filters/CenterlineExtractor.h>
#include <bkCMR/FlowDirCorrection.h>
#include <bkCMR/dataset/FlowImage2DT.h>
#include <bkCMR/dataset/FlowImage3DT.h>
#include <bkCMR/FlowTimeShift.h>
#include <bkCMR/filters/IVSDImageFilter.h>
#include <bkCMR/filters/TMIPImageFilter.h>
#include <bkCMR/filters/LPCImageFilter.h>
#include <bkCMR/filters/PhaseUnwrapping2DT.h>
#include <bkCMR/filters/PhaseUnwrapping3DT.h>
#include <bkCMR/filters/PressureMapImageFilter.h>
#include <bkCMR/dataset/Vessel.h>
#include <bkCMR/sampling/VesselSegmentationInFlowFieldSizeImageFilter.h>
#include <bkDicom/DicomDirImporter_CMR.h>
#include <bkDicom/DicomImageInfos.h>
#include <bk/StringUtils>
#include <bk/ThreadPool>

#ifdef BK_EMIT_PROGRESS

    #include <bk/Localization>
    #include <bk/Progress>

#endif

namespace bk
{
  inline namespace cmr
  {
    //====================================================================================================
    //===== MEMBERS
    //====================================================================================================
    struct Dataset::Impl
    {
        std::string project_path; //!< path of current directory; ends with /
        DicomDirImporter_CMR importer;
        FlowImage3DT flow_image_3dt;
        FlowDirCorrection flow_dir_correction;
        FlowTimeShift flow_time_shift;
        CardiacCycleDefinition cardiac_cycle_definition;
        std::vector<Vessel> vessels;
        PhaseUnwrapping3DT phase_unwrapping_3dt;
        std::map<unsigned int/*imgId*/, PhaseUnwrapping2DT> phase_unwrapping_2dt;

        // todo velocity offset correction
    };

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    Dataset::Dataset() = default;
    Dataset::Dataset(const Dataset&) = default;
    Dataset::Dataset(Dataset&&) noexcept = default;
    Dataset::~Dataset() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const std::string& Dataset::project_path() const
    { return _pdata->project_path; }

    std::string Dataset::project_path_without_slash_ending() const
    { return _pdata->project_path.back() == '/' ? string_utils::chop_back(_pdata->project_path, 1) : _pdata->project_path; }

    DicomDirImporter_CMR& Dataset::importer()
    { return _pdata->importer; }

    const DicomDirImporter_CMR& Dataset::importer() const
    { return _pdata->importer; }

    bool Dataset::is_importer_loaded() const
    { return _pdata->importer.is_import_successful(); }

    bool Dataset::has_magnitude_images() const
    { return !_pdata->importer.class_3dt_magnitude_images().empty(); }

    bool Dataset::has_signal_intensity_image() const
    { return !_pdata->importer.class_3dt_signal_intensity_images().empty(); }

    std::vector<unsigned int> Dataset::ids_of_local_image_copies() const
    {
        const std::vector<unsigned int> class_3dt_flow_images = _pdata->importer.class_3dt_flow_images();
        const std::vector<unsigned int> class_3dt_anatomical_images = _pdata->importer.class_3dt_anatomical_images();
        const std::vector<unsigned int> class_3dt_magnitude_images = _pdata->importer.class_3dt_magnitude_images();
        const std::vector<unsigned int> class_3dt_signal_intensity_images = _pdata->importer.class_3dt_signal_intensity_images();
        const std::vector<unsigned int> class_2dt_flow_images = _pdata->importer.class_2dt_flow_images();
        const std::vector<unsigned int> class_2dt_anatomical_images = _pdata->importer.class_2dt_anatomical_images();
        const std::vector<unsigned int> class_2d_anatomical_images = _pdata->importer.class_2d_anatomical_images();
        const std::vector<unsigned int> class_3d_anatomical_images = _pdata->importer.class_3d_anatomical_images();

        unsigned int N = 0;
        N += class_3dt_flow_images.size();
        N += class_3dt_anatomical_images.size();
        N += class_3dt_magnitude_images.size();
        N += class_3dt_signal_intensity_images.size();
        N += class_2dt_flow_images.size();
        N += class_2dt_anatomical_images.size();
        N += class_2d_anatomical_images.size();
        N += class_3d_anatomical_images.size();

        std::vector<unsigned int> ids;
        ids.reserve(N);

        for (unsigned int x: class_3dt_flow_images)
        { ids.push_back(x); }

        for (unsigned int x: class_3dt_anatomical_images)
        { ids.push_back(x); }

        for (unsigned int x: class_3dt_magnitude_images)
        { ids.push_back(x); }

        for (unsigned int x: class_3dt_signal_intensity_images)
        { ids.push_back(x); }

        for (unsigned int x: class_2dt_flow_images)
        { ids.push_back(x); }

        for (unsigned int x: class_2dt_anatomical_images)
        { ids.push_back(x); }

        for (unsigned int x: class_2d_anatomical_images)
        { ids.push_back(x); }

        for (unsigned int x: class_3d_anatomical_images)
        { ids.push_back(x); }

        std::sort(ids.begin(), ids.end());

        return ids;
    }

    bool Dataset::has_local_image_copies() const
    {
        const std::vector<std::string> paths = filepaths_of_local_image_copies();

        for (const std::string& p: paths)
        {
            if (!std::filesystem::exists(p))
            { return false; }
        }

        return true;
    }

    bool Dataset::is_flow_image_3dt_loaded() const
    { return _pdata->flow_image_3dt.num_values() > 1; }

    FlowImage3DT* Dataset::flow_image_3dt()
    {
        if (!is_flow_image_3dt_loaded())
        { load_flow_image_3dt(); }

        if (!is_flow_image_3dt_loaded())
        { return nullptr; }

        return &_pdata->flow_image_3dt;
    }

    const FlowImage3DT* Dataset::flow_image_3dt() const
    { return &_pdata->flow_image_3dt; }

    bool Dataset::has_flow_dir_correction() const
    { return std::filesystem::exists(filepath_flow_dir_correction()); }

    bool Dataset::has_flow_time_shift() const
    { return std::filesystem::exists(filepath_flow_time_shift()); }

    bool Dataset::has_cardiac_cycle_definition() const
    { return std::filesystem::exists(filepath_cardiac_cycle_definition()); }

    unsigned int Dataset::num_vessels() const
    { return _pdata->vessels.size(); }

    Vessel* Dataset::vessel(unsigned int i)
    { return i < num_vessels() ? &_pdata->vessels[i] : nullptr; }

    Vessel* Dataset::vessel(std::string_view name, bool case_sensitive)
    {
        for (Vessel& v: _pdata->vessels)
        {
            if (string_utils::equals(v.name(), name, case_sensitive))
            { return &v; }
        }

        return nullptr;
    }

    const Vessel* Dataset::vessel(unsigned int i) const
    { return i < num_vessels() ? &_pdata->vessels[i] : nullptr; }

    const Vessel* Dataset::vessel(std::string_view name, bool case_sensitive) const
    {
        for (const Vessel& v: _pdata->vessels)
        {
            if (string_utils::equals(v.name(), name, case_sensitive))
            { return &v; }
        }

        return nullptr;
    }

    bool Dataset::has_vessel(std::string_view name, bool case_sensitive) const
    {
        if (std::filesystem::exists(dirpath_vessel_without_slash_ending(name)))
        { return true; }

        if (_pdata->vessels.empty())
        { return false; }

        for (const Vessel& v: _pdata->vessels)
        {
            if (string_utils::equals(v.name(), name, case_sensitive))
            { return true; }
        }

        return false;
    }

    bool Dataset::vessel_has_centerline_ids(const Vessel* v) const
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::vessel_has_centerline_ids - Vessel (v) is nullptr" << std::endl;
            return false;
        }

        return v->has_centerline_ids();
    }

    bool Dataset::vessel_has_centerlines(const Vessel* v) const
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::vessel_has_centerline_ids - Vessel (v) is nullptr" << std::endl;
            return false;
        }

        return v->has_centerlines();
    }

    bool Dataset::vessels_have_centerline_ids() const
    {
        for (const Vessel& v: _pdata->vessels)
        {
            if (!vessel_has_centerline_ids(&v))
            { return false; }
        }

        return true;
    }

    bool Dataset::vessels_have_centerlines() const
    {
        for (const Vessel& v: _pdata->vessels)
        {
            if (!vessel_has_centerlines(&v))
            { return false; }
        }

        return true;
    }

    FlowDirCorrection& Dataset::flow_image_3dt_dir_correction()
    { return _pdata->flow_dir_correction; }

    const FlowDirCorrection& Dataset::flow_image_3dt_dir_correction() const
    { return _pdata->flow_dir_correction; }

    FlowTimeShift& Dataset::flow_image_3dt_time_shift()
    { return _pdata->flow_time_shift; }

    const FlowTimeShift& Dataset::flow_image_3dt_time_shift() const
    { return _pdata->flow_time_shift; }

    CardiacCycleDefinition& Dataset::flow_image_3dt_cardiac_cycle_definition()
    { return _pdata->cardiac_cycle_definition; }

    const CardiacCycleDefinition& Dataset::flow_image_3dt_cardiac_cycle_definition() const
    { return _pdata->cardiac_cycle_definition; }

    const PhaseUnwrapping3DT& Dataset::phase_unwrapping_3dt() const
    { return _pdata->phase_unwrapping_3dt; }

    const std::map<unsigned int, PhaseUnwrapping2DT>& Dataset::phase_unwrapping_2dt() const
    { return _pdata->phase_unwrapping_2dt; }

    const PhaseUnwrapping2DT* Dataset::phase_unwrapping_2dt(unsigned int imgId) const
    {
        if (auto it = _pdata->phase_unwrapping_2dt.find(imgId); it != _pdata->phase_unwrapping_2dt.end())
        { return &it->second; }

        return nullptr;
    }

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    Dataset& Dataset::operator=(const Dataset&) = default;
    Dataset& Dataset::operator=(Dataset&&) noexcept = default;

    void Dataset::set_project_path(std::string_view path)
    {
        _pdata->project_path = path;

        _pdata->project_path = string_utils::replace(_pdata->project_path, "\\", "/");

        if (!string_utils::ends_with(path, "/"))
        { _pdata->project_path = string_utils::append(_pdata->project_path, "/"); }
    }

    Vessel* Dataset::add_vessel(std::string_view name)
    {
        Vessel v;
        v.set_name(name.data());

        return add_vessel(std::move(v));
    }

    Vessel* Dataset::add_vessel(Vessel&& v)
    {
        _pdata->vessels.emplace_back(std::move(v));
        return &_pdata->vessels.back();
    }

    bool Dataset::remove_vessel(Vessel* v)
    {
        if (v == nullptr)
        { return false; }

        _pdata->vessels.erase(std::remove_if(_pdata->vessels.begin(), _pdata->vessels.end(), [&](const Vessel& x) -> bool
        { return string_utils::equals(v->name(), x.name()); }), _pdata->vessels.end());

        if (std::filesystem::exists(dirpath_vessel_without_slash_ending(v)))
        { std::filesystem::remove_all(dirpath_vessel_without_slash_ending(v)); }

        return true;
    }

    bool Dataset::remove_vessel(std::string_view name)
    {
        bool success = false;

        if (std::filesystem::exists(dirpath_vessel_without_slash_ending(name)))
        {
            std::filesystem::remove_all(dirpath_vessel_without_slash_ending(name));
            success = true;
        }

        _pdata->vessels.erase(std::remove_if(_pdata->vessels.begin(), _pdata->vessels.end(), [&](const Vessel& v) -> bool
        {
            if (string_utils::equals(name, v.name()))
            {
                success = true;
                return true;
            }

            return false;
        }), _pdata->vessels.end());

        return success;
    }

    //====================================================================================================
    //===== IMAGES
    //====================================================================================================
    std::string Dataset::filepath_flow_image(unsigned int v) const
    { return _pdata->project_path + dcmbytes + string_utils::from_number(_pdata->importer.class_3dt_flow_images()[v]); }

    std::string Dataset::filepath_tmip_magnitude_3dt() const
    { return _pdata->project_path + "tmip_mag"; }

    std::string Dataset::filepath_lpc() const
    { return _pdata->project_path + "lpc"; }

    std::string Dataset::filepath_ivsd() const
    { return _pdata->project_path + "ivsd"; }

    std::string Dataset::filepath_tmip_signal_3dt() const
    { return _pdata->project_path + "tmip_signal"; }

    std::string Dataset::filepath_tmip_anatomical_3dt(unsigned int imgId) const
    {
        std::stringstream s;
        s << _pdata->project_path;
        s << "tmip_anatomy";
        s << imgId;

        return s.str();
    }

    std::vector<std::string> Dataset::filepaths_of_local_image_copies() const
    {
        const std::string filepath_base = string_utils::append(_pdata->project_path, dcmbytes);
        const std::vector<unsigned int> ids = ids_of_local_image_copies();

        std::vector<std::string> paths;
        paths.reserve(ids.size());

        for (unsigned int imgId: ids)
        { paths.emplace_back(filepath_base + string_utils::from_number(imgId)); }

        return paths;
    }

    std::string Dataset::filepath_pressure_map_of_vessel(const Vessel& v) const
    { return _pdata->project_path + vessel_dir + v.name() + "/" + v.name() + ".pm"; }

    std::string Dataset::filepath_static_tissue_threshold() const
    { return _pdata->project_path + "stati"; }

    bool Dataset::has_local_image_copy(std::string_view filepath) const
    { return std::filesystem::exists(filepath.data()); }

    bool Dataset::has_local_image_copy_dcmbytes(unsigned int imgId) const
    { return std::filesystem::exists(_pdata->project_path + dcmbytes + string_utils::from_number(imgId)); }

    std::unique_ptr<DicomImage<double, 3>> Dataset::load_local_image_copy(std::string_view filepath) const
    {
        if (has_local_image_copy(filepath))
        {
            std::ifstream file(filepath.data(), std::ios_base::in | std::ios_base::binary);

            if (file.good())
            {
                // size
                std::uint16_t sz[3] = {0, 0, 0};
                file.read(reinterpret_cast<char*>(sz), 3 * sizeof(std::uint16_t));

                // world matrix
                double w[25] = {0};
                file.read(reinterpret_cast<char*>(&w), 25 * sizeof(double));
                Mat5d W;
                for (unsigned int i = 0; i < 25; ++i)
                { W[i] = w[i]; }

                #ifdef BK_EMIT_PROGRESS
                const unsigned int N = sz[0] * sz[1] * sz[2];
                Progress& prog = bk_progress.emplace_task(N + N / 4, ___("Loading local image"));
                #endif

                auto img = std::make_unique<DicomImage<double, 3>>();
                img->set_size(sz[0], sz[1], sz[2]);
                img->geometry().transformation().set_world_matrix(W);
                img->geometry().transformation().set_dicom_image_type_3d();

                #ifdef BK_EMIT_PROGRESS
                prog.increment(N / 4);
                #endif

                // values
                std::vector<double> buf(img->num_values());
                file.read(reinterpret_cast<char*>(buf.data()), img->num_values() * sizeof(double));

                #ifdef BK_EMIT_PROGRESS
                prog.increment(N / 2);
                #endif

                #pragma omp parallel for
                for (int i = 0; i < static_cast<int>(img->num_values()); ++i)
                { (*img)[i] = buf[i]; }

                #ifdef BK_EMIT_PROGRESS
                prog.increment(N / 4);
                #endif

                buf.clear();
                buf.shrink_to_fit();

                #ifdef BK_EMIT_PROGRESS
                prog.increment(N / 4);
                #endif

                file.close();

                #ifdef BK_EMIT_PROGRESS
                prog.set_finished();
                #endif

                return img;
            }
        }

        std::cerr << "loading local image copy failed (" << filepath.data() << ")" << std::endl;

        return nullptr;
    }

    std::unique_ptr<DicomImage<double, -1>> Dataset::load_local_image_copy_dcmbytes(unsigned int imgId) const
    {
        if (has_local_image_copy_dcmbytes(imgId))
        {
            const std::string dcmpath = _pdata->project_path + dcmbytes + string_utils::from_number(imgId);
            bool success = false;
            const std::vector<char> imgBytes = _pdata->importer.load_dcm_image_bytes(dcmpath, &success);

            if (success)
            { return _pdata->importer.read_image_from_bytes(imgId, imgBytes); }
        }

        // no local image copy or file not good -> read from original data
        return _pdata->importer.read_image(imgId);
    }

    bool Dataset::load_flow_image_3dt(DatasetFilter flags)
    {
        const std::vector<unsigned int> flow_image_ids = _pdata->importer.class_3dt_flow_images();

        if (flow_image_ids.size() != 3)
        {
            std::cerr << "Dataset::load_flow_image_3dt - number of flow images != 3" << std::endl;
            return false;
        }

        std::array<double, 3> venc = {//
            _pdata->importer.venc_in_m_per_s(flow_image_ids[0]),//
            _pdata->importer.venc_in_m_per_s(flow_image_ids[1]),//
            _pdata->importer.venc_in_m_per_s(flow_image_ids[2])//
        };

        const std::reference_wrapper<DicomImageInfos> info[3] = {_pdata->importer.image_infos(flow_image_ids[0]), _pdata->importer.image_infos(flow_image_ids[1]), _pdata->importer.image_infos(flow_image_ids[2])};

        #ifdef BK_EMIT_PROGRESS
        const unsigned int numel = info[0].get().Rows * info[0].get().Columns * info[0].get().Slices * info[0].get().TemporalPositions;
        Progress& prog = bk_progress.emplace_task(4 * numel + 10, ___("Loading 3D+T flow images"));
        #endif

        std::unique_ptr<DicomImage<double, -1>> f[3] = {nullptr, nullptr, nullptr};

        //------------------------------------------------------------------------------------------------------
        // has local image copies?
        //------------------------------------------------------------------------------------------------------
        if (has_local_image_copies())
        {
            for (unsigned int v = 0; v < 3; ++v)
            {
                bool success = true;

                f[v] = std::move(_pdata->importer.read_image_from_bytes(flow_image_ids[v], _pdata->importer.load_dcm_image_bytes(filepath_flow_image(v), &success)));

                if (!success)
                { std::cerr << "Dataset::load_flow_image_3dt - could not read local flow image copy " << v << std::endl; }

                #ifdef BK_EMIT_PROGRESS
                prog.increment(numel);
                #endif
            }
        }
            //------------------------------------------------------------------------------------------------------
            // default loading
            //------------------------------------------------------------------------------------------------------
        else
        {
            for (unsigned int v = 0; v < 3; ++v)
            {
                f[v] = std::move(_pdata->importer.read_image(flow_image_ids[v]));

                #ifdef BK_EMIT_PROGRESS
                prog.increment(numel);
                #endif
            }
        }

        const auto size = f[0]->geometry().size();
        _pdata->flow_image_3dt.set_size(size);
        _pdata->flow_image_3dt.geometry().transformation().set_world_matrix(info[0].get().worldMatrix, info[0].get().TemporalResolution);

        // derive flow vector ordering from world matrix
        unsigned int order[3] = {0, 1, 2};
        double dirfac[3] = {1, 1, 1};
        for (unsigned int colid = 0; colid < 3; ++colid)
        {
            double maxabsval = 0;
            for (unsigned int rowid = 0; rowid < 3; ++rowid)
            {
                const double tempval = info[0].get().worldMatrix(rowid, colid);
                if (maxabsval < std::abs(tempval))
                {
                    maxabsval = std::abs(tempval);

                    order[colid] = rowid;
                    dirfac[colid] = std::signbit(tempval) ? -1 : +1;
                }
            }
        }

        const int lipv[3] = {info[0].get().LargestImagePixelValue / 2, info[1].get().LargestImagePixelValue / 2, info[2].get().LargestImagePixelValue / 2};

        #pragma omp parallel for
        for (unsigned int z = 0; z < size[2]; ++z)
        {
            for (unsigned int y = 0; y < size[1]; ++y)
            {
                for (unsigned int x = 0; x < size[0]; ++x)
                {
                    for (unsigned int t = 0; t < size[3]; ++t)
                    {
                        const unsigned int lid = grid_to_list_id(size, x, y, z, t);
                        Vec3d v((*f[order[0]])[lid] - lipv[order[0]], (*f[order[1]])[lid] - lipv[order[1]], (*f[order[2]])[lid] - lipv[order[2]]);

                        v[0] /= static_cast<double>(lipv[order[0]]) * dirfac[0];
                        v[0] *= venc[0];

                        v[1] /= static_cast<double>(lipv[order[1]]) * dirfac[1];
                        v[1] *= venc[1];

                        v[2] /= static_cast<double>(lipv[order[2]]) * dirfac[2];
                        v[2] *= venc[2];

                        _pdata->flow_image_3dt[lid] = std::move(v);
                    }
                }
            }

            #ifdef BK_EMIT_PROGRESS
                #pragma omp critical
            { prog.increment(size[0] * size[1] * size[3]); }
            #endif
        }

        if (flags & DatasetFilter_FlowDirCorrection)
        { _pdata->flow_dir_correction.apply(_pdata->flow_image_3dt); }

        if (flags & DatasetFilter_PhaseUnwrapping)
        {
            load_phase_unwrapping_3dt();
            _pdata->phase_unwrapping_3dt.apply(_pdata->flow_image_3dt, venc);
        }

        if (flags & DatasetFilter_VelocityOffset)
        {
            //load_eddy_current_correction_3dt();
            //correct_eddy_currents_3dt();
            // todo
        }

        if (flags & DatasetFilter_TimeShift)
        { _pdata->flow_time_shift.apply(_pdata->flow_image_3dt); }

        _pdata->flow_image_3dt.calc_world_matrix_rotational_part();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    std::vector<std::unique_ptr<FlowImage2DT>> Dataset::flow_images_2dt(DatasetFilter flags)
    {
        const std::vector<unsigned int> imgIds = _pdata->importer.class_2dt_flow_images();
        std::vector<std::unique_ptr<FlowImage2DT>> images;
        images.reserve(imgIds.size());

        for (unsigned int dcm_id: imgIds)
        { images.emplace_back(flow_image_2dt(dcm_id, flags)); }

        return images;
    }

    std::unique_ptr<FlowImage2DT> Dataset::flow_image_2dt(unsigned int dcm_id, DatasetFilter flags)
    {
        std::unique_ptr<DicomImage<double, -1>> img = has_local_image_copy_dcmbytes(dcm_id) ? load_local_image_copy_dcmbytes(dcm_id) : _pdata->importer.read_image(dcm_id);

        auto ff = std::make_unique<FlowImage2DT>();
        ff->set_size(img->size());
        ff->geometry().transformation().set_world_matrix(img->geometry().transformation().world_matrix_with_time());

        #pragma omp parallel for
        for (unsigned int i = 0; i < img->num_values(); ++i)
        { (*ff)[i] = (*img)[i]; }

        if (flags & DatasetFilter_PhaseUnwrapping)
        {
            PhaseUnwrapping2DT pu;
            pu.apply(*ff, _pdata->importer.venc_in_m_per_s(dcm_id));
        }

        if (flags & DatasetFilter_VelocityOffset)
        {
            // todo
        }

        return ff;
    }

    int Dataset::anatomical_2dt_image_id_of_flow_image_2dt(unsigned int flowimg_dcm_id, bool* success)
    {
        // assumption:
        // flow and anatomical image correspond if they have an identical world matrix

        const DicomImageInfos& ffinfo = _pdata->importer.image_infos(flowimg_dcm_id);

        for (auto aid : _pdata->importer.class_2dt_anatomical_images())
        {
            if (ffinfo.worldMatrix == _pdata->importer.image_infos(aid).worldMatrix)
            {
                if (success != nullptr)
                { *success = true; }

                return aid;
            }
        }

        if (success != nullptr)
        { *success = false; }

        return -1;
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::lpc() const
    {
        if (has_local_image_copy(filepath_lpc()))
        { return load_local_image_copy(filepath_lpc()); }

        assert(is_flow_image_3dt_loaded() && "3dt flow image must be loaded prior to calling this function!");
        return LPCImageFilter::apply(_pdata->flow_image_3dt);
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::lpc(bool load_flow_img_if_necessary)
    {
        if (has_local_image_copy(filepath_lpc()))
        { return load_local_image_copy(filepath_lpc()); }

        if (load_flow_img_if_necessary && !is_flow_image_3dt_loaded())
        { load_flow_image_3dt(); }

        return lpc();
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::ivsd() const
    {
        if (has_local_image_copy(filepath_ivsd()))
        { return load_local_image_copy(filepath_ivsd()); }

        assert(is_flow_image_3dt_loaded() && "3dt flow image must be loaded prior to calling this function!");
        return IVSDImageFilter::apply(_pdata->flow_image_3dt);
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::ivsd(bool load_flow_img_if_necessary, DatasetFilter flags)
    {
        if (has_local_image_copy(filepath_ivsd()))
        { return load_local_image_copy(filepath_ivsd()); }

        if (load_flow_img_if_necessary && !is_flow_image_3dt_loaded())
        { load_flow_image_3dt(flags); }

        return ivsd();
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::tmip_magnitude_3dt() const
    {
        if (has_local_image_copy(filepath_tmip_magnitude_3dt()))
        { return load_local_image_copy(filepath_tmip_magnitude_3dt()); }

        const std::vector<unsigned int> magnitudeImageIds = _pdata->importer.class_3dt_magnitude_images();
        const unsigned int numMagnitudeImages = magnitudeImageIds.size();

        assert((numMagnitudeImages == 1 || numMagnitudeImages == 3) && "invalid number of magnitude images (should be 1 or 3)!");

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(numMagnitudeImages, ___("Loading magnitude images"));
        #endif

        std::vector<std::unique_ptr<DicomImage<double, -1>>> m;

        for (unsigned int id : magnitudeImageIds)
        {
            m.push_back(load_local_image_copy_dcmbytes(id));

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        if (numMagnitudeImages == 1)
        { return TMIPImageFilter::apply(*m[0]); }
        else //if (numMagnitudeImages == 3)
        { return TMIPImageFilter::apply(*m[0], *m[1], *m[2]); }
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::tmip_signal_intensity_3dt() const
    {
        if (has_local_image_copy(filepath_tmip_signal_3dt()))
        { return load_local_image_copy(filepath_tmip_signal_3dt()); }

        const std::vector<unsigned int> imageIds = _pdata->importer.class_3dt_signal_intensity_images();
        if (imageIds.empty())
        {
            std::cerr << "signal intensity images not found! (image id not present in importer's class)" << std::endl;
            return nullptr;
        }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(1, ___("Loading signal intensity image"));
        #endif

        std::unique_ptr<DicomImage<double, -1>> m = load_local_image_copy_dcmbytes(imageIds[0]);

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return TMIPImageFilter::apply(*m);
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::tmip_anatomical_3dt(unsigned int dcmImgId) const
    {
        if (has_local_image_copy(filepath_tmip_anatomical_3dt(dcmImgId)))
        { return load_local_image_copy(filepath_tmip_anatomical_3dt(dcmImgId)); }

        const std::vector<unsigned int> imageIds = _pdata->importer.class_3dt_anatomical_images();
        if (std::find(imageIds.begin(), imageIds.end(), dcmImgId) == imageIds.end())
        {
            std::cerr << "anatomical image " << dcmImgId << " not found! (image id not present in importer's class)" << std::endl;
            return nullptr;
        }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(1, ___("Loading anatomical image"));
        #endif

        std::unique_ptr<DicomImage<double, -1>> m = load_local_image_copy_dcmbytes(dcmImgId);

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return TMIPImageFilter::apply(*m);
    }

    std::unique_ptr<DicomImage<double, 3>> Dataset::anatomical_image_3d(unsigned int dcmImgId) const
    {
        const std::unique_ptr<DicomImage<double, -1>> m = load_local_image_copy_dcmbytes(dcmImgId);

        auto res = std::make_unique<DicomImage<double, 3>>();
        res->set_size(m->size());
        res->geometry().transformation().set_world_matrix(m->geometry().transformation().world_matrix_with_time());
        res->geometry().transformation().set_dicom_image_type_3d();

        #pragma omp parallel for
        for (unsigned int i = 0; i < m->num_values(); ++i)
        { (*res)[i] = (*m)[i]; }

        return res;
    }

    std::unique_ptr<DicomImage<double, 4>> Dataset::pressure_map(PressureMapImageFilter pmf) const
    {
        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(_pdata->vessels.size(), ___("Loading pressure map"));
        #endif

        std::vector<std::reference_wrapper<const Vessel>> vesselsToProcess;
        std::vector<bool> hasPressureMap(_pdata->vessels.size(), false);

        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            hasPressureMap[i] = has_local_image_copy(filepath_pressure_map_of_vessel(_pdata->vessels[i]));

            if (!hasPressureMap[i])
            { vesselsToProcess.emplace_back(_pdata->vessels[i]); }
        }

        /*
         * calculate pressure map for vessels that were not processed yet
         */
        std::unique_ptr<DicomImage<double, 4>> pm = nullptr;

        if (!vesselsToProcess.empty())
        {
            pm = pmf.apply(_pdata->flow_image_3dt, vesselsToProcess);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(vesselsToProcess.size());
            #endif
        }
        else // if no new vessel had to be processed
        {
            pm->set_size(_pdata->flow_image_3dt.size());
            pm->geometry().transformation().set_world_matrix(_pdata->flow_image_3dt.geometry().transformation().world_matrix_with_time());
        }

        /*
         * copy existing pressure maps
         */
        for (unsigned int i = 0; i < num_vessels(); ++i)
        {
            if (hasPressureMap[i])
            {
                std::ifstream file(filepath_pressure_map_of_vessel(_pdata->vessels[i]), std::ios_base::in | std::ios_base::binary);

                if (file.good())
                {
                    std::uint16_t pos[3] = {0, 0, 0};
                    std::vector<double> buf(_pdata->flow_image_3dt.size(3));

                    while (!file.eof())
                    {
                        file.read(reinterpret_cast<char*>(pos), 3 * sizeof(std::uint16_t));
                        file.read(reinterpret_cast<char*>(buf.data()), _pdata->flow_image_3dt.size(3) * sizeof(double));

                        for (unsigned int t = 0; t < _pdata->flow_image_3dt.size(3); ++t)
                        { (*pm)(pos[0], pos[1], pos[2], t) = buf[t]; }
                    }

                    file.close();
                }

                #ifdef BK_EMIT_PROGRESS
                prog.increment(1);
                #endif
            }
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return pm;
    }

    std::unique_ptr<DicomImage<double, 4>> Dataset::pressure_map() const
    { return pressure_map(PressureMapImageFilter()); }

    std::unique_ptr<DicomImage<double, 3>> Dataset::vessel_segmentation_in_flow_field_3dt_size(const Vessel& v) const
    {
        assert(is_flow_image_3dt_loaded() && "3dt flow image must be loaded!");
        return VesselSegmentationInFlowFieldSizeImageFilter::apply(_pdata->flow_image_3dt, v);
    }

    bool Dataset::extract_centerlines(Vessel* v, unsigned int upscale, int distance_penalty_exponent, unsigned int smooth_iterations, unsigned int smooth_kernel_size, double smooth_relaxation)
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::extract_centerlines - Vessel (v) is nullptr!" << std::endl;
            return false;
        }

        CenterlineExtractor centerlineExtractor;
        centerlineExtractor.set_image_upscale(upscale);
        centerlineExtractor.set_distance_penalty_exponent(distance_penalty_exponent);
        centerlineExtractor.set_num_smooth_iterations(smooth_iterations);
        centerlineExtractor.set_smooth_kernel_size(smooth_kernel_size);
        centerlineExtractor.set_smooth_relaxation(smooth_relaxation);

        const auto seg = this->vessel_segmentation_in_flow_field_3dt_size(*v);
        auto[centerlines, success] = centerlineExtractor.extract_centerlines(v->mesh(), *seg, v->centerline_seed_id(), v->centerline_target_ids());

        v->clear_centerlines();
        v->add_centerlines(centerlines.begin(), centerlines.end());

        return success;
    }

    bool Dataset::extract_centerlines(unsigned int upscale, int distance_penalty_exponent, unsigned int smooth_iterations, unsigned int smooth_kernel_size, double smooth_relaxation)
    {
        unsigned int numCenterlinesTotal = 0;
        const unsigned int numVessels = num_vessels();

        #ifdef BK_EMIT_PROGRESS
        for (auto& v: _pdata->vessels)
        { numCenterlinesTotal += v.centerline_target_ids().size(); }

        Progress& prog = bk_progress.emplace_task(numCenterlinesTotal, ___("Extracting centerlines of all vessels"));
        #endif

        Clock clock;
        clock.start();

        bool overallSuccess = true;

        for (auto& v: _pdata->vessels)
        {
            const bool success = extract_centerlines(&v, upscale, distance_penalty_exponent, smooth_iterations, smooth_kernel_size, smooth_relaxation);
            overallSuccess = success && overallSuccess;

            #ifndef BK_EMIT_PROGRESS
            numCenterlinesTotal += v.centerlines().num_lines();
            #else
            prog.increment(1);
            #endif
        }

        clock.stop();

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        std::cout << "Extracted " << numCenterlinesTotal << " centerlines in " << numVessels << " vessels " << clock.time_in_sec() << " s (" << clock.time_in_milli_sec() << " ms)" << std::endl;

        return overallSuccess;
    }

    bool Dataset::has_static_tissue_threshold() const
    { return std::filesystem::exists(filepath_static_tissue_threshold()); }

    double Dataset::static_tissue_threshold() const
    {
        double sc = 0;

        if (!has_static_tissue_threshold())
        {
            std::cerr << "Dataset::static_tissue_threshold - stati file not found" << std::endl;
            return sc;
        }

        std::ifstream file(filepath_static_tissue_threshold(), std::ios_base::in | std::ios_base::binary);
        if (file.good())
        { file.read(reinterpret_cast<char*>(&sc), sizeof(double)); }
        file.close();

        return sc;
    }

    //====================================================================================================
    //===== FILTERS
    //====================================================================================================
    bool Dataset::determine_phase_wraps_2dt()
    {
        const std::vector<unsigned int> imgIds = _pdata->importer.class_2dt_flow_images();

        _pdata->phase_unwrapping_2dt.clear();

        for (unsigned int id: imgIds)
        {
            std::unique_ptr<FlowImage2DT> ff = flow_image_2dt(id, DatasetFilter_None);

            PhaseUnwrapping2DT pu;

            const bk::Clock clock = pu.init(*ff, _pdata->importer.venc_in_m_per_s(id));
            std::cout << "phase unwrapping 2D+T (img " << id << "):" << std::endl;
            std::cout << "\tN wrapped voxels: " << pu.num_wrapped_voxels() << std::endl;
            std::cout << "\ttime: " << clock.time_in_milli_sec() << " ms" << std::endl;

            _pdata->phase_unwrapping_2dt.emplace(id, std::move(pu));
        }

        return true;
    }

    bool Dataset::determine_phase_wraps_3dt(bool reload_flow_image)
    {
        if (reload_flow_image)
        {
            //load_flow_image_3dt(DatasetFilter_None);
            load_flow_image_3dt(DatasetFilter_FlowDirCorrection);
        }

        if (!is_flow_image_3dt_loaded())
        { return false; }

        std::vector<unsigned int> flow_img_ids = _pdata->importer.class_3dt_flow_images();

        std::array<double, 3> venc = {//
            _pdata->importer.venc_in_m_per_s(flow_img_ids[0]),//
            _pdata->importer.venc_in_m_per_s(flow_img_ids[1]),//
            _pdata->importer.venc_in_m_per_s(flow_img_ids[2])//
        };

        const bk::Clock clock = _pdata->phase_unwrapping_3dt.init(_pdata->flow_image_3dt, venc);
        std::cout << "phase unwrapping 3D+T:" << std::endl;
        std::cout << "\tN wrapped voxels in x: " << _pdata->phase_unwrapping_3dt.num_wrapped_voxels(0) << std::endl;
        std::cout << "\tN wrapped voxels in y: " << _pdata->phase_unwrapping_3dt.num_wrapped_voxels(1) << std::endl;
        std::cout << "\tN wrapped voxels in z: " << _pdata->phase_unwrapping_3dt.num_wrapped_voxels(2) << std::endl;
        std::cout << "\ttime: " << clock.time_in_sec() << " s" << std::endl;

        return true;
    }

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void Dataset::clear()
    {
        // todo

        _pdata->project_path = "";
        _pdata->importer.clear();
        _pdata->flow_image_3dt.clear();
        _pdata->flow_dir_correction;
        _pdata->vessels.clear();
        _pdata->phase_unwrapping_3dt.clear();
        _pdata->phase_unwrapping_2dt.clear();
    }

    void Dataset::delete_local_image_copies_if_incomplete() const
    {
        if (!has_local_image_copies())
        { delete_local_image_copies(); }
    }

    void Dataset::delete_local_image_copies() const
    {
        std::vector<std::string> paths = filepaths_of_local_image_copies();
        paths.emplace_back(filepath_tmip_magnitude_3dt());
        paths.emplace_back(filepath_lpc());
        paths.emplace_back(filepath_ivsd());
        paths.emplace_back(filepath_tmip_signal_3dt());

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(paths.size(), ___("Removing local image copies"));
        #endif

        for (const std::string& p: paths)
        {
            std::filesystem::remove(p);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif
    }

    std::vector<double> Dataset::mean_forward_velocity_in_vessel(Vessel* v, DatasetFilter filter)
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::mean_forward_velocity_in_vessel - v is nullptr" << std::endl;
            return {};
        }

        auto fut_cl = bk_threadpool.enqueue([&]()
                                            {
                                                #pragma omp parallel for
                                                for (unsigned int i = 0; i < v->num_centerlines(); ++i)
                                                { v->centerlines()[i].geometry().construct_kd_tree(); }
                                            });

        auto fut_f = bk_threadpool.enqueue([&]()
                                           {
                                               if (!is_flow_image_3dt_loaded())
                                               { load_flow_image_3dt(filter); }
                                           });

        fut_cl.get();
        fut_f.get();

        FlowImage3DT* f = flow_image_3dt();
        const auto fsize = f->geometry().size();
        const std::unique_ptr<DicomImage<double, 3>> seg = vessel_segmentation_in_flow_field_3dt_size(*v);

        const unsigned int T = fsize[3];
        std::vector<double> avgVelo(T + 1, 0);
        std::vector<unsigned int> cnt(T + 1, 0);

        #pragma omp parallel for schedule(dynamic, 1)
        for (unsigned int x = 0; x < fsize[0]; ++x)
        {
            for (unsigned int y = 0; y < fsize[1]; ++y)
            {
                for (unsigned int z = 0; z < fsize[2]; ++z)
                {
                    if ((*seg)(x, y, z) == 0)
                    { continue; }

                    const auto worldPos3D = f->geometry().transformation().to_world_coordinates(x, y, z, 0).sub_matrix<0, 2, 0, 0>();
                    const auto[clid, cp] = v->closest_centerline_and_point_id(worldPos3D);
                    const auto& cl = v->centerlines()[clid];
                    const auto clTangent = cl.local_coordinate_system_at_point(cp.point_id).col(2);

                    for (unsigned int t = 0; t < T; ++t)
                    {
                        const bk::Vec3d veloVec = f->flow_vector_world_rotated_at_grid_pos(x, y, z, t);
                        const double forwardVelocity = veloVec.dot(clTangent);

                        #pragma omp critical(mean_forward_velocity_in_vessel)
                        {
                            avgVelo[t] += forwardVelocity;
                            ++cnt[t];
                        }
                    } // for t
                } // for z
            } // for y
        } // for x

        for (unsigned int t = 0; t < T; ++t)
        { avgVelo[t] /= cnt[t]; }

        // copy last from first time step since the dataset is cyclic
        avgVelo.back() = avgVelo.front();

        return avgVelo;
    }

    //====================================================================================================
    //===== I/O
    //====================================================================================================
    std::string Dataset::filepath_importer() const
    { return _pdata->project_path + _pdata->importer.dataset_name() + ".cmr"; }

    std::string Dataset::filepath_flow_dir_correction() const
    { return _pdata->project_path + "dir.fdc"; }

    std::string Dataset::filepath_flow_time_shift() const
    { return _pdata->project_path + "fts"; }

    std::string Dataset::filepath_cardiac_cycle_definition() const
    { return _pdata->project_path + "ccd"; }

    std::string Dataset::filepath_phase_unwrapping_2dt() const
    { return _pdata->project_path + "phase_wraps_2dt.pu"; }

    std::string Dataset::filepath_phase_unwrapping_3dt() const
    { return _pdata->project_path + "phase_wraps_3dt.pu"; }

    std::string Dataset::dirpath_vessels() const
    { return dirpath_vessels_without_slash_ending() + "/"; }

    std::string Dataset::dirpath_vessels_without_slash_ending() const
    { return project_path() + vessel_dir; }

    std::string Dataset::dirpath_vessel(const Vessel* v) const
    { return dirpath_vessel_without_slash_ending(v) + "/"; }

    std::string Dataset::dirpath_vessel(std::string_view name) const
    { return dirpath_vessel_without_slash_ending(name) + "/"; }

    std::string Dataset::dirpath_vessel_without_slash_ending(const Vessel* v) const
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::dirpath_vessel - vessel is nullptr" << std::endl;
            return project_path() + vessel_dir;
        }

        return project_path() + vessel_dir + v->name();
    }

    std::string Dataset::dirpath_vessel_without_slash_ending(std::string_view name) const
    { return project_path() + vessel_dir + name.data(); }

    std::string Dataset::filepath_segmentation3D_of_vessel(const Vessel* v) const
    { return filepath_segmentation3D_of_vessel(v->name()); }

    std::string Dataset::filepath_segmentation3D_of_vessel(std::string_view name) const
    { return dirpath_vessel(name) + name.data() + ".seg3"; }

    std::string Dataset::filepath_mesh_of_vessel(const Vessel* v) const
    { return filepath_mesh_of_vessel(v->name()); }

    std::string Dataset::filepath_mesh_of_vessel(std::string_view name) const
    { return dirpath_vessel(name) + name.data() + ".mesh"; }

    std::string Dataset::filepath_centerline_ids_of_vessel(const Vessel* v) const
    { return filepath_centerline_ids_of_vessel(v->name()); }

    std::string Dataset::filepath_centerline_ids_of_vessel(std::string_view name) const
    { return dirpath_vessel(name) + name.data() + ".clids"; }

    std::string Dataset::filepath_centerlines_of_vessel(const Vessel* v) const
    { return filepath_centerlines_of_vessel(v->name()); }

    std::string Dataset::filepath_centerlines_of_vessel(std::string_view name) const
    { return dirpath_vessel(name) + name.data() + ".cl"; }

    std::string Dataset::filepath_land_marks_of_vessel(const Vessel* v) const { return filepath_land_marks_of_vessel(v->name()); }
    std::string Dataset::filepath_land_marks_of_vessel(std::string_view name) const { return dirpath_vessel(name) + name.data() + ".lm"; }

    std::string Dataset::filepath_flowjet_of_vessel(const Vessel* v) const
    { return filepath_flowjet_of_vessel(v->name()); }

    std::string Dataset::filepath_flowjet_of_vessel(std::string_view name) const
    { return dirpath_vessel(name) + name.data() + ".fj"; }

    bool Dataset::delete_file_if_exists(std::string_view path) const
    {
        if (std::filesystem::exists(path))
        {
            std::filesystem::remove(path);
            return true;
        }

        return false;
    }

    bool Dataset::save_local_dcmbyte_image_copies() const
    {
        const std::vector<unsigned int> ids = ids_of_local_image_copies();
        const std::vector<std::string> filepaths = filepaths_of_local_image_copies();
        std::vector<std::future<void>> tasks;

        if (ids.empty())
        { return false; }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(ids.size(), ___("Saving local DICOM image copies"));
        #endif

        for (unsigned int i = 0; i < ids.size(); ++i)
        {
            tasks.emplace_back(bk_threadpool.enqueue([&, i]()
                                                     {
                                                         std::vector<char> imgbytes = std::move(this->_pdata->importer.read_image_bytes(ids[i]));
                                                         std::ofstream file(filepaths[i], std::ios_base::out | std::ios_base::binary);

                                                         if (file.good())
                                                         {
                                                             file.write(imgbytes.data(), imgbytes.size() * sizeof(char));
                                                             file.close();
                                                         }

                                                         #ifdef BK_EMIT_PROGRESS
                                                         prog.increment(1);
                                                         #endif
                                                     }));
        }

        for (std::future<void>& f: tasks)
        { f.get(); }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    bool Dataset::save_local_image_copy(std::string_view filepath, const DicomImage<double, 3>& img) const
    {
        std::ofstream file(filepath.data(), std::ios_base::out | std::ios_base::binary);

        if (!file.good())
        { return false; }

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(3, ___("Saving local image copy"));
        #endif

        std::uint16_t ui16temp = 0;
        double dtemp = 0;

        // size
        const auto size = img.geometry().size();
        for (unsigned int i = 0; i < 3; ++i)
        {
            ui16temp = size[i];
            file.write(reinterpret_cast<char*>(&ui16temp), sizeof(std::uint16_t));
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // world matrix
        const Mat5d& w = img.geometry().transformation().world_matrix_with_time();
        for (unsigned int i = 0; i < w.num_elements(); ++i)
        {
            dtemp = w[i];
            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        // values
        for (unsigned int i = 0; i < img.num_values(); ++i)
        {
            dtemp = img[i];
            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        file.close();

        return true;
    }

    bool Dataset::save_importer() const
    { return _pdata->importer.save(filepath_importer()); }

    bool Dataset::load_importer()
    { return _pdata->importer.load(filepath_importer()); }

    bool Dataset::save_pressure_map(PressureMapImageFilter pmf) const
    {
        bool success = true;
        const std::unique_ptr<DicomImage<double, 4>> pm = pressure_map(pmf);

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(_pdata->vessels.size() * 5, ___("Saving pressure map"));
        #endif

        for (const Vessel& v: _pdata->vessels)
        {
            std::ofstream file(filepath_pressure_map_of_vessel(v), std::ios_base::out | std::ios_base::binary);

            if (!file.good())
            {
                success = false;
                continue;
            }

            double dtemp = 0;
            const auto size = pm->size();
            const std::unique_ptr<DicomImage<double, 3>> seg = vessel_segmentation_in_flow_field_3dt_size(v);

            #ifdef BK_EMIT_PROGRESS
            prog.increment(1);
            #endif

            // values
            //    - large parts of the image (everything outside segmentation) will be 0.
            //    -> save only positions inside seg
            for (std::uint16_t x = 0; x < static_cast<std::uint16_t>(size[0]); ++x)
            {
                for (std::uint16_t y = 0; y < static_cast<std::uint16_t>(size[1]); ++y)
                {
                    for (std::uint16_t z = 0; z < static_cast<std::uint16_t>(size[2]); ++z)
                    {
                        if ((*seg)(x, y, z) == 0)
                        { continue; }

                        file.write(reinterpret_cast<char*>(&x), sizeof(std::uint16_t));
                        file.write(reinterpret_cast<char*>(&y), sizeof(std::uint16_t));
                        file.write(reinterpret_cast<char*>(&z), sizeof(std::uint16_t));

                        // save values of all temporal positions
                        for (std::uint16_t t = 0; t < static_cast<std::uint16_t>(size[3]); ++t)
                        {
                            dtemp = (*pm)(x, y, z, t);
                            file.write(reinterpret_cast<char*>(&dtemp), sizeof(double));
                        } // for t
                    } // for z
                } // for y
            } // for x

            file.close();

            #ifdef BK_EMIT_PROGRESS
            prog.increment(4);
            #endif
        } // for v: vessels

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return success;
    }

    bool Dataset::save_pressure_map() const
    { return save_pressure_map(PressureMapImageFilter()); }

    bool Dataset::delete_file_pressure_map_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_if_exists(filepath_pressure_map_of_vessel(*v)); }

    bool Dataset::save_flow_dir_correction()
    { return _pdata->flow_dir_correction.save(filepath_flow_dir_correction()); }

    bool Dataset::load_flow_dir_correction()
    { return _pdata->flow_dir_correction.load(filepath_flow_dir_correction()); }

    bool Dataset::delete_file_flow_dir_correction()
    { return delete_file_if_exists(filepath_flow_dir_correction()); }

    bool Dataset::save_flow_time_shift()
    { return _pdata->flow_time_shift.save(filepath_flow_time_shift()); }

    bool Dataset::load_flow_time_shift()
    { return _pdata->flow_time_shift.load(filepath_flow_time_shift()); }

    bool Dataset::delete_file_flow_time_shift()
    { return delete_file_if_exists(filepath_flow_time_shift()); }

    bool Dataset::save_cardiac_cycle_definition()
    { return _pdata->cardiac_cycle_definition.save(filepath_cardiac_cycle_definition()); }

    bool Dataset::load_cardiac_cycle_definition()
    { return _pdata->cardiac_cycle_definition.load(filepath_cardiac_cycle_definition()); }

    bool Dataset::delete_file_cardiac_cycle_definition()
    { return delete_file_if_exists(filepath_cardiac_cycle_definition()); }

    bool Dataset::save_phase_unwrapping_2dt()
    {
        std::ofstream file(filepath_phase_unwrapping_2dt(), std::ios_base::binary | std::ios_base::out);

        if (!file.good())
        { return false; }

        bool success = true;

        const std::uint8_t num_2dt_flow_images = static_cast<std::uint8_t>(_pdata->phase_unwrapping_2dt.size());
        file.write(reinterpret_cast<const char*>(&num_2dt_flow_images), sizeof(std::uint8_t));

        for (auto it = _pdata->phase_unwrapping_2dt.begin(); it != _pdata->phase_unwrapping_2dt.end(); ++it)
        {
            // dcm id
            const std::uint8_t dcm_id = static_cast<std::uint8_t>(it->first);
            file.write(reinterpret_cast<const char*>(&dcm_id), sizeof(std::uint8_t));

            success &= it->second.save(file);
        }

        file.close();

        return success;
    }

    bool Dataset::load_phase_unwrapping_2dt()
    {
        _pdata->phase_unwrapping_2dt.clear();

        std::ifstream file(filepath_phase_unwrapping_2dt(), std::ios_base::binary | std::ios_base::in);

        if (!file.good())
        { return false; }

        bool success = true;

        std::uint8_t num_2dt_flow_images = 0;
        file.read(reinterpret_cast<char*>(&num_2dt_flow_images), sizeof(std::uint8_t));

        for (unsigned int i = 0; i < num_2dt_flow_images; ++i)
        {
            // dcm id
            std::uint8_t dcm_id = 0;
            file.read(reinterpret_cast<char*>(&dcm_id), sizeof(std::uint8_t));

            PhaseUnwrapping2DT pu;
            success &= pu.load(file);

            _pdata->phase_unwrapping_2dt.emplace(dcm_id, std::move(pu));
        }

        file.close();

        return success;
    }

    bool Dataset::delete_file_phase_unwrapping_2dt()
    { return delete_file_if_exists(filepath_phase_unwrapping_2dt()); }

    bool Dataset::save_phase_unwrapping_3dt()
    { return _pdata->phase_unwrapping_3dt.save(filepath_phase_unwrapping_3dt()); }

    bool Dataset::load_phase_unwrapping_3dt()
    { return _pdata->phase_unwrapping_3dt.load(filepath_phase_unwrapping_3dt()); }

    bool Dataset::delete_file_phase_unwrapping_3dt()
    { return delete_file_if_exists(filepath_phase_unwrapping_3dt()); }

    bool Dataset::save_ivsd()
    {
        auto img = ivsd();
        return save_local_image_copy(filepath_ivsd(), *img);
    }

    bool Dataset::save_magnitude_tmip_3dt()
    {
        auto img = tmip_magnitude_3dt();
        return save_local_image_copy(filepath_tmip_magnitude_3dt(), *img);
    }

    bool Dataset::save_anatomical_tmip_3dt(unsigned int imgId)
    {
        auto img = tmip_anatomical_3dt(imgId);
        return save_local_image_copy(filepath_tmip_anatomical_3dt(imgId), *img);
    }

    bool Dataset::save_static_tissue_threshold(double t) const
    {
        std::ofstream file(filepath_static_tissue_threshold(), std::ios_base::out | std::ios_base::binary);
        file.write(reinterpret_cast<char*>(&t), sizeof(double));
        file.close();

        return true;
    }

    bool Dataset::save_vessel(const Vessel* v, VesselComponent comp) const
    {
        if (v == nullptr)
        {
            std::cerr << "Dataset::save_vessel - vessel is nullptr" << std::endl;
            return false;
        }

        if (!std::filesystem::exists(dirpath_vessel_without_slash_ending(v)))
        { std::filesystem::create_directories(dirpath_vessel_without_slash_ending(v)); }

        const std::string path = dirpath_vessel(v);

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog = bk_progress.emplace_task(5, ___("Saving vessel \"@0\"", v->name()));
        #endif

        if (comp & VesselComponent_Segmentation3D && v->has_segmentation3D())
        { v->save_segmentation3D(path + v->name()); }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        if (comp & VesselComponent_Mesh && v->has_mesh())
        { v->save_mesh(path + v->name()); }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        if (comp & VesselComponent_Centerlines)
        {
            if (v->has_centerlines())
            { v->save_centerlines(path + v->name()); }

            if (v->has_centerline_ids())
            { v->save_centerline_ids(path + v->name()); }
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        if (comp & VesselComponent_FlowJet)
        {
            // todo: flowjets
        }

        #ifdef BK_EMIT_PROGRESS
        prog.increment(1);
        #endif

        if (comp & VesselComponent_Pressure)
        {
            // (todo: pressure)    
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    unsigned int Dataset::load_vessels(VesselComponent comp)
    {
        _pdata->vessels.clear();

        unsigned int vesselCnt = 0;

        std::vector<std::string> vesselNames;
        for (auto& p: std::filesystem::directory_iterator(dirpath_vessels_without_slash_ending()))
        {
            if (p.is_directory())
            { vesselNames.emplace_back(p.path().filename().string()); }
        }

        std::sort(vesselNames.begin(), vesselNames.end());

        #ifdef BK_EMIT_PROGRESS
        bk::Progress& prog_all = bk_progress.emplace_task(vesselNames.size(), ___("Loading vessels"));
        #endif

        for (unsigned int vid = 0; vid < vesselNames.size(); ++vid)
        {
            const std::string currentName = vesselNames[vid];

            #ifdef BK_EMIT_PROGRESS
            bk::Progress& prog_current = bk_progress.emplace_task(5, ___("Loading \"@0\"", currentName));
            #endif

            Vessel v;
            v.set_name(vesselNames[vid]);

            const std::string path = dirpath_vessel(currentName);
            bool success = true;

            if (comp & VesselComponent_Segmentation3D)
            { success &= v.load_segmentation3D(filepath_segmentation3D_of_vessel(currentName)); }

            #ifdef BK_EMIT_PROGRESS
            prog_current.increment(1);
            #endif

            if (comp & VesselComponent_Mesh)
            { success &= v.load_mesh(filepath_mesh_of_vessel(currentName)); }

            #ifdef BK_EMIT_PROGRESS
            prog_current.increment(1);
            #endif

            if (comp & VesselComponent_Centerlines)
            {
                success &= v.load_centerlines(filepath_centerlines_of_vessel(currentName));
                success &= v.load_centerline_ids(filepath_centerline_ids_of_vessel(currentName));
            }

            #ifdef BK_EMIT_PROGRESS
            prog_current.increment(1);
            #endif

            if (comp & VesselComponent_FlowJet)
            {
                // todo: flowjets
            }

            #ifdef BK_EMIT_PROGRESS
            prog_current.increment(1);
            #endif

            if (comp & VesselComponent_Pressure)
            {
                // (todo: pressure)    
            }

            if (success)
            {
                add_vessel(std::move(v));
                ++vesselCnt;
            }

            #ifdef BK_EMIT_PROGRESS
            prog_current.set_finished();
            prog_all.increment(1);
            #endif
        } // for vid

        #ifdef BK_EMIT_PROGRESS
        prog_all.set_finished();
        #endif

        return vesselCnt;
    }

    bool Dataset::save_mesh_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : save_mesh_of_vessel(v->mesh(), v->name()); }

    bool Dataset::save_mesh_of_vessel(std::string_view name) const
    { return save_mesh_of_vessel(vessel(name)); }

    bool Dataset::save_mesh_of_vessel(const TriangularMesh3D& mesh, const Vessel* v) const
    { return v == nullptr ? false : save_mesh_of_vessel(mesh, v->name()); }

    bool Dataset::save_mesh_of_vessel(const TriangularMesh3D& mesh, std::string_view name) const
    { return mesh.save(filepath_mesh_of_vessel(name)); }

    bool Dataset::delete_file_segmentation3D_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_segmentation3D_of_vessel(v->name()); }

    bool Dataset::delete_file_segmentation3D_of_vessel(std::string_view name) const
    { return delete_file_if_exists(filepath_segmentation3D_of_vessel(name)); }

    bool Dataset::delete_file_mesh_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_mesh_of_vessel(v->name()); }

    bool Dataset::delete_file_mesh_of_vessel(std::string_view name) const
    { return delete_file_if_exists(filepath_mesh_of_vessel(name)); }

    bool Dataset::delete_file_flowjet_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_flowjet_of_vessel(v->name()); }

    bool Dataset::delete_file_flowjet_of_vessel(std::string_view name) const
    { return delete_file_if_exists(filepath_flowjet_of_vessel(name)); }

    bool Dataset::delete_file_centerlines_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_centerlines_of_vessel(v->name()); }

    bool Dataset::delete_file_centerlines_of_vessel(std::string_view name) const
    { return delete_file_if_exists(filepath_centerlines_of_vessel(name)); }

    bool Dataset::delete_file_centerline_ids_of_vessel(const Vessel* v) const
    { return v == nullptr ? false : delete_file_centerline_ids_of_vessel(v->name()); }

    bool Dataset::delete_file_centerline_ids_of_vessel(std::string_view name) const
    { return delete_file_if_exists(filepath_centerline_ids_of_vessel(name)); }
  } // inline namespace cmr
} // namespace bk
