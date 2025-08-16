cbuffer VertexUniforms : register(b0)
{
    float4x4 u_MVP;
    bool u_IsWireframe;
}

struct VSOutput
{
    float4 m_Position : SV_Position;
    float3 m_Color : COLOR;
};

/////////////// Vertex Shader ///////////////////
static const float3 g_Pos[] =
{
    float3(-1.0, -1.0, 1.0),
    float3(1.0, -1.0, 1.0),
    float3(1.0, 1.0, 1.0),
    float3(-1.0, 1.0, 1.0),
    float3(-1.0, -1.0, -1.0),
    float3(1.0, -1.0, -1.0),
    float3(1.0, 1.0, -1.0),
    float3(-1.0, 1.0, -1.0),
};

static const float3 g_Col[] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(1.0, 1.0, 0.0),
    float3(1.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
    float3(0.0, 1.0, 0.0),
    float3(1.0, 0.0, 0.0),
};

static const int g_Indices[36] =
{
    // front
    0, 1, 2, 2, 3, 0,
    // right
    1, 5, 6, 6, 2, 1,
    // back
    7, 6, 5, 5, 4, 7,
    // left
    4, 0, 3, 3, 7, 4,
    // bottom
    4, 5, 1, 1, 0, 4,
    // top
    3, 2, 6, 6, 7, 3,
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput result;

    int idx = g_Indices[vertexID];

    result.m_Position = mul(u_MVP, float4(g_Pos[idx], 1.0));
    result.m_Color = u_IsWireframe ? float3(0.0, 0.0, 0.0) : g_Col[idx];

    return result;
}

/////////////// Pixel Shader ///////////////////
#define PI 3.14159265359

cbuffer PixelUniforms : register(b1)
{
    float u_Time; // Time elapsed since startup
    float2 u_Resolution; // Canvas size (width, height)
    float2 u_Mouse; // Mouse coordinates (x, y)
};

float4 PSMain(VSOutput input) : SV_Target
{
    return float4(input.m_Color, 1.0);
}
