Shader "Unect/VisualizeIR"
{
    Properties
    {
        _MainTex ("Infrared", 2D) = "black" {}
        _Gain ("Gain", Range(1, 64)) = 8
        _Gamma ("Gamma", Range(0.2, 1.0)) = 0.32
        _Floor ("Black level", Range(0, 0.05)) = 0.0
    }

    SubShader
    {
        Tags { "RenderType"="Opaque" "PreviewType"="Plane" }
        Cull Off
        ZWrite Off
        Lighting Off
        Blend SrcAlpha OneMinusSrcAlpha

        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _MainTex;
            float _Gain;
            float _Gamma;
            float _Floor;

            fixed4 frag(v2f_img i) : SV_Target
            {
                float raw = tex2D(_MainTex, i.uv).r;
                float v = pow(saturate(max(raw - _Floor, 0.0) * _Gain), _Gamma);

                return fixed4(v, v, v, 1);
            }
            ENDCG
        }
    }

    Fallback Off
}