using Unect;
using Unect.Editor;
using UnityEditor;
using UnityEngine;

/// <summary>
/// Provides a custom editor for the <see cref="KinectStreamer" />.
/// </summary>
[CustomEditor(typeof(KinectStreamer))]
public class KinectStreamerEditor : UnityEditor.Editor
{

    /// <summary>
    /// Returns the sensor state label for a <see cref="UnectSession" />.
    /// </summary>
    /// <param name="session">The session for which to obtain the sensor state label.</param>
    /// <returns>A string that prints the sensor state.</returns>
    private static string GetSensorLabel(UnectSession session) 
        => session is null || !session.IsValid ? "No session" : session.SensorState.ToString();

    /// <inheritdoc />
    public override bool RequiresConstantRepaint() 
        => Application.isPlaying;

    /// <inheritdoc />
    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();

        if (!Application.isPlaying)
        {
            EditorGUILayout.HelpBox("Streams are only visible in play mode.", MessageType.Info);
            return;
        }
        else
        {
            var self = (KinectStreamer)target;

            EditorGUILayout.Space();
            EditorGUILayout.LabelField("Sensor", GetSensorLabel(self.Session), EditorStyles.boldLabel);

            KinectTextureUIHelper.Draw("Depth", self.DepthTexture);
            KinectTextureUIHelper.Draw("Color", self.ColorTexture);
        }
    }

}