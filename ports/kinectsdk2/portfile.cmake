if(NOT VCPKG_TARGET_IS_WINDOWS OR VCPKG_TARGET_IS_UWP OR VCPKG_TARGET_IS_MINGW)
    message(FATAL_ERROR "${PORT} requires a desktop Windows/MSVC triplet.")
endif()

if(VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    set(KINECT_ARCH "x64")
elseif(VCPKG_TARGET_ARCHITECTURE STREQUAL "x86")
    set(KINECT_ARCH "x86")
else()
    message(FATAL_ERROR "The Kinect SDK 2.0 only ships x86 and x64 binaries (triplet arch: ${VCPKG_TARGET_ARCHITECTURE}).")
endif()

set(VCPKG_POLICY_SKIP_DUMPBIN_CHECKS enabled)
set(VCPKG_POLICY_DLLS_IN_STATIC_LIBRARY enabled)

set(kinect_hints "")

if(DEFINED ENV{KINECTSDK20_DIR})
    file(TO_CMAKE_PATH "$ENV{KINECTSDK20_DIR}" kinect_env_dir)
    string(REGEX REPLACE "/+$" "" kinect_env_dir "${kinect_env_dir}")
    list(APPEND kinect_hints "${kinect_env_dir}")
endif()

string(REGEX REPLACE "\\.([^.]*)$" "_\\1" KINECT_SDK_VERSION "${VERSION}")

foreach(program_files IN ITEMS "$ENV{ProgramW6432}" "$ENV{ProgramFiles}")
    if(program_files)
        file(TO_CMAKE_PATH "${program_files}" program_files)
        list(APPEND kinect_hints "${program_files}/Microsoft SDKs/Kinect/v${KINECT_SDK_VERSION}")
    endif()
endforeach()

set(KINECT_ROOT "")

foreach(hint IN LISTS kinect_hints)
    if(EXISTS "${hint}/inc/Kinect.h")
        set(KINECT_ROOT "${hint}")
        break()
    endif()
endforeach()

if(NOT KINECT_ROOT)
    message(FATAL_ERROR "Could not find the Kinect for Windows SDK 2.0 under: ${kinect_hints}. Please download and install KinectSDK-v${KINECT_SDK_VERSION}-Setup.exe first.")
endif()

message(STATUS "Using Kinect for Windows SDK 2.0 at: ${KINECT_ROOT}")

file(GLOB kinect_headers "${KINECT_ROOT}/inc/*.h")

if(NOT kinect_headers)
    message(FATAL_ERROR "No headers found in ${KINECT_ROOT}/inc")
endif()

file(INSTALL ${kinect_headers} DESTINATION "${CURRENT_PACKAGES_DIR}/include")

set(kinect_libs "Kinect20.lib")

if("face" IN_LIST FEATURES)
    list(APPEND kinect_libs "Kinect20.Face.lib")
endif()

if("fusion" IN_LIST FEATURES)
    list(APPEND kinect_libs "Kinect20.Fusion.lib")
endif()

if("vgb" IN_LIST FEATURES)
    list(APPEND kinect_libs "Kinect20.VisualGestureBuilder.lib")
endif()

foreach(lib IN LISTS kinect_libs)
    set(lib_path "${KINECT_ROOT}/Lib/${KINECT_ARCH}/${lib}")

    if(NOT EXISTS "${lib_path}")
        message(FATAL_ERROR "Expected ${lib_path} but it is missing. Please re-install the SDK using KinectSDK-v${KINECT_SDK_VERSION}-Setup.exe.")
    endif()

    file(INSTALL "${lib_path}" DESTINATION "${CURRENT_PACKAGES_DIR}/lib")
    file(INSTALL "${lib_path}" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib")
endforeach()

function(kinect_install_redist subdir dll datadir)
    set(dll_path "${KINECT_ROOT}/Redist/${subdir}/${KINECT_ARCH}/${dll}")

    if(NOT EXISTS "${dll_path}")
        message(FATAL_ERROR "Could not find ${dll_path}. Please re-install the SDK using KinectSDK-v${KINECT_SDK_VERSION}-Setup.exe.")
    endif()

    file(INSTALL "${dll_path}" DESTINATION "${CURRENT_PACKAGES_DIR}/bin")
    file(INSTALL "${dll_path}" DESTINATION "${CURRENT_PACKAGES_DIR}/debug/bin")

    if(datadir AND EXISTS "${KINECT_ROOT}/Redist/${subdir}/${KINECT_ARCH}/${datadir}")
        file(INSTALL "${KINECT_ROOT}/Redist/${subdir}/${KINECT_ARCH}/${datadir}" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}/data")
    endif()
endfunction()

if("face" IN_LIST FEATURES)
    kinect_install_redist("Face" "Kinect20.Face.dll" "NuiDatabase")
endif()

if("fusion" IN_LIST FEATURES)
    kinect_install_redist("Fusion" "Kinect20.Fusion.dll" "")
endif()

if("vgb" IN_LIST FEATURES)
    kinect_install_redist("VGB" "Kinect20.VisualGestureBuilder.dll" "vgbtechs")
endif()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/kinectsdk2-config.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(WRITE "${CURRENT_PACKAGES_DIR}/share/${PORT}/copyright" "The Microsoft Kinect for Windows SDK 2.0 is proprietary software licensed by Microsoft Corporation. See the SDK EULA delivered with the installer for redistribution terms.")