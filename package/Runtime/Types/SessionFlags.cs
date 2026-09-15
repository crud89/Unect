namespace Unect
{

    /// <summary>
    /// Represents flags that control the session behavior.
    /// </summary>
    [System.Flags]
    public enum SessionFlags : uint
    {

        /// <summary>
        /// Corresponds to a default session behavior.
        /// </summary>
        None = 0,

        /// <summary>
        /// Keeps the session alive, even after all references are released through <see cref="UnectReleaseSession" />.
        /// </summary>
        KeepAlive = 1 << 0

    }

}