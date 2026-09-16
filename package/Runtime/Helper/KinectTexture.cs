using System;
using UnityEngine;

namespace Unect.Helper
{

    /// <summary>
    /// A helper that stores a texture from a Kinect stream.
    /// </summary>
    [Serializable]
    public class KinectTexture : IDisposable
    {

        /// <summary>
        /// Stores the texture.
        /// </summary>
        private Texture2D texture;

        /// <summary>
        /// Stores the underlying session.
        /// </summary>
        private UnectSession session;

        /// <summary>
        /// Stores the last generation of the stream.
        /// </summary>
        private ulong lastGeneration = 0;

        /// <summary>
        /// Stores the relative timestamp of the previous texture fetch.
        /// </summary>
        private long lastTimestamp = 0;

        /// <summary>
        /// Stores the relative timestamp of the current texture fetch.
        /// </summary>
        private long timestamp = 0;

        /// <summary>
        /// Stores the underlying stream from which the texture is obtained.
        /// </summary>
        private StreamIndex stream;

        /// <summary>
        /// Returns the texture.
        /// </summary>
        public Texture2D Texture
        {
            get => texture;
        }

        /// <summary>
        /// Returns the underlying session.
        /// </summary>
        public UnectSession Session
        {
            get => session;
        }

        /// <summary>
        /// Returns the last generation of the underlying stream.
        /// </summary>
        public ulong LastGeneration
        {
            get => lastGeneration;
        }

        /// <summary>
        /// Returns the relative timestamp of the current texture fetch.
        /// </summary>
        public long Timestamp
        {
            get => timestamp;
        }

        /// <summary>
        /// Returns the latency of the last texture fetch.
        /// </summary>
        public long Latency
        {
            get => timestamp - lastTimestamp;
        }

        /// <summary>
        /// Returns the underlying stream.
        /// </summary>
        public StreamIndex Stream
        {
            get => stream;
        }

        /// <summary>
        /// Creates a new texture instance.
        /// </summary>
        /// <param name="session">The session from which the texture is fetched.</param>
        /// <param name="stream">The stream from which the texture is obtained.</param>
        /// <param name="colorFormat">The color format of the texture (ignored if <paramref name="stream" /> is <see cref="StreamIndex.Depth" />).</param>
        /// <exception cref="ArgumentOutOfRangeException">Thrown if the <paramref name="stream" /> is out of range.</exception>
        /// <exception cref="ArgumentException">Thrown if <paramref name="stream" /> equals <see cref="StreamIndex.Body" />.</exception>
        public KinectTexture(UnectSession session, StreamIndex stream, TextureFormat colorFormat = TextureFormat.BGRA32)
        {
            if (stream >= StreamIndex.Count)
                throw new ArgumentOutOfRangeException(nameof(stream));

            if (stream == StreamIndex.Body)
                throw new ArgumentException("A texture cannot be associated with a body stream.", nameof(stream));

            // Get the stream info and create the texture.
            var info = session.GetStreamInfo(stream);
            texture = new Texture2D(info.width, info.height, stream == StreamIndex.Depth ? TextureFormat.R16 : colorFormat, false);

            // Store the session.
            this.session = session;
            this.stream = stream;
        }

        /// <summary>
        /// Disposes the texture.
        /// </summary>
        public void Dispose()
        {
            if (texture is not null)
            {
                if (Application.isPlaying) 
                    UnityEngine.Object.Destroy(texture);
                else
                    UnityEngine.Object.DestroyImmediate(texture);
            }

            session = null;
            texture = null;
        }

        /// <summary>
        /// Updates the texture by fetching it from the underlying stream.
        /// </summary>
        /// <exception cref="ObjectDisposedException">Thrown if the underlying session was disposed.</exception>
        public void Update()
        {
            if (session is null)
                throw new ObjectDisposedException("The underlying Kinect session has been disposed.");

            // Check if there's an update on the stream first.
            ImageView image;

            if (session.PeekGeneration(stream) == lastGeneration)
                return;
            else if (!session.TryLockImage(stream, out image))
                return;

            try
            {
                texture.LoadRawTextureData(image.data, image.size);
                texture.Apply(false);
                lastGeneration = image.generation;
                lastTimestamp = timestamp;
                timestamp = image.timestamp;
            }
            finally
            {
                session.UnlockImage(stream);
            }
        }

    }

}