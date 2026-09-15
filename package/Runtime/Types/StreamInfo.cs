using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Stores meta-data about a stream.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct StreamInfo
    {

        /// <summary>
        /// The width of a frame in the stream.
        /// </summary>
        public int width;

        /// <summary>
        /// The height of a frame in the stream.
        /// </summary>
        public int height;

        /// <summary>
        /// The depth of the stream format.
        /// </summary>
        public uint bytesPerPixel;

        /// <summary>
        /// The number of pixels of a frame in the stream.
        /// </summary>
        public int pixelCount;

        /// <summary>
        /// The overall size of a frame in the stream in bytes.
        /// </summary>
        uint totalSize;

    }

}