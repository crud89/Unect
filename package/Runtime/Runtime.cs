using System;
using System.Collections.Generic;
using UnityEngine;

namespace Unect
{

    /// <summary>
    /// A container for session handles that handle domain reload and application lifecycle.
    /// </summary>
    public static class UnectRuntime
    {
        /// <summary>
        /// Tracks the runtime initialization state.
        /// </summary>
        private static bool _initialized = false;

        /// <summary>
        /// Stores a list of all sessions created from the runtime.
        /// </summary>
        private static readonly List<UnectSession> _sessions = new List<UnectSession>();

        /// <summary>
        /// Resets the static state on a domain reload.
        /// </summary>
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStaticState()
        {
            _initialized = false;

            foreach (var session in _sessions)
                session.Detach();

            _sessions.Clear();

            Application.quitting -= OnApplicationShutdown;
            Application.quitting += OnApplicationShutdown;
        }

        /// <summary>
        /// Handles the application shutdown to reset the static state and release the native interop layer.
        /// </summary>
        internal static void OnApplicationShutdown()
        {
            DetachWithoutClosing();

            try
            {
                // Shutdown the native interop layer.
                UnectInterop.Unect_Shutdown(); 
            }
            catch (DllNotFoundException)
            {
                Debug.Log("The Unect native interop library was not found.");
            }
        }

        /// <summary>
        /// Loads the native interop layer and validates ABI version compatibility.
        /// </summary>
        /// <exception cref="DllNotFoundException">Thrown if the native interop layer library could not be found or is incompatible.</exception>
        /// <exception cref="EntryPointNotFoundException">Thrown if the ABI version could not be obtained from the native interop layer.</exception>
        /// <exception cref="InvalidOperationException">Thrown if an ABI version mismatch was detected between the plugin and the loaded native interop layer.</exception>
        internal static void EnsureInitialized()
        {
            if (_initialized) 
                return;

            int abiVersion;

            try
            {
                abiVersion = UnectInterop.Unect_GetAbiVersion();
            }
            catch (DllNotFoundException e)
            {
                throw new DllNotFoundException("Unable to find a compatible version for 'libunect.dll'.", e);
            }
            catch (EntryPointNotFoundException e)
            {
                throw new EntryPointNotFoundException("The detected 'libunect.dll' file does not provide the expected entry point to obtain the ABI version.", e);
            }

            if (abiVersion != BinaryInterface.Version)
                throw new InvalidOperationException($"Mismatch between expected ABI {BinaryInterface.Version} and detected ABI {abiVersion} detected.");

            _initialized = true;
        }

        /// <summary>
        /// Starts to track a session.
        /// </summary>
        /// <param name="session">The session to track.</param>
        internal static void StartTracking(UnectSession session)
        {
            if (!_sessions.Contains(session))
                _sessions.Add(session);
        }

        /// <summary>
        /// Stops tracking a session.
        /// </summary>
        /// <param name="session">The session that should no longer be tracked.</param>
        internal static bool StopTracking(UnectSession session) 
            => _sessions.Remove(session);

        /// <summary>
        /// Detaches all sessions without releasing them.
        /// </summary>
        internal static void DetachWithoutClosing()
        {
            foreach (var session in _sessions) 
                session.Detach();

            _sessions.Clear();
            _initialized = false;
        }

    }

}