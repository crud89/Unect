using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Wraps a session to the native interop.
    /// </summary>
    public sealed unsafe class UnectSession : IDisposable
    {

        /// <summary>
        /// Stores the native session handle.
        /// </summary>
        private ulong _handle;

        /// <summary>
        /// Stores the epoch the session was created in.
        /// </summary>
        private readonly uint _epoch;

        /// <summary>
        /// Stores an array of locked states for each stream.
        /// </summary>
        private readonly bool[] _locked = new bool[(int)StreamIndex.Count];

        /// <summary>
        /// Returns the native session handle.
        /// </summary>
        public ulong Handle => _handle;

        /// <summary>
        /// Returns `true`, if the session is still valid and `false` otherwise.
        /// </summary>
        public bool IsValid 
            => _handle != 0 && _epoch == UnectInterop.Unect_Epoch() && UnectInterop.Unect_SessionValid(_handle) != 0;

        /// <summary>
        /// Creates a new session.
        /// </summary>
        /// <param name="handle">The native session handle.</param>
        /// <param name="epoch">The epoch in which the session is created.</param>
        private UnectSession(ulong handle, uint epoch)
        {
            _handle = handle;
            _epoch = epoch;
        }

        /// <summary>
        /// Acquires a new session instance.
        /// </summary>
        /// <param name="streams">A mask that indicates the streams the session wants to access.</param>
        /// <param name="colorFormat">The color format used to access the color stream.</param>
        /// <param name="bufferCount">The number of back buffers in the session.</param>
        /// <param name="flags">The session flags.</param>
        /// <returns>The session instance.</returns>
        /// <exception cref="UnectException">Thrown if the acquisition of the session failed.</exception>
        public static UnectSession Acquire(StreamType streams, ColorFormat colorFormat = ColorFormat.Bgra32, int bufferCount = 3, SessionFlags flags = SessionFlags.None)
        {
            // Make sure the runtime is initialized.
            UnectRuntime.EnsureInitialized();

            // Create a session descriptor.
            var desc = new SessionDesc
            {
                streams = streams,
                colorFormat = colorFormat,
                bufferCount = bufferCount,
                flags = flags
            };

            ulong handle;
            var result = UnectInterop.Unect_GetSession(&desc, &handle);

            if (result.Failed())
                throw new UnectException(result);

            return new UnectSession(handle, UnectInterop.Unect_Epoch());
        }

        /// <summary>
        /// Detaches the session from the underlying native session.
        /// </summary>
        internal void Detach()
        {
            ReleaseAllLocks();
            _handle = 0;
        }

        /// <summary>
        /// Implements session release.
        /// </summary>
        /// <remarks>
        /// Note that there is no finalizer in this class on purpose, as it would be called on a different thread, which violates the native contract.
        /// </remarks>
        public void Dispose()
        {
            if (_handle == 0)
                return;

            ReleaseAllLocks();

            UnectInterop.Unect_ReleaseSession(_handle);
            _handle = 0;

            UnectRuntime.StopTracking(this);
        }

        /// <summary>
        /// Returns the current state of the Kinect sensor.
        /// </summary>
        public SensorState SensorState
        {
            get
            {
                SensorState state;

                if (UnectInterop.Unect_GetSensorState(_handle, &state).Failed())
                    return SensorState.Closed;
                else 
                    return state;
            }
        }

        /// <summary>
        /// Returns meta-data about a stream at <paramref name="stream" />.
        /// </summary>
        /// <param name="stream">The index of the stream.</param>
        /// <returns>The meta-data about the stream.</returns>
        /// <exception cref="UnectException">Thrown if the stream info could not be retrieved.</exception>
        public StreamInfo GetStreamInfo(StreamIndex stream)
        {
            StreamInfo info;
            var result = UnectInterop.Unect_GetStreamInfo(_handle, stream, &info);

            if (result.Failed())
                throw new UnectException(result);

            return info;
        }

        /// <summary>
        /// Returns the current generation of the stream indicated by <paramref name="stream" />.
        /// </summary>
        /// <param name="stream">The index of the stream.</param>
        /// <returns>The generation of the stream.</returns>
        public ulong PeekGeneration(StreamIndex stream)
            => UnectInterop.Unect_PeekGeneration(_handle, stream);

        #region "Image and Body Retrieval"

        /// <summary>
        /// Attempts to retrieve an image from the back buffer.
        /// </summary>
        /// <param name="stream">The image stream.</param>
        /// <param name="image">The view that stores the image contents.</param>
        /// <returns>`true`, if the image was successfully acquired and `false` otherwise.</returns>
        /// <exception cref="UnectException">Thrown, if the image acquisition failed.</exception>
        public bool TryLockImage(StreamIndex stream, out ImageView image)
        {
            fixed (ImageView* p = &image)
            {
                var result = UnectInterop.Unect_LockImage(_handle, stream, p);

                if (result.Failed())
                    throw new UnectException(result);
                else if (result == UnectResult.Ok)
                {
                    _locked[Convert.ToInt32(stream)] = true;
                    return true;
                }

                return false;
            }
        }

        /// <summary>
        /// Releases an image from a stream back to the back buffer.
        /// </summary>
        /// <param name="stream">The image stream.</param>
        public void UnlockImage(StreamIndex stream)
        {
            var i = Convert.ToInt32(stream);

            if (!_locked[i]) 
                return;

            _locked[i] = false;
            UnectInterop.Unect_UnlockImage(_handle, stream);
        }

        /// <summary>
        /// Attempts to retrieve a body from the back buffer.
        /// </summary>
        /// <param name="body">The view that stores the body contents.</param>
        /// <returns>`true`, if the body was successfully acquired and `false` otherwise.</returns>
        /// <exception cref="UnectException">Thrown, if the body acquisition failed.</exception>
        public bool TryLockBodies(out BodyView body)
        {
            fixed (BodyView* p = &body)
            {
                var result = UnectInterop.Unect_LockBodies(_handle, p);

                if (result.Failed())
                    throw new UnectException(result);
                else if (result == UnectResult.Ok)
                {
                    _locked[Convert.ToInt32(StreamIndex.Body)] = true;
                    return true;
                }

                return false;
            }
        }

        /// <summary>
        /// Releases a body from the stream back to the back buffer.
        /// </summary>
        public void UnlockBodies()
        {
            var i = Convert.ToInt32(StreamIndex.Body);

            if (!_locked[i]) 
                return;

            _locked[i] = false;
            UnectInterop.Unect_UnlockBodies(_handle);
        }

        /// <summary>
        /// Releases all locks held by the session.
        /// </summary>
        internal void ReleaseAllLocks()
        {
            for (int i = 0; i < _locked.Length; ++i)
            {
                if (!_locked[i])
                    continue;

                _locked[i] = false;

                if (i == (int)StreamIndex.Body)
                    UnectInterop.Unect_UnlockBodies(_handle);
                else
                    UnectInterop.Unect_UnlockImage(_handle, (StreamIndex)i);
            }
        }

        #endregion

        #region "Mapping"

        /// <summary>
        /// Returns the current mapping generation.
        /// </summary>
        public uint MappingGeneration 
            => UnectInterop.Unect_GetMappingGeneration(_handle);

        /// <summary>
        /// Maps a depth frame into camera space.
        /// </summary>
        /// <param name="depth">The depth values.</param>
        /// <param name="destination">The camera-space positions.</param>
        /// <exception cref="UnectException">Thrown if the mapping failed.</exception>
        public void MapDepthFrameToCameraSpace(ReadOnlySpan<ushort> depth, Span<Vector3> destination)
        {
            fixed (ushort* d = depth)
            fixed (Vector3* o = destination)
            {
                var result = UnectInterop.Unect_MapDepthFrameToCameraSpace(_handle, d, depth.Length, o, destination.Length);

                if (result.Failed())
                    throw new UnectException(result);
            }
        }

        /// <summary>
        /// Maps a depth frame into color space.
        /// </summary>
        /// <param name="depth">The depth values.</param>
        /// <param name="destination">The uv coordinates of the depth values in the color image.</param>
        /// <exception cref="UnectException">Thrown if the mapping failed.</exception>
        public void MapDepthFrameToColorSpace(ReadOnlySpan<ushort> depth, Span<Vector2> destination)
        {
            fixed (ushort* d = depth)
            fixed (Vector2* o = destination)
            {
                var result = UnectInterop.Unect_MapDepthFrameToColorSpace(_handle, d, depth.Length, o, destination.Length);

                if (result.Failed())
                    throw new UnectException(result);
            }
        }

        /// <summary>
        /// Maps a color frame into depth space.
        /// </summary>
        /// <param name="depth">The depth values.</param>
        /// <param name="destination">The uv coordinates of the color values in the depth image.</param>
        /// <exception cref="UnectException">Thrown if the mapping failed.</exception>
        public void MapColorFrameToDepthSpace(ReadOnlySpan<ushort> depth, Span<Vector2> destination)
        {
            fixed (ushort* d = depth)
            fixed (Vector2* o = destination)
            {
                var result = UnectInterop.Unect_MapColorFrameToDepthSpace(_handle, d, depth.Length, o, destination.Length);

                if (result.Failed())
                    throw new UnectException(result);
            }
        }

        #endregion

        #region "Diagnostics"

        /// <summary>
        /// Polls sensor events from the session.
        /// </summary>
        /// <param name="buffer">The event buffer.</param>
        /// <param name="dropped">The number of dropped events.</param>
        /// <returns>The number of events written into the buffer.</returns>
        /// <exception cref="UnectException">Thrown, if polling failed.</exception>
        public int PollEvents(Span<Event> buffer, out int dropped)
        {
            fixed (Event* b = buffer)
            {
                int count, d;
                var result = UnectInterop.Unect_PollEvents(_handle, b, buffer.Length, &count, &d);

                if (result.Failed())
                    throw new UnectException(result);
                else
                    dropped = d;

                return count;
            }
        }

        /// <summary>
        /// Retrieves statistic information about a stream.
        /// </summary>
        /// <param name="stream">The stream for which to retrieve the information.</param>
        /// <returns>An object that stores the stream information.</returns>
        /// <exception cref="UnectException">Thrown if the acquisition failed.</exception>
        public StreamStats GetStreamStats(StreamIndex stream)
        {
            StreamStats stats;
            var result = UnectInterop.Unect_GetStreamStats(_handle, stream, &stats);
            
            if (result.Failed())
                throw new UnectException(result);

            return stats;
        }

        #endregion

    }

}