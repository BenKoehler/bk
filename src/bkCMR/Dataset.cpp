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
#include <filesystem>
#include <utility>

#include <bkCMR/Dataset.h>
#include <bkCMR/FlowDirCorrection.h>
#include <bkCMR/FlowImage3DT.h>
#include <bkCMR/Vessel.h>
#include <bkDicom/DicomDirImporter_CMR.h>
#include <bkDicom/DicomImageInfos.h>
#include <bk/StringUtils>

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
        std::string          project_path; //!< path of current directory; ends with /
        DicomDirImporter_CMR importer;
        FlowImage3DT         flow_image_3dt;
        FlowDirCorrection    flow_dir_correction;
        std::vector<Vessel>  vessels;

        // todo flow dir correction
        // todo phase unwrapping
        // todo eddy current correction
    };

    //====================================================================================================
    //===== DEFINITIONS
    //====================================================================================================
    const std::string Dataset::dcmbytes = "dcmbytes";

    //====================================================================================================
    //===== CONSTRUCTORS & DESTRUCTOR
    //====================================================================================================
    Dataset::Dataset() = default;
    Dataset::Dataset(const Dataset&) = default;
    Dataset::Dataset(Dataset&&) = default;
    Dataset::~Dataset() = default;

    //====================================================================================================
    //===== GETTER
    //====================================================================================================
    const std::string& Dataset::project_path() const
    { return _pdata->project_path; }

    DicomDirImporter_CMR& Dataset::importer()
    { return _pdata->importer; }

    const DicomDirImporter_CMR& Dataset::importer() const
    { return _pdata->importer; }

    bool Dataset::has_magnitude_images() const
    { return !_pdata->importer.class_3dt_magnitude_images().empty(); }

    bool Dataset::has_signal_intensity_image() const
    { return !_pdata->importer.class_3dt_signal_intensity_images().empty(); }

    std::vector<unsigned int> Dataset::ids_of_local_image_copies() const
    {
        const std::vector<unsigned int> class_3dt_flow_images             = _pdata->importer.class_3dt_flow_images();
        const std::vector<unsigned int> class_3dt_anatomical_images       = _pdata->importer.class_3dt_anatomical_images();
        const std::vector<unsigned int> class_3dt_magnitude_images        = _pdata->importer.class_3dt_magnitude_images();
        const std::vector<unsigned int> class_3dt_signal_intensity_images = _pdata->importer.class_3dt_signal_intensity_images();
        const std::vector<unsigned int> class_2dt_flow_images             = _pdata->importer.class_2dt_flow_images();
        const std::vector<unsigned int> class_2dt_anatomical_images       = _pdata->importer.class_2dt_anatomical_images();
        const std::vector<unsigned int> class_2d_anatomical_images        = _pdata->importer.class_2d_anatomical_images();
        const std::vector<unsigned int> class_3d_anatomical_images        = _pdata->importer.class_3d_anatomical_images();

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

    unsigned int Dataset::num_vessels() const
    { return _pdata->vessels.size(); }

    Vessel* Dataset::vessel(unsigned int i)
    { return i < num_vessels() ? &_pdata->vessels[i] : nullptr; }

    Vessel* Dataset::vessel(const std::string& name, bool case_sensitive)
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

    const Vessel* Dataset::vessel(const std::string& name, bool case_sensitive) const
    {
        for (const Vessel& v: _pdata->vessels)
        {
            if (string_utils::equals(v.name(), name, case_sensitive))
            { return &v; }
        }

        return nullptr;
    }

    bool Dataset::vessel_has_centerline_ids(const Vessel* v) const
    {
        // todo

        return false;
    }

    bool Dataset::vessel_has_centerlines(const Vessel* v) const
    {
        // todo

        return false;
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

    //====================================================================================================
    //===== SETTER
    //====================================================================================================
    Dataset& Dataset::operator=(const Dataset&) = default;
    Dataset& Dataset::operator=(Dataset&&) = default;

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

        return true;
    }

    //====================================================================================================
    //===== IMAGES
    //====================================================================================================
    std::string Dataset::filepath_flow_image(unsigned int v) const
    { return _pdata->project_path + dcmbytes + string_utils::from_number(_pdata->importer.class_3dt_flow_images()[v]); }

    std::string Dataset::filepath_tmip_mag() const
    { return _pdata->project_path + "tmip_mag"; }

    std::string Dataset::filepath_lpc() const
    { return _pdata->project_path + "lpc"; }

    std::string Dataset::filepath_ivsd() const
    { return _pdata->project_path + "ivsd"; }

    std::string Dataset::filepath_tmip_signal() const
    { return _pdata->project_path + "tmip_signal"; }

    std::vector<std::string> Dataset::filepaths_of_local_image_copies() const
    {
        const std::string               filepath_base = string_utils::append(_pdata->project_path, dcmbytes);
        const std::vector<unsigned int> ids           = ids_of_local_image_copies();

        std::vector<std::string> paths;
        paths.reserve(ids.size());

        for (unsigned int imgId: ids)
        { paths.emplace_back(filepath_base + string_utils::from_number(imgId)); }

        return paths;
    }

    bool Dataset::load_flow_image_3dt(DatasetFilter_ flags)
    {
        const std::vector<unsigned int> flow_image_ids = _pdata->importer.class_3dt_flow_images();

        if (flow_image_ids.size() != 3)
        { return false; }

        const std::reference_wrapper<DicomImageInfos> info[3] = {_pdata->importer.image_infos(flow_image_ids[0]), _pdata->importer.image_infos(flow_image_ids[1]), _pdata->importer.image_infos(flow_image_ids[2])};

        #ifdef BK_EMIT_PROGRESS
        const unsigned int numel = info[0].get().Rows * info[0].get().Columns * info[0].get().Slices * info[0].get().TemporalPositions;
        Progress& prog = bk_progress.emplace_task(4 * numel + 10, ___("loading 3D+T flow images"));
        #endif

        std::unique_ptr<DicomImage<double, -1>> f[3] = {nullptr, nullptr, nullptr};

        //------------------------------------------------------------------------------------------------------
        // has local image copies?
        //------------------------------------------------------------------------------------------------------
        if (has_local_image_copies())
        {
            for (unsigned int v = 0; v < 3; ++v)
            {
                const std::string  filepath = filepath_flow_image(v);
                const unsigned int filesize = std::filesystem::file_size(filepath);

                std::vector<char> imgbytes(filesize);

                std::ifstream file(filepath, std::ios_base::in | std::ios_base::binary);
                file.read(imgbytes.data(), filesize * sizeof(char));
                file.close();

                f[v] = std::move(_pdata->importer.read_image_from_bytes(flow_image_ids[v], imgbytes));

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

        _pdata->flow_image_3dt.geometry().transformation().set_world_matrix(info[0].get().worldMatrix);
        _pdata->flow_image_3dt.geometry().transformation().set_temporal_resolution(info[0].get().TemporalResolution);

        // derive flow vector ordering from world matrix
        unsigned int      order[3]  = {0, 1, 2};
        double            dirfac[3] = {1, 1, 1};
        for (unsigned int colid     = 0; colid < 3; ++colid)
        {
            double            maxabsval = 0;
            for (unsigned int rowid     = 0; rowid < 3; ++rowid)
            {
                const double tempval = info[0].get().worldMatrix(rowid, colid);
                if (maxabsval < std::abs(tempval))
                {
                    maxabsval = std::abs(tempval);

                    order[colid]  = rowid;
                    dirfac[colid] = std::signbit(tempval) ? -1 : +1;
                }
            }
        }

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
                        Vec3d              v((*f[order[0]])[lid] - info[order[0]].get().LargestImagePixelValue / 2, (*f[order[1]])[lid] - info[order[1]].get().LargestImagePixelValue / 2, (*f[order[2]])[lid] - info[order[2]].get().LargestImagePixelValue / 2);

                        v[0] /= static_cast<double>(info[order[0]].get().LargestImagePixelValue / 2) * dirfac[0];
                        v[1] /= static_cast<double>(info[order[1]].get().LargestImagePixelValue / 2) * dirfac[1];
                        v[2] /= static_cast<double>(info[order[2]].get().LargestImagePixelValue / 2) * dirfac[2];

                        v *= _pdata->importer.venc_3dt_in_m_per_s();

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
            unwrap_phases_3dt();
            // todo
        }

        if (flags & DatasetFilter_VelocityOffset)
        {
            load_eddy_current_correction_3dt();
            correct_eddy_currents_3dt();
            // todo
        }

        #ifdef BK_EMIT_PROGRESS
        prog.set_finished();
        #endif

        return true;
    }

    std::vector<std::unique_ptr<DicomImage<double, -1>>> Dataset::flow_images_2dt(DatasetFilter_ flags);
    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::flow_image_2dt(unsigned int dcm_id, DatasetFilter_ flags);

    int Dataset::anatomical_2dt_image_id_of_flow_image_2dt(unsigned int flowimg_dcm_id);

    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::lpc() const;
    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::ivsd() const;
    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::tmip_magnitude_3dt() const;
    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::tmip_signal_intensity_3dt() const;
    std::unique_ptr<bk::DicomImage<double, 4>> Dataset::pressure_map() const;
    std::unique_ptr<bk::DicomImage<double, 3>> Dataset::vessel_segmentation_in_flow_field_3dt_size(const Vessel* v) const;

    //====================================================================================================
    //===== FUNCTIONS
    //====================================================================================================
    void Dataset::clear()
    {
        // todo
    }

    void Dataset::delete_local_image_copies_if_incomplete() const
    {
        if (!has_local_image_copies())
        {delete_local_image_copies();}
    }

    void Dataset::delete_local_image_copies() const
    {
        std::vector<std::string> paths = filepaths_of_local_image_copies();
        paths.emplace_back(filepath_tmip_mag());
        paths.emplace_back(filepath_lpc());
        paths.emplace_back(filepath_ivsd());
        paths.emplace_back(filepath_tmip_signal());

        #ifdef BK_EMIT_PROGRESS
        Progress& prog = bk_progress.emplace_task(paths.size(), ___("removing local image copies"));
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

    //====================================================================================================
    //===== I/O
    //====================================================================================================
    std::string Dataset::filepath_flow_dir_correction() const
    { return _pdata->project_path + "dir.fdc"; }

    bool Dataset::save_flow_dir_correction()
    { return _pdata->flow_dir_correction.save(filepath_flow_dir_correction()); }

    bool Dataset::load_flow_dir_correction()
    { return _pdata->flow_dir_correction.load(filepath_flow_dir_correction()); }
  } // inline namespace cmr
} // namespace bk

