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

#include <iostream>

#include <bkMath/Matrix>

void example_basic_usage()
{
    bk::Mat3d A(1,2,3,4,5,6,7,8,9); // column-wise storage
    std::cout << A << std::endl;
    // output: 1 4 7
    //         2 5 8
    //         3 6 9

    A(0,0) = -1;

    bk::Mat3d B = A + bk::MatrixFactory::Identity_Mat_3D();
    std::cout << B << std::endl;
    // output: 0 4 7
    //         2 6 8
    //         3 6 10

    std::cout << (B * bk::Vec3f::One()).transpose() << std::endl;
    // output: 11 16 19

    // auto x = A * Vec2d(1,1); // static assertion fails; size mismatch

    bk::MatXi C; // matrix with dynamic size
    C.set_size(3,4);
}

void example_access()
{
    bk::ColMat4i C(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    std::cout << C << std::endl;
    // output: 0 4 8 12
    //         1 5 9 13
    //         2 6 10 14
    //         3 7 11 15

    bk::RowMat4i R(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    std::cout << R << std::endl;
    // output: 0 1 2 3
    //         4 5 6 7
    //         8 9 10 11
    //         12 13 14 15

    std::cout << "C(2,2) == R(2,2) -> " << C(2, 2) << " == " << R(2, 2) << " -> " << (C(2, 2) == R(2, 2)) << std::endl;

    // using iterators
    for (auto it = C.begin(); it != C.end(); ++it)
    {std::cout << *it << " ";}
    std::cout << std::endl;
    // output: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

    for (auto x: R)
    {std::cout << x << " ";}
    std::cout << std::endl;
    // output: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

    auto Cr0 = C.row(0); // copy of row 0
    std::cout << Cr0 << std::endl; // output: 0 4 8 12
    Cr0.set_zero();
    std::cout << Cr0 << std::endl; // output: 0 0 0 0
    std::cout << C << std::endl;
    // output: 0 4 8 12
    //         1 5 9 13
    //         2 6 10 14
    //         3 7 11 15

    auto Cr0ref = C.row_ref(0); // reference to row 0
    std::cout << Cr0ref << std::endl; // output: 0 4 8 12
    Cr0ref.set_zero();
    std::cout << Cr0ref << std::endl; // output: 0 0 0 0
    std::cout << C << std::endl;
    // output: 0 0 0 0
    //         1 5 9 13
    //         2 6 10 14
    //         3 7 11 15

    const bk::Mat3i temp = R.sub_matrix</*rowId first =*/0, /*rowId last =*/2, /*colId first =*/0, /*colId last =*/2>();
    //const bk::Mat3i temp = R.sub_matrix<>(0,2,0,2); // same as above; runtime version
    R.sub_matrix_ref(0,2,0,2).set_identity();
    std::cout << R << std::endl;
    // output: 1 0 0 3
    //         0 1 0 7
    //         0 0 1 11
    //         12 13 14 15

    R.sub_matrix_ref<0,2,0,2>() = temp;
    std::cout << R << std::endl;
    // output: 0 1 2 3
    //         4 5 6 7
    //         8 9 10 11
    //         12 13 14 15
}

void example_dynamic_matrices()
{
    bk::MatXi C; // matrix with dynamic size; default alignment is column-wise
    C.set_size(2, 2);
    C.set(1, 2, 3, 4);

    std::cout << C[0] << C[1] << C[2] << C[3] << std::endl; // internal storage does not change
    // output: 1234
    std::cout << C << std::endl;
    // output: 1 3
    //         2 4

    C.set_alignment(bk::MatrixAlignment::RowMajor); // for dynamic matrices, only one flag is changed here; no copying
    std::cout << C[0] << C[1] << C[2] << C[3] << std::endl;
    // output: 1234
    std::cout << C << std::endl;
    // output: 1 2
    //         3 4

    C.add_col_right();
    std::cout << C << std::endl;
    // output: 1 2 0
    //         3 4 0

    C.add_row_bottom();
    std::cout << C << std::endl;
    // output: 1 2 0
    //         3 4 0
    //         0 0 0

    C.transpose_internal(); // only changes alignment flag
    std::cout << C << std::endl;
    // output: 1 3 0
    //         2 4 0
    //         0 0 0

    auto D = std::move(C); // dynamic matrices can be moved
    // std::cout << C << std::endl; // crashes

    D.set_identity();
    std::cout << D << std::endl;
    // output: 1 0 0
    //         0 1 0
    //         0 0 1

    D.set_size(4,4); // does not keep element positions
    std::cout << D << std::endl;
    // output: 1 1 1 0
    //         0 0 0 0
    //         0 0 0 0
    //         0 0 0 0

    D = bk::MatrixFactory::Identity_Mat_3D();
    D.resize_existing(4,4); // keeps element positions
    std::cout << D << std::endl;
    // output: 1 0 0 0
    //         0 1 0 0
    //         0 0 1 0
    //         0 0 0 0


    // combination with static matrices is no problem
    auto E = D + bk::Mat4d::Identity();
    std::cout << E << std::endl;
    // output: 2 0 0 0
    //         0 2 0 0
    //         0 0 2 0
    //         0 0 0 1
}

void example_modification()
{
    bk::Matrix<std::int32_t,3,6> A;
    A.col_ref(0) = {1,1,1};
    A.col_ref<1>() = bk::Vec3ui::Constant(2);
    A.col_ref(2) = bk::MatrixFactory::Random_ct_Vec_3D< 123 >(3,6);
    A.col_ref(3).set_constant(4);
    A.col_ref<4>().set_x_axis();
    A.col_ref<5>().set(6,7,8);

    std::cout << A << std::endl;
    // output: 1 2 3 4 1 6
    //         1 2 3 4 0 7
    //         1 2 5 4 0 8

    A.swap_cols_internal(0,1);
    std::cout << A << std::endl;
    // output: 2 1 3 4 1 6
    //         2 1 3 4 0 7
    //         2 1 5 4 0 8

    auto B = A.normalize_cols();
    std::cout << typeid(typename decltype(B)::value_type).name() << std::endl; // output: d (double -> B is bk::Matrix<d,3,6>)
    std::cout << B << std::endl;
    // output: 0.57735 0.57735 0.624695 0.57735 1 0.491539
    //         0.57735 0.57735 0.624695 0.57735 0 0.573462
    //         0.57735 0.57735 0.468521 0.57735 0 0.655386

    B.sub_matrix_ref(/*rowId first =*/0, /*rowId last =*/0, /*colId first =*/0, /*colId last =*/5).negate_internal();
    std::cout << B << std::endl;
    // output: -0.57735 -0.57735 -0.624695 -0.57735 -1 -0.491539
    //         0.57735 0.57735 0.624695 0.57735 0 0.573462
    //         0.57735 0.57735 0.468521 0.57735 0 0.655386
}

void example_queries()
{
    // initialize random matrix at compile-time;
    // the template parameter is a seed number; you can use "__LINE__*__COUNTER__" by default
    constexpr bk::Mat3d A = bk::MatrixFactory::Random_ct_Mat_3D< 12345 >(-5, 5);
    std::cout << A << std::endl;
    // output: -4 0 -1
    //          2 -3 1
    //          1 3 -2

    constexpr bk::Mat3d S = A.transpose() * A; // symmetric matrix
    std::cout << S << std::endl;
    // output: 21 -3 4
    //         -3 18 -9
    //         4 -9 6

    constexpr double precision = 1e-10;
    std::cout << S.is_symmetric(precision) << std::endl;        // output: 1
    std::cout << S.is_upper_triangular(precision) << std::endl; // output: 0
    std::cout << S.is_lower_triangular(precision) << std::endl; // output: 0
    std::cout << S.is_triangular(precision) << std::endl;       // output: 0
    std::cout << S.is_upper_hessenberg(precision) << std::endl; // output: 0
    std::cout << S.is_lower_hessenberg(precision) << std::endl; // output: 0
    std::cout << S.is_hessenberg(precision) << std::endl;       // output: 0
    std::cout << S.is_orthogonal(precision) << std::endl;       // output: 0
    std::cout << S.is_orthonormal(precision) << std::endl;      // output: 0
}

void example_comparison()
{
    bk::RowVec3i v(1,2,3);
    bk::RowVec3i w(3,2,1);

    std::cout << (v == w) << std::endl;                    // output: 0
    std::cout << (v != w) << std::endl;                    // output: 1
    std::cout << v.is_equal_cwise(w) << std::endl;         // output: 0 1 0
    std::cout << v.is_not_equal_cwise(w) << std::endl;     // output: 1 0 1
    std::cout << v.is_lesser_cwise(w) << std::endl;        // output: 1 0 0
    std::cout << v.is_lesser_equal_cwise(w) << std::endl;  // output: 1 1 0
    std::cout << v.is_greater_cwise(w) << std::endl;       // output: 0 0 1
    std::cout << v.is_greater_equal_cwise(w) << std::endl; // output: 0 1 1
    std::cout << v.min_cwise(w) << std::endl;              // output: 1 2 1
    std::cout << v.max_cwise(w) << std::endl;              // output: 3 2 3
}

void example_misc_functions()
{
    bk::Mat3d A(1,2,3,4,5,6,7,8,9);
    std::cout << A << std::endl;
    // output: 1 4 7
    //         2 5 8
    //         3 6 9

    constexpr bk::Mat3d Noise = bk::MatrixFactory::Random_ct_Mat_3D<0>(-0.25,0.25);
    std::cout << Noise << std::endl;
    // output: -0.230076 0.0840524 -0.207694
    //         -0.0053686 -0.128483 -0.184459
    //         0.168798 -0.204893 0.235427

    std::cout << A.sum() << std::endl; // sum of all elements; output: 45
    std::cout << A.sum_of_cols(0) + A.sum_of_rows(0,2) << std::endl; //(1+2+3) + (1+4+7) + (3+6+9); output: 36
    std::cout << A.mean() << std::endl; // output: 5
    std::cout << A.variance() << std::endl; // output: 6.66667
    std::cout << A.stdev() << std::endl; // output: 2.58199
    std::cout << *A.min_element() << std::endl; // iterator to min element; output: 1
    std::cout << *A.max_element() << std::endl; // iterator to max element; output: 9
    std::cout << A.abs_cwise() << std::endl;
    // output: 1 4 7
    //         2 5 8
    //         3 6 9

    std::cout << A.cbrt_cwise() << std::endl;
    // output: 1 1.5874 1.91293
    //         1.25992 1.70998 2
    //         1.44225 1.81712 2.08008

    std::cout << A.exp_cwise() << std::endl;
    // output: 2.71828 54.5982 1096.63
    //         7.38906 148.413 2980.96
    //         20.0855 403.429 8103.08

    std::cout << A.logi_cwise() << std::endl;
    // output: 0 1 1
    //         0 1 2
    //         1 1 2

    //std::cout << A.mod_cwise(3) << std::endl; // for integral value types only
    std::cout << A.pow_cwise(2) << std::endl;
    // output: 1 16 49
    //         4 25 64
    //         9 36 81

    std::cout << (A.mult_cwise(Noise)).sign_cwise() << std::endl;
    // output: 0 1 0
    //         0 0 0
    //         1 0 1

    std::cout << A.sqr_cwise() << std::endl;
    // output: 1 16 49
    //         4 25 64
    //         9 36 81

    std::cout << A.sin_cwise() << std::endl;
    // output: 0.841471 -0.756802 0.656987
    //         0.909297 -0.958924 0.989358
    //         0.14112 -0.279415 0.412118

    std::cout << A.cos_cwise() << std::endl;
    // output: 0.540302 -0.653644 0.753902
    //         -0.416147 0.283662 -0.1455
    //         -0.989992 0.96017 -0.91113

    std::cout << A.tan_cwise() << std::endl;
    // output: 1.55741 1.15782 0.871448
    //         -2.18504 -3.38052 -6.79971
    //         -0.142547 -0.291006 -0.452316

    std::cout << (A + Noise).ceil_cwise() << std::endl;
    // output: 1 5 7
    //         2 5 8
    //         4 6 10

    std::cout << (A + Noise).floor_cwise() << std::endl;
    // output: 0 4 6
    //         1 4 7
    //         3 5 9

    std::cout << (A + Noise).round_cwise() << std::endl;
    // output: 1 4 7
    //         2 5 8
    //         3 6 9

    std::cout << A.clamp_cwise(3,6) << std::endl;
    // output: 3 4 6
    //         3 5 6
    //         3 6 6

    std::cout << A.sort_ascending() << std::endl;
    // output: 1 4 7
    //         2 5 8
    //         3 6 9

    std::cout << A.sort_descending() << std::endl;
    // output: 9 6 3
    //         8 5 2
    //         7 4 1

    std::cout << A.interpolate_linear(bk::Mat3d(2,3,4,5,6,7,8,9,10), 0.5) << std::endl;
    // output: 1.5 4.5 7.5
    //         2.5 5.5 8.5
    //         3.5 6.5 9.5
}

void example_vector_calculus()
{
    constexpr bk::Vec2d v2 = bk::MatrixFactory::Y_Axis_2D();
    std::cout << v2.transpose() << std::endl; // output: 0 1

    std::cout << v2.rotate_deg_clockwise(90).transpose() << std::endl; // rotate right; output: 1 0
    std::cout << v2.rotate_rad_counterclockwise(1.57079632679 /*pi/2*/).transpose() << std::endl; // rotate left; output: -1 0

    const auto perp = v2.perpendicular();
    std::cout << perp.transpose() << std::endl; // output: -1 0
    std::cout << bk::radians_to_degrees(perp.angle(v2)) << std::endl; // output: 90



    constexpr bk::RowVec3d v3(1, 1, 0);
    std::cout << v3.cross(bk::RowVec3d(1, 2, 1).normalize()).normalize() << std::endl; // output: 0.57735 -0.57735 0.57735

    double rotationAngle = 0;
    bk::RowVec3d rotationAxis; // optional parameters filled by rotate_onto_direction()
    auto v3rotated = v3.rotate_onto_direction(bk::MatrixFactory::Z_Axis_3D().transpose(), &rotationAngle, &rotationAxis);

    std::cout << v3rotated << std::endl; // output: 0 0 1.41421
    std::cout << "rotated around [" << rotationAxis << "] by " << bk::radians_to_degrees(rotationAngle) << " deg" << std::endl;
    // output: rotated around [-0.707107 0.707107 0] by -90 deg

    std::cout << v3.rotate_deg_around_z_axis(180) << std::endl;
    // output: -1 -1 0
}

void example_decompositions_via_eigen()
{
    bk::Mat3d A = bk::Mat3d::Random_float(-1,1);
    std::cout << A << std::endl;
    // output: 0.23778 0.206847 -0.363259
    //         -0.120888 -0.398631 -0.0917691
    //         0.48491 0.699328 -0.0202351

    bk::MatXd b;
    b.set_size(3, 1);
    std::iota(b.begin(), b.end(), 1);
    std::cout << b.transpose() << std::endl; // output: 1 2 3

    /*
     * use QR decomposition to solve linear equation system
     */
    auto qr = A.qr();
    std::cout << qr.is_valid() << std::endl; // QR decomposition was successful?; output: 1

    // solve() returns std::optional<> with the result vector type
    auto opt_res_qr = qr.solve(b);

    if (opt_res_qr)
    {
        std::cout << opt_res_qr.value().transpose() << std::endl; // output: 28.6768 -15.3845 7.25796
        std::cout << qr.is_valid() << std::endl; // output: 1
    }
    else
    {
        std::cout << "solve via QR decomposition failed" << std::endl;
        std::cout << qr.is_valid() << std::endl; // output: 0
    }


    /*
     * use SVD to solve linear equation system
     */
    if (auto opt_res_svd = A.svd().solve(b); opt_res_svd)
    {
        std::cout << opt_res_svd.value().transpose() << std::endl; // output: 28.6768 -15.3845 7.25796
    }
    else
    { std::cout << "solve via SVD failed" << std::endl; }



    /*
     * eigenanalysis of non-symmetric matrix A
     * yields complex eigenvalues and eigenvectors
     */
    auto eig = A.eigenanalysis();
    // analogous to QR and SVD, eigenvalue and -vectors are return as std::optional

    // eigenvalues
    if (auto eigenvalues /*column vector*/ = eig.eigenvalues(); eigenvalues)
    {
        std::cout << eigenvalues.value().transpose() << std::endl;
        // output: (0.00136861,0.417492) (0.00136861,-0.417492) (-0.183824,0)
    }
    else
    { std::cout << "eigenanalysis failed" << std::endl; }

    // eigenvectors
    if (auto eigenvectors = eig.eigenvectors(); eigenvectors)
    {
        // square matrix where each column is an eigenvector
        const auto V = eigenvectors.value();

        for (unsigned int c = 0; c < V.num_cols(); ++c)
        { std::cout << "eigenvector " << c << ": " << V.col(c).transpose() << std::endl; }
        // output: eigenvector 0: (-0.213424,0.609427) (-0.176824,-0.129919) (0.460827,0.567927)
        //         eigenvector 1: (-0.213424,-0.609427) (-0.176824,0.129919) (0.460827,-0.567927)
        //         eigenvector 2: (-0.677027,0) (0.576469,0) (-0.457513,0)
    }
    else
    { std::cout << "eigenanalysis failed" << std::endl; }

    // eigenvalues without eigenvectors:
    std::cout << A.eigenvalues().value().transpose() << std::endl;
    // output: (0.00136861,0.417492) (0.00136861,-0.417492) (-0.183824,0)

    /*
     * eigenanalysis of a symmetric matrix S
     * - faster than the general version
     * - eigenvalues and -vectors are real
     * - eigenvectors are orthonormal
     */
    auto S = A.transpose() * A;
    auto eigSymm = S.eigenanalysis_symmetric();

    // eigenvalues
    if (auto eigenvalues /*column vector*/ = eigSymm.eigenvalues(); eigenvalues)
    {
        std::cout << eigenvalues.value().transpose() << std::endl;
        // output: 0.00716382 0.145456 0.985211
    }
    else
    { std::cout << "eigenanalysis failed" << std::endl; }

    // eigenvectors
    if (auto eigenvectors = eigSymm.eigenvectors(); eigenvectors)
    {
        const auto V = eigenvectors.value();

        for (unsigned int c = 0; c < V.num_cols(); ++c)
        { std::cout << "eigenvector " << c << ": " << V.col(c).transpose() << std::endl; }
        // output: eigenvector 0: 0.809198 -0.491917 0.321273
        //         eigenvector 1: -0.214101 0.262328 0.940927
        //         eigenvector 2: 0.547137 0.830181 -0.106955
    }
    else
    { std::cout << "eigenanalysis failed" << std::endl; }

    // eigenvalues without eigenvectors:
    std::cout << S.eigenvalues_symmetric().value().transpose() << std::endl;
    // output: 0.00716382 0.145456 0.985211
}

int main(int, char**)
{
    example_basic_usage();
    example_access();
    example_dynamic_matrices();
    example_modification();
    example_queries();
    example_comparison();
    example_misc_functions();
    example_vector_calculus();
    example_decompositions_via_eigen();

    return EXIT_SUCCESS;
}

