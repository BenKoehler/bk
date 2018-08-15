function(_bk_make_example name libname)
    target_link_libraries(${name} PRIVATE bk::${libname})

    target_include_directories(${name}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            $<INSTALL_INTERFACE:include/${libname}>
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/${libname}>)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            DESTINATION ${CMAKE_INSTALL_BINDIR}/${libname}/examples)
endfunction()