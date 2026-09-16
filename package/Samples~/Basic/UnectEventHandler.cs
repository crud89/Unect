using Unect;
using UnityEngine;

/// <summary>
/// Handles events from the interop layer by logging them to the console.
/// </summary>
/// <remarks>
/// The event system pushes events into a concurrent event queue, which can then be polled from Unity. It is not necessary to do this, but
/// if you want to do it for diagnostics reason, keep in mind that there should always only ever be one consumer, as polling removes the 
/// events from the queue.
/// 
/// This implementation merely logs the events onto the console. A more sensible approach would dispatch the events instead through Unity's
/// event system. This can be used to invalidate coordinate mappings or manage tracked bodies through enter/exit events.
/// </remarks>
[RequireComponent(typeof(KinectStreamer))]
public class UnectEventHandler : MonoBehaviour
{

    /// <summary>
    /// Stores the events polled during <see cref="Update" />.
    /// </summary>
    private readonly Unect.Event[] events = new Unect.Event[32];

    /// <summary>
    /// Polls the events from the session.
    /// </summary>
    private void Update()
    {
        // Get the session from the streamer. If you intend to have multiple consumers handle different streams, storing the session in a
        // central component instead is a sensible choice.
        var session = this.GetComponent<KinectStreamer>()?.Session;

        if (session is null)
            return;

        // Check if there's any sensor state events (disconnects can happen).
        // NOTE: By contract there should be only a single event consumer, so if you intend to share a session over multiple components, make
        //       sure to handle event polling in a central location instead. Having no event consumer is an option as well.
        var eventCount = session.PollEvents(events, out var dropped);

        // Dropped events indicate a spinning streamer, which can prevent frames from being pushed through the pipeline. If this happens,
        // it might hint on an underlying library issue or hardware defect. Please open an issue in the Unect repository if you ever happen
        // to encounter such a situation!
        if (dropped > 0)
            Debug.LogWarning($"{dropped} events have been dropped.");

        for (int i = 0; i < eventCount; i++)
        {
            switch (events[i].type)
            {
                case Unect.EventType.SensorState:
                    Debug.Log($"Kinect sensor state changed: {(SensorState)events[i].param1}");
                    break;
                case Unect.EventType.MappingChanged:
                    Debug.Log("Kinect coordinate mapping changed.");
                    break;
                case Unect.EventType.BodyEntered:
                    Debug.Log("Kinect body entered.");
                    break;
                case Unect.EventType.BodyLeft:
                    Debug.Log("Kinect body left.");
                    break;
                case Unect.EventType.StreamError:
                    Debug.LogError("An error occurred while streaming from a Kinect sensor.");
                    break;
                default:
                    break;
            }
        }
    }

}
