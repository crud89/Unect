using System;
using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Stores a view over an image from a stream.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ImageView
    {

        /// <summary>
        /// Stores a pointer to the image contents.
        /// </summary>
        public IntPtr data;

        /// <summary>
        /// Stores the size of the image buffer.
        /// </summary>
        public int size;

        /// <summary>
        /// Stores the width of the image in pixels.
        /// </summary>
        public int width;

        /// <summary>
        /// Stores the height of the image in pixels.
        /// </summary>
        public int height;

        /// <summary>
        /// Stores the relative timestamp of the frame arrival in milliseconds.
        /// </summary>
        public long timestamp;

        /// <summary>
        /// Stores the monotonically incrementing frame generation.
        /// </summary>
        public ulong generation;

    }

}