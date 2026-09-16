using System;
using Unect;
using Unect.Helper;
using UnityEditor;
using UnityEngine;

namespace Unect.Editor
{

    /// <summary>
    /// Provides a convenient way to display the contents of a <see cref="KinectTexture" /> in the editor.
    /// </summary>
    public static class KinectTextureUIHelper
    {

        /// <summary>
        /// Retrieves the material for a shader and creates it, if <paramref name="material" /> is not yet initialized.
        /// </summary>
        /// <param name="material">The material reference that should be retrieved or created.</param>
        /// <param name="shaderName">The name of the shader to initialize the material with.</param>
        /// <param name="patch">An optional callback to patch a recently created material with.</param>
        /// <returns>The material reference.</returns>
        private static Material GetMaterial(ref Material material, string shaderName, Action<Material> patch = null)
        {
            if (material is null)
            {
                var shader = Shader.Find(shaderName);

                if (shader is not null)
                {
                    material = new Material(shader) { hideFlags = HideFlags.HideAndDontSave };

                    if (patch is not null)
                        patch(material);
                }
            }

            return material;
        }

        /// <summary>
        /// Stores the material used for drawing the depth texture.
        /// </summary>
        private static Material depthMaterial;

        /// <summary>
        /// Retrieves the material used for drawing the depth texture.
        /// </summary>
        private static Material DepthMaterial => GetMaterial(ref depthMaterial, "Unect/VisualizeDepth");

        /// <summary>
        /// Stores the material used for drawing the infrared texture.
        /// </summary>
        private static Material infraredMaterial;

        /// <summary>
        /// Retrieves the material used for drawing the infrared texture.
        /// </summary>
        private static Material InfraredMaterial => GetMaterial(ref infraredMaterial, "Unect/VisualizeIR", m => m.SetFloat("_Gain", 8.0f));

        /// <summary>
        /// Stores the material used for drawing the long exposure infrared texture.
        /// </summary>
        private static Material longInfraredMaterial;

        /// <summary>
        /// Retrieves the material used for drawing the long exposure infrared texture.
        /// </summary>
        private static Material LongExposureIRMaterial => GetMaterial(ref longInfraredMaterial, "Unect/VisualizeIR", m => m.SetFloat("_Gain", 4.0f));

        /// <summary>
        /// Stores the material used for drawing the body index texture.
        /// </summary>
        private static Material bodyIndexMaterial;

        /// <summary>
        /// Retrieves the material used for drawing the body index texture.
        /// </summary>
        private static Material BodyIndexMaterial => GetMaterial(ref bodyIndexMaterial, "Unect/VisualizeBodyIndex");

        /// <summary>
        /// Retrieves the material used for drawing a texture.
        /// </summary>
        /// <param name="texture">The texture that should be drawn.</param>
        /// <returns>The material used for drawing the texture.</returns>
        private static Material MaterialFor(KinectTexture texture) => texture.Stream switch
        {
            StreamIndex.Depth => DepthMaterial,
            StreamIndex.Infrared => InfraredMaterial,
            StreamIndex.LongExposureIR => LongExposureIRMaterial,
            StreamIndex.BodyIndex => BodyIndexMaterial,
            _ => null
        };

        /// <summary>
        /// Draws a <see cref="KinectTexture" /> into the inspector.
        /// </summary>
        /// <param name="label">A custom label for the texture.</param>
        /// <param name="texture">The texture to draw.</param>
        /// <param name="padding">The horizontal padding.</param>
        public static void Draw(string label, KinectTexture texture, float padding = 40.0f)
        {
            var tex = texture?.Texture;

            EditorGUILayout.LabelField(label, tex == null ? "uninitialized" : $"{tex.width}x{tex.height} - {tex.format} (generation: {texture.LastGeneration})");

            if (tex is null)
                return;

            var width = EditorGUIUtility.currentViewWidth - padding;
            var rect = GUILayoutUtility.GetRect(width, width * tex.height / tex.width, GUILayout.ExpandWidth(false));
            var mat = MaterialFor(texture);

            EditorGUI.DrawPreviewTexture(rect, tex, mat, ScaleMode.ScaleToFit);
        }

    }

}