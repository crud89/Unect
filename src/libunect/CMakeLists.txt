cmake_minimum_required(VERSION 3.23)
project(libunect LANGUAGES CXX)

include(GenerateExportHeader)

find_package(kinectsdk2 CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED 
    "include/libunect.h"
	"main.cxx"
)

target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::kinectsdk2::kinectsdk2)
kinectsdk2_deploy_data(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Debug"
    MAP_IMPORTED_CONFIG_MINSIZEREL "Release"
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

install(TARGETS ${PROJECT_NAME} 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBRARY_DIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBRARY_DIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINARY_DIR}
)
