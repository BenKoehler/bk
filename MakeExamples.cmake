function(_bk_make_example name libname)
    target_link_libraries(${name} PRIVATE bk::${libname})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            DESTINATION ${CMAKE_INSTALL_BINDIR}/${libname})
endfunction()