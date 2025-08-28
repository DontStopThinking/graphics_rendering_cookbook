cbuffer VertexUniforms : register(b0)
{
    float4x4 u_MVP;
    bool u_IsWireframe;
}

struct VSOutput
{
    float4 m_Position : SV_Position;
    float3 m_Color : COLOR;
    float2 m_UV : TEXCOORD0;
};

/////////////// Vertex Shader ///////////////////

float3 PositionToRGB(float3 pos)
{
    return (pos + 1.0) * 0.5;
}

VSOutput VSMain(float3 vertex : POSITION)
{
    VSOutput result;

    result.m_Position = mul(u_MVP, float4(vertex, 1.0f));
    result.m_Color = u_IsWireframe ? float3(0.0, 0.0, 0.0) : PositionToRGB(vertex);
    result.m_UV = vertex.xz * 5.0f;

    return result;
}

/////////////// Pixel Shader ///////////////////
#define PI 3.14159265359

cbuffer PixelUniforms : register(b1)
{
    float u_Time; // Time elapsed since startup
    float2 u_Resolution; // Canvas size (width, height)
    float2 u_Mouse; // Mouse coordinates (x, y)
}

Texture2D g_Tex : register(t0);
sampler g_Sampler : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    return g_Tex.Sample(g_Sampler, input.m_UV) * float4(input.m_Color, 1.0);
}
