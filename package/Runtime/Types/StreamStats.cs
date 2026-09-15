using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Stores diagnostic information about a stream.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct StreamStats
    {

        /// <summary>
        /// Stores the number of arrived frames.
        /// </summary>
        public ulong framesArrived;

        /// <summary>
        /// Stores the number of dropped frames.
        /// </summary>
        public ulong framesDropped;

        /// <summary>
        /// Stores the current frame-rate.
        /// </summary>
        public float framerate;

        /// <summary>
        /// Stores the latency of the last frame in milliseconds.
        /// </summary>
        public float lastLatency;

    }

}