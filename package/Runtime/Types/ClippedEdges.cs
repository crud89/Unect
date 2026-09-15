namespace Unect
{

    /// <summary>
    /// Masks the edges that are clipped.
    /// </summary>
    [System.Flags]
    public enum ClippedEdges : uint
    {

        /// <summary>
        /// No edge is clipped.
        /// </summary>
        None = 0,

        /// <summary>
        /// Indicates that the right edge is clipped.
        /// </summary>
        Right =  1 << 0,

        /// <summary>
        /// Indicates that the left edge is clipped.
        /// </summary>
        Left = 1 << 1,

        /// <summary>
        /// Indicates that the top edge is clipped.
        /// </summary>
        Top = 1 << 2,

        /// <summary>
        /// Indicates that the bottom edge is clipped.
        /// </summary>
        Bottom = 1 << 3

    }

}