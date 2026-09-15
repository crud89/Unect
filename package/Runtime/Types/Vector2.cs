using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Represents a two-dimensional vector.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
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
        /// Converts the vector into a Unity Engine vector.
        /// </summary>
        /// <returns>An instance of the Unity engine vector.</returns>
        public UnityEngine.Vector2 ToUnity()
            => new UnityEngine.Vector2(x, y);

    }

}