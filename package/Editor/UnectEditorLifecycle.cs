using UnityEngine;
using UnityEditor;

namespace Unect.Editor
{

    /// <summary>
    /// Manages the native interop layer lifecycle in the editor.
    /// </summary>
    /// <remarks>
    /// The managed domain gets reset thoughout the editor lifetime, for example when entering or exiting play-mode. Meanwhile the native interop layer remains
    /// loaded and its static state is never released. Without handling, this leaves the sensor opened and streaming, while the reloaded domain would be unable
    /// to open it again, forcing a full editor restart onto the user. This class implements the logic to reset the native interop layer accordingly.
    /// </remarks>
    [InitializeOnLoad]
    internal static class UnectEditorLifecycle
    {

        /// <summary>
        /// The static constructor, executed on each domain reload.
        /// </summary>
        static UnectEditorLifecycle()
        {
            // Hook up assembly reload and application quit events to the runtime handlers.
            AssemblyReloadEvents.beforeAssemblyReload -= UnectRuntime.DetachWithoutClosing;
            AssemblyReloadEvents.beforeAssemblyReload += UnectRuntime.DetachWithoutClosing;

            EditorApplication.quitting -= UnectRuntime.OnApplicationShutdown;
            EditorApplication.quitting += UnectRuntime.OnApplicationShutdown;

            // Handle play mode transitions.
            EditorApplication.playModeStateChanged -= OnPlayModeStateChanged;
            EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
        }

        /// <summary>
        /// Handles editor play mode transition.
        /// </summary>
        /// <param name="change">The type of the transition.</param>
        private static void OnPlayModeStateChanged(PlayModeStateChange change)
        {
            if (change == PlayModeStateChange.ExitingPlayMode)
                UnectRuntime.DetachWithoutClosing();
        }

        /// <summary>
        /// Forces the native interop layer to shutdown the Kinect sensor. The next session acquisition will create a new generation.
        /// </summary>
        [MenuItem("Tools/Unect/Shutdown Kinect Sensor")]
        private static void ForceShutdown()
        {
            UnectRuntime.OnApplicationShutdown();
            Debug.Log("Kinect sensor closed.");
        }

    }

}
