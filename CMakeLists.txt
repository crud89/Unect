cmake_minimum_required(VERSION 3.23)

# Read the version from the package.json.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package/package.json" UNECT_PKG_JSON)
string(JSON UNECT_PACKAGE_VERSION GET "${UNECT_PKG_JSON}" "version")
string(JSON UNECT_PACKAGE_NAME    GET "${UNECT_PKG_JSON}" "name")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" UNECT_VERSION_CORE "${UNECT_PACKAGE_VERSION}")

if(NOT UNECT_VERSION_CORE)
    message(FATAL_ERROR "Could not parse a version from package/package.json: '${UNECT_PACKAGE_VERSION}'")
endif()

# Define the project.
project(unect LANGUAGES CXX VERSION ${UNECT_VERSION_CORE})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/libunect/)
include(cmake/UnityPackage.cmake)