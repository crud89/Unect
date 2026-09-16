cmake_minimum_required(VERSION 3.23)
project(libunect LANGUAGES CXX)

include(GenerateExportHeader)

find_package(kinectsdk2 CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED 
    "include/libunect.h"
    "include/body.h"
    "include/vector.h"

    "wrapper/kinect.h"
    "wrapper/kinect.cxx"

    "helpers.hpp"
    "internal.h"

    "events.h"
    "events.cxx"
    "registry.h"
    "registry.cxx"
	"libunect.cxx"
)

target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::kinectsdk2::kinectsdk2)
kinectsdk2_deploy_data(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Debug"
    MAP_IMPORTED_CONFIG_MINSIZEREL "Release"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    PREFIX ""
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

generate_export_header(${PROJECT_NAME}
    BASE_NAME ${PROJECT_NAME}
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}_export.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

if (MSVC)
    target_compile_options(libunect PRIVATE $<$<CONFIG:Release>:/Zi>)
    target_link_options(libunect PRIVATE $<$<CONFIG:Release,RelWithDebInfo>:/OPT:REF /OPT:ICF /DEBUG>)
endif (MSVC)