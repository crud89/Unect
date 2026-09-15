using System;

namespace Unect
{

    /// <summary>
    /// Wraps a result from the native interop layer in an exception.
    /// </summary>
    public sealed class UnectException : Exception
    {

        /// <summary>
        /// The result that caused the exception.
        /// </summary>
        public UnectResult Result { get; }

        /// <summary>
        /// Initializes a new exception instance.
        /// </summary>
        /// <param name="result">The return code that caused the exception.</param>
        public UnectException(UnectResult result)
            : base($"Unect operation failed: {result.AsString()} ({(int)result}).")
        {
            Result = result;
        }

    }

}