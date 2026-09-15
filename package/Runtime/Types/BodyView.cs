using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Represents a view over a frame from the body stream.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct BodyView
    {

        /// <summary>
        /// Stores a pointer to the bodies array.
        /// </summary>
        public Body* bodies;

        /// <summary>
        /// Stores the number of bodies in <see cref="bodies" />.
        /// </summary>
        public int bodyCount;

        /// <summary>
        /// Stores the number of bodies in <see cref="bodies"/> that are tracked.
        /// </summary>
        public int trackedBodies;

        /// <summary>
        /// Stores the floor clipping plane in parametric form.
        /// </summary>
        public Vector4 floorPlane;

        /// <summary>
        /// Stores the relative timestamp in milliseconds.
        /// </summary>
        public long timestamp;

        /// <summary>
        /// Stores the monotonically incrementing generation.
        /// </summary>
        public ulong generation;

    }

}