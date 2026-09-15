namespace Unect
{

    /// <summary>
    /// Represents supported color formats.
    /// </summary>
    public enum ColorFormat : int
    {

        /// <summary>
        /// Outputs colors in BGRA order with 32 bit depth.
        /// </summary>
        Bgra32 = 0,

        /// <summary>
        /// Outputs colors in RGBA order with 32 bit color depth.
        /// </summary>
        Rgba32 = 1,

        /// <summary>
        /// A native color format without conversion cost.
        /// </summary>
        Yuv2 = 2

    }

}