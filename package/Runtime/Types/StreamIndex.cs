namespace Unect
{

    /// <summary>
    /// Maps indices to individual streams.
    /// </summary>
    public enum StreamIndex : int
    {
        /// <summary>
        /// Corresponds to the index of the depth image stream.
        /// </summary>
        Depth = 0,

        /// <summary>
        /// Corresponds to the index of the color image stream.
        /// </summary>
        Color,

        /// <summary>
        /// Corresponds to the index of the IR image stream.
        /// </summary>
        Infrared,

        /// <summary>
        /// Corresponds to the index of the long exposure IR image stream.
        /// </summary>
        LongExposureIR,

        /// <summary>
        /// Corresponds to the index of the body index image stream.
        /// </summary>
        BodyIndex,

        /// <summary>
        /// Corresponds to the index of the body stream.
        /// </summary>
        Body,

        /// <summary>
        /// Stores the number of available streams.
        /// </summary>
        Count

    }

}