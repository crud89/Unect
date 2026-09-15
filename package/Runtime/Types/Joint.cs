using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Represents a singular joint of the skeleton of a tracked body.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Joint
    {

        /// <summary>
        /// Stores the position of the joint.
        /// </summary>
        public Vector3 Position;

        /// <summary>
        /// Stores the orientation of the joint.
        /// </summary>
        public Vector4 Orientation;

        /// <summary>
        /// Stores the tracking state for the joint.
        /// </summary>
        public TrackingState State;

    }

}