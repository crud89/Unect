namespace Unect
{

    /// <summary>
    /// Represents a type of a data stream.
    /// </summary>
    [System.Flags]
    public enum StreamType : uint
    {

        /// <summary>
        /// Corresponds to no data stream.
        /// </summary>
        None = 0,

        /// <summary>
        /// Corresponds to the depth image stream.
        /// </summary>
        Depth = 1 << 0,

        /// <summary>
        /// Corresponds to the color image stream.
        /// </summary>
        Color = 1 << 1,

        /// <summary>
        /// Corresponds to the IR image stream.
        /// </summary>
        Infrared = 1 << 2,

        /// <summary>
        /// Corresponds to the long exposure IR image stream.
        /// </summary>
        LongExposureIR = 1 << 3,

        /// <summary>
        /// Corresponds to the body index image stream.
        /// </summary>
        BodyIndex = 1 << 4,

        /// <summary>
        /// Corresponds to the body stream.
        /// </summary>
        Body = 1 << 5,

        /// <summary>
        /// Corresponds to all streams.
        /// </summary>
        All = 0x3F

    }

}