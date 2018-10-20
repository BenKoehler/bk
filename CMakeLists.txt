cmake_minimum_required(VERSION 3.10)

project(bk
        VERSION 1.0
        LANGUAGES CXX)

# ------------------------------------------------------------------------------------------------------------------------------------
# options
# ----------
set(png_DIR "" CACHE PATH "Location of libpng. If not available, the source will be downloaded automatically.")
set(GDCM_DIR "" CACHE PATH "Location of GDCM. If not available, the source will be downloaded automatically.")

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(ENABLE_SUPERBUILD "Pull dependencies' source code if not found." ON)
option(ENABLE_PROGRESS "Algorithms emit progress using bkProgress library" ON)
option(GL_USE_QT "All GL classes work with Qt and require a QtOpenGLFunctions object in the constructor" OFF)
#option(GL_USE_QT "All GL classes work with Qt and require a QtOpenGLFunctions object in the constructor" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
#option(BUILD_EXAMPLES "Build examples" ON)

if (${ENABLE_PROGRESS})
    add_definitions(-DBK_EMIT_PROGRESS)
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# dependencies (third party)
# ----------
# PTHREAD
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# PNG
find_package(PNG REQUIRED)
if (NOT PNG_FOUND)
    message(WARNING "LIBPNG NOT FOUND!")
else ()
    message(STATUS "DETECTED LIBPNG!")
endif ()

# OPENMP
find_package(OpenMP REQUIRED)
if (NOT OPENMP_FOUND)
    message(WARNING "OPENMP NOT FOUND!")
else ()
    message(STATUS "DETECTED OPENMP!")
endif ()

# GDCM
find_package(GDCM QUIET)
if (NOT GDCM_FOUND)
    message(WARNING "GDCM NOT FOUND!")
else ()
    message(STATUS "DETECTED GDCM!")

    if (NOT TARGET GDCM::GDCM)
        add_library(GDCM::GDCM INTERFACE IMPORTED)

        set_target_properties(GDCM::GDCM PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GDCM_INCLUDE_DIRS}"
                INTERFACE_LINK_DIRECTORIES "${GDCM_LIBRARY_DIRS}"
                INTERFACE_LINK_LIBRARIES "gdcmCommon;gdcmDICT;gdcmDSED;gdcmIOD;gdcmMSFF;gdcmMEXD;gdcmjpeg8;gdcmjpeg12;gdcmjpeg16;gdcmcharls")

    endif ()
endif ()

# EIGEN
if (EXISTS ${Eigen3_DIR}/share/eigen3/cmake)
    set(Eigen3_DIR ${Eigen3_DIR}/share/eigen3/cmake)
endif ()

find_package(Eigen3 QUIET NO_MODULE)

if (NOT Eigen3_FOUND AND NOT TARGET Eigen3::Eigen AND ${ENABLE_SUPERBUILD})
    find_package(Git REQUIRED)
    if (NOT EXISTS dep/Eigen3)
        execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/eigenteam/eigen-git-mirror dep/Eigen3
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    else ()
        execute_process(COMMAND ${GIT_EXECUTABLE} pull
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dep/Eigen3)
    endif ()

    add_subdirectory(dep/Eigen3)
else ()
    message(STATUS "DETECTED EIGEN3!")
endif ()

# OPENGL
find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(WARNING "OpenGL NOT FOUND!")
else ()
    message(STATUS "DETECTED OpenGL!")
endif ()

# GLEW
find_package(GLEW QUIET)

if (GLEW_FOUND)
    message(STATUS "DETECTED GLEW!")

    if (NOT TARGET GLEW::GLEW)
        message(STATUS "DETECTED GLEW!")

        add_library(GLEW::GLEW INTERFACE IMPORTED)

        set_target_properties(GLEW::GLEW PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIRS}"
                INTERFACE_LINK_DIRECTORIES "${GLEW_LIBRARY_DIRS}"
                INTERFACE_LINK_LIBRARIES "GLEW")
    endif ()
else ()
    message(WARNING "GLEW NOT FOUND!")
endif ()


# FREETYPE
find_package(Freetype REQUIRED)
if (NOT Freetype_FOUND)
    message(WARNING "FREETYPE NOT FOUND!")
else ()
    message(STATUS "DETECTED FREETYPE!")
endif ()

if (GL_USE_QT)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5OpenGL REQUIRED)

    if (NOT Qt5Core_FOUND OR NOT Qt5Widgets_FOUND OR NOT Qt5Gui_FOUND OR NOT Qt5OpenGL_FOUND)
        message(WARNING "QT5 NOT FOUND!")
    else ()
        message(STATUS "DETECTED QT5!")
    endif ()
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# definitions
# ----------
include(MakeLib.cmake)
include(MakeExamples.cmake)

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkAlgorithm
# ----------
_bk_make_lib_header(algorithm Algorithm)

target_link_libraries(bkAlgorithm INTERFACE bk::bkMath)

_bk_install_lib_header(Algorithm)

if (BUILD_EXAMPLES)
    set(BK_CURRENT_LIBRARY bkAlgorithm)

    # histogram
    add_executable(histogram_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/histogram.cpp)
    _bk_make_example(histogram_example ${BK_CURRENT_LIBRARY})

    # modification
    add_executable(modification_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/modification.cpp)
    _bk_make_example(modification_example ${BK_CURRENT_LIBRARY})

    # smooth
    add_executable(smooth_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/smooth.cpp)
    _bk_make_example(smooth_example ${BK_CURRENT_LIBRARY})

    # statistics
    add_executable(statistics_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/statistics.cpp)
    _bk_make_example(statistics_example ${BK_CURRENT_LIBRARY})

    # threshold
    add_executable(threshold_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/threshold.cpp)
    _bk_make_example(threshold_example ${BK_CURRENT_LIBRARY})
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkDataset
# ----------
set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/dataobject/filter/SmoothPointValuesFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/AverageSmoothingImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/BinomialSmoothingImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/ConnectedComponentsAnalysisImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/ConnectedComponentsAnalysisKeepLargestRegionImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/DistanceMapImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/FFTImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/HistogramEqualizationImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/IFFTImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/IntervalThresholdImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/LaplaceBinomialImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/LaplaceImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MarchingCubesFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MaximumImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MedianImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MinimumImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalClosingAndOpeningImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalClosingImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalDilationImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalErosionImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalOpeningAndClosingImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/MorphologicalOpeningImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/SobelImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/ThresholdImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/image/filter/UnsharpMaskingImageFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/line/ScalarLineFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/line/ScalarLineThreshold.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDataset/transformation/DicomTransformation.cpp)

add_library(bkDataset ${SOURCE_FILES})
_bk_make_lib(dataset Dataset)

if (PNG_FOUND)
    target_compile_definitions(bkDataset PUBLIC BK_LIB_PNG_AVAILABLE)
endif ()

target_link_libraries(bkDataset PUBLIC bk::bkAlgorithm)
target_link_libraries(bkDataset PUBLIC bk::bkTools)
target_link_libraries(bkDataset PUBLIC bk::bkMath)
target_link_libraries(bkDataset PUBLIC bk::bkTypeTraits)
if (PNG_FOUND)
    target_link_libraries(bkDataset PUBLIC PNG::PNG)
endif ()
if (OpenMP_CXX_FOUND)
    target_link_libraries(bkDataset PRIVATE OpenMP::OpenMP_CXX)
endif ()

_bk_install_lib(dataset Dataset DATASET)

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkDicom
# ----------
if (GDCM_FOUND)
    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDicom/DicomDirImporter.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDicom/DicomFileInfos.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/bkDicom/DicomImageInfos.cpp)

    add_library(bkDicom ${SOURCE_FILES})
    _bk_make_lib(dicom Dicom)

    target_link_libraries(bkDicom PUBLIC stdc++fs)
    target_link_libraries(bkDicom PUBLIC bk::bkMath)
    target_link_libraries(bkDicom PUBLIC bk::bkDataset)
    target_link_libraries(bkDicom PUBLIC bk::bkTools)
    target_link_libraries(bkDicom PUBLIC GDCM::GDCM)

    target_compile_definitions(bkDicom PUBLIC BK_LIB_GDCM_AVAILABLE)

    _bk_install_lib(dicom Dicom DICOM)
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkGL
# ----------
set(SOURCE_FILES # BUFFER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/Buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/RegisteredValue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/FBO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/IBO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/SSBO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/UBO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/UBOSpecialization.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/buffer/VBO.cpp
        # SHADER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/shader/ComputeShader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/shader/Shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/shader/ShaderLibrary.cpp
        # RENDERABLE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/AbstractRenderable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/AbstractSceneRenderable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/ColorBarView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/ScreenQuad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/background/AbstractBackground.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/background/GradientBackground.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/background/UniformBackground.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/dvr/DVRImageView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/dvr/DVRImage3DView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/dvr/DVRImage4DView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/segmenter/GrayImageGraphCutView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/texture/Texture2DView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/AbstractGraySliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/AbstractRGBSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/AbstractSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/GrayDICOMSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/GrayImageLiveThresholdSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/GrayImagePlusSegmentationSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/GrayImageSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/RGBDICOMFlowImageSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/RGBDICOMSliceView.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/image/RGBImageSliceView.cpp 
        #        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/imageplane/ImagePlanePreviewView.cpp # TODO
        #        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/imageplane/ImagePlaneView.cpp # TODO

        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/MultiLinePlotView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotAbstractDataVectorView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotArea.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotAxis.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/AreaPlotDataVector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotDataVector.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotLine.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/PlotMarker.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/SingleLinePlotView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/plot/AreaPlotView.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/line/LineView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/mesh/TriangularMesh3DView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/mesh/TriangularMesh3DCenterlineExtractionView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/text/TextView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/renderable/transfer_function/WindowingTransferFunctionView.cpp
        # TEXTURE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/texture/AbstractTexture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/texture/Texture2D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/texture/Texture3D.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/texture/TextureCubeMap.cpp
        # TRACKBALL
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/trackball/Quaternion.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/trackball/TrackBall.cpp
        # VAO
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/vao/VAO.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/vao/VertexAttributePointer.cpp
        #
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/AbstractBindable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/AbstractObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/Animator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/Camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/gl_error.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/Interactor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/KeyBoard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/Mouse.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/OrderIndependentTransparency.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/SuperSampler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBODVR.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOGlobal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOLine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOPhong.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOPlotArea.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOPlot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOPlotLine.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOSelectionSphere.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOSliceView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/UBOText.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/Renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkGL/WindowGeometry.cpp
        )

add_library(bkGL ${SOURCE_FILES})
_bk_make_lib(gl GL)

if (PNG_FOUND)
    target_compile_definitions(bkGL PUBLIC BK_LIB_PNG_AVAILABLE)
endif ()

if (GL_USE_QT)
    target_compile_definitions(bkGL PUBLIC BK_LIB_QT_AVAILABLE)
endif ()

if (GDCM_FOUND)
    target_compile_definitions(bkGL PUBLIC BK_LIB_GDCM_AVAILABLE)
endif ()

if (UNIX) # TODO something more sophisticated
    target_compile_definitions(bkGL PUBLIC BK_GL_FONT_PATH="/usr/share/fonts/TTF/DejaVuSans.ttf")
else ()
    target_compile_definitions(bkGL PUBLIC BK_GL_FONT_PATH="C:/Windows/Fonts/arial.ttf")
endif ()

target_link_libraries(bkGL PUBLIC bk::bkAlgorithm)
target_link_libraries(bkGL PUBLIC bk::bkDataset)
target_link_libraries(bkGL PUBLIC bk::bkTools)
target_link_libraries(bkGL PUBLIC bk::bkMath)
target_link_libraries(bkGL PUBLIC bk::bkTypeTraits)
if (PNG_FOUND)
    target_link_libraries(bkGL PUBLIC PNG::PNG)
endif ()
if (OpenMP_CXX_FOUND)
    target_link_libraries(bkGL PUBLIC OpenMP::OpenMP_CXX)
endif ()
if (GDCM_FOUND)
    target_link_libraries(bkGL PUBLIC GDCM::GDCM)
endif ()

if (GL_USE_QT)
    target_link_libraries(bkGL PUBLIC Qt5::Core)
    target_link_libraries(bkGL PUBLIC Qt5::Gui)
    target_link_libraries(bkGL PUBLIC Qt5::Widgets)
    target_link_libraries(bkGL PUBLIC Qt5::OpenGL)
else ()
    target_link_libraries(bkGL PUBLIC OpenGL::GL)
    target_link_libraries(bkGL PUBLIC GLEW::GLEW)
endif ()

target_link_libraries(bkGL PUBLIC Freetype::Freetype)
target_link_libraries(bkGL PUBLIC stdc++fs)

_bk_install_lib(gl GL GL)

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkMath
# ----------
set(SOURCE_FILES
        # fft
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkMath/fft/fft.cpp)

add_library(bkMath ${SOURCE_FILES})
_bk_make_lib(math Math ${SOURCE_FILES})

target_link_libraries(bkMath PUBLIC bk::bkTools)
if (OpenMP_CXX_FOUND)
    target_link_libraries(bkMath PUBLIC OpenMP::OpenMP_CXX)
endif ()

_bk_install_lib(math Math MATH)

if (BUILD_EXAMPLES)
    set(BK_CURRENT_LIBRARY bkMath)

    # matrix
    add_executable(matrix_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/matrix.cpp)
    _bk_make_example(matrix_example ${BK_CURRENT_LIBRARY})
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkTools
# ----------
set(SOURCE_FILES
        # bitvector
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/bitvector/BitVectorDynamic.cpp
        #clock
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/clock/Clock.cpp
        # color
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/color/AbstractColor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/color/ColorBarRGBA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/color/ColorRGBA.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/color/WindowingTransferFunction.cpp
        # io
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/io/read_text_file.cpp
        # localization
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/localization/LocalizationManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/localization/GlobalLocalizationManager.cpp
        # options
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/options/Options.cpp
        # progress
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/progress/Progress.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/progress/ProgressManager.cpp
        # string_utils
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/string_utils/string_utils.cpp
        # threadpool
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/threadpool/ThreadPool.cpp
        )

add_library(bkTools ${SOURCE_FILES})
_bk_make_lib(tools Tools)

target_link_libraries(bkTools PUBLIC Threads::Threads)

_bk_install_lib(tools Tools TOOLS)

# add localization crawler tool
add_executable(localization_crawler ${CMAKE_CURRENT_SOURCE_DIR}/src/bkTools/localization/localization_crawler.cpp)
target_link_libraries(localization_crawler PRIVATE bk::bkTools)
target_link_libraries(localization_crawler PRIVATE stdc++fs)

if (WIN32) # .exe for binaries
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/localization_crawler.exe
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            DESTINATION ${CMAKE_INSTALL_BINDIR}/bkTools)
else ()
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/localization_crawler
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            DESTINATION ${CMAKE_INSTALL_BINDIR}/bkTools)
endif ()


if (BUILD_EXAMPLES)
    set(BK_CURRENT_LIBRARY bkTools)

    # bitvector
    add_executable(bitvector_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/bitvector.cpp)
    _bk_make_example(bitvector_example ${BK_CURRENT_LIBRARY})

    # clock
    add_executable(clock_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/clock.cpp)
    _bk_make_example(clock_example ${BK_CURRENT_LIBRARY})

    # graphcut
    if (PNG_FOUND)
        add_executable(graphcut_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/graphcut.cpp)
        target_link_libraries(graphcut_example PRIVATE PNG::PNG)
        if (OpenMP_CXX_FOUND)
            target_link_libraries(graphcut_example PRIVATE OpenMP::OpenMP_CXX)
        endif ()
        _bk_make_example(graphcut_example ${BK_CURRENT_LIBRARY})
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/graphcut_input_image.png
                ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/graphcut_input_inside.png
                ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/graphcut_input_outside.png
                DESTINATION ${CMAKE_INSTALL_BINDIR}/${BK_CURRENT_LIBRARY}/examples)
    else ()
        message(WARNING "LIBPNG NOT FOUND! SKIPPING GRAPH CUT EXAMPLE!")
    endif ()

    # kdtree
    add_executable(kdtree_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/kdtree.cpp)
    _bk_make_example(kdtree_example ${BK_CURRENT_LIBRARY})

    # localization
    add_executable(localization_example
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/localization/localization.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/localization/my_custom_localization.cpp)
    _bk_make_example(localization_example ${BK_CURRENT_LIBRARY})
    target_compile_definitions(localization_example PRIVATE BK_LOCALIZATION)

    # ndcontainer
    add_executable(ndcontainer_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/ndcontainer.cpp)
    _bk_make_example(ndcontainer_example ${BK_CURRENT_LIBRARY})

    # cpimpl
    add_executable(cpimpl_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/cpimpl.cpp)
    _bk_make_example(cpimpl_example ${BK_CURRENT_LIBRARY})

    # options
    add_executable(options_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/options.cpp)
    _bk_make_example(options_example ${BK_CURRENT_LIBRARY})

    # progress
    add_executable(progress_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/progress.cpp)
    _bk_make_example(progress_example ${BK_CURRENT_LIBRARY})

    # signal
    add_executable(signal_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/signal.cpp)
    _bk_make_example(signal_example ${BK_CURRENT_LIBRARY})

    # singleton
    add_executable(singleton_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/singleton.cpp)
    _bk_make_example(singleton_example ${BK_CURRENT_LIBRARY})

    # threadpool
    add_executable(threadpool_example ${CMAKE_CURRENT_SOURCE_DIR}/examples/${BK_CURRENT_LIBRARY}/threadpool.cpp)
    _bk_make_example(threadpool_example ${BK_CURRENT_LIBRARY})
endif ()

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bkTypeTraits
# ----------
_bk_make_lib_header(type_traits TypeTraits)
_bk_install_lib_header(TypeTraits)

# ------------------------------------------------------------------------------------------------------------------------------------
# add library bk
# ----------
add_library(bk ${CMAKE_CURRENT_SOURCE_DIR}/src/bk/dummy.cpp)
add_library(bk::bk ALIAS bk)

target_compile_features(bk PUBLIC cxx_std_17)

set_target_properties(bk PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

target_link_libraries(bk PUBLIC bk::bkAlgorithm)
target_link_libraries(bk PUBLIC bk::bkDataset)
target_link_libraries(bk PUBLIC bk::bkDicom)
target_link_libraries(bk PUBLIC bk::bkGL)
target_link_libraries(bk PUBLIC bk::bkMath)
target_link_libraries(bk PUBLIC bk::bkTools)
target_link_libraries(bk PUBLIC bk::bkTypeTraits)

install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/bk
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS bk
        EXPORT bkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT bkTargets
        FILE bkTargets.cmake
        NAMESPACE bk::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bkConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/bkConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/bkConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bkConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bkConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk)

export(EXPORT bkTargets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/bkTargets.cmake
        NAMESPACE bk::)
