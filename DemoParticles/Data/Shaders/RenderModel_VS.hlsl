#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

cbuffer SkinnedConstantBuffer : register(b2)
{
    float4x4 boneTransforms[96];
};

// Per-vertex data used as input to the vertex shader.
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

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 UVs : TEXCOORD0;
    float3 normal :TEXCOORD1;
    //float4 blendWeight : TEXCOORD2;
    //nointerpolation uint4 boneIndices : TEXCOORD3;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    float4x4 skinTransform = boneTransforms[input.boneIndices[0]] * input.blendWeight[0];
    skinTransform += boneTransforms[input.boneIndices[1]] * input.blendWeight[1];
    skinTransform += boneTransforms[input.boneIndices[2]] * input.blendWeight[2];
    skinTransform += boneTransforms[input.boneIndices[3]] * input.blendWeight[3];
    output.Position = mul(float4(input.Position, 1.0), skinTransform);
    output.Position = mul(output.Position, mul(world, viewProj));
    

    //output.Position = mul(float4(input.Position, 1.0), mul(world, viewProj));

    /*float weight0 = input.blendWeight[0];
	float weight1 = 1.0f - weight0;

	float4 p     = weight0 * mul(float4(input.Position, 1.0f), boneTransforms[input.boneIndices[0]]);
	p += weight1 * mul(float4(input.Position, 1.0f), boneTransforms[input.boneIndices[1]]);
	p.w = 1.0f;
    output.Position = mul(p, mul(world, viewProj));
    */

    //output.Position = mul(float4(input.Position, 1.0), boneTransforms[input.boneIndices[0]]) * input.blendWeight[0];
    //output.Position = mul(output.Position, mul(world, viewProj));

    /*float4 p = mul(float4(input.Position, 1.0), boneTransforms[input.boneIndices[0]]) * input.blendWeight[0];
    p += mul(float4(input.Position, 1.0), boneTransforms[input.boneIndices[1]]) * input.blendWeight[1];
    p += mul(float4(input.Position, 1.0), boneTransforms[input.boneIndices[2]]) * input.blendWeight[2];
    p += mul(float4(input.Position, 1.0), boneTransforms[input.boneIndices[3]]) * input.blendWeight[3];
    p.w = 1.0;
    output.Position = mul(p, mul(world, viewProj));
*/
    
    
    //output.Position = mul(float4(input.Position, 1.0), mul(world, viewProj));
    
    output.normal = mul(float4(input.normal, 1.0), skinTransform).xyz;
    output.UVs = input.texCoord;


//TEMP
    //output.blendWeight = input.blendWeight;
    //output.boneIndices = input.boneIndices;
//TEMP

    return output;
}
