Shader "Unect/VisualizeBodyIndex"
{
    Properties
    {
        _MainTex ("BodyIndex", 2D) = "black" {}
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        Pass
        {
            CGPROGRAM
            #pragma vertex vert_img
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _MainTex;

            static const fixed3 BODY_COLORS[6] =
            {
                fixed3(0.90, 0.20, 0.20),
                fixed3(0.20, 0.55, 0.95),
                fixed3(0.95, 0.75, 0.15),
                fixed3(0.25, 0.80, 0.35),
                fixed3(0.75, 0.35, 0.90),
                fixed3(0.20, 0.85, 0.85)
            };

            fixed4 frag(v2f_img i) : SV_Target
            {
                float raw = tex2D(_MainTex, i.uv).r;
                int idx = (int)round(raw * 255.0);

                if (idx > 5) 
                    return float4(0, 0, 0, 1);

                return fixed4(BODY_COLORS[idx], 1);
            }
            ENDCG
        }
    }
}