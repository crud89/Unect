namespace Unect
{

    /// <summary>
    /// Identifies the type of an event in the interop layer.
    /// </summary>
    public enum EventType : uint
    {

        /// <summary>
        /// An invalid or unknown event.
        /// </summary>
        None = 0,

        /// <summary>
        /// Indicates that the sensor state has changed.
        /// </summary>
        SensorState = 1,

        /// <summary>
        /// Indicates that a body has entered the tracking area.
        /// </summary>
        BodyEntered = 2,

        /// <summary>
        /// Indicates that a body has left the tracking area.
        /// </summary>
        BodyLeft = 3,

        /// <summary>
        /// Indicates that the coordinate mapper has been updated.
        /// </summary>
        MappingChanged = 4,

        /// <summary>
        /// Indicates that a streaming error occured.
        /// </summary>
        StreamError = 5

    }

}