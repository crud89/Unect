namespace Unect
{

    /// <summary>
    /// Represents the states of a sensor.
    /// </summary>
    public enum SensorState : int
    {

        /// <summary>
        /// The sensor is currently closed.
        /// </summary>
        Closed = 0,

        /// <summary>
        /// The sensor is currently opening.
        /// </summary>
        Opening,

        /// <summary>
        /// The sensor is available.
        /// </summary>
        Available,

        /// <summary>
        /// The sensor is not available.
        /// </summary>
        Unavailable

    }

}