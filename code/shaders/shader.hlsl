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
static const float3 g_Pos[] =
{
    float3(-1.0, -1.0, -1.0),
    float3(1.0, -1.0, -1.0),
    float3(1.0,  1.0, -1.0),
    float3(-1.0,  1.0, -1.0),

    float3(-1.0, -1.0,  1.0),
    float3(1.0, -1.0,  1.0),
    float3(1.0,  1.0,  1.0),
    float3(-1.0,  1.0,  1.0),

    float3(-1.0, -1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0, -1.0,  1.0),

    float3(1.0, -1.0, -1.0),
    float3(1.0,  1.0, -1.0),
    float3(1.0,  1.0,  1.0),
    float3(1.0, -1.0,  1.0),

    float3(-1.0, -1.0, -1.0),
    float3(-1.0, -1.0,  1.0),
    float3(1.0, -1.0,  1.0),
    float3(1.0, -1.0, -1.0),

    float3(-1.0,  1.0, -1.0),
    float3(-1.0,  1.0,  1.0),
    float3(1.0,  1.0,  1.0),
    float3(1.0,  1.0, -1.0),
};

static const int g_Indices[] =
{
    0, 1, 2, 0, 2, 3,
    6, 5, 4, 7, 6, 4,
    8, 9, 10, 8, 10, 11,
    14, 13, 12, 15, 14, 12,
    16, 17, 18, 16, 18, 19,
    22, 21, 20, 23, 22, 20,
};

static const float2 g_UV[] =
{
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
    float2(0.0, 0.0), float2(1.0, 0.0), float2(1.0, 1.0), float2(0.0, 1.0),
};

float3 PositionToRGB(float3 pos)
{
    return (pos + 1.0) * 0.5;
}

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput result;

    int idx = g_Indices[vertexID];

    result.m_Position = mul(u_MVP, float4(g_Pos[idx], 1.0));
    result.m_Color = u_IsWireframe ? float3(0.0, 0.0, 0.0) : PositionToRGB(g_Pos[idx]);
    result.m_UV = g_UV[idx % 24u] * 5.0;

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
