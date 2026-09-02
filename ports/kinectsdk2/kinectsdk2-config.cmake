cmake_minimum_required(VERSION 3.15)

if(TARGET unofficial::kinectsdk2::kinectsdk2)
    return()
endif()

get_filename_component(_kinect2_prefix "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(unofficial_kinectsdk2_DATA_DIR "${CMAKE_CURRENT_LIST_DIR}/data" CACHE INTERNAL "")

macro(_kinect2_import _component _libname)
    if(EXISTS "${_kinect2_prefix}/lib/${_libname}")
        add_library(unofficial::kinectsdk2::${_component} UNKNOWN IMPORTED)
        set_target_properties(unofficial::kinectsdk2::${_component} PROPERTIES
            IMPORTED_LOCATION "${_kinect2_prefix}/lib/${_libname}"
            IMPORTED_LOCATION_RELEASE "${_kinect2_prefix}/lib/${_libname}"
            INTERFACE_INCLUDE_DIRECTORIES "${_kinect2_prefix}/include")
        if(EXISTS "${_kinect2_prefix}/debug/lib/${_libname}")
            set_target_properties(unofficial::kinectsdk2::${_component} PROPERTIES
                IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
                IMPORTED_LOCATION_DEBUG "${_kinect2_prefix}/debug/lib/${_libname}")
        endif()
    endif()
endmacro()

_kinect2_import(kinectsdk2 "Kinect20.lib")
_kinect2_import(face       "Kinect20.Face.lib")
_kinect2_import(fusion     "Kinect20.Fusion.lib")
_kinect2_import(vgb        "Kinect20.VisualGestureBuilder.lib")

if(NOT TARGET unofficial::kinectsdk2::kinectsdk2)
    set(unofficial-kinectsdk2_FOUND FALSE)
    set(unofficial-kinectsdk2_NOT_FOUND_MESSAGE "Kinect20.lib was not found under ${_kinect2_prefix}/lib")
    return()
endif()

foreach(_kinect2_comp IN ITEMS face fusion vgb)
    if(TARGET unofficial::kinectsdk2::${_kinect2_comp})
        set_property(TARGET unofficial::kinectsdk2::${_kinect2_comp} APPEND
                     PROPERTY INTERFACE_LINK_LIBRARIES unofficial::kinectsdk2::kinectsdk2)
    endif()
endforeach()

unset(_kinect2_comp)

function(kinectsdk2_deploy_data _target)
    foreach(_dir IN ITEMS "NuiDatabase" "vgbtechs")
        if(EXISTS "${unofficial_kinectsdk2_DATA_DIR}/${_dir}")
            add_custom_command(TARGET ${_target} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory
                        "${unofficial_kinectsdk2_DATA_DIR}/${_dir}"
                        "$<TARGET_FILE_DIR:${_target}>/${_dir}"
                COMMENT "Deploying Kinect data from ${_dir} along ${_target}")
        endif()
    endforeach()
endfunction()