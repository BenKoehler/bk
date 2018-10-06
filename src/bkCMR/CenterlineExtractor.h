#pragma once

#ifndef BK_CENTERLINEEXTRACTOR_H
#define BK_CENTERLINEEXTRACTOR_H

#include <memory>
#include <utility>
#include <vector>

#include <bk/CopyablePIMPL>
#include <bk/Line>
#include <bk/Image>
#include <bk/Mesh>

namespace bk
{
  inline namespace cmr
  {
    class CenterlineExtractor
    {
        //====================================================================================================
        //===== DEFINITIONS
        //====================================================================================================

        //====================================================================================================
        //===== MEMBERS
        //====================================================================================================
      private:
        class Impl;
        bk::cpimpl<Impl> _pdata;

        //====================================================================================================
        //===== CONSTRUCTORS & DESTRUCTOR
        //====================================================================================================
      public:
        CenterlineExtractor();
        CenterlineExtractor(const CenterlineExtractor&);
        CenterlineExtractor(CenterlineExtractor&&) noexcept;
        ~CenterlineExtractor();

        //====================================================================================================
        //===== GETTER
        //====================================================================================================
        [[nodiscard]] unsigned int image_upscale() const;
        [[nodiscard]] int distance_penalty_exponent() const;
        [[nodiscard]] unsigned int num_smooth_iterations() const;
        [[nodiscard]] unsigned int smooth_kernel_size() const;
        [[nodiscard]] double smooth_relaxation() const;

        //====================================================================================================
        //===== SETTER
        //====================================================================================================
        CenterlineExtractor& operator=(const CenterlineExtractor&);
        CenterlineExtractor& operator=(CenterlineExtractor&&) noexcept;

        void set_image_upscale(unsigned int image_upscale);
        void set_distance_penalty_exponent(int distance_penalty_exponent);
        void set_num_smooth_iterations(unsigned int num_smooth_iterations);
        void set_smooth_kernel_size(unsigned int smooth_kernel_size);
        void set_smooth_relaxation(double smooth_relaxation);

        //====================================================================================================
        //===== FUNCTIONS
        //====================================================================================================
        std::pair<std::vector<Line3D> /*centerlines*/, bool /*success*/> extract_centerlines(TriangularMesh3D& mesh, const bk::DicomImage<double,3>& seg, unsigned int seed_meshVertexId, const std::vector<unsigned int>& target_meshVertexIds) const;
    }; // class CenterlineExtractor
  } // inline namespace cmr
} // namespace bk

#endif //BK_CENTERLINEEXTRACTOR_H


