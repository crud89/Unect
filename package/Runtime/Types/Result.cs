using System;
using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// A collection of valid error codes.
    /// </summary>
    public enum UnectResult : int
    {

        /// <summary>
        /// No error occurred.
        /// </summary>
        Ok = 0,

        /// <summary>
        /// There's currently no new frame available.
        /// </summary>
        NoFrame = 1,

        /// <summary>
        /// The requested operation failed.
        /// </summary>
        Fail = -1,

        /// <summary>
        /// An invalid argument has been provided.
        /// </summary>
        InvalidArgument = -2,

        /// <summary>
        /// An handle has outlived its generation/epoch.
        /// </summary>
        StaleSession = -3,

        /// <summary>
        /// The requested stream is not enabled.
        /// </summary>
        StreamDisabled = -4,

        /// <summary>
        /// The provided buffer is too small.
        /// </summary>
        BufferTooSmall = -5,

        /// <summary>
        /// The sensor is unavailable.
        /// </summary>
        UnavailableSensor = -6,

        /// <summary>
        /// A resource is already locked.
        /// </summary>
        AlreadyLocked = -7,

        /// <summary>
        /// A resource is not locked.
        /// </summary>
        NotLocked = -8,

        /// <summary>
        /// There has been an ABI mismatch.
        /// </summary>
        AbiMismatch = -9,

        /// <summary>
        /// The requested operation is unsupported.
        /// </summary>
        NotSupported = -10,

        /// <summary>
        /// The requested operation is currently not implemented.
        /// </summary>
        NotImplemented = -11

    }

    /// <summary>
    /// Contains extension methods to the <see cref="UnectResult"/> enumeration.
    /// </summary>
    public static class UnectResultExtensions
    {

        /// <summary>
        /// Checks if a result is a hard error.
        /// </summary>
        /// <param name="result">The result of an operation.</param>
        /// <returns>`true`, if the result is an error and `false` otherwise.</returns>
        public static bool Failed(this UnectResult result)
            => Convert.ToInt32(result) < 0;

        /// <summary>
        /// Checks if a result is successful.
        /// </summary>
        /// <param name="result">The result of an operation.</param>
        /// <returns>`true`, if the result indicates success and `false` otherwise.</returns>
        public static bool Succeeded(this UnectResult result)
            => Convert.ToInt32(result) >= 0;

        /// <summary>
        /// Converts a result into a human readable string.
        /// </summary>
        /// <param name="result">The result to convert.</param>
        /// <returns>The human-readable string representation of <paramref name="result" />.</returns>
        public static string AsString(this UnectResult result)
        {
            var p = UnectInterop.Unect_GetResultString(result);

            return p == IntPtr.Zero ? result.ToString() : Marshal.PtrToStringAnsi(p);
        }

    }

}