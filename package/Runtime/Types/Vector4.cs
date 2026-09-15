using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Represents a two-dimensional vector.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4
    {

        /// <summary>
        /// Stores the x component of the vector.
        /// </summary>
        public float x; 

        /// <summary>
        /// Stores the y component of the vector.
        /// </summary>
        public float y;

        /// <summary>
        /// Stores the z component of the vector.
        /// </summary>
        public float z;

        /// <summary>
        /// Stores the w component of the vector.
        /// </summary>
        public float w;

        /// <summary>
        /// Converts the vector into a Unity Engine vector.
        /// </summary>
        /// <returns>An instance of the Unity engine vector.</returns>
        public UnityEngine.Vector4 ToUnity()
            => new UnityEngine.Vector4(x, y, z, w);

        /// <summary>
        /// Converts the vector into a Unity Engine quaternion.
        /// </summary>
        /// <returns>An instance of the Unity engine quaternion.</returns>
        public UnityEngine.Quaternion ToQuaternion()
            => new UnityEngine.Quaternion(-x, -y, z, w);

    }

}