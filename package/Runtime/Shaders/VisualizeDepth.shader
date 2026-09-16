Shader "Unect/VisualizeDepth"
{
    Properties
    {
        _MainTex ("Depth", 2D) = "black" {}
        _MinDepth ("Min mm", Float) = 500
        _MaxDepth ("Max mm", Float) = 4500
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" "PreviewType"="Plane" }
        Cull Off
        ZWrite Off
        Lighting Off
        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _MainTex;
            float _MinDepth, _MaxDepth;

            fixed4 frag(v2f_img i) : SV_Target
            {
                float depth = tex2D(_MainTex, i.uv).r * 65535.0;

                if (depth <= 0) 
                    return fixed4(0, 0, 0, 1);

                float t = saturate((depth - _MinDepth) / (_MaxDepth - _MinDepth));
                return fixed4(1 - t, 1 - t, 1 - t, 1);
            }
            ENDCG
        }
    }
}