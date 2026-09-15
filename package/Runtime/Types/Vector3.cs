using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Represents a two-dimensional vector.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
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
        /// Converts the vector into a Unity Engine vector.
        /// </summary>
        /// <returns>An instance of the Unity engine vector.</returns>
        public UnityEngine.Vector3 ToUnity() 
            => new UnityEngine.Vector3(x, y, z);

    }

}