set(UNECT_PKG_SRC   "${CMAKE_SOURCE_DIR}/package")
set(UNECT_PKG_STAGE "${CMAKE_BINARY_DIR}/unity-package/package")
set(UNECT_DIST_DIR  "${CMAKE_BINARY_DIR}/dist")
set(UNECT_PLUGIN_SUBDIR "Runtime/Plugins/x86_64")

# Used for development to copy a fresh DLL build into the package source directory. Note that this file is .gitignored.
add_custom_target(unity-sync
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNECT_PKG_SRC}/${UNECT_PLUGIN_SUBDIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:libunect>" "${UNECT_PKG_SRC}/${UNECT_PLUGIN_SUBDIR}/libunect.dll"
    DEPENDS libunect
    COMMENT "Copying libunect.dll into '${UNECT_PKG_SRC}'..." 
    VERBATIM)

# Packages the project into a tarball release.
add_custom_target(unity-package
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/unity-package"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNECT_PKG_STAGE}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${UNECT_PKG_SRC}" "${UNECT_PKG_STAGE}"

    # Overwrite any local copy of the libunect.dll file from unity-sync.
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNECT_PKG_STAGE}/${UNECT_PLUGIN_SUBDIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:libunect>" "${UNECT_PKG_STAGE}/${UNECT_PLUGIN_SUBDIR}/libunect.dll"

    # Delete debug symbols and verify package contents.
    COMMAND ${CMAKE_COMMAND} -E rm -f "${UNECT_PKG_STAGE}/${UNECT_PLUGIN_SUBDIR}/libunect.pdb"

    # Package everything into a tarball.
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNECT_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar czf "${UNECT_DIST_DIR}/${UNECT_PACKAGE_NAME}-${UNECT_PACKAGE_VERSION}.tgz" -- package

    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/unity-package"

    DEPENDS libunect
    COMMENT "Packing ${LIBUNECT_PACKAGE_NAME}-${LIBUNECT_PACKAGE_VERSION}.tgz.."
    VERBATIM)

# Package debug symbols.
add_custom_target(unity-package-debug-symbols
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNECT_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_PDB_FILE:libunect>" "${UNECT_DIST_DIR}/libunect-${UNECT_PACKAGE_VERSION}.pdb"
    DEPENDS libunect
    VERBATIM)