using System.Runtime.InteropServices;

namespace Unect
{

    /// <summary>
    /// Stores the data for an event.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Event
    {

        /// <summary>
        /// Stores the type of the event.
        /// </summary>
        public EventType type;

        /// <summary>
        /// Stores the lower parameter for the event.
        /// </summary>
        public int param1;

        /// <summary>
        /// Stores the upper parameter for the event.
        /// </summary>
        public ulong param2;

        /// <summary>
        /// Stores the timestamp of the event in milliseconds.
        /// </summary>
        public ulong timestamp;

    }

}