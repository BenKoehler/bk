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

#ifndef BK_WORLDMATRIXTRANSFORMATION_H
#define BK_WORLDMATRIXTRANSFORMATION_H

#include <bk/Matrix>

#include <bkDataset/transformation/TransformationBase.h>

namespace bk
{
  template<int TDims = -1> class WorldMatrixTransformation : public details::TransformationBase<TDims, WorldMatrixTransformation<TDims>>
  {
      //====================================================================================================
      //===== DEFINITIONS
      //====================================================================================================
      using self_type = WorldMatrixTransformation<TDims>;
      using base_type = details::TransformationBase<TDims, WorldMatrixTransformation<TDims>>;

      friend base_type;

      /// @{ -------------------------------------------------- HELPERS: TEMPLATE PARAMETERS
      [[nodiscard]] static constexpr int WorldMatrixRowsCols() noexcept
      { return TDims == -1 ? -1 : TDims + 1; }
      /// @}

    public:
      using value_type = typename base_type::value_type;

      //====================================================================================================
      //===== MEMBERS
      //====================================================================================================
    private:
      Mat<WorldMatrixRowsCols(), WorldMatrixRowsCols(), value_type> _world_matrix;
      Mat<WorldMatrixRowsCols(), WorldMatrixRowsCols(), value_type> _inverse_world_matrix;
      // extracted information
      Vec<TDims, value_type> _scale;
      Vec<TDims, value_type> _translation;
      Mat<TDims, TDims, value_type> _rot_shear_matrix;

      //====================================================================================================
      //===== CONSTRUCTORS & DESTRUCTOR
      //====================================================================================================
    public:
      /// @{ -------------------------------------------------- CTOR
      WorldMatrixTransformation()
          : _world_matrix(MatrixFactory::Identity(_world_matrix)),
            _inverse_world_matrix(_world_matrix),
            _scale(MatrixFactory::One(_scale)),
            _translation(MatrixFactory::Zero(_translation)),
            _rot_shear_matrix(MatrixFactory::Identity(_rot_shear_matrix))
      { /* do nothing */ }

      WorldMatrixTransformation(const self_type&) = default;
      WorldMatrixTransformation(self_type&&) noexcept = default;

      template<typename TMat>
      WorldMatrixTransformation(const TMat& w)
      { set_world_matrix(w); }
      /// @}

      /// @{ -------------------------------------------------- DTOR
      ~WorldMatrixTransformation() = default;
      /// @}

      //====================================================================================================
      //===== GETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- GET NUM DIMENSIONS
      [[nodiscard]] unsigned int num_dimensions() const
      { return _scale.num_rows(); }
      /// @}

      /// @{ -------------------------------------------------- GET SCALE
      [[nodiscard]] const Vec<TDims, value_type>& scale() const
      { return _scale; }

      [[nodiscard]] value_type scale(unsigned int dimid) const
      { return _scale[dimid]; }
      /// @}

      /// @{ -------------------------------------------------- GET TRANSLATION
      [[nodiscard]] const Vec<TDims, value_type>& translation() const
      { return _translation; }

      [[nodiscard]] value_type translation(unsigned int dimid) const
      { return _translation[dimid]; }
      /// @}

      /// @{ -------------------------------------------------- GET ROTATION/SHEAR MATRIX
      [[nodiscard]] const Mat<TDims, TDims, value_type>& rotation_shear_matrix() const
      { return _rot_shear_matrix; }
      /// @}

      /// @{ -------------------------------------------------- GET (INVERSE) WORLD MATRIX
      [[nodiscard]] const Mat<WorldMatrixRowsCols(), WorldMatrixRowsCols(), value_type>& world_matrix() const
      { return _world_matrix; }

      [[nodiscard]] const Mat<WorldMatrixRowsCols(), WorldMatrixRowsCols(), value_type>& inverse_world_matrix() const
      { return _inverse_world_matrix; }
      /// @}

      //====================================================================================================
      //===== SETTER
      //====================================================================================================
      /// @{ -------------------------------------------------- OPERATOR =
      [[maybe_unused]] self_type& operator=(const self_type&) = default;
      [[maybe_unused]] self_type& operator=(self_type&&) noexcept = default;
      /// @}

      /// @{ -------------------------------------------------- SET WORLD MATRIX
      template<typename TMat>
      void set_world_matrix(const TMat& w)
      {
          _world_matrix = w;
          _inverse_world_matrix = _world_matrix.pseudo_inverse();

          _extract_transformation_components();
      }
      /// @}

      //====================================================================================================
      //===== FUNCTIONS
      //====================================================================================================
    private:
      void _extract_transformation_components()
      {
          // https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

          /* given the following world matrix:
           *       a b c d
           *       e d g h
           *       i j k l
           *       0 0 0 1
           */

          /*
           * - the translation vector is [d, h, l]
           */
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { _translation[i] = _world_matrix(i, num_dimensions()); }

          /*
           * - the scale vector [s0, s1, s2] is (||[a, e, i]||, ||[b, f, j]||, ||[c, g, k]||)
           */
          for (unsigned int i = 0; i < num_dimensions(); ++i)
          { _scale[i] = _world_matrix.col(i).norm(); }

          /*
           * - the rotation+shear matrix is the upper 3x3 of:
           *       a/s0  b/s1  c/s2  0
           *       e/s0  f/s1  g/s2  0
           *       h/s0  i/s1  j/s2  0
           *        0     0     0    1
           */
          for (unsigned int colid = 0; colid < num_dimensions(); ++colid)
          {
              for (unsigned int rowid = 0; rowid < num_dimensions(); ++rowid)
              { _rot_shear_matrix(rowid, colid) = _world_matrix(rowid, colid) / _scale[colid]; }
          }
      }

      /// @{ -------------------------------------------------- TO WORLD COORDINATES
      template<typename T>
      void to_world_coordinates_impl(T& vec) const
      {
          const unsigned int N = _world_matrix.num_rows();

          auto v = MatrixFactory::create_col_vector<value_type, WorldMatrixRowsCols()>(N);

          // v = vec to homoegeneous coordinates
          for (unsigned int i = 0; i < N - 1; ++i)
          { v[i] = vec[i]; }

          v[N - 1] = 1;

          // transform
          v = (_world_matrix * v);

          // vec = v from homogeneous coordinates
          for (unsigned int i = 0; i < N - 1; ++i)
          { vec[i] = v[i] / v[N - 1]; }

          // alternative calculation:
          // (_world_matrix * v.to_homogeneous_coordinates()).from_homogeneous_coordinates();
      }
      /// @}

      /// @{ -------------------------------------------------- TO OBJECT COORDINATES
      template<typename T>
      void to_object_coordinates_impl(T& vec) const
      {
          const unsigned int N = _world_matrix.num_rows();

          auto v = MatrixFactory::create_col_vector<value_type, WorldMatrixRowsCols()>(N);

          // v = vec to homoegeneous coordinates
          for (unsigned int i = 0; i < N - 1; ++i)
          { v[i] = vec[i]; }

          v[N - 1] = 1;

          // transform
          v = (_inverse_world_matrix * v);

          // vec = v from homogeneous coordinates
          for (unsigned int i = 0; i < N - 1; ++i)
          { vec[i] = v[i] / v[N - 1]; }

          // alternative calculation:
          // (_inverse_world_matrix * v.to_homogeneous_coordinates()).from_homogeneous_coordinates();
      }
      /// @}
  }; // class WorldMatrixTransformation
} // namespace bk

#endif //BK_WORLDMATRIXTRANSFORMATION_H
