#bkMath
    
####Matrix:

This matrix class supports:

* Static size matrices (fixed at compile-time) with constexpr support (creation and calculations)
* Dynamic size matrices (set at run-time)
* Functionality depending on matrix size, e.g.:
    * Angles for column-vectors (bk::Matrix<T,N,1>) and row-vectors (bk::Matrix<T,1,N>),
    * Cross product for bk::Vec2d/bk::Vec3d, and
    * Trace for square matrices.
* Math operators consider value types to prevent errors, e.g.:
    * bk::Vec3ui - bk::Vec3ui could become negative -> result is bk::Vec3i to prevent potential underflow
    * bk::Vec2i.div_cwise(bk::Vec2i) returns bk::Vec2d to avoid whole number division
* MatrixFactory provides special standard matrices, e.g.:
    * Zero, One, Identity
    * Rotation_Matrices (2D/3D General or around X/Y/Z axis)
    * Random (also at compile-time)
* RefMatrix allows to manipulate parts of the matrix, e.g.,
    * bk::Mat3d::col_ref<2>().set_zero() (or bk::Mat3d::col_ref(2).set_zero()) sets the third column (index 2) to 0
* Decompositions are provided via Eigen:
    * bk::Mat4d.qr()
    * bk::Mat4d.svd()
    * bk::Mat4d.eigenvalues()
    * bk::Mat4d.rank()
    * bk::Mat4d.condition()
* matrix_traits at compile-time and run-time


Matrix access is provided by:

* operator()(rowId, colId) -> default access 
* operator[](listId) -> access underlying data vector; useful for vectors
* iterators
* sub matrix copies via:
    * row<Id> or row(id)
    * col<Id> or col(id)
    * sub_matrix<RowIdFrom, RowIdTo, ColIdFrom, ColIdTo>() or sub_matrix(rowIdFrom, rowIdTo, colIdFrom, colIdTo)
* sub matrix references via:
    * row_ref<Id> or row_ref(id)
    * col_ref<Id> or col_ref(id)
    * sub_matrix_ref<RowIdFrom, RowIdTo, ColIdFrom, ColIdTo>() or sub_matrix_ref(rowIdFrom, rowIdTo, colIdFrom, colIdTo)
    
    
Most function come in 2 versions: foo() and foo_internal(). The first applied the operation and returns a new matrix, the second applied the operation directly to the matrix object.


If the pre-processor macro "BK_LIB_EIGEN_AVAILABLE" is defined, special wrappers for Eigen ( http://eigen.tuxfamily.org/index.php?title=Main_Page ) functionality will be enabled.
These allow to perform QR decomposition, SVD, and EigenAnalysis of the matrix.

If you want to stick with Eigen's MPL 2 license (vast majority of Eigen) and avoid the few LGPL-licensed Eigen parts, then also define "EIGEN_MPL2_ONLY"