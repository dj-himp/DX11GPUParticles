#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

cbuffer debugConstantBuffer : register(b2)
{
    uint boneID;

    uint padding[3];
}

cbuffer boneTransformConstantBuffer : register(b3)
{
    float4x4 boneTransforms[96];
}

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
    float4 color : TEXCOORD2;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    float4x4 BoneTransform = boneTransforms[input.boneIndices[0]] * input.blendWeight[0];
    BoneTransform     += boneTransforms[input.boneIndices[1]] * input.blendWeight[1];
    BoneTransform     += boneTransforms[input.boneIndices[2]] * input.blendWeight[2];
    BoneTransform     += boneTransforms[input.boneIndices[3]] * input.blendWeight[3];
    output.Position = mul(float4(input.Position, 1.0), BoneTransform);
    output.Position = mul(output.Position, mul(world, viewProj));

    //output.Position = mul(float4(input.Position, 1.0), mul(world, viewProj));
    //output.pos = mul(float4(input.Position.xyz, 1.0), worldViewProj);
    output.normal = input.normal;
    output.UVs = input.texCoord;
    
    output.color = float4(1,1,1,1);
    /*for(int i=0;i<4;++i)
    {
        if(input.boneIndices[i] == boneID)
        {
            if(input.blendWeight[i] >= 0.7)
                output.color = float4(1,0,0,1);
            else if(input.blendWeight[i] >= 0.4 && input.blendWeight[i] <= 0.6)
                output.color = float4(0,1,0,1);
            else if(input.blendWeight[i] >= 0.1)
                output.color = float4(1,1,0,1);
        }
    }*/

    return output;
}
