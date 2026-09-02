#pragma once

#include <libunect/libunect_export.h>

#include <stdint.h>

#if defined(_WIN32)
#  define UNECT_CALL __stdcall
#else 
#  define UNECT_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
    /// <summary>
    /// Stores the ABI version (used during development to check if a library should be reloaded).
    /// </summary>
    constexpr static int32_t UNECT_ABI_VERSION = 1;

    /// <summary>
    /// Returns the current ABI version.
    /// </summary>
    /// <returns>The current ABI version.</returns>
    static LIBUNECT_EXPORT int32_t UNECT_CALL Unect_GetAbiVersion() {
        return UNECT_ABI_VERSION;
    }
#endif

#pragma region "Enumerations"

    /// <summary>
    /// A collection of valid error codes.
    /// </summary>
    typedef enum UnectResult {

        /// <summary>
        /// No error occurred.
        /// </summary>
        UNECT_OK                    = 0,

        /// <summary>
        /// There's currently no new frame available.
        /// </summary>
        UNECT_NO_FRAME              = 1,

        /// <summary>
        /// The requested operation failed.
        /// </summary>
        UNECT_E_FAIL                = -1,

        /// <summary>
        /// An invalid argument has been provided.
        /// </summary>
        UNECT_E_INVALID_ARG         = -2,

        /// <summary>
        /// An handle has outlived its generation.
        /// </summary>
        UNECT_E_STALE_SESSION       = -3,

        /// <summary>
        /// The requested stream is not enabled.
        /// </summary>
        UNECT_E_STREAM_NOT_ENABLED  = -4,

        /// <summary>
        /// The provided buffer is too small.
        /// </summary>
        UNECT_E_BUFFER_TOO_SMALL    = -5,

        /// <summary>
        /// The sensor is unavailable.
        /// </summary>
        UNECT_E_SENSOR_UNAVAILABLE  = -6,

        /// <summary>
        /// A resource is already locked.
        /// </summary>
        UNECT_E_ALREADY_LOCKED      = -7,

        /// <summary>
        /// A resource is not locked.
        /// </summary>
        UNECT_E_NOT_LOCKED          = -8,

        /// <summary>
        /// There has been an ABI mismatch.
        /// </summary>
        UNECT_E_ABI_MISMATCH        = -9,

        /// <summary>
        /// The requested operation is unsupported.
        /// </summary>
        UNECT_E_UNSUPPORTED         = -10

    } UnectResult;

    /// <summary>
    /// Represents a type of a data stream.
    /// </summary>
    typedef enum UnectStreamType {

        /// <summary>
        /// Corresponds to no data stream.
        /// </summary>
        UNECT_STREAM_NONE              = 0,

        /// <summary>
        /// Corresponds to the depth image stream.
        /// </summary>
        UNECT_STREAM_DEPTH             = 1 << 0,

        /// <summary>
        /// Corresponds to the color image stream.
        /// </summary>
        UNECT_STREAM_COLOR             = 1 << 1,

        /// <summary>
        /// Corresponds to the IR image stream.
        /// </summary>
        UNECT_STREAM_INFRARED          = 1 << 2,

        /// <summary>
        /// Corresponds to the long exposure IR image stream.
        /// </summary>
        UNECT_STREAM_LONG_EXPOSURE_IR  = 1 << 3,

        /// <summary>
        /// Corresponds to the body index image stream.
        /// </summary>
        UNECT_STREAM_BODY_INDEX        = 1 << 4,

        /// <summary>
        /// Corresponds to the body stream.
        /// </summary>
        UNECT_STREAM_BODY              = 1 << 5,

        /// <summary>
        /// Corresponds to all streams.
        /// </summary>
        UNECT_STREAM_ALL               = 0x3F

    } UnectStreamType;

    /// <summary>
    /// Maps indices to individual streams.
    /// </summary>
    typedef enum UnectStreamIndex {

        /// <summary>
        /// Corresponds to the index of the depth image stream.
        /// </summary>
        UNECT_SI_DEPTH = 0,

        /// <summary>
        /// Corresponds to the index of the color image stream.
        /// </summary>
        UNECT_SI_COLOR,

        /// <summary>
        /// Corresponds to the index of the IR image stream.
        /// </summary>
        UNECT_SI_INFRARED,

        /// <summary>
        /// Corresponds to the index of the long exposure IR image stream.
        /// </summary>
        UNECT_SI_LONG_EXPOSURE_IR,

        /// <summary>
        /// Corresponds to the index of the body index image stream.
        /// </summary>
        UNECT_SI_BODY_INDEX,

        /// <summary>
        /// Corresponds to the index of the body stream.
        /// </summary>
        UNECT_SI_BODY,

        /// <summary>
        /// Stores the number of available streams.
        /// </summary>
        UNECT_SI_COUNT
    } UnectStreamIndex;

    /// <summary>
    /// Represents supported color formats.
    /// </summary>
    typedef enum UnectColorFormat {

        /// <summary>
        /// Outputs colors in BGRA order with 32 bit depth.
        /// </summary>
        UNECT_COLOR_BGRA32              = 0,

        /// <summary>
        /// Outputs colors in RGBA order with 32 bit color depth.
        /// </summary>
        UNECT_COLOR_RGBA32              = 1,

        /// <summary>
        /// A native color format without conversion cost.
        /// </summary>
        UNECT_COLOR_YUY2                = 2

    } UnectColorFormat;

#pragma endregion

#ifdef __cplusplus
} // extern "C"
#endif