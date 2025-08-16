/////////////// Vertex Shader ///////////////////
float4 VSMain(uint vertexID : SV_VertexID) : SV_Position
{
    float4 verts[] =
    {
        float4(-1.0, -1.0, 0.0, 1.0),
        float4(1.0, -1.0, 0.0, 1.0),
        float4(-1.0, 1.0, 0.0, 1.0),
        float4(1.0, 1.0, 0.0, 1.0),
    };
    return verts[vertexID];
}

/////////////// Pixel Shader ///////////////////
#define PI 3.14159265359

cbuffer Uniforms : register(b0)
{
    float uTime; // Time elapsed since startup
    float2 uResolution; // Canvas size (width, height)
    float2 uMouse; // Mouse coordinates (x, y)
};

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
    float4 result;

    // Lovely gradient
    float2 uv = pos.xy / uResolution.xy;
    result = float4(uv, 0.5 + 0.5 * sin(uTime), 1.0);

    return result;
}
