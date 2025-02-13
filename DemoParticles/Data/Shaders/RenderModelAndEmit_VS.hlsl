#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

cbuffer SkinnedConstantBuffer : register(b2)
{
    float4x4 boneTransforms[96];
};

struct VertexShaderInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texCoord : TEXCOORD;
    float4 blendWeight : BLENDWEIGHT;
    uint4 boneIndices : BLENDINDICES;
};

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal :TEXCOORD1;
    float4 worldPos: TEXCOORD2;
};

// Simple shader to do vertex processing on the GPU.
GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    float4x4 skinTransform = boneTransforms[input.boneIndices[0]] * input.blendWeight[0];
    skinTransform += boneTransforms[input.boneIndices[1]] * input.blendWeight[1];
    skinTransform += boneTransforms[input.boneIndices[2]] * input.blendWeight[2];
    skinTransform += boneTransforms[input.boneIndices[3]] * input.blendWeight[3];

    output.Position = mul(float4(input.Position, 1.0), skinTransform);
    //output.Position = float4(input.Position, 1.0);    
    
    //output.Position = float4(2.0 * input.texCoord.x - 1.0, 2.0 * input.texCoord.y - 1.0, 0.0, 1.0);
    //output.Position = mul(float4(input.Position, 1.0), mul(world, viewProj));

    //output.normal = mul(float4(input.normal, 1.0), world);
    output.normal = mul(float4(input.normal, 0.0), skinTransform).xyz;
    output.normal = mul(float4(output.normal, 0.0), world).xyz;

    output.uv = input.texCoord;
    //output.worldPos = mul(float4(input.Position, 1.0), world);
    output.worldPos = mul(output.Position, world);
    output.color = input.Color;

    return output;
}
