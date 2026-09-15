using System;
using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Imports the native interop layer.
    /// </summary>
    internal static unsafe class UnectInterop
    {

        /// <summary>
        /// Stores the library name.
        /// </summary>
        internal const string libunect = "libunect";

        /// <summary>
        /// Returns the current ABI version.
        /// </summary>
        /// <returns>The current ABI version.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern int Unect_GetAbiVersion();

        /// <summary>
        /// Returns a human-readable representation of an error code.
        /// </summary>
        /// <param name="result">The error (or result) code.</param>
        /// <returns>A human-readable representation of the result.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern IntPtr Unect_GetResultString(UnectResult result);

        /// <summary>
        /// Acquires a new session handle.
        /// </summary>
        /// <param name="sessionDesc">The session descriptor used to determine the session state.</param>
        /// <param name="session">The pointer to the session handle.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_GetSession(SessionDesc* sessionDesc, ulong* session);

        /// <summary>
        /// Releases a session handle.
        /// </summary>
        /// <param name="session">The session handle to release.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_ReleaseSession(ulong session);

        /// <summary>
        /// Verifies if a session is valid.
        /// </summary>
        /// <param name="session">The session to verify.</param>
        /// <returns>`true` if the session is valid and `false` otherwise.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern int Unect_SessionValid(ulong session);

        /// <summary>
        /// Unconditionally shuts down the library.
        /// </summary>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern void Unect_Shutdown();

        /// <summary>
        /// Returns the epoch of the library runtime.
        /// </summary>
        /// <remarks>
        /// An epoch is incremented whenever all sessions are released. This signals to callers that resources to this library should be rebound.
        /// </remarks>
        /// <returns>The epoch of the library runtime.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern uint Unect_Epoch();

        /// <summary>
        /// Retrieves the state of the connected sensor.
        /// </summary>
        /// <param name="session">The session handle on which to query for the sensor.</param>
        /// <param name="state">A pointer to the state variable.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_GetSensorState(ulong session, SensorState* state);

        /// <summary>
        /// Returns meta-data about a specific stream.
        /// </summary>
        /// <param name="session">The session from which to acquire the stream.</param>
        /// <param name="stream">The stream to query.</param>
        /// <param name="info">A pointer to the stream info.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_GetStreamInfo(ulong session, StreamIndex stream, StreamInfo* info);

        /// <summary>
        /// Returns the generation of an image stream.
        /// </summary>
        /// <param name="session">The session on which to query the stream.</param>
        /// <param name="stream">The index of the stream.</param>
        /// <returns>The image generation on the stream.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern ulong Unect_PeekGeneration(ulong session, StreamIndex stream);

        /// <summary>
        /// Acquires an image from the image stream.
        /// </summary>
        /// <param name="session">The session from which to acquire the image.</param>
        /// <param name="stream">The stream from which to obtain the image.</param>
        /// <param name="image">A pointer to the image view.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_LockImage(ulong session, StreamIndex stream, ImageView* image);

        /// <summary>
        /// Releases image back to the stream.
        /// </summary>
        /// <param name="session">The session from which the image was acquired.</param>
        /// <param name="stream">The stream from which the image was obtained.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_UnlockImage(ulong session, StreamIndex stream);

        /// <summary>
        /// Acquires a body from the bodies stream.
        /// </summary>
        /// <param name="session">The session from which to acquire the body.</param>
        /// <param name="body">A pointer to the body view.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_LockBodies(ulong session, BodyView* body);

        /// <summary>
        /// Releases a body back to the stream.
        /// </summary>
        /// <param name="session">The session from which the image was acquired.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_UnlockBodies(ulong session);

        /// <summary>
        /// Returns the current mapping generation, that is incremented, if the underlying sensor changes the coordinate mapping reference.
        /// </summary>
        /// <param name="session">The session from which to obtain the mapping generation.</param>
        /// <returns>The current mapping generation.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern uint Unect_GetMappingGeneration(ulong session);

        /// <summary>
        /// Maps a depth frame into camera space.
        /// </summary>
        /// <param name="session">The session used for mapping.</param>
        /// <param name="data">The depth data buffer.</param>
        /// <param name="size">The size of the depth data buffer.</param>
        /// <param name="result">A pointer to the result buffer.</param>
        /// <param name="resultCapacity">The size of the result buffer.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_MapDepthFrameToCameraSpace(ulong session, ushort* data, int size, Vector3* result, int resultCapacity);

        /// <summary>
        /// Maps a depth frame into color space.
        /// </summary>
        /// <param name="session">The session used for mapping.</param>
        /// <param name="data">The depth data buffer.</param>
        /// <param name="size">The size of the depth data buffer.</param>
        /// <param name="result">A pointer to the result buffer.</param>
        /// <param name="resultCapacity">The size of the result buffer.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_MapDepthFrameToColorSpace(ulong session, ushort* data, int size, Vector2* result, int resultCapacity);

        /// <summary>
        /// Maps a color frame into depth space.
        /// </summary>
        /// <param name="session">The session used for mapping.</param>
        /// <param name="data">The depth data buffer.</param>
        /// <param name="size">The size of the depth data buffer.</param>
        /// <param name="result">A pointer to the result buffer.</param>
        /// <param name="resultCapacity">The size of the result buffer.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_MapColorFrameToDepthSpace(ulong session, ushort* data, int size, Vector2* result, int resultCapacity);

        /// <summary>
        /// Maps a set of points into color space.
        /// </summary>
        /// <param name="session">The session used for mapping.</param>
        /// <param name="data">The points array.</param>
        /// <param name="size">The size of the points array</param>
        /// <param name="result">The coordinates of the points in the color image.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_MapCameraPointsToColorSpace(ulong session, Vector3* data, int size, Vector2* result);

        /// <summary>
        /// Maps a set of points into depth space.
        /// </summary>
        /// <param name="session">The session used for mapping.</param>
        /// <param name="data">The points array.</param>
        /// <param name="size">The size of the points array</param>
        /// <param name="result">The coordinates of the points in the color image.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_MapCameraPointsToDepthSpace(ulong session, Vector3* data, int size, Vector2* result);

        /// <summary>
        /// Acquires a number of queued events from the event queue.
        /// </summary>
        /// <param name="session">The session that hosts the event queue.</param>
        /// <param name="events">A pointer to the events buffer.</param>
        /// <param name="capacity">The number of events in the events buffer.</param>
        /// <param name="eventCount">A pointer that receives the number of events written into the buffer.</param>
        /// <param name="dropCount">A pointer that receives the number of dropped events.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_PollEvents(ulong session, Event* events, int capacity, int* eventCount, int* droppedCount);

        /// <summary>
        /// Returns the current diagnostics for a particular stream.
        /// </summary>
        /// <param name="session">The session that contains the stream.</param>
        /// <param name="stream">The stream about which to obtain the diagnostics.</param>
        /// <param name="stats">A pointer to the diagnostics container.</param>
        /// <returns>The return code of the function.</returns>
        [DllImport(libunect, CallingConvention = CallingConvention.StdCall)]
        internal static extern UnectResult Unect_GetStreamStats(ulong session, StreamIndex stream, StreamStats* stats);

    }

}