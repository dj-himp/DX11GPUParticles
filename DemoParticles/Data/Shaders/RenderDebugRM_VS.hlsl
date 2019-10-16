#include "Globals.h"

/*cbuffer FrustrumCornersConstantBuffer : register(b0)
{
    float4 FrustumCorners[4];
};*/

struct VertexShaderInput
{
    float4 Position : POSITION;
    float2 uv : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 rayDir : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float4 FSQ_GetFrustumRay(in float2 texCoord)
{
    float index = texCoord.x + (texCoord.y * 2);
    return FrustumCorners[index];
}

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = float4(input.Position.xyz, 1.0);

    output.rayDir = FSQ_GetFrustumRay(input.uv);
    output.uv = input.uv;


    return output;
}