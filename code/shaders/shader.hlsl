struct VSOutput
{
    float4 m_Position : SV_Position;
    float3 m_Color : COLOR;
};

/////////////// Vertex Shader ///////////////////
static const float2 g_Pos[] =
{
    float2(-0.6, -0.4),
    float2(0.6, -0.4),
    float2(0.0, 0.6),
};

static const float3 g_Col[] =
{
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0),
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput result;

    result.m_Position = float4(g_Pos[vertexID], 0.0, 1.0);
    result.m_Color = g_Col[vertexID];

    return result;
}

/////////////// Pixel Shader ///////////////////
#define PI 3.14159265359

cbuffer Uniforms : register(b0)
{
    float u_Time; // Time elapsed since startup
    float2 u_Resolution; // Canvas size (width, height)
    float2 u_Mouse; // Mouse coordinates (x, y)
};

float4 PSMain(VSOutput input) : SV_Target
{
    return float4(input.m_Color, 1.0);
}
