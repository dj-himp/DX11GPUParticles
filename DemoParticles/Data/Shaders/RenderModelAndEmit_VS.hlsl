#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

struct VertexShaderInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texCoord : TEXCOORD;
};

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal :TEXCOORD1;
    float4 worldPos: TEXCOORD2;
};

// Simple shader to do vertex processing on the GPU.
GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;

    //output.Position = float4(input.Position, 1.0);    
    output.Position = float4(2.0 * input.texCoord.x - 1.0, 2.0 * input.texCoord.y - 1.0, 0.0, 1.0);
    //output.Position = mul(float4(input.Position, 1.0), mul(world, viewProj));
    output.normal = input.normal;
    output.normal = mul(float4(input.normal, 1.0), world);
    output.uv = input.texCoord;
    output.worldPos = mul(float4(input.Position, 1.0), world);

    return output;
}
