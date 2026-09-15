using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Unect
{

    /// <summary>
    /// Represents a tracked body.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct Body
    {

        /// <summary>
        /// Stores the maximum number of joints.
        /// </summary>
        public const int JointCount = 25;

        /// <summary>
        /// Stores the maximum number of tracked bodies.
        /// </summary>
        public const int MaxBodies = 6;

        /// <summary>
        /// Stores the unique ID of the tracked body.
        /// </summary>
        public ulong id;

        /// <summary>
        /// Stores a non-zero value, if the body is tracked.
        /// </summary>
        public int tracked;

        /// <summary>
        /// Stores a mask that indicates the edges at which the body is clipped.
        /// </summary>
        public ClippedEdges clippedEdges;

        /// <summary>
        /// Stores the state of the left hand.
        /// </summary>
        public HandState handLeftState;

        /// <summary>
        /// Stores the state of the right hand.
        /// </summary>
        public HandState handRightState;

        /// <summary>
        /// Stores the pose confidence for the left hand.
        /// </summary>
        public Confidence handLeftConfidence;

        /// <summary>
        /// Stores the pose confidence for the right hand.
        /// </summary>
        public Confidence handRightConfidence;

        /// <summary>
        /// Stores the lean tracking state.
        /// </summary>
        public TrackingState leanTrackingState;

        /// <summary>
        /// Stores the lean direction of the body.
        /// </summary>
        public Vector2 lean;

        /// <summary>
        /// Stores a non-zero value, if the body is restricted.
        /// </summary>
        public int restricted;

        /// <summary>
        /// Stores the buffer that contains the individual joints.
        /// </summary>
        private fixed byte _joints[JointCount * 32]; // sizeof(Joint)

        /// <summary>
        /// Returns `true`, if the body is tracked and `false` otherwise.
        /// </summary>
        public bool IsTracked => tracked != 0;

        /// <summary>
        /// Returns `true`, if the body is restricted and `false` otherwise.
        /// </summary>
        public bool IsRestricted => restricted != 0;

        /// <summary>
        /// Returns the joint identified by <paramref name="type"/>.
        /// </summary>
        /// <param name="type">The type of the joint to obtain.</param>
        /// <returns>The joint identified by <paramref name="type"/>.</returns>
        /// <exception cref="ArgumentOutOfRangeException">Thrown, if the joint maps to an invalid joint index.</exception>
        public Joint GetJoint(JointType type)
        {
            var i = Convert.ToUInt32(type);

            if (i >= JointCount)
                throw new ArgumentOutOfRangeException(nameof(type));

            fixed (byte* p = _joints)
                return ((Joint*)p)[i];
        }

        /// <summary>
        /// Returns the joint identified by <paramref name="type"/>.
        /// </summary>
        /// <param name="type">The type of the joint to obtain.</param>
        /// <returns>The joint identified by <paramref name="type"/>.</returns>
        public Joint this[JointType type]
        {
            get => GetJoint(type);
        }

    }

}