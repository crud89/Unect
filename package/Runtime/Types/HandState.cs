namespace Unect
{
    
    /// <summary>
    /// Stores the state of a hand.
    /// </summary>
    public enum HandState : int
    {

        /// <summary>
        /// The hand state is not known.
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// The hand is currently not tracked.
        /// </summary>
        NotTracked,

        /// <summary>
        /// The hand is opened.
        /// </summary>
        Open, 

        /// <summary>
        /// The hand is closed.
        /// </summary>
        Closed,

        /// <summary>
        /// The index and middle finger point upward.
        /// </summary>
        Lasso

    }

}