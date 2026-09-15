using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Stores meta-data about a session.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SessionDesc
    {

        /// <summary>
        /// A mask that indicates the streams that are enabled in the session.
        /// </summary>
        public StreamType streams;

        /// <summary>
        /// The color format provided for color streams in the session.
        /// </summary>
        public ColorFormat colorFormat;

        /// <summary>
        /// The number of back buffers in the session.
        /// </summary>
        public int bufferCount;

        /// <summary>
        /// The session flags.
        /// </summary>
        public SessionFlags flags;

    }

}