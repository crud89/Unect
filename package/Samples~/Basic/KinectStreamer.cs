using Unect;
using Unect.Helper;
using UnityEngine;

/// <summary>
/// Implements a basic streamer that pulls color and depth images from the Kinect sensor through the Unect runtime.
/// </summary>
public class KinectStreamer : MonoBehaviour
{

    /// <summary>
    /// Stores the kinect session.
    /// </summary>
    private UnectSession session;

    /// <summary>
    /// Stores the depth texture.
    /// </summary>
    private KinectTexture depthTexture;

    /// <summary>
    /// Stores the color texture.
    /// </summary>
    private KinectTexture colorTexture;

    /// <summary>
    /// Returns the underlying session.
    /// </summary>
    public UnectSession Session
    {
        get => session;
    }

    /// <summary>
    /// Returns the underlying depth texture instance.
    /// </summary>
    public KinectTexture DepthTexture
    {
        get => depthTexture;
    }

    /// <summary>
    /// Returns the underlying color texture instance.
    /// </summary>
    public KinectTexture ColorTexture
    {
        get => colorTexture;
    }

    /// <summary>
    /// Executed when the script gets enabled.
    /// </summary>
    private void OnEnable()
    {
        // Acquire a new session. This script is responsible for managing its lifetime, i.e., calling dispose on it when it gets disabled.
        session = UnectSession.Acquire(StreamType.Depth | StreamType.Color, ColorFormat.Bgra32, flags: SessionFlags.KeepAlive);

        // Create the textures.
        depthTexture = new KinectTexture(session, StreamIndex.Depth);
        colorTexture = new KinectTexture(session, StreamIndex.Color, TextureFormat.BGRA32);
    }

    /// <summary>
    /// Executed when the script gets disabled.
    /// </summary>
    private void OnDisable()
    {
        depthTexture?.Dispose();
        colorTexture?.Dispose();
        session?.Dispose();

        session = null;
        depthTexture = null;
        colorTexture = null;
    }

    /// <summary>
    /// Updates the textures from the underlying streams.
    /// </summary>
    private void Update()
    {
        if (session is null || !session.IsValid)
            return;

        // Update the textures.
        depthTexture.Update();
        colorTexture.Update();
    }

}