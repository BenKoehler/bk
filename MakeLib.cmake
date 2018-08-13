include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

function(_bk_make_lib name Name)
    add_library(bk::bk${Name} ALIAS bk${Name})

    target_compile_features(bk${Name} PUBLIC cxx_std_17)

    target_compile_options(bk${Name} PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
            -pipe -march=native
            $<$<CONFIG:Release>:-O3 -flto>
            $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
            $<$<CXX_COMPILER_ID:MSVC>:
            $<$<CONFIG:Debug>:/Od /Wall /Zi>>)

    set_target_properties(bk${Name} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
            )

    target_include_directories(bk${Name}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/bk${Name}>
            $<INSTALL_INTERFACE:include>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(_bk_make_lib_header name Name)
    add_library(bk${Name} INTERFACE)
    add_library(bk::bk${Name} ALIAS bk${Name})

    target_compile_features(bk${Name} INTERFACE cxx_std_17)

    target_compile_definitions(bk${Name} INTERFACE LIBRARY_HEADER_ONLY)

    target_include_directories(bk${Name} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/bk${Name}>
            $<INSTALL_INTERFACE:include>)
endfunction()

function(_bk_install_lib name Name NAME)
    generate_export_header(bk${Name}
            EXPORT_MACRO_NAME BK${NAME}_EXPORT
            EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/bk${Name}/lib/bk${Name}_export.h)

    install(TARGETS bk${Name}
            EXPORT bk${Name}Targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/include/bk${Name}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/bk${Name}/LICENSE
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/bk${Name}/README.md
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/bk${Name})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/bk${Name}/lib/bk${Name}_export.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bk${Name}/lib)

    install(EXPORT bk${Name}Targets
            FILE bk${Name}Targets.cmake
            NAMESPACE bk::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bk${Name}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}ConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    export(EXPORT bk${Name}Targets
            FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/bk${Name}Targets.cmake
            NAMESPACE bk::)
endfunction()

function(_bk_install_lib_header name Name)
    install(TARGETS bk${Name}
            EXPORT bk${Name}Targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/include/bk${Name}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/bk${Name}/LICENSE
            ${CMAKE_CURRENT_SOURCE_DIR}/doc/bk${Name}/README.md
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/bk${Name})

    install(EXPORT bk${Name}Targets
            FILE bk${Name}Targets.cmake
            NAMESPACE bk::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bk${Name}Config.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}Config.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion)

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}Config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/bk${Name}ConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bk${Name})

    export(EXPORT bk${Name}Targets
            FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/bk${Name}Targets.cmake
            NAMESPACE bk::)
endfunction()