namespace Unect
{

    /// <summary>
    /// Stores the tracking state of a joint.
    /// </summary>
    public enum TrackingState : int
    {

        /// <summary>
        /// The joint is currently not tracked.
        /// </summary>
        NotTracked = 0,

        /// <summary>
        /// The joint pose is inferred.
        /// </summary>
        Inferred,

        /// <summary>
        /// The joint pose is tracked.
        /// </summary>
        Tracked

    }

}